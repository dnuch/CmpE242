#include <asm/io.h>

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/kobject.h>
#include <linux/delay.h>

#include "stepper_242.h"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Danny Nuch danny.nuch@sjsu.edu 009744943");
MODULE_DESCRIPTION("Rasbperry Pi 3b GPIO LKM");
MODULE_VERSION("1");

// clock manager register password "5a"
static const uint32_t passwd = (0x5a << 24);
static const uint32_t max_clock_divider_value = 4095;

static volatile gpio_registers_s * gpio_registers;
static volatile pwm_clock_manager_registers_s * pwm_clock_manager_registers;
static volatile pwm_registers_s * pwm_registers;

static void set_gpio_function(int pin_number, gpio_function_e gpio_function) {
	// Each register holds up to 10 pin configurations at 3 bits each (0-29)
	const int register_index = (pin_number / 10);
        const int bit_index = ((pin_number % 10) * 3) % 32;

	const uint32_t bit_mask = 0b111 << bit_index;
	// Clear with bit mask
	gpio_registers->GPFSEL[register_index] &= ~bit_mask;
	gpio_registers->GPFSEL[register_index] |= (gpio_function << bit_index);

	printk("Pin %i set with value %x\n", pin_number, gpio_function);
}

static void set_gpio_output(int pin_number, gpio_output_e gpio_output) {
	const int register_index = (pin_number / 32);
	const uint32_t bit_index = (1 << (pin_number % 32));

	switch (gpio_output) {
		case GPIO_OUTPUT__HIGH:
			gpio_registers->GPSET[register_index] = bit_index;
			break;
		
		case GPIO_OUTPUT__LOW:
		default:
			gpio_registers->GPCLR[register_index] = bit_index;
			break;
	}

	printk("Pin %i set with output %x\n", pin_number, gpio_output);
}

static void initialize_pwm0(void) {
	const uint32_t enab = (1 << 4);
	// PWM frequency = 19.2MHz clock source
	const uint32_t clock_source = (1 << 0);
	const uint32_t pwen1 = (1 << 0);
	const uint32_t msen1 = (1 << 7);

	// Disable PWM
	pwm_registers->CTL = 0;
	// Disable PWM clock manager generator
	pwm_clock_manager_registers->CTL = passwd;
	// Set control register
	pwm_clock_manager_registers->DIV = (passwd | (4095 << 12));
	pwm_clock_manager_registers->CTL = (passwd | clock_source);
	pwm_clock_manager_registers->CTL |= (passwd | enab);

	pwm_registers->RNG1 = 4095;
	// Set 50% duty cycle
	pwm_registers->DAT1 = 1;
	// Set M/S mode and enable PWM channel
        pwm_registers->CTL = msen1;
	pwm_registers->CTL |= pwen1;

	printk("pwm ctl: %x, dat: %u, rng: %u\n", pwm_registers->CTL, pwm_registers->DAT1, pwm_registers->RNG1);
	printk("pwm clock ctl: %x, div: %u\n", pwm_clock_manager_registers->CTL, pwm_clock_manager_registers->DIV >> 12);
}

static void set_pwm0_frequency(uint32_t desired_frequency_hz) {
	// Desired PWM frequency = 19.2MHz clock source / clock_divider / clock_range
	// clock_divider / clock_range = 19.2 MHz / desired
	const uint32_t pwm_core_clock_frequency_hz = 19200000;
	uint32_t clock_range = 2;
	uint32_t clock_divider = pwm_core_clock_frequency_hz / desired_frequency_hz;
	uint32_t pwm_cm_ctl_previous = 0;
	uint32_t pwm_ctl_previous = 0;
	if (0 != desired_frequency_hz) {
		if (clock_divider > max_clock_divider_value) {
			clock_range = clock_divider / max_clock_divider_value;
			clock_divider = 4095;
		 } else {
			clock_divider /= 2; // divide by 2 since clock_range must be 2
		}
		
		
		// Disable PWM
		pwm_ctl_previous = pwm_registers->CTL;
		pwm_registers->CTL = 0;
		// Disable PWM clock manager generator
		pwm_cm_ctl_previous = pwm_clock_manager_registers->CTL;
		pwm_clock_manager_registers->CTL = passwd;

		pwm_clock_manager_registers->DIV = (passwd | (clock_divider << 12));
		pwm_registers->RNG1 = clock_range;
		pwm_registers->DAT1 = 1;
		pwm_clock_manager_registers->CTL = (passwd | pwm_cm_ctl_previous);
		pwm_registers->CTL = pwm_ctl_previous;

		printk("Desired frequency set: %u\n", desired_frequency_hz);
		printk("pwm ctl: %x, dat: %u, rng: %u\n", pwm_registers->CTL, pwm_registers->DAT1, pwm_registers->RNG1);
		printk("pwm clock ctl: %x, div: %u\n", pwm_clock_manager_registers->CTL, pwm_clock_manager_registers->DIV >> 12);
	}
}

static ssize_t gpio_function_store(struct device *dev, struct device_attribute *dev_attr, const char *buf, unsigned int count) {
	int pin_number;
	gpio_function_e gpio_function;

	printk("gpio_function: %s\n", buf);
	sscanf(buf, "%u %u", &pin_number, &gpio_function);

	set_gpio_function(pin_number, gpio_function);

	return count;	
}

static ssize_t gpio_output_store(struct device *dev, struct device_attribute *dev_attr, const char *buf, unsigned int count) {
	uint32_t pin_number;
	gpio_output_e gpio_output;

	printk("gpio_output: %s\n", buf);
	sscanf(buf, "%u %u", &pin_number, &gpio_output);

	set_gpio_output(pin_number, gpio_output);

	return count;
}

static ssize_t pwm0_frequency_store(struct device *dev, struct device_attribute *dev_attr, const char *buf, unsigned int count) {
	uint32_t pwm_frequency_hz = 0;
	
	printk("pwm0_frequency: %s\n", buf);
	sscanf(buf, "%u", &pwm_frequency_hz);

	set_pwm0_frequency(pwm_frequency_hz);

	return count;
}

static struct device * stepper_dev;
static struct kobject * stepper_root;

static const struct device_attribute gpio_output_attribute = {
	.attr = {
		.name = "gpio_output",
		.mode = 222, // write only mode
	},
	.show = NULL, 
	.store = gpio_output_store
};

static const struct device_attribute gpio_function_attribute = {
	.attr = {
		.name = "gpio_function",
		.mode = 222, // write only mode
	},
	.show = NULL, 
	.store = gpio_function_store
};

static const struct device_attribute pwm0_frequency_attribute = {
	.attr = {
		.name = "pwm0_frequency",
		.mode = 222, // write only mode
	},
	.show = NULL,
	.store = pwm0_frequency_store
};

static int stepper_242_init(void) {
	
	pr_alert("stepper_242: init\n");

	gpio_registers = (volatile gpio_registers_s *)ioremap(GPIO_BASE, sizeof(gpio_registers_s));
	pwm_clock_manager_registers = (volatile pwm_clock_manager_registers_s *)ioremap(PWM_CLOCK_BASE, sizeof(pwm_clock_manager_registers_s));
	pwm_registers = (volatile pwm_registers_s *)ioremap(PWM_BASE, sizeof(pwm_registers_s));
	if ((NULL == gpio_registers) || (NULL == pwm_clock_manager_registers) || (NULL == pwm_registers)) {
		printk("GPIO, PWM clock, PWM peripheral remap failed\n");
	} else {
		// Create /sys/devices/gpio_242
		stepper_dev = root_device_register("gpio_242");
		stepper_root = &stepper_dev->kobj;
		// Create /sys/devices/stepper_242/gpio_function
		if (sysfs_create_file(stepper_root, &gpio_function_attribute.attr)) {
			printk("gpio_function file failed\n");
		}
		// Create /sys/devices/stepper_242/gpio_output
		if (sysfs_create_file(stepper_root, &gpio_output_attribute.attr)) {
			printk("gpio_output file failed\n");
		}
		// Create /sys/devices/stepper_242/pwm0_frequency
		if (sysfs_create_file(stepper_root, &pwm0_frequency_attribute.attr)) {
			printk("pwm0_frequency file failed\n");
		} 
		initialize_pwm0();
	}

	return 0;
}


static void stepper_242_exit(void) {
	pr_alert("stepper_242: exit\n");
	pwm_registers->CTL = 0;
	pwm_registers->DAT1 = 0;
	pwm_registers->RNG1 = 0;
	pwm_clock_manager_registers->CTL = passwd;
	pwm_clock_manager_registers->DIV = passwd;
	sysfs_remove_file(stepper_root, &gpio_function_attribute.attr);
	sysfs_remove_file(stepper_root, &gpio_output_attribute.attr);
	sysfs_remove_file(stepper_root, &pwm0_frequency_attribute.attr);
	root_device_unregister(stepper_dev);
	iounmap(gpio_registers);
	iounmap(pwm_clock_manager_registers);
	iounmap(pwm_registers);
}

module_init(stepper_242_init);
module_exit(stepper_242_exit);


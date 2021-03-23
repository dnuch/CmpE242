#include <asm/io.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

#define PERIPHERAL_BASE 0x3F000000
#define GPIO_BASE (PERIPHERAL_BASE + 0x200000)

// Using 12 registers out of 41
typedef struct {
	uint32_t GPFSEL[6];
	uint32_t reserve_slot_one;
	uint32_t GPSET[2];
	uint32_t reserved_slot_two;
	uint32_t GPCLR[2];
} gpio_registers_s;

typedef enum {
	GPIO_FUNCTION__INPUT  = 0b000,
	GPIO_FUNCTION__OUTPUT = 0b001,
	GPIO_FUNCTION__ALT0   = 0b100,
	GPIO_FUNCTION__ALT1   = 0b101,
	GPIO_FUNCTION__ALT2   = 0b110,
	GPIO_FUNCTION__ALT3   = 0b111,
	GPIO_FUNCTION__ALT4   = 0b011,
	GPIO_FUNCTION__ALT5   = 0b010
} gpio_function_e;

typedef enum {
	GPIO_OUTPUT__LOW,
	GPIO_OUTPUT__HIGH
} gpio_output_e;

static volatile gpio_registers_s * gpio_registers;

static void set_gpio_function(int pin_number, gpio_function_e gpio_function) {
	// Each register holds up to 10 pin configurations at 3 bits each (0-29)
	const int register_index = pin_number / 10;
        const int bit_index = (pin_number % 10) * 3;

	const uint32_t bit_mask = 0b111 << bit_index;
	// Clear with bit mask
	gpio_registers->GPFSEL[register_index] &= ~(bit_mask);
	gpio_registers->GPFSEL[register_index] |= (gpio_function << bit_index);

	printk("Pin %i set with function %x\n", pin_number, gpio_function);
}

static void set_gpio_output(int pin_number, gpio_output_e gpio_output) {
	const int register_index = pin_number / 32;
	const uint32_t bit_index = 1 << (pin_number % 32);

	switch (gpio_output) {
		case GPIO_OUTPUT__HIGH:
			gpio_registers->GPSET[register_index] = bit_index;
			break;
		
		case GPIO_OUTPUT__LOW:
		default:
			gpio_registers->GPCLR[register_index] = bit_index;
			break;
	}
}

static const int LED_pin_number = 27;

static int gpio_242_init(void) {
	
	pr_alert("gpio_242: init\n");

	gpio_registers = (volatile gpio_registers_s *)ioremap(GPIO_BASE, 0xB0);
	if (NULL == gpio_registers) {
		printk("GPIO remap failed\n");
	} else {
		printk("GPIO remap address %p\n", gpio_registers);
		set_gpio_function(LED_pin_number, GPIO_FUNCTION__OUTPUT);
		set_gpio_output(LED_pin_number, GPIO_OUTPUT__HIGH);
	}

	return 0;
}


static void gpio_242_exit(void) {
	pr_alert("gpio_242: exit\n");
	set_gpio_output(LED_pin_number, GPIO_OUTPUT__LOW);
	iounmap(gpio_registers);
}

module_init(gpio_242_init);
module_exit(gpio_242_exit);


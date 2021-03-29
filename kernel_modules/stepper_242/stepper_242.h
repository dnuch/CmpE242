#ifndef _STEPPER_242
#define _STEPPER_242

// Raspberry Pi 3B Peripheral Base
#define PERIPHERAL_BASE 0x3F000000
#define PWM_CLOCK_BASE  (PERIPHERAL_BASE + 0x1010A0)
#define GPIO_BASE	(PERIPHERAL_BASE + 0x200000)
#define PWM_BASE 	(PERIPHERAL_BASE + 0x20C000)

typedef struct {
        uint32_t GPFSEL[6];
        uint32_t reserved_reg0;
        uint32_t GPSET[2];
        uint32_t reserved_reg1;
        uint32_t GPCLR[2];
} gpio_registers_s;

typedef enum {
        GPIO_FUNCTION__INPUT  = 0,
        GPIO_FUNCTION__OUTPUT = 1,
        GPIO_FUNCTION__ALT0   = 4,
        GPIO_FUNCTION__ALT1   = 5,
        GPIO_FUNCTION__ALT2   = 6,
        GPIO_FUNCTION__ALT3   = 7,
        GPIO_FUNCTION__ALT4   = 3,
        GPIO_FUNCTION__ALT5   = 2
} gpio_function_e;

typedef enum {
        GPIO_OUTPUT__LOW = 0,
        GPIO_OUTPUT__HIGH
} gpio_output_e;

typedef struct {
	uint32_t CTL;
	uint32_t DIV;
} pwm_clock_manager_registers_s;

typedef struct {
	uint32_t CTL;
	uint32_t STA;
	uint32_t DMAC;
	uint32_t reserved_reg0;
	uint32_t RNG1;
	uint32_t DAT1;
	uint32_t FIF1;
} pwm_registers_s;

#endif // _STEPPER_242


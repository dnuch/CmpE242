load_module.sh: Inserts LKM into Kernel

remove_module.sh: Remove LKM from Kernel

denny_stepper_program.py: Stepper motor driving script by interfacing GPIO function, output, PWM0 frequency userspace files in /sys/devices/gpio_242
ex: $ python3 danny_stepper_program.py --led-pin 27 --led-pin-level 1 --gpio-dir-pin 22 --gpio-dir-level 1 --pwm0-pin 18 --pwm0-pin-function 2 --pwm0-frequency 800


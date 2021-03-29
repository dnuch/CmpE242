#!/usr/bin/env python3
## Danny Nuch danny.nuch@sjsu.edu 009744943

from argparse import ArgumentParser

import subprocess
import sys

GPIO_FUNCTION_OUTPUT = 1

KERNEL_ROOT_DIR = "gpio_242"
KERNEL_SYS_DEVICE_PATH = "/sys/devices/{}".format(KERNEL_ROOT_DIR)
GPIO_FUNCTION_FILEPATH = "{}/gpio_function".format(KERNEL_SYS_DEVICE_PATH)
GPIO_OUTPUT_FILEPATH = "{}/gpio_output".format(KERNEL_SYS_DEVICE_PATH)
PWM0_FREQUENCY_FILEPATH = "{}/pwm0_frequency".format(KERNEL_SYS_DEVICE_PATH)

def get_args():
    parser = ArgumentParser(description="Danny Nuch danny.nuch@sjsu.edu 009744943 stepper driver")
    parser.add_argument("--led-pin", type=int)
    parser.add_argument("--led-pin-level", type=int)
    parser.add_argument("--gpio-dir-pin", type=int)
    parser.add_argument("--gpio-dir-level", type=int)
    parser.add_argument("--pwm0-pin", type=int)
    parser.add_argument("--pwm0-pin-function", type=int)
    parser.add_argument("--pwm0-frequency", type=int)

    return parser.parse_args()


def main():
    args = get_args();
    # Set LED function
    subprocess.run('echo {} {} > {}'.format(args.led_pin, GPIO_FUNCTION_OUTPUT, GPIO_FUNCTION_FILEPATH),shell=True)
    
    # Set LED output level
    subprocess.run('echo {} {} > {}'.format(args.led_pin, args.led_pin_level, GPIO_OUTPUT_FILEPATH),shell=True)
    
    # Set GPIO DIR function
    subprocess.run('echo {} {} > {}'.format(args.gpio_dir_pin, GPIO_FUNCTION_OUTPUT, GPIO_FUNCTION_FILEPATH),shell=True)
    
    # Set GPIO DIR output level
    subprocess.run('echo {} {} > {}'.format(args.gpio_dir_pin, args.gpio_dir_level, GPIO_OUTPUT_FILEPATH),shell=True)
    
    # Set PWM0 function
    subprocess.run('echo {} {} > {}'.format(args.pwm0_pin, args.pwm0_pin_function, GPIO_FUNCTION_FILEPATH),shell=True)
    
    # Set PWM0 frequency
    subprocess.run('echo {} > {}'.format(args.pwm0_frequency, PWM0_FREQUENCY_FILEPATH),shell=True)
    


if __name__ == "__main__":
    sys.exit(main())


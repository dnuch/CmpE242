import subprocess
import sys
import board
import busio
import time
import adafruit_ads1x15.ads1115 as ADS
from adafruit_ads1x15.ads1x15 import Mode
from adafruit_ads1x15.analog_in import AnalogIn

MAX_VOLTAGE_GAIN = 4.096
TOTAL_RESOLUTION = 32768
MAX_VREF = 3.3
MAX_RESOLUTION_FOR_VREF = MAX_VREF / (MAX_VOLTAGE_GAIN / TOTAL_RESOLUTION)

# Max PWM frequency
MAX_PWM_FREQ = 2000

# PWM frequency unit per digital output
PWM_STEP_PER_DIGITAL_OUT = MAX_PWM_FREQ / MAX_RESOLUTION_FOR_VREF 

# Max rate is 860 SPS
RATE = 860

# Time for sampling
END_TIME = 1

def main():
    i2c = busio.I2C(board.SCL, board.SDA)
    ads = ADS.ADS1115(i2c)
    ads.data_rate = RATE
    # Continuous sample mode
    # ads.mode = Mode.CONTINUOUS
    channel = AnalogIn(ads, ADS.P0)
    
    while (1):
        PWM_freq = round(PWM_STEP_PER_DIGITAL_OUT * channel.value)
        print('Current PWM frequency: {}'.format(PWM_freq))
        subprocess.call(['python3', 'danny_stepper_program.py', '--led-pin', '27', '--led-pin-level', '1', '--gpio-dir-pin', '22', '--gpio-dir-level', '1', '--pwm0-pin', '18', '--pwm0-pin-function', '2', '--pwm0-frequency', '{}'.format(PWM_freq)])
        time.sleep(0.1);

if __name__ == "__main__":
    sys.exit(main())


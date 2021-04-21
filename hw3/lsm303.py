import board
import busio
import adafruit_lsm303_accel
import adafruit_lsm303dlh_mag
import sys
import time

def main():
    print(
        "HW3: Interfacing LSM303 accelerometer and magnetometer\n"
        "Danny Nuch 009744943 danny.nuch@sjsu.edu\n"
    )

    i2c = busio.I2C(board.SCL, board.SDA)
    
    while (1):
        acc_data = adafruit_lsm303_accel.LSM303_Accel(i2c)
        print("Accel (m/s^2) : X = {0[0]:0.3f}, Y = {0[1]:0.3f}, Z = {0[2]:0.3f}".format(acc_data.acceleration))

        mag_data = adafruit_lsm303dlh_mag.LSM303DLH_Mag(i2c)
        print("Mag (uT)      : X = {0[0]:0.3f}, Y = {0[1]:0.3f}, Z = {0[2]:0.3f}".format(mag_data.magnetic))
        time.sleep(1)


if __name__ == "__main__":
    sys.exit(main())


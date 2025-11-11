#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "main.h"
#include "swi2c.h"
#include "mpu6050.h"


#define WHO_AM_I_REG        0x75
#define PWR_MGMT_1_REG      0x6B
#define SMPLRT_DIV_REG      0x19
#define ACCEL_CONFIG_REG    0x1C
#define ACCEL_XOUT_H_REG    0x3B
#define TEMP_OUT_H_REG      0x41
#define GYRO_CONFIG_REG     0x1B
#define GYRO_XOUT_H_REG     0x43

#define MPU6050_ADDR    0x68


bool mpu6050_init(void)
{
    swi2c_init();

    uint8_t whoami;
    swi2c_read(MPU6050_ADDR, WHO_AM_I_REG, &whoami, 1);

    if (whoami != 0x68)
    {
        return false;
    }

    uint8_t data;

    // reset the device
    data = 0;
    swi2c_write(MPU6050_ADDR, PWR_MGMT_1_REG, &data, 1);

    delay_ms(100);

    // set the sample rate to 1kHz
    data = 0x07;
    swi2c_write(MPU6050_ADDR, SMPLRT_DIV_REG, &data, 1);

    // set the accelerometer configuration to 2g
    data = 0x00;
    swi2c_write(MPU6050_ADDR, ACCEL_CONFIG_REG, &data, 1);

    // set the gyro configuration to 250deg/s
    data = 0x00;
    swi2c_write(MPU6050_ADDR, GYRO_CONFIG_REG, &data, 1);

    return true;
}

void mpu6050_read_accel(mpu6050_accel_t *accel)
{
    uint8_t raw_data[6];

    swi2c_read(MPU6050_ADDR, ACCEL_XOUT_H_REG, raw_data, 6);

    accel->x_raw = (int16_t)(raw_data[0] << 8 | raw_data[1]);
    accel->y_raw = (int16_t)(raw_data[2] << 8 | raw_data[3]);
    accel->z_raw = (int16_t)(raw_data[4] << 8 | raw_data[5]);

    accel->x = accel->x_raw / 16384.0f;
    accel->y = accel->y_raw / 16384.0f;
    accel->z = accel->z_raw / 16384.0f;
}

void mpu6050_read_gyro(mpu6050_gyro_t *gyro)
{
    uint8_t raw_data[6];

    swi2c_read(MPU6050_ADDR, GYRO_XOUT_H_REG, raw_data, 6);

    gyro->x_raw = (int16_t)(raw_data[0] << 8 | raw_data[1]);
    gyro->y_raw = (int16_t)(raw_data[2] << 8 | raw_data[3]);
    gyro->z_raw = (int16_t)(raw_data[4] << 8 | raw_data[5]);

    gyro->x = gyro->x_raw / 131.0f;
    gyro->y = gyro->y_raw / 131.0f;
    gyro->z = gyro->z_raw / 131.0f;
}

float mpu6050_read_temper(void)
{
    uint8_t raw_data[2];

    swi2c_read(MPU6050_ADDR, TEMP_OUT_H_REG, raw_data, 2);

    int16_t temp_raw = (int16_t)(raw_data[0] << 8 | raw_data[1]);

    return (float)temp_raw / 340.0f + 36.53f;
}

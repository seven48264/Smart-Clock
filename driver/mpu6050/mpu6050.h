#ifndef __MPU6050_H__
#define __MPU6050_H__


#include <stdbool.h>
#include <stdint.h>


typedef struct
{
    int16_t x_raw;
    int16_t y_raw;
    int16_t z_raw;

    float x;
    float y;
    float z;
} mpu6050_accel_t;

typedef struct
{
    int16_t x_raw;
    int16_t y_raw;
    int16_t z_raw;

    float x;
    float y;
    float z;
} mpu6050_gyro_t;


bool mpu6050_init(void);
void mpu6050_read_accel(mpu6050_accel_t *accel);
void mpu6050_read_gyro(mpu6050_gyro_t *gyro);
float mpu6050_read_temper(void);


#endif /* __MPU6050_H__ */

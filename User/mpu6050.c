//
// Created by Whitolf on 2019/11/30.
//

#include "mpu6050.h"
#include "i2c.h"

u_char MPU_6050_Init(){
    u_char res = 0, ret = 0;
    MPU_6050_Write(MPU_WRITE, MPU_PWR_MGMT1_REG, 0x80, 1);  //复位MPU6050
    HAL_Delay(50);
    MPU_6050_Write(MPU_WRITE, MPU_PWR_MGMT1_REG, 0x03, 1);  //以陀螺仪Z轴为参考锁相环时钟
    HAL_Delay(50);
    //MPU_6050_Write(MPU_WRITE, MPU_PWR_MGMT1_REG, 0x00, 1);
    HAL_Delay(50);
    MPU_Set_Rate(25);   //设置采样率50Hz
    HAL_Delay(15);  //更改CLKSEL时的PLL建立时间最大为10ms。这里使用15毫秒
    MPU_Set_LPF(25);  //MPU6050的数字低通滤波器
    MPU_Set_Gyro_Fsr(3);    //陀螺仪传感器,±2000dps
    MPU_Set_Accel_Fsr(2);   //加速度传感器,±8g (4096 LSB/g)
    MPU_6050_Write(MPU_WRITE, MPU_INTBP_CFG_REG,1 << 7 | 0 << 6 | 0 << 5 | 0 << 4 | 0 << 3 | 0 << 2 | 1 << 1 | 0 << 0, 8);  //INT引脚低电平有效
    MPU_6050_Write(MPU_WRITE, MPU_PWR_MGMT2_REG, 0x00, 1);
    //MPU_Set_Rate(50);   //设置采样率50Hz
    MPU_6050_Read(MPU_READ, MPU_DEVICE_ID_REG, &res, 1);   //读MPU6050状态
    if (res == MPU_RA_WHO_AM_I)   //验证器件ID正确
    {
        ret = 0;
    } else {
        ret = 1;
    }
    return ret;
}
//MPU6050陀螺仪传感器满量程范围
u_char MPU_Set_Gyro_Fsr(u_char fsr){
    return MPU_6050_Write(MPU_WRITE,MPU_GYRO_CFG_REG,fsr<<3,1);
}
//MPU6050加速度传感器满量程范围
u_char MPU_Set_Accel_Fsr(u_char fsr){
    return MPU_6050_Write(MPU_WRITE,MPU_ACCEL_CFG_REG,fsr<<3,1);
}
//MPU6050的数字低通滤波器
u_char MPU_Set_LPF(u_char lpf) {
    u_char data = 0;
    if (lpf >= 188) {
        data = 1;
    }
    else if (lpf >= 98) {
        data = 2;
    }
    else if (lpf >= 42) {
        data = 3;
    }
    else if (lpf >= 20) {
        data = 4;
    }
    else if (lpf >= 10) {
        data = 5;
    }
    else {
        data = 6;
    }
    return MPU_6050_Write(MPU_WRITE, MPU_CFG_REG, &data, 1);
}
//MPU6050的采样率(假定Fs=1KHz)
u_char MPU_Set_Rate(u_char rate){
    u_char data;
    if(rate>1000){
        rate=1000;
    }
    if(rate<4){
        rate=4;
    }
    data=1000/rate-1;
    return MPU_6050_Write(MPU_WRITE,MPU_SAMPLE_RATE_REG,&data,1);	//设置数字低通滤波器
}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void){
    u_char buf[2];
    short raw;
    float temp;
    MPU_6050_Read(MPU_RA_WHO_AM_I,MPU_TEMP_OUTH_REG,buf,2);
    raw=(buf[0]<<8)| buf[1];
    temp=36.53+((double)raw)/340;
//  temp = (long)((35 + (raw / 340)) * 65536L);
    return temp;
}

//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],ret;
    ret=MPU_6050_Read(MPU_READ,MPU_GYRO_XOUTH_REG,buf,6);
    if(ret==0)
    {
        *gx=((uint16_t)buf[0]<<8)|buf[1];
        *gy=((uint16_t)buf[2]<<8)|buf[3];
        *gz=((uint16_t)buf[4]<<8)|buf[5];
    }
    return ret;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],ret;
    ret=MPU_6050_Read(MPU_READ,MPU_ACCEL_XOUTH_REG,buf,6);
    if(ret==0)
    {
        *ax=((uint16_t)buf[0]<<8)|buf[1];
        *ay=((uint16_t)buf[2]<<8)|buf[3];
        *az=((uint16_t)buf[4]<<8)|buf[5];
    }
    return ret;;
}


u_char MPU_6050_Write(u_char addr, u_char reg, u_char *buf, u_char len){
    HAL_I2C_Mem_Write(MPU6050_IIC_PORT,addr,reg,I2C_MEMADD_SIZE_8BIT, buf, len,100);
    return 0;
}
u_char MPU_6050_Read(u_char addr, u_char reg, u_char *buf, u_char len){
    HAL_I2C_Mem_Read(MPU6050_IIC_PORT,addr,reg,I2C_MEMADD_SIZE_8BIT, buf, len,100);
    return 0;
}

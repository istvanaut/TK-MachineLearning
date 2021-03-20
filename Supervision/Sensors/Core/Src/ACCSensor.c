#include "ACCSensor.h"
#include "bno055_stm32.h"

bno055_vector_t acceleration;
bno055_vector_t gyro;
bno055_vector_t euler;

bno055_vector_t accOffset;



void initACCSensor(I2C_HandleTypeDef* hi2c_device){
	bno055_assignI2C(hi2c_device);
	bno055_setup();
	bno055_setOperationMode(BNO055_OPERATION_MODE_NDOF);
	HAL_Delay(100);
	accOffset = bno055_getVectorAccelerometer();
	//accOffset = bno055_getVectorLinearAccel();
}

bno055_vector_t getAccWithMeasure(void){
	acceleration = bno055_getVectorAccelerometer();
	//acceleration = bno055_getVectorLinearAccel();

	acceleration.x -= accOffset.x;
	acceleration.y -= accOffset.y;
	acceleration.z -= accOffset.z;

	return acceleration;
}

bno055_vector_t getMeasuredAcc(void){
	return acceleration;
}

bno055_vector_t getGyroWithMeasure(void){
	gyro = bno055_getVectorGyroscope();
	return gyro;
}

bno055_vector_t getMeasuredGyro(void){
	return gyro;
}

bno055_vector_t getEulerWithMeasure(void){
	euler = bno055_getVectorEuler();
	euler.x = (euler.x <= 360 && euler.x > 180) ? (euler.x - 360):(euler.x);
	euler.y = (euler.y <= 360 && euler.y > 180) ? (euler.y - 360):(euler.y);
	euler.z = (euler.z <= 360 && euler.z > 180) ? (euler.z - 360):(euler.z);
	return euler;
}

bno055_vector_t getMeasuredEuler(void){
	return euler;
}

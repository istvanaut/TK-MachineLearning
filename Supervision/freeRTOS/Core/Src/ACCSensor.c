#include "ACCSensor.h"
#include "bno055_stm32.h"

extern osSemaphoreId_t SemACCHandle;

static bno055_vector_t acceleration;
static bno055_vector_t gyro;
static bno055_vector_t euler;

static bno055_vector_t accOffset;

void initACCSensor(I2C_HandleTypeDef* hi2c_device){
	bno055_assignI2C(hi2c_device);
	bno055_setup();
	bno055_setOperationMode(BNO055_OPERATION_MODE_NDOF);
	HAL_Delay(100);
	accOffset = bno055_getVectorAccelerometer();
	//accOffset = bno055_getVectorLinearAccel();
}

void AccMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorAccelerometer();
	//acceleration = bno055_getVectorLinearAccel();
	osSemaphoreAcquire(SemACCHandle, 0);
	acceleration = temp;
	acceleration.x -= accOffset.x;
	acceleration.y -= accOffset.y;
	acceleration.z -= accOffset.z;
	
	acceleration.x *= -1;
	acceleration.y *= -1;
	
	osSemaphoreRelease(SemACCHandle);
}

bno055_vector_t getAcc(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, 5);
	temp = acceleration;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}

void GyroMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorGyroscope();

	osSemaphoreAcquire(SemACCHandle, 0);
	gyro = temp;
	osSemaphoreRelease(SemACCHandle);
}

bno055_vector_t getGyro(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, 5);
	temp = gyro;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}

void EulerMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorEuler();

	osSemaphoreAcquire(SemACCHandle, 0);
	euler = temp;
	euler.x = (euler.x <= 360 && euler.x > 180) ? (euler.x - 360):(euler.x);
	euler.y = (euler.y <= 360 && euler.y > 180) ? (euler.y - 360):(euler.y);
	euler.z = (euler.z <= 360 && euler.z > 180) ? (euler.z - 360):(euler.z);
	osSemaphoreRelease(SemACCHandle);
}

bno055_vector_t getEuler(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, 5);
	temp = euler;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}

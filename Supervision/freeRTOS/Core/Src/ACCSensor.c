#include "ACCSensor.h"
#include "bno055_stm32.h"

extern osSemaphoreId_t SemACCHandle;

static bno055_vector_t acceleration;
static bno055_vector_t gyro;
static bno055_vector_t euler;

static bno055_vector_t accOffset;


//Initialize the sensor in NDOF mode and measure the offset values
void initACCSensor(I2C_HandleTypeDef* hi2c_device){
	setACC_OS_RUNNING(0);
	bno055_assignI2C(hi2c_device);
	bno055_setup();
	bno055_setOperationMode(BNO055_OPERATION_MODE_NDOF);
	HAL_Delay(100);
	accOffset = bno055_getVectorAccelerometer();
	//accOffset = bno055_getVectorLinearAccel();
}

//Measure the acceleration in each axis and subtract the offset values
//It is using blocking semaphore. DO NOT call from ISR.
void AccMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorAccelerometer();
	//temp = bno055_getVectorLinearAccel();
	temp.x -= accOffset.x;
	temp.y -= accOffset.y;
	temp.z -= accOffset.z;

	temp.x *= -1;
	temp.y *= -1;

	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	acceleration = temp;
	osSemaphoreRelease(SemACCHandle);
}

//Return the acceleration values
//It is using blocking semaphore. DO NOT call from ISR.
bno055_vector_t getAcc(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	temp = acceleration;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}

//Measure the angular velocity in each axis
//It is using blocking semaphore. DO NOT call from ISR.
void GyroMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorGyroscope();

	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	gyro = temp;
	osSemaphoreRelease(SemACCHandle);
}

//Return the angular velocities
//It is using blocking semaphore. DO NOT call from ISR.
bno055_vector_t getGyro(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	temp = gyro;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}

//Measure the relative angle from starting position and transform it to -180° and +180° range
//It is using blocking semaphore. DO NOT call from ISR.
void EulerMeasure(void){
	bno055_vector_t temp;
	temp = bno055_getVectorEuler();
	temp.x = (temp.x <= 360 && temp.x > 180) ? (temp.x - 360):(temp.x);
	temp.y = (temp.y <= 360 && temp.y > 180) ? (temp.y - 360):(temp.y);
	temp.z = (temp.z <= 360 && temp.z > 180) ? (temp.z - 360):(temp.z);

	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	euler = temp;
	osSemaphoreRelease(SemACCHandle);
}

//Return the relative angle from starting position
//It is using blocking semaphore. DO NOT call from ISR.
bno055_vector_t getEuler(void){
	bno055_vector_t temp;
	osSemaphoreAcquire(SemACCHandle, osWaitForever);
	temp = euler;
	osSemaphoreRelease(SemACCHandle);
	return temp;
}


//0: OS not running 1: OS running
//0: It is using HAL_Delay 1: It is using OS_delay
void setACC_OS_RUNNING(uint8_t value){
	bno055_setFreeRTOSRunning(value);
}

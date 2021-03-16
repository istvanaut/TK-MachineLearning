#include "ACCSensor.h"
#include "bno055_stm32.h"

bno055_vector_t acceleration;

bno055_vector_t accOffset;

void initACCSensor(I2C_HandleTypeDef* hi2c_device){
	bno055_assignI2C(hi2c_device);
	bno055_setup();
	bno055_setOperationMode(BNO055_OPERATION_MODE_ACCONLY);
	accOffset = bno055_getVectorAccelerometer();
}

bno055_vector_t getAccWithMeasure(void){
	acceleration = bno055_getVectorAccelerometer();
	acceleration.x -= accOffset.x;
	acceleration.y -= accOffset.y;
	acceleration.z -= accOffset.z;
	return acceleration;
}

bno055_vector_t getMeasuredAcc(void){
	return acceleration;
}

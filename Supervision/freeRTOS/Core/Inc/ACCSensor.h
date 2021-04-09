#include "main.h"
#include "bno055.h"
#include "cmsis_os.h"

void initACCSensor(I2C_HandleTypeDef* hi2c_device);

void AccMeasure(void);

bno055_vector_t getAcc(void);

void GyroMeasure(void);

bno055_vector_t getGyro(void);

void EulerMeasure(void);

bno055_vector_t getEuler(void);

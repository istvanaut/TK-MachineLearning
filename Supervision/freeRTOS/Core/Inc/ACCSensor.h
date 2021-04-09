#include "main.h"
#include "bno055.h"
#include "cmsis_os.h"

void initACCSensor(I2C_HandleTypeDef* hi2c_device);

void AccMeasure(void);

bno055_vector_t getMeasuredAcc(void);

bno055_vector_t getGyroWithMeasure(void);

bno055_vector_t getMeasuredGyro(void);

bno055_vector_t getEulerWithMeasure(void);

bno055_vector_t getMeasuredEuler(void);

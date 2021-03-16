#include "main.h"
#include "bno055.h"

void initACCSensor(I2C_HandleTypeDef* hi2c_device);

bno055_vector_t getAccWithMeasure(void);

bno055_vector_t getMeasuredAcc(void);

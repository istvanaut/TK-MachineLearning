#ifndef SENSOR_STATES_HEADER   /* Include guard */
#define SENSOR_STATES_HEADER

typedef struct SensorStates
{
	/* Previous sensor states*/
	uint32_t lightsensor;
	uint32_t usLeft;
	uint32_t usRight;
	uint32_t laser;
}SensorStates;

typedef struct MotorStates
{
    /* Performed previous action*/
	float motorLeft;
	float motorRight;
    float reward;
}MotorStates;

typedef struct CameraStates
{
    uint8_t imgp[CAM_HEIGHT*CAM_WIDTH];
    uint8_t imgc[CAM_HEIGHT*CAM_WIDTH];
}CameraStates;

typedef struct{
    uint32_t lightsensorP;
	uint32_t usLeftP;
	uint32_t usRightP;
	uint32_t laserP;
	/* Performed previous action*/
	float leftM;
	float rightM;
    float reward;
    uint32_t lightsensorC;
	uint32_t usLeftC;
	uint32_t usRightC;
	uint32_t laserC;
    uint8_t imgp[CAM_HEIGHT*CAM_WIDTH];
    uint8_t imgc[CAM_HEIGHT*CAM_WIDTH];
}AllStates;

union sensorStatePU
{
    SensorStates* sensorStates;
    MotorStates* motorStates;
    uint8_t* buf;
};
union sensorStateCU
{
    SensorStates* sensorStates;
    MotorStates* motorStates;
    CameraStates* cameraStates;
    uint8_t* buf;
};
#endif
#ifndef SERVO_PWM
#define SERVO_PWM

// DEFINES
#define PWM_FREQ 50 //Hz
#define PWM_PERIOD 20 //ms
#define PWM_LOAD 0x30D4 //12500
#define PWM_LOAD_90PERCENT 0x2BF2 //11250

void InitPWMforServo(void);
void position_servo(uint8_t Degrees, uint32_t Load);

#endif
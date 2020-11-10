#ifndef SERVO_PWM
#define SERVO_PWM

// DEFINES
#define PWM_FREQ 50

void InitPWMforServo(void);
void position_servo(uint8_t Degrees, uint32_t Load);

#endif
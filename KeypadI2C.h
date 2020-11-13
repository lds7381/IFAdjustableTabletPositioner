#ifndef KEYPAD_I2C
#define KEYPAD_I2C

// DEFINES
#define SCL_LP = 6;
#define SCL_HP = 4;
#define SCL_CLK = 100000;
#define SLAVE_ADDRESS_BASE 0x4B;

void Initialise_I2C(void);
void I2C_EnableMaster(void);
void I2C_EnableSlave(void);
void I2C_EnableSlaveDataInterrupt( void );

#endif
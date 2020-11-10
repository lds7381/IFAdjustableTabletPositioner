#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"


#define SCL_LP = 6;
#define SCL_HP = 4;
#define SCL_CLK = 100000;
#define SLAVE_ADDRESS_BASE 0x4B;

// Initialize the I2C module on the Microcontroller
void Initialise_I2C(void){
	
	//enable the I2C clock in System control module
	SYSCTL_RCGCI2C_R |= 0x1;
	//wait for above bit to be set
	while (!(SYSCTL_PRI2C_R & 0x1)){}
		
	//configuree pins for I2C SDA and SDL
	//SCL
	GPIOPinTypeI2CSCL(GPIO_PORTB_BASE, GPIO_PIN_2);
	GPIOPinConfigure(GPIO_PCTL_PB2_I2C0SCL);
	//SDA (open-drain)
	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_3); 
	GPIOPinConfigure(GPIO_PCTL_PB3_I2C0SDA);
		
	return;
	
}

// Enable the MC as the I2C Master
void I2C_EnableMaster(void){
	
	// set the 5th bit of the control register to enable master mode
	I2C0_MCR_R |= 0x10;
	// wait for bit to get set
	while (!(I2C0_MCR_R & 0x10)){}
		
	// set the desired SCL clock speed of 100Kbps
	uint32_t TPR = (SysCtlClockGet() / (20*100000))-1;
	I2C0_MTPR_R = TPR;
		
}

// Enable the keypad as the I2C Slave
void I2C_EnableSlave(void){
	
	// set master function enable
	I2C0_MCR_R |= I2C_MCR_MFE;
	// set slave own adress
	I2C0_SOAR_R |= (I2C_SOAR_OAR_M & 0x4B);
	// enable device active
	I2C0_SCSR_R |= I2C_SCSR_DA;
	
}

// Enable interrupts from the slave
void I2C_EnableSlaveDataInterrupt( void ){
	
	//clear prior interrupts and unmask interrupt for data
	I2C0_SICR_R |= I2C_SICR_DATAIC; 
	I2C0_SIMR_R |= I2C_SIMR_DATAIM;
	
	//enable IRQ handler for slave interrupts
	
}

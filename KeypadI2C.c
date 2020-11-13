#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/i2c.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "KeypadI2C.h"


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

static void HandleInterrupt(void){
	
	volatile uint32_t readback;
	
	// Check to see if this is a data interrupt
	if(I2C0_SMIS_R & I2C_SMIS_DATAMIS){
		// Clear Interrupt Flag
		I2C0_SICR_R |= I2C_SMIS_DATAMIS;
		
		// Read to force the clear of the intterupt flag
		readback = I2C0_SICR_R;
		
		// Reading the SCSR will clear it so it must be copied
		uint32_t scr = I2C0_SCSR_R;
		
		// Data the will be received or transmitted to/from the slave
		uint8_t data;
		
		//Checks for if this Data is to be for receiving
		if(scr & I2C_SCSR_RREQ){
			data = (uint8_t)I2C0_SDR_R;
		}
		//Checks for if this Data is to be for Transmitting
		else if(scr & I2C_SCSR_TREQ){
			I2C0_SDR_R = data;
		}
		
	}
	
}

static int InvokeMasterCommand(uint32_t mcs){
	
	int error = 0;
	
	// Clear the RIS bit (master interrupt)
	I2C0_MICR_R |= I2C_MICR_IC;
	
	// Invoke the command
	I2C0_MCS_R = mcs;
	
	// Wait until the RIS bit is set and 
	while(!(I2C0_MRIS_R & I2C_MRIS_RIS));
	
	// Check the error status
	mcs = I2C0_MCS_R;
	error |= mcs & (I2C_MCS_ARBLST | I2C_MCS_DATACK | I2C_MCS_ADRACK | I2C_MCS_ERROR);
	
	// If there was and error but it was not an Abitiration Lost, then issue a STOP
	if (error & I2C_MCS_ERROR && !(error & I2C_MCS_ARBLST)){
		I2C0_MCS_R = I2C_MCS_STOP;
	}
	
	return error;
	
}

int I2C_MasterWrite(uint8_t slaveAddress, char* data, int size, bool repeatedStart){
	
	// Set the slave address. The R/S (0) bit is cleared for a write
	I2C0_MSA_R = (I2C0_MSA_R & ~0xFF) | (slaveAddress << 1);
	
	int error = 0;
	
	for(int i = 0; i < size && !error; i++){
		// Note data in MDR reads back as 0 in the debugger
		I2C0_MDR_R = data[i];
		
		uint32_t mcs = I2C_MCS_RUN;
		
		if(i == 0){
			
			// Wait until the bus is idle unless this is a repeated start, if it is the bus will be busy because a transaction is still in progress
			if(!repeatedStart){
				while(I2C0_MCS_R & I2C_MCS_BUSBSY);
			}
			
			// Start if this is the first byte
			mcs |= I2C_MCS_START;
			
		}
		
		// Stop if this is the last byte
		if(i == (size - 1)){
			mcs |= I2C_MCS_STOP;
		}
		
		error = InvokeMasterCommand(mcs);
		
	}
	
	return error;
	
}



// Includes
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"
#include <stdio.h>
#include "driverlib/uart.h"
#include "driverlib/flash.h"
#include "ProfileFlash.h"
#include "ServoPWM.h"

// Constants


//-----------------------------------------------------------------------------------------------------------------------------
// Methods
//-----------------------------------------------------------------------------------------------------------------------------

void MCInit(void){
	
	//Variables
	uint32_t ui32Period; // Timer Delay Variable
	
	//Clock Setup
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // Configures the clock to run at 40 MHz
	
	//GPIO Configuatrion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); // Enables interrupt controller for Port E
	
	//Timer Configuration
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0); // Enables the clock to the peripheral
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	
	//Delay Calculation
	ui32Period = (SysCtlClockGet() / 10) / 2; // Interrupt is generated at 1/2 of the desired period
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1); //Sets the timer load to the desired interrupt period

	//Interrupt Enable
	IntEnable(INT_TIMER0A); // Enable interrupt vector INT_TIMER0A
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); 
	IntMasterEnable();
	
	//UART Enable
	//UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	//IntEnable(INT_UART0); //enable the UART interrupt 
	//UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
	//UARTEnable(UART0_BASE);
	
}

void GPIO_Init(void){
	
	// Enable and provide a clock to GPIO Port E in Run mode by writing to
	// bit 4 in the GPIO I/O Clock Gating Control Register
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	
	// Select one of several possible multiplexed functions for pin PE4
	GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_M0PWM4;

}







//-----------------------------------------------------------------------------------------------------------------------------
// Intrrupt Handlers and Misc.
//-----------------------------------------------------------------------------------------------------------------------------

//Handles an interrupt on Timer0
void Timer0IntHandler(void){
	// CLears the Interrupt timer
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
	//Reads the current state of the pin PF2 to see if interrupt is from there
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); // will send 0 to pin PF1 (THIS IS JUST TEST CODE)
		}
}

void UART0Handler(void){
	
	uint32_t ui32Status;
	ui32Status = UARTIntStatus(UART0_BASE, true); //get interrupt status
	UARTIntClear(UART0_BASE, ui32Status); //ckear the asserted interrupts
	
	
	while(UARTCharsAvail(UART0_BASE)){
		UARTCharPutNonBlocking(UART0_BASE, UARTCharGetNonBlocking(UART0_BASE)); //echo
	}
}

//-----------------------------------------------------------------------------------------------------------------------------
// Main Method
//-----------------------------------------------------------------------------------------------------------------------------

int main(void){
	

	//UARTCharPut(UART0_BASE, 'a');
	MCInit(); // Initialize the board
	GPIO_Init();
	InitPWMforServo();

	//Enable Flash Memory Read/Write
	uint16_t flashKey_ = Flash_Enable();
	
	//MASS ERASE
	//FLASH_FMC_R = (flashKey_ << 16) | 0x4;
	//while(FLASH_FMC_R & 0x4) {}
	
	//check to see if offset has been set in Flash, if not, write 0 to
	//initialize the offset, otherwise read the value of offset from Flash
	uint32_t offset = Flash_Read_Offset();
	if (offset == 0xFFFFFFFF){
		Flash_Write_Offset();
	}
	else{
		offset = OFFSET_FLASH_BASE_ADDR[0];
	}
	
		
//	uint32_t studentProf[4] = {1234, 10, 15, 32};
//	Flash_Write(&studentProf, 4);
//	offset = Flash_Read_Offset();
//	uint32_t studentProf2[4] = {5678, 30, 100, 150};
//	Flash_Write(&studentProf2, 4);
//	offset = Flash_Read_Offset();
//	uint32_t studentProf3[4] = {1678, 50, 800, 300};
//	Flash_Write(&studentProf3, 4);
//	offset = Flash_Read_Offset();
	
	uint32_t studentProf[4];
	Flash_Read(&studentProf, 4, 5678);
	
	volatile uint32_t ui32PWMClock = SysCtlClockGet() / 64;
	uint32_t ui32Load = (ui32PWMClock / PWM_FREQ) - 1;
	position_servo(120, ui32Load);
	
	//Main Code
	while (1){
		
	}
}







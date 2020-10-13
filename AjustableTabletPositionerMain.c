#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

//-----------------------------------------------------------------------------------------------------------------------------
// Methods
//-----------------------------------------------------------------------------------------------------------------------------

void MCInit(void){
	
	//Variables
	uint32_t ui32Period; // Timer Delay Variable
	
	//Clock Setup
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN); // Configures the clock to run at 40 MHz
	
	//GPIO Configuatrion
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF); // Enables interrupt controller for Port F
	
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
}

//-----------------------------------------------------------------------------------------------------------------------------
// Intrrupt Handlers and Misc.
//-----------------------------------------------------------------------------------------------------------------------------

//Handles an interrupt on Timer0
void Timer0IntHandler(void){
	// CLears the Interrupt timer
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	
}

//-----------------------------------------------------------------------------------------------------------------------------
// Main Method
//-----------------------------------------------------------------------------------------------------------------------------

int main(void){
	MCInit(); // Initialize the board
	
	//Main Code
	while (1){
		
		//Reads the current state of the pin PF2 to see if interrupt is from there
		if(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2)){
				GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0); // will send 0 to pin PF1 (THIS IS JUST TEST CODE)
		}
	//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8PinData); For writing to the GPIO PIN1,2,3
}







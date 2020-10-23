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

// Constants
#define PWM_FREQ 55

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

void InitPWMforServo(uint32_t Load, uint32_t PWMClock, uint8_t Adjust){
	// Setting the adjust to '83' will create a 1.5ms pulse width for the pwm
	Adjust = 83; 
	// Divides the system clock by 64 to run the clock at 625kHz which is 40Mhz/64.
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);	 //Enables PWM1
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Enables port D for GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //Enables port E for GPIO
	
	// Enabling pin PE4 for PWM output
	GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);
	GPIOPinConfigure(GPIO_PCTL_PE4_M0PWM4);
	
	//Configures module 1 PWM generator 0 as a down-counter and load the count value. 
	PWMClock = SysCtlClockGet() / 64;
	Load = (PWMClock / PWM_FREQ) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_0, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_0, Load);
	
	//Enables PWM to run
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, Adjust * Load / 1000); // sets the pulse with using the adjust and load, 
	PWMOutputState(PWM1_BASE, PWM_OUT_0_BIT, true);								// to change with change the ajust/load (currently runs for 1.5ms)
	PWMGenEnable(PWM1_BASE, PWM_GEN_0);
}

// Creates a pulse width from degrees that will be sent to the servo motor (0 to 180 degrees)
void position_servo(uint8_t Degrees, uint32_t Load) 
{
    uint8_t ui8Adjust;

    if (Degrees > 180){
       ui8Adjust = 111;
    }else{
       ui8Adjust = 56.0 + ((float)Degrees * (111.0 - 56.0)) / 180.0; // Calculating the adjust from the degrees given
    }
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_0, ui8Adjust * Load / 1000);
}

void CreateNewStudent(uint8_t Id){
		
}

void get_servo_position(){
	
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

//-----------------------------------------------------------------------------------------------------------------------------
// Main Method
//-----------------------------------------------------------------------------------------------------------------------------

int main(void){
	volatile uint32_t ui32Load;
	uint32_t ui32PWMClock;
	uint8_t ui8Adjust;
	uint8_t ui8Degrees;
	
	MCInit(); // Initialize the board
	InitPWMforServo(ui32Load, ui32PWMClock, ui8Adjust);
	
	//Main Code
	while (1){
		
	}
}







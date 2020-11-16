#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "driverlib/pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "ServoPWM.h"

void InitPWMforServo(void){
	
	uint8_t ui8Degrees;
	
	//Configures module 1 PWM generator 0 as a down-counter and load the count value. 
	volatile uint32_t ui32PWMClock = SysCtlClockGet() / 64; //625kHz
	volatile uint32_t ui32Load = (ui32PWMClock/PWM_FREQ) - 1; 
	
	//Enable PWM Clock in the System Control Module
	// #1
	SYSCTL_RCGC0_R |= 0x00100000;
	
	// Enable PWM0 and port E for GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);	 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	
	// Enabling pin PE4 for PWM output and configuring the alternate function of pin
	// #2
	GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4); 
	// #3
	GPIOPinConfigure(GPIO_PCTL_PE4_M0PWM4);
	
	// Divides the system clock by 64 to run the clock at 625kHz which is 40Mhz/64.
	SysCtlPWMClockSet(SYSCTL_RCC_USEPWMDIV);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	
	
	//Configures the PWM generator
	PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	//Sets the period of a PWM generator.
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Load);
	//Enables the timer/counter for a PWM generator block.
	PWMGenEnable(PWM0_BASE, PWM_GEN_2);
	
}


// Initializes PWM Generator 0 with a 50 Hz Frequency, a 10% duty cycle on
// the M0PWM4 pin (PE4)
void ALT_InitPWMforServo(void){
	
	// #1 Enable the PWM clock by writing a value of 0x0010.0000 to the RCGC0
	// register in the System Control module
	// Enables PWM Clock Gating Control (Bit/Field name PWM0)
	SYSCTL_RCGC0_R = SYSCTL_RCGC0_PWM0;
	
	// #2 Enable the clock to the appropriate GPIO module via the RCGC2 register
	// in the System Control module
	// Enables Port E Clock Gating Control (Bit/Field name GPIO
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
	
	// #3 Enable the appropriate pins for their alternate function using the
	// GPIOAFSEL register in the GPIO module
	// GPIO Port E Advanced Peripheral Bus (APB)
	// this assumes the PORT E GPIO Module clock was enabled first
	// this assumes the GPIO Port Control register was used to select
	// a multiplexed function (ex M0PWM4, alternate function #4)
	GPIO_PORTE_AFSEL_R |= 0x1;
	
	// #4 Configure the PMCn fields in the GPIOPCTL register to assign the
	// PWM signal to the appropriate pin
	GPIO_PORTE_PCTL_R |= GPIO_PCTL_PE4_M0PWM4;
	
	// #5 Configure the Run-Mode Clock Configuration register in the System
	// Control module to use the PWM divide (USEPWMDIV) and set the divider
	// (PWMDIV) to divide by 2 (000)
	SYSCTL_RCC_R |= SYSCTL_RCC_USEPWMDIV;
	SYSCTL_RCC_R |= SYSCTL_RCC_PWMDIV_2;
	
	// #6 Configure the PWM generator for countdown mode with immediate updates
	// to parameters
	// PWMn_X where 'n' is the module and 'X' is the generator
	// The PWM2 block produces the MnPWM4 output
	PWM0_2_CTL_R = 0x0;
	PWM0_2_GENA_R = PWM_2_GENA_ACTLOAD_ONE | PWM_2_GENA_ACTCMPAD_ZERO;
	PWM0_2_GENB_R = PWM_2_GENB_ACTLOAD_ONE | PWM_2_GENB_ACTCMPBD_ZERO;
	
	// #7 Set the Period
	
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
    PWMPulseWidthSet(PWM0_BASE, PWM_OUT_4, ui8Adjust * Load / 1000);
		PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true);
		PWM0_ENABLE_R |= 0x4;

}

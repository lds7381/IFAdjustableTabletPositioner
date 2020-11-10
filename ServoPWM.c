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
	volatile uint32_t ui32PWMClock = SysCtlClockGet() / 64;
	volatile uint32_t ui32Load = (ui32PWMClock / PWM_FREQ) - 1;
	
	// Enable PWM0 and port E for GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);	 
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	// Divides the system clock by 64 to run the clock at 625kHz which is 40Mhz/64.
	SysCtlPWMClockSet(SYSCTL_RCC_USEPWMDIV);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_2);
	
	// Enabling pin PE4 for PWM output
	GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);
	GPIOPinConfigure(GPIO_PCTL_PE4_M0PWM4);
	
	//Configures the PWM generator
	PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	//Sets the period of a PWM generator.
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Load);
	//Enables the timer/counter for a PWM generator block.
	PWMGenEnable(PWM0_BASE, PWM_GEN_2);
	
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

}

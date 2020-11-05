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

// Constants
#define PWM_FREQ 50
#define FLASH_BASE_ADDR ((volatile uint32_t*)0x00020000)
#define PROF_STRUCT_LEN 4

static uint16_t flashKey_ = 0;
uint32_t idnum[10];
uint32_t xpos[10];
uint32_t ypos[10];
uint32_t zpos[10];
uint32_t offset = 1;

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
	
	//UART Enable
	//UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	//IntEnable(INT_UART0); //enable the UART interrupt 
	//UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT); //only enable RX and TX interrupts
	//UARTEnable(UART0_BASE);
	
}

void InitPWMforServo(){
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM0);	 //Enables PWM0
	//SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD); //Enables port D for GPIO
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE); //Enables port E for GPIO
	
	// Enabling pin PE4 for PWM output
	GPIOPinTypePWM(GPIO_PORTE_BASE, GPIO_PIN_4);
	GPIOPinConfigure(GPIO_PCTL_PE4_M0PWM4);
	
	// Divides the system clock by 64 to run the clock at 625kHz which is 40Mhz/64.
	SysCtlPWMClockSet(SYSCTL_RCC_USEPWMDIV);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_2);

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
		//UARTCharPut(UART0_BASE, 'a');
}

//
// Enable Flash Memory Writing
//
void Flash_Enable(void){
	
	if (FLASH_BOOTCFG_R & 0x10) {
		flashKey_ = 0xA442;
	}
	else{
		flashKey_ = 0x71D5;
	}
	
}


//
// WRITE TO FLASH MEMORY
//
int Flash_Write(const void* data, int wordCount){
	
	if (flashKey_ == 0){
		return -1;
	}
	
	for (int i=0; i < wordCount; i++){
	
		FLASH_FMD_R = ((volatile uint32_t*)data)[i];
		FLASH_FMA_R &= 0xFFFC0000;
		FLASH_FMA_R |= (uint32_t)FLASH_BASE_ADDR + (offset * sizeof(uint32_t));
		offset++;
		
		FLASH_FMC2_R = (flashKey_ << 16) | 0x1;
		while( FLASH_FMC_R & 0x1) {}
		}
	
	return 0;
		
}

//
// READ FLASH MEMORY
//
void Flash_Read(void* data, int wordCount){
	
	for (int i = 0; i < wordCount; i++){
		((uint32_t*)data)[i] = FLASH_BASE_ADDR[i];
	}
	
}

void CreateNewStudentToFlash(){
	
	
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
	volatile uint32_t ui32Load;
	volatile uint32_t ui32PWMClock;
	uint8_t ui8Degrees;
	
	
	//UARTCharPut(UART0_BASE, 'a');
	MCInit(); // Initialize the board
	InitPWMforServo();
	//Configures module 1 PWM generator 0 as a down-counter and load the count value. 
	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQ) - 1;
	PWMGenConfigure(PWM0_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM0_BASE, PWM_GEN_2, ui32Load);
	
	//Enables PWM to run
	PWMOutputState(PWM0_BASE, PWM_OUT_4_BIT, true);								// to change with change the ajust/load (currently runs for 1.5ms)
	PWMGenEnable(PWM0_BASE, PWM_GEN_2);
	
	idnum[0] = 1234;
	xpos[0] = 10;
	ypos[0] = 15;
	zpos[0] = 32;
	CreateNewStudentToFlash();
	
	//Main Code
	while (1){
		
		
		
		position_servo(120, ui32Load);
	}
}







// INCLUDES
#include <stdint.h>
#include <stdio.h>
#include "inc/tm4c123gh6pm.h"
#include "ProfileFlash.h"
	
// GLOBALS
static uint16_t flashKey_ = 0;
uint32_t offset = 0;

//-----------------------------------------------------------------------------------------------------------------------------
// Methods
//-----------------------------------------------------------------------------------------------------------------------------

// Enable Flash Memory Writing
uint16_t Flash_Enable(void){
	
	if (FLASH_BOOTCFG_R & 0x10) {
		flashKey_ = 0xA442;
	}
	else{
		flashKey_ = 0x71D5;
	}
	
	return flashKey_;
	
}

// Erase the Flash Memory page containing the offset so it can be updated
int Flash_Erase_Offset(void){
	
	if (flashKey_ == 0){
		return -1;
	}
	
	FLASH_FMA_R &= 0xFFFC0000;
	FLASH_FMA_R |= ((uint32_t)OFFSET_FLASH_BASE_ADDR);
	FLASH_FMC_R = (flashKey_ << 16) | 0x2;
	while (FLASH_FMC_R & 0x2) {}
	
	return 0;
		
}

// Write the offset to Flash Memory
int Flash_Write_Offset(void){
	
	if (flashKey_ == 0){
		return -1;
	}
	
	//put the data to be written to flash in the FMD register
	//((volatile uint32_t*)offset)[0]
	FLASH_FMD_R = offset;
	//clear the FMA register while preserving reserved bits
	FLASH_FMA_R &= 0xFFFC0000;
	//set the FMA register to the desired target address
	FLASH_FMA_R |= (uint32_t)OFFSET_FLASH_BASE_ADDR;

	//Trigger a write operation and poll until successful
	FLASH_FMC_R = (flashKey_ << 16) | 0x1;
	while(FLASH_FMC_R & 0x1) {}
		
	return 0;
		
}

// Read the offset from Flash memory
uint32_t Flash_Read_Offset(){
	
	return OFFSET_FLASH_BASE_ADDR[0];
	
}


// WRITE TO FLASH MEMORY
int Flash_Write(const void* data, int wordCount){
	
	if (flashKey_ == 0){
		return -1;
	}
	offset = Flash_Read_Offset();
	
	for (int i=0; i < wordCount; i++){
	
		//put the data to be written to flash in the FMD register
		FLASH_FMD_R = ((volatile uint32_t*)data)[i];
		//clear the FMA register while preserving reserved bits
		FLASH_FMA_R &= 0xFFFC0000;
		//set the FMA register to the desired target address
		FLASH_FMA_R |= (uint32_t)FLASH_BASE_ADDR + (offset * sizeof(uint32_t));
		offset++;
		
		//Trigger a write operation and poll until successful
		FLASH_FMC_R = (flashKey_ << 16) | 0x1;
		while(FLASH_FMC_R & 0x1) {}
	}
	
	Flash_Erase_Offset();
	Flash_Write_Offset();
	
	return 0;
		
}


// READ FLASH MEMORY
void Flash_Read(void* data, int wordCount, uint32_t id){
	
	//search for ID number in flash memory
	int index = 0;
	offset = Flash_Read_Offset();
	for (int i = 0; i < offset; i+=4){
		if (FLASH_BASE_ADDR[i] == id){
			index = i;
		}
	}
	
	for (int i = 0; i < wordCount; i++){
		((uint32_t*)data)[i] = FLASH_BASE_ADDR[index + i];
	}
	
}


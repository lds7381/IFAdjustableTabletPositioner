#ifndef PROFILE_FLASH
#define PROFILE_FLASH

// DEFINES
#define FLASH_BASE_ADDR ((volatile uint32_t*)0x00020000)
#define OFFSET_FLASH_BASE_ADDR ((volatile uint32_t*)0x0001E000)
	
uint16_t Flash_Enable(void);
int Flash_Erase_Offset(void);
int Flash_Write_Offset(void);
uint32_t Flash_Read_Offset(void);
int Flash_Write(const void* data, int wordCount);
void Flash_Read(void* data, int wordCount, uint32_t id);

#endif
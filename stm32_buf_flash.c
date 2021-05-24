#include "stm32_buf_flash.h"

#define FLASH_BANK_NUMBER FLASH_BANK_1
#define BUF_SIZE PAGE_SIZE / 4 /* 1024 KB in 32-bit words */
#ifndef STALL_SAFE
#define STALL_SAFE true
#endif

/* MUST be aligned or CPU hard fault*/
static uint32_t flash_buf[BUF_SIZE] __attribute__((aligned)) = {0};

/* Reading or writing while Flash is busy
 * results in a hard stall.
 * Always check FLASH_SR for BSY bit(1) before
 * any operation if STALL_SAFE.
 */
volatile uint32_t* const FLASH_SR = 0x4002200C;

/* forward declarations */
bool __erase_flash_page(uint8_t page);
bool __write_flash_page(uint8_t page);
uint32_t __calc_offset_addr(uint8_t page);

/* fill the page buffer from given page number
 */

bool flash_buffer_fill(const uint8_t page) {
  if(page > PAGE_END) return false;
#if STALL_SAFE
  while(*FLASH_SR & 1);
#endif
  uint32_t start = FLASH_BASE_PAGE + (page * PAGE_SIZE);
  memcpy(flash_buf, (uint32_t *)(start), PAGE_SIZE);
  return true;
}

/* flush the page buffer to given page number
 * 
 * First erase the flash page, wait, check success
 * and then try to write the buffer to it.
 * The buffer is not cleared by this operation.
 */
bool flash_buffer_flush(const uint8_t page){
  if(page > PAGE_END) return false;

  if(false == __erase_flash_page(page))
    return false;

  if (true == __write_flash_page(page))
    return true;
    
  return false;
  
}

const uint32_t* flash_buffer_handle(){
  return &flash_buf;
}

const uint32_t flash_buffer_size() {
  return BUF_SIZE;
}

uint32_t flash_read_word(const uint16_t pos){
  if(pos>BUF_SIZE-1) return false;
  return flash_buf[pos];
}

bool flash_write_word(const uint16_t pos, const uint32_t value){
  if(pos>BUF_SIZE-1) return false;
  flash_buf[pos] = value;
  return true;
}

/* calculate the start address for a particular page */
inline uint32_t __calc_offset_addr(uint8_t page) {
  return (uint32_t) FLASH_BASE_PAGE + (page * PAGE_SIZE);
}
bool __erase_flash_page(uint8_t page){
  FLASH_EraseInitTypeDef EraseInitStruct;
  uint32_t address      = __calc_offset_addr(page);
  uint32_t address_end  = address + PAGE_SIZE - 1;
  uint32_t pageError    = 0;
  uint64_t data         = 0;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
  EraseInitStruct.Banks = FLASH_BANK_NUMBER;
  EraseInitStruct.PageAddress = address;
  EraseInitStruct.NbPages = 1;
  if(HAL_FLASH_Unlock() == HAL_OK){
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|FLASH_FLAG_PGERR);
  if(HAL_FLASHEx_Erase(&EraseInitStruct, &pageError) == HAL_OK) {
    HAL_FLASH_Lock();
    return true;
   }
  }
  return false;
}


bool __write_flash_page(uint8_t page){
  uint16_t  offset       = 0;
  uint32_t address      = __calc_offset_addr(page);
  uint32_t address_end  = address + PAGE_SIZE - 1;
  uint32_t data = 0;
  if(HAL_FLASH_Unlock() == HAL_OK) {
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR|FLASH_FLAG_PGERR);
  while(address <= address_end) {
    data = flash_buf[offset];
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data) == HAL_OK){
    address += 4;
    offset++;  
    } else {
      /* abort on error */
      address  = address_end + 1;
      HAL_FLASH_Lock();
      return false;
    }
  }
  HAL_FLASH_Lock();
  return true;
  }
  return false;
}

/* (STM32F10xxx 128K Flash specific)
 * This library exposes a buffered API for doing Flash I/O
 * with more control than Arduino's EEPROM API.
 * 
 * The buffer for a page holds 256 x 32-bit word (1 KiB)
 * and is either flushed to or filled from a specific
 * physical Flash memory page.
 * 
 * The constants defined make it simple to adapt this to program-
 * specific constraints or quirks.
 * 
 * This API is limited to 1K pages and uses a uint16_t for the buffer position.
 * Fix this and buffer size if there is an interest in running on
 * high-density STM32F10xxx devices with 256K Flash that use 2K pages.
 * TODO: GD32F30xxx devices with >512K Flash
 * have two banks with differing page sizes (2K vs 4K)
 * which requires more complex handling
*/

#include "stm32_def.h"
#include <stdbool.h>
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#ifndef FLASH_START
#define FLASH_START 0x08000000
#endif

/* STM32F10xxx 128K flash pages are 1K.
 * For high-density, use 2K
 */
#ifndef PAGE_SIZE
#define PAGE_SIZE 0x400
#endif

/* Default to page 0 unless user redefined.
 * The first 1K page in flash on medium density STM32F1XX
 * Note: This is where your arduino code lives. Tread carefully.
*/
#ifndef PAGE_START
#define PAGE_START 0 
#endif

/* The final page id
 * Note: devices other than medium-density will
 * have a greater or smaller page count.
 * Refer to PM0075 'STM32F10xxx Flash memory microcontrollers'
 */
#define PAGE_END 127

/* calculate the address of the first page */	
#ifndef FLASH_BASE_PAGE
#define FLASH_BASE_PAGE FLASH_START + (PAGE_START * PAGE_SIZE)
#endif

/* The API only supports 32-bit word values currently.
 * Either pack your smaller values or expand the API.
 */
	bool            flash_buffer_fill(const uint8_t page);
	bool            flash_buffer_flush(const uint8_t page);
  const uint32_t*       flash_buffer_handle();
  const uint32_t        flash_buffer_size();
	uint32_t        flash_read_word(const uint16_t pos);
	bool            flash_write_word(const uint16_t pos, const uint32_t value);
#ifdef __cplusplus
		}
#endif

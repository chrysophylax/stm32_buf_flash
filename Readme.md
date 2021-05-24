
A very simple STM32F1xx focused library for reading and writing to the flash
memory. AFAICT, there is no simple library out there that lets you control
which page you are writing to and is buffered.


As flash memory can only be written from high to low, it always requires a page erase before writing new values to the memory cells. To prevent wearing out the flash memory too fast, we buffer it in an array representing one page.


To include and use the API, drop the .h and .c file into your project and add
```Arduino
#include "stm32_buf_flash.h"
```


For extra clarity, I recommend checking out memory-stm32.ino which is a test file that exercises the API.


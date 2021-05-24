#line 2 "memory-stm32.ino"

#include <AUnit.h>
using namespace aunit;
#include "stm32_buf_flash.h"


test(basicWriteRead) {
  uint8_t page = 120;
  uint32_t word = 0xCAFEBABE;
  assertTrue(flash_write_word(1, word));
  assertTrue(flash_buffer_flush(page));
  assertTrue(flash_buffer_fill(page));
  assertEqual(word, flash_read_word(1));
}
test(bounds) {
  assertFalse(flash_write_word(flash_buffer_size()+1, 0xFF));
  assertFalse(flash_buffer_flush(255));
  assertFalse(flash_buffer_fill(255));
}
test(reflash) {
  uint8_t page = 121;
  for(int i=0;i<flash_buffer_size();i++){
    flash_write_word((uint16_t)i, (uint32_t)0xDECAFBAD);
  }
  flash_buffer_flush(page);
  flash_write_word((uint16_t)255, (uint32_t)0xDEADBEEF);
  flash_buffer_flush(page);
  flash_buffer_fill(page);
  assertEqual((uint32_t)0xDEADBEEF, flash_read_word(255));
  assertEqual((uint32_t)0xDECAFBAD, flash_read_word(254));
}
void setup() {
  Serial.begin(115200);
  while(!Serial){;}
  Serial.print("Base flash address: 0x");Serial.println(FLASH_BASE_PAGE, HEX);
  Serial.print("Page size: 0x");Serial.println(PAGE_SIZE,HEX);
  Serial.println("Running tests...");
  TestRunner::setVerbosity(Verbosity::kAll);
}

void loop() {
  aunit::TestRunner::run();  
}

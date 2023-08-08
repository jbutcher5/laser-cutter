#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC 0x11

uint8_t *transmition(const char *s) {
  // Final buffer to be returned at the end
  static uint8_t buffer[256];
  buffer[0] = strlen(s)/2;
  
  // Create null-terminated buffer
  char tmp[3];
  tmp[2] = 0;

  // Read data into buffer
  for (int i = 0; i < strlen(s); i+=2) {
    int buff_index = (i / 2) + 1;

    tmp[0] = s[i];
    tmp[1] = s[i + 1];

    buffer[buff_index] = strtol(tmp, 0, 16);
  }

  return buffer;
}

uint8_t descramble(uint8_t s) {
  /* Found on https://stefan.schuermans.info/rdcam/scrambling.html
   * For descrambling bytes.
   */
  uint8_t a = (s + 0xFF) & 0xFF;
  uint8_t b = a ^ MAGIC;
  uint8_t p = b & 0x7E | b >> 7 & 0x01 | b << 7 & 0x80;
  return p;
}

void descramble_buffer(uint8_t *buff) {
  for (int i = 1; i < buff[0]; i++) {
    buff[i] = descramble(buff[i]);
  }
}

int main() {
  uint8_t *b = transmition("4b9296701832925a12");

  descramble_buffer(b);

  for (int i = 1; i < b[0]; i++) {
    printf("0x%x\n", b[i]);
  }
  
  return 0;
}

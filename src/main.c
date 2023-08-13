#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC 0x11

typedef struct {
  uint8_t hh;
  uint8_t h;
  uint8_t m;
  uint8_t l;
  uint8_t ll;
} ABSCOORD;

uint64_t abscoord_to_int(ABSCOORD *x) {
  uint64_t buffer = 0;

  for (int i = 0, k = 28; i < 5; i++, k -= 7)
    buffer |= *(((uint8_t *)x) + i) << k;

  return buffer;
}

uint8_t *transmition(const char *s) {
  // Final buffer to be returned at the end
  static uint8_t buffer[256];
  buffer[0] = strlen(s) / 2;

  // Create null-terminated buffer
  char tmp[3];
  tmp[2] = 0;

  // Read data into buffer
  for (int i = 0; i < strlen(s); i += 2) {
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

typedef struct {
  uint8_t *buffer;
  uint64_t size;
  uint8_t *curr;
  uint8_t *next;
} ParserContext;

typedef struct {
  uint8_t *buffer;
  uint64_t size;
} Message;

ParserContext *
new_parser_ctx(uint8_t *buffer, uint64_t size) {
  ParserContext *ctx = malloc(sizeof(ParserContext));
  *ctx = (ParserContext){buffer, size, 0, 0};

  return ctx;
}

Message next_message(ParserContext *p) {
  uint8_t *curr = p->curr;
  uint8_t *next = p->next;
  Message message = {0, 0};

  if (!curr) {
    curr = p->buffer;

    for (uint8_t *byte = curr + 1;
         byte < (p->buffer + p->size) || !(*byte & 128); byte++, message.size++) {

      if (byte >= (p->buffer + p->size)) {
	next = 0;
	break;
      }

      next = byte;
    }
  }

  p->curr = curr;
  p->next = next;

  message.buffer = curr;

  return message;
}

int main(void) {
  uint8_t x[6] = {
    0x80, 0x00, 0x00, 0x02, 0x3C, 0x02,
  };

  ParserContext *ctx = new_parser_ctx((uint8_t *)&x, 6);

  Message m = next_message(ctx);
  
  return 0;
}

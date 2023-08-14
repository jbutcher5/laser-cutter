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

uint8_t descramble(uint8_t s) {
  /* Found on https://stefan.schuermans.info/rdcam/scrambling.html
   * For descrambling bytes.
   */
  uint8_t a = (s + 0xFF) & 0xFF;
  uint8_t b = a ^ MAGIC;
  uint8_t p = b & 0x7E | b >> 7 & 0x01 | b << 7 & 0x80;
  return p;
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

ParserContext *new_parser_ctx(uint8_t *buffer, uint64_t size) {
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
         byte < (p->buffer + p->size) || !(*byte & 128);
         byte++, message.size++) {

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
  uint8_t x[7] = {
      0x80, 0x00, 0x00, 0x02, 0x3C, 0x02, 0x80
  };

  ParserContext *ctx = new_parser_ctx((uint8_t *)&x, 6);

  Message m = next_message(ctx);

  printf("%" PRIu64 "\n", m.size);

  return 0;
}

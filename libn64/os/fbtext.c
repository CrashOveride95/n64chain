//
// libn64/os/fbtext.c: Framebuffer text routines.
//
// n64chain: A (free) open-source N64 development toolchain.
// Copyright 2014-16 Tyler J. Stachecki <stachecki.tyler@gmail.com>
//
// This file is subject to the terms and conditions defined in
// 'LICENSE', which is part of this source code package.
//

#include <fbtext.h>
#include <libn64.h>
#include <stdint.h>

// Methods for rendering a character to a 16 or 32-bit RGBA framebuffer.
// These methods do not handle escape characters and do not advance x/y.
// They are strictly intended to render a character.
libn64func
static inline const uint8_t *get_font_data(char c);

libn64func
static unsigned libn64_fbchar16(const struct libn64_fbtext_context *context,
    uint32_t fb_address, char c);

libn64func
static unsigned libn64_fbchar32(const struct libn64_fbtext_context *context,
    uint32_t fb_address, char c);

// 95 member font table; starts with the ' ' char.
// This blob is licensed under the public domain.
const uint8_t *get_font_data(char c) {
  static const uint8_t libn64_font_table[] = {
    // <space>
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '!'
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x00,0x00,0x10,0x10,0x00,0x00,
  
    // '"'
    0x24,0x24,0x24,0x24,0x24,0x24,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '#'
    0x24,0x24,0x24,0x24,0x7E,0x7E,0x24,0x24,
    0x7E,0x7E,0x24,0x24,0x24,0x24,0x00,0x00,
  
    // '$'
    0x10,0x10,0x3C,0x3C,0x50,0x50,0x38,0x38,
    0x14,0x14,0x78,0x78,0x10,0x10,0x00,0x00,
  
    // '%'
    0x00,0x00,0x62,0x62,0x64,0x64,0x08,0x08,
    0x10,0x10,0x26,0x26,0x46,0x46,0x00,0x00,
  
    // '&'
    0x30,0x30,0x48,0x48,0x48,0x48,0x30,0x30,
    0x4A,0x4A,0x44,0x44,0x3A,0x3A,0x00,0x00,
  
    // '''
    0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '('
    0x10,0x10,0x20,0x20,0x40,0x40,0x40,0x40,
    0x40,0x40,0x20,0x20,0x10,0x10,0x00,0x00,
  
    // ')'
    0x10,0x10,0x08,0x08,0x04,0x04,0x04,0x04,
    0x04,0x04,0x08,0x08,0x10,0x10,0x00,0x00,
  
    // '*'
    0x10,0x10,0x54,0x54,0x38,0x38,0x10,0x10,
    0x38,0x38,0x54,0x54,0x10,0x10,0x00,0x00,
  
    // '+'
    0x00,0x00,0x10,0x10,0x10,0x10,0x7C,0x7C,
    0x10,0x10,0x10,0x10,0x00,0x00,0x00,0x00,
  
    // ','
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x08,0x08,0x08,0x08,0x10,0x10,0x00,0x00,
  
    // '-'
    0x00,0x00,0x00,0x00,0x00,0x00,0x7E,0x7E,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '.'
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x10,0x10,0x00,0x00,
  
    // '/'
    0x00,0x00,0x02,0x02,0x04,0x04,0x08,0x08,
    0x10,0x10,0x20,0x20,0x40,0x40,0x00,0x00,
  
    // '0'
    0x3C,0x3C,0x42,0x42,0x46,0x46,0x5A,0x5A,
    0x62,0x62,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // '1'
    0x08,0x08,0x18,0x18,0x08,0x08,0x08,0x08,
    0x08,0x08,0x08,0x08,0x1C,0x1C,0x00,0x00,
  
    // '2'
    0x3C,0x3C,0x42,0x42,0x02,0x02,0x1C,0x1C,
    0x20,0x20,0x40,0x40,0x7E,0x7E,0x00,0x00,
  
    // '3'
    0x7E,0x7E,0x02,0x02,0x04,0x04,0x1C,0x1C,
    0x02,0x02,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // '4'
    0x04,0x04,0x0C,0x0C,0x14,0x14,0x24,0x24,
    0x7E,0x7E,0x04,0x04,0x04,0x04,0x00,0x00,
  
    // '5'
    0x7E,0x7E,0x40,0x40,0x7C,0x7C,0x02,0x02,
    0x02,0x02,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // '6'
    0x1E,0x1E,0x20,0x20,0x40,0x40,0x7C,0x7C,
    0x42,0x42,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // '7'
    0x7E,0x7E,0x02,0x02,0x04,0x04,0x08,0x08,
    0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
  
    // '8'
    0x3C,0x3C,0x42,0x42,0x42,0x42,0x3C,0x3C,
    0x42,0x42,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // '9'
    0x3C,0x3C,0x42,0x42,0x42,0x42,0x3E,0x3E,
    0x02,0x02,0x04,0x04,0x78,0x78,0x00,0x00,
  
    // ':'
    0x00,0x00,0x00,0x00,0x10,0x10,0x00,0x00,
    0x10,0x10,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // ';'
    0x00,0x00,0x00,0x00,0x08,0x08,0x00,0x00,
    0x08,0x08,0x08,0x08,0x10,0x00,0x00,0x00,
  
    // '<'
    0x04,0x04,0x08,0x08,0x10,0x10,0x20,0x20,
    0x10,0x10,0x08,0x08,0x04,0x04,0x00,0x00,
  
    // '='
    0x00,0x00,0x00,0x00,0x7E,0x7E,0x00,0x00,
    0x7E,0x7E,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '>'
    0x20,0x20,0x10,0x10,0x08,0x08,0x04,0x04,
    0x08,0x08,0x10,0x10,0x20,0x20,0x00,0x00,
  
    // '?'
    0x20,0x20,0x40,0x40,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '@'
    0x3C,0x3C,0x42,0x42,0x4A,0x4A,0x56,0x56,
    0x4C,0x4C,0x40,0x40,0x3E,0x3E,0x00,0x00,
  
    // 'A'
    0x18,0x18,0x24,0x24,0x42,0x42,0x42,0x42,
    0x7E,0x7E,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'B'
    0x7C,0x7C,0x42,0x42,0x42,0x42,0x7C,0x7C,
    0x42,0x42,0x42,0x42,0x7C,0x7C,0x00,0x00,
  
    // 'C'
    0x3C,0x3C,0x42,0x42,0x40,0x40,0x40,0x40,
    0x40,0x40,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'D'
    0x7C,0x7C,0x42,0x42,0x42,0x42,0x42,0x42,
    0x42,0x42,0x42,0x42,0x7C,0x7C,0x00,0x00,
  
    // 'E'
    0x7E,0x7E,0x40,0x40,0x40,0x40,0x7C,0x7C,
    0x40,0x40,0x40,0x40,0x7E,0x7E,0x00,0x00,
  
    // 'F'
    0x7E,0x7E,0x40,0x40,0x40,0x40,0x7C,0x7C,
    0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,
  
    // 'G'
    0x3E,0x3E,0x40,0x40,0x40,0x40,0x40,0x4E,
    0x4E,0x42,0x42,0x42,0x3E,0x3E,0x00,0x00,
  
    // 'H'
    0x42,0x42,0x42,0x42,0x42,0x42,0x7E,0x7E,
    0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'I'
    0x38,0x38,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x38,0x38,0x00,0x00,
  
    // 'J'
    0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,
    0x02,0x02,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'K'
    0x42,0x42,0x44,0x44,0x48,0x48,0x70,0x70,
    0x48,0x48,0x44,0x44,0x42,0x42,0x00,0x00,
  
    // 'L'
    0x40,0x40,0x40,0x40,0x40,0x40,0x40,0x40,
    0x40,0x40,0x40,0x40,0x7E,0x7E,0x00,0x00,
  
    // 'M'
    0x42,0x42,0x66,0x66,0x5A,0x5A,0x5A,0x5A,
    0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'N'
    0x42,0x42,0x62,0x62,0x52,0x52,0x5A,0x5A,
    0x4A,0x4A,0x46,0x46,0x42,0x42,0x00,0x00,
  
    // 'O'
    0x3C,0x3C,0x42,0x42,0x42,0x42,0x42,0x42,
    0x42,0x42,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'P'
    0x7C,0x7C,0x42,0x42,0x42,0x42,0x7C,0x7C,
    0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,
  
    // 'Q'
    0x3C,0x3C,0x42,0x42,0x42,0x42,0x42,0x42,
    0x4A,0x4A,0x44,0x44,0x3A,0x3A,0x00,0x00,
  
    // 'R'
    0x7C,0x7C,0x42,0x42,0x42,0x42,0x7C,0x7C,
    0x48,0x48,0x44,0x44,0x42,0x42,0x00,0x00,
  
    // 'S'
    0x3C,0x3C,0x42,0x42,0x40,0x40,0x3C,0x3C,
    0x02,0x02,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'T'
    0x7C,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
  
    // 'U'
    0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
    0x42,0x42,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'V'
    0x42,0x42,0x42,0x42,0x42,0x42,0x42,0x42,
    0x42,0x42,0x24,0x24,0x18,0x18,0x00,0x00,
  
    // 'W'
    0x42,0x42,0x42,0x42,0x42,0x42,0x5A,0x5A,
    0x5A,0x5A,0x66,0x66,0x42,0x42,0x00,0x00,
  
    // 'X'
    0x42,0x42,0x42,0x42,0x24,0x24,0x18,0x18,
    0x24,0x24,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'Y'
    0x44,0x44,0x44,0x44,0x28,0x28,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
  
    // 'Z'
    0x7E,0x7E,0x02,0x02,0x04,0x04,0x18,0x18,
    0x20,0x20,0x40,0x40,0x7E,0x7E,0x00,0x00,
  
    // '['
    0x7E,0x7E,0x60,0x60,0x60,0x60,0x60,0x60,
    0x60,0x60,0x60,0x60,0x7E,0x7E,0x00,0x00,
  
    // <backslash>
    0x00,0x00,0x40,0x40,0x20,0x20,0x10,0x10,
    0x08,0x08,0x04,0x04,0x02,0x02,0x00,0x00,
  
    // ']'
    0x7E,0x7E,0x06,0x06,0x06,0x06,0x06,0x06,
    0x06,0x06,0x06,0x06,0x7E,0x7E,0x00,0x00,
  
    // '^'
    0x00,0x00,0x00,0x00,0x10,0x10,0x28,0x28,
    0x44,0x44,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // '_'
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x7E,0x7E,0x00,0x00,
  
    // '`'
    0x20,0x20,0x10,0x10,0x08,0x08,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  
    // 'a'
    0x00,0x00,0x00,0x00,0x3C,0x3C,0x02,0x02,
    0x3E,0x3E,0x42,0x42,0x3E,0x3E,0x00,0x00,
  
    // 'b'
    0x40,0x40,0x40,0x40,0x7C,0x7C,0x42,0x42,
    0x42,0x42,0x42,0x42,0x7C,0x7C,0x00,0x00,
  
    // 'c'
    0x00,0x00,0x00,0x00,0x3E,0x3E,0x40,0x40,
    0x40,0x40,0x40,0x40,0x3E,0x3E,0x00,0x00,
  
    // 'd'
    0x02,0x02,0x02,0x02,0x3E,0x3E,0x42,0x42,
    0x42,0x42,0x42,0x42,0x3E,0x3E,0x00,0x00,
  
    // 'e'
    0x00,0x00,0x00,0x00,0x3C,0x3C,0x42,0x42,
    0x7E,0x7E,0x40,0x40,0x3E,0x3E,0x00,0x00,
  
    // 'f'
    0x1C,0x1C,0x22,0x22,0x20,0x20,0x7C,0x7C,
    0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,
  
    // 'g'
    0x00,0x00,0x00,0x00,0x3C,0x3C,0x42,0x42,
    0x42,0x42,0x3E,0x3E,0x02,0x02,0x3C,0x3C,
  
    // 'h'
    0x40,0x40,0x40,0x40,0x7C,0x7C,0x42,0x42,
    0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'i'
    0x10,0x10,0x00,0x00,0x30,0x30,0x10,0x10,
    0x10,0x10,0x10,0x10,0x38,0x38,0x00,0x00,
  
    // 'j'
    0x04,0x04,0x00,0x00,0x3C,0x3C,0x04,0x04,
    0x04,0x04,0x04,0x04,0x44,0x44,0x38,0x38,
  
    // 'k'
    0x40,0x40,0x40,0x40,0x42,0x42,0x44,0x44,
    0x78,0x78,0x44,0x44,0x42,0x42,0x00,0x00,
  
    // 'l'
    0x30,0x30,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x38,0x38,0x00,0x00,
  
    // 'm'
    0x00,0x00,0x00,0x00,0x66,0x66,0x5A,0x5A,
    0x5A,0x5A,0x5A,0x5A,0x42,0x42,0x00,0x00,
  
    // 'n'
    0x00,0x00,0x00,0x00,0x7C,0x7C,0x42,0x42,
    0x42,0x42,0x42,0x42,0x42,0x42,0x00,0x00,
  
    // 'o'
    0x00,0x00,0x00,0x00,0x3C,0x3C,0x42,0x42,
    0x42,0x42,0x42,0x42,0x3C,0x3C,0x00,0x00,
  
    // 'p'
    0x00,0x00,0x00,0x00,0x7C,0x7C,0x42,0x42,
    0x42,0x42,0x7C,0x7C,0x40,0x40,0x40,0x40,
  
    // 'q'
    0x00,0x00,0x00,0x00,0x3E,0x3E,0x42,0x42,
    0x42,0x42,0x3E,0x3E,0x02,0x02,0x02,0x02,
  
    // 'r'
    0x00,0x00,0x00,0x00,0x5E,0x5E,0x60,0x60,
    0x40,0x40,0x40,0x40,0x40,0x40,0x00,0x00,
  
    // 's'
    0x00,0x00,0x00,0x00,0x3E,0x3E,0x40,0x40,
    0x3C,0x3C,0x02,0x02,0x7C,0x7C,0x00,0x00,
  
    // 't'
    0x10,0x10,0x10,0x10,0x7C,0x7C,0x10,0x10,
    0x10,0x10,0x12,0x12,0x0C,0x0C,0x00,0x00,
  
    // 'u'
    0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,
    0x42,0x42,0x46,0x46,0x3A,0x3A,0x00,0x00,
  
    // 'v'
    0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,
    0x42,0x42,0x24,0x24,0x18,0x18,0x00,0x00,
  
    // 'w'
    0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,
    0x5A,0x5A,0x5A,0x5A,0x66,0x66,0x00,0x00,
  
    // 'x'
    0x00,0x00,0x00,0x00,0x42,0x42,0x24,0x24,
    0x18,0x18,0x24,0x24,0x42,0x42,0x00,0x00,
  
    // 'y'
    0x00,0x00,0x00,0x00,0x42,0x42,0x42,0x42,
    0x42,0x42,0x3E,0x3E,0x02,0x02,0x3C,0x3C,
  
    // 'z'
    0x00,0x00,0x00,0x00,0x7E,0x7E,0x04,0x04,
    0x18,0x18,0x20,0x20,0x7E,0x7E,0x00,0x00,
  
    // '{'
    0x0E,0x0E,0x18,0x18,0x18,0x18,0x70,0x70,
    0x18,0x18,0x18,0x18,0x0E,0x0E,0x00,0x00,
  
    // '|'
    0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,
    0x10,0x10,0x10,0x10,0x10,0x10,0x00,0x00,
  
    // '}'
    0x70,0x70,0x18,0x18,0x18,0x18,0x0E,0x0E,
    0x18,0x18,0x18,0x18,0x70,0x70,0x00,0x00,
  
    // '~'
    0x00,0x00,0x00,0x00,0x24,0x24,0x54,0x54,
    0x48,0x48,0x00,0x00,0x00,0x00,0x00,0x00,
  };
  
  return libn64_font_table + (c << 4) - (' ' << 4);
}

unsigned libn64_fbchar16(const struct libn64_fbtext_context *context,
    uint32_t fb_address, char c) {
  const uint8_t *font = get_font_data(c);
  unsigned i, j;

  fb_address += (context->x << 4);

  for (i = 0; i < 16; i++) {
    uint8_t bitmask = font[i];

    // Flush line contents if valid; otherwise flag it dirty.
    // This prevents an otherwise redundant read from memory.
    __asm__ __volatile__(
      "cache 0xD, 0x0(%0)\n\t"
      :: "r"(fb_address)
    );

    for (fb_address += 8 << 1, j = 0; j < 8; j++) {
      unsigned bit = (bitmask >> j) & 0x1;
      uint16_t color = context->colors[bit];

      __asm__ __volatile__(
        "sh %0, -2(%1)\n\t"
        :: "r"(color), "r"(fb_address)
        : "memory"
      );

      fb_address -= 2;
    }

    // Ensure the line gets written to memory.
    __asm__ __volatile__(
      "cache 0x19, 0x0(%0)\n\t"
      :: "r"(fb_address)
    );

    fb_address += context->fb_width;
  }

  return 1;
}

unsigned libn64_fbchar32(const struct libn64_fbtext_context *context,
    uint32_t fb_address, char c) {
  const uint8_t *font = get_font_data(c);
  unsigned i, j;

  fb_address += (context->x << 5);

  for (i = 0; i < 16; i++) {
    uint8_t bitmask = font[i];

    // Flush line contents if valid; otherwise flag it dirty.
    // This prevents an otherwise redundant read from memory.
    __asm__ __volatile__(
      "cache 0xD, 0x0(%0)\n\t"
      "cache 0xD, 0x10(%0)\n\t"
      :: "r"(fb_address)
    );

    for (fb_address += 8 << 2, j = 0; j < 8; j ++) {
      unsigned bit = (bitmask >> j) & 0x1;
      uint16_t color = context->colors[bit];

      __asm__ __volatile__(
        "sw %0, -4(%1)\n\t"
        :: "r"(color), "r"(fb_address)
        : "memory"
      );

      fb_address -= 4;
    }

    // Ensure the line gets written to memory.
    __asm__ __volatile__(
      "cache 0x19, 0x0(%0)\n\t"
      "cache 0x19, 0x10(%0)\n\t"
      :: "r"(fb_address)
    );


    fb_address += context->fb_width;
  }

  return 2;
}

void libn64_fbtext_init(struct libn64_fbtext_context *context,
    uint32_t fb_origin, uint32_t fg_color, uint32_t bg_color,
    uint16_t fb_width, enum libn64_fbtext_mode mode) {
  context->colors[LIBN64_FBTEXT_COLOR_BG] = bg_color;
  context->colors[LIBN64_FBTEXT_COLOR_FG] = fg_color;

  context->fb_origin = fb_origin | 0x80000000;
  context->x = 0;
  context->y = 0;

  if (mode == LIBN64_FBTEXT_16BPP) {
    context->render_char = libn64_fbchar16;
    context->fb_width = fb_width << 1;
  } else {
    context->render_char = libn64_fbchar32;
    context->fb_width = fb_width << 2;
  }
}

void libn64_fbtext_puts(struct libn64_fbtext_context *context,
    const char *string) {
  uint32_t fb_address;
  unsigned i;

  for (i = 0; string[i] != '\0'; i++) {
    unsigned mode;

    switch (string[i]) {
      case '\n':
        context->x = 0;
        context->y++;
        continue;

      default:
        break;
    }

    fb_address = context->fb_origin + context->fb_width * (context->y << 4);
    mode = context->render_char(context, fb_address, string[i]);
    context->x++;

    if ((context->x + 1) > context->fb_width / (8 << mode)) {
      context->x = 0;
      context->y++;
    }
  }
}

void libn64_fbtext_putu32(struct libn64_fbtext_context *context,
    uint32_t u32) {
  char string[9];
  int i;

  for (string[8] = '\0', i = 7; i >= 0; i--) {
    uint8_t byte = u32 & 0xF;

    string[i] = byte + (byte < 0xA ? '0' : ('A' - 10));
    u32 >>= 4;
  }

  libn64_fbtext_puts(context, string);
}


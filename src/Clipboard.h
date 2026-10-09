#pragma once

#include <stddef.h>
#include <stdint.h>

// Custom clipboard protocol version and permissions advertised in serverinfo.
#define SS_CLIPBOARD_VERSION 1
#define SS_CLIPBOARD_HOST_TO_CLIENT 1
#define SS_CLIPBOARD_CLIENT_TO_HOST 2
#ifndef SS_CLIPBOARD_TEXT_MAX
#define SS_CLIPBOARD_TEXT_MAX 32755
#endif

// Validate a whole Unicode scalar sequence, rejecting embedded NUL, overlong
// encodings, surrogate code points, truncation, and values above U+10FFFF.
// Empty text is reserved for subscription; it does not clear either clipboard.
static inline int SsClipboardTextValid(const char* text, size_t length) {
    size_t i = 0;
    if (length > SS_CLIPBOARD_TEXT_MAX || (length != 0 && text == NULL)) {
        return 0;
    }
    while (i < length) {
        uint32_t cp;
        uint32_t minimum;
        unsigned int remaining;
        unsigned char first = (unsigned char)text[i++];
        if (first == 0) return 0;
        if (first < 0x80) continue;
        if (first >= 0xC2 && first <= 0xDF) {
            cp = first & 0x1F; minimum = 0x80; remaining = 1;
        }
        else if (first >= 0xE0 && first <= 0xEF) {
            cp = first & 0x0F; minimum = 0x800; remaining = 2;
        }
        else if (first >= 0xF0 && first <= 0xF4) {
            cp = first & 0x07; minimum = 0x10000; remaining = 3;
        }
        else return 0;
        if (remaining > length - i) return 0;
        while (remaining-- != 0) {
            unsigned char next = (unsigned char)text[i++];
            if ((next & 0xC0) != 0x80) return 0;
            cp = (cp << 6) | (next & 0x3F);
        }
        if (cp < minimum || cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) return 0;
    }
    return 1;
}

// Decode the host notification payload (token LE32, length LE32, text).
// Only an exact, nonempty payload is accepted. No allocation is performed.
static inline int SsClipboardDecode(const unsigned char* payload, size_t size,
                                    uint32_t* token, const char** text, uint32_t* length) {
    uint32_t n;
    if (payload == NULL || size < 8) return 0;
    n = (uint32_t)payload[4] | ((uint32_t)payload[5] << 8) |
        ((uint32_t)payload[6] << 16) | ((uint32_t)payload[7] << 24);
    if (n == 0 || n != size - 8 || !SsClipboardTextValid((const char*)payload + 8, n)) return 0;
    *token = (uint32_t)payload[0] | ((uint32_t)payload[1] << 8) |
             ((uint32_t)payload[2] << 16) | ((uint32_t)payload[3] << 24);
    *length = n;
    *text = (const char*)payload + 8;
    return 1;
}

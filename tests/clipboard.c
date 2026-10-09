#include "Clipboard.h"
#include "Input.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int main(void) {
    char* large = malloc(SS_CLIPBOARD_TEXT_MAX + 1);
    unsigned char packet[] = {0x78,0x56,0x34,0x12,3,0,0,0,0xE6,0x97,0xA5};
    uint32_t token, length;
    const char* text;
    const char* bad[] = {"\x80", "\xC0\xAF", "\xE0\x80\xAF", "\xED\xA0\x80",
                         "\xF4\x90\x80\x80", "\xF5\x80\x80\x80", "\xF0\x9F\x98", "\xC2x"};
    size_t i;
    assert(offsetof(SS_CLIPBOARD_PACKET, text) == 12);
    assert(SsClipboardTextValid(NULL, 0));
    assert(!SsClipboardTextValid(NULL, 1));
    assert(SsClipboardTextValid("ASCII\r\n", 7));
    assert(SsClipboardTextValid("\xE6\x97\xA5\xE6\x9C\xAC\xF0\x9F\x98\x80", 10));
    assert(!SsClipboardTextValid("a\0b", 3));
    for (i = 0; i < sizeof(bad)/sizeof(bad[0]); i++) assert(!SsClipboardTextValid(bad[i], strlen(bad[i])));
    memset(large, 'a', SS_CLIPBOARD_TEXT_MAX + 1);
    assert(SsClipboardTextValid(large, SS_CLIPBOARD_TEXT_MAX));
    assert(!SsClipboardTextValid(large, SS_CLIPBOARD_TEXT_MAX + 1));
    assert(SsClipboardDecode(packet, sizeof(packet), &token, &text, &length));
    assert(token == 0x12345678 && length == 3 && memcmp(text, packet+8, 3) == 0);
    for (i = 0; i < sizeof(packet); i++) assert(!SsClipboardDecode(packet, i, &token, &text, &length));
    packet[4] = 2;
    assert(!SsClipboardDecode(packet, sizeof(packet), &token, &text, &length));
    packet[4] = 0;
    assert(!SsClipboardDecode(packet, 8, &token, &text, &length));
    packet[4] = 3; packet[10] = 0;
    assert(!SsClipboardDecode(packet, sizeof(packet), &token, &text, &length));
    free(large);
    puts("Clipboard protocol validation passed");
    return 0;
}

#include "urlcoder.h"

uintptr_t urlencode(u_char *dst, u_char *src, size_t size, unsigned int type)
{
    unsigned int n;
    uint32_t *escape;
    static u_char hex[] = "0123456789ABCDEF";
    /* " ", "#", "%", "?", %00-%1F, %7F-%FF */
    static uint32_t uri[] = {
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */

        /* ?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"! */
        0xfc00886d, /* 1111 1100 0000 0000 1000 1000 0110 1101 */

        /* _^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@ */
        0x78000000, /* 0111 1000 0000 0000 0000 0000 0000 0000 */

        /* ~}| {zyx wvut srqp onml kjih gfed cba` */
        0xa8000000, /* 1010 1000 0000 0000 0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff /* 1111 1111 1111 1111 1111 1111 1111 1111 */
    };
    /* " ", "#", "%", "+", "?", %00-%1F, %7F-%FF */
    static uint32_t args[] = {
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */

        /* ?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"! */
        0x80000829, /* 1000 0000 0000 0000 0000 1000 0010 1001 */

        /* _^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */

        /* ~}| {zyx wvut srqp onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000 0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff /* 1111 1111 1111 1111 1111 1111 1111 1111 */
    };
    /* " ", "#", """, "%", "'", %00-%1F, %7F-%FF */
    static uint32_t html[] = {
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */

        /* ?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"! */
        0x000000ad, /* 0000 0000 0000 0000 0000 0000 1010 1101 */

        /* _^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */

        /* ~}| {zyx wvut srqp onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000 0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff /* 1111 1111 1111 1111 1111 1111 1111 1111 */
    };
    /* " ", """, "%", "'", %00-%1F, %7F-%FF */
    static uint32_t refresh[] = {
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */

        /* ?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"! */
        0x00000085, /* 0000 0000 0000 0000 0000 0000 1000 0101 */

        /* _^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */

        /* ~}| {zyx wvut srqp onml kjih gfed cba` */
        0x80000000, /* 1000 0000 0000 0000 0000 0000 0000 0000 */

        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */
        0xffffffff /* 1111 1111 1111 1111 1111 1111 1111 1111 */
    };
    /* " ", "%", %00-%1F */
    static uint32_t memcached[] = {
        0xffffffff, /* 1111 1111 1111 1111 1111 1111 1111 1111 */

        /* ?>=< ;:98 7654 3210 /.-, +*)( '&%$ #"! */
        0x00000021, /* 0000 0000 0000 0000 0000 0000 0010 0001 */

        /* _^]\ [ZYX WVUT SRQP ONML KJIH GFED CBA@ */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */

        /* ~}| {zyx wvut srqp onml kjih gfed cba` */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */

        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */
        0x00000000, /* 0000 0000 0000 0000 0000 0000 0000 0000 */
    };
    /* mail_auth is the same as memcached */
    static uint32_t *map[] =
    { uri, args, html, refresh, memcached, memcached };
    escape = map[type];

    if(dst == NULL) {
        /* find the number of the characters to be escaped */
        n = 0;

        while(size) {
            if(escape[*src >> 5] & (1 << (*src & 0x1f))) {
                n++;
            }

            src++;
            size--;
        }

        return (uintptr_t) n;
    }

    while(size) {
        if(escape[*src >> 5] & (1 << (*src & 0x1f))) {
            *dst++ = '%';
            *dst++ = hex[*src >> 4];
            *dst++ = hex[*src & 0xf];
            src++;

        } else {
            *dst++ = *src++;
        }

        size--;
    }

    return (uintptr_t) dst;
}

/* XXX we also decode '+' to ' ' */
void urldecode(u_char **dst, u_char **src, size_t size, unsigned int type)
{
    u_char *d, *s, ch, c, decoded;
    enum {
        sw_usual = 0,
        sw_quoted,
        sw_quoted_second
    } state;
    d = *dst;
    s = *src;
    state = 0;
    decoded = 0;

    while(size--) {
        ch = *s++;

        switch(state) {
            case sw_usual:
                if(ch == '?'
                   && (type & (UNESCAPE_URI | UNESCAPE_REDIRECT))) {
                    *d++ = ch;
                    goto done;
                }

                if(ch == '%') {
                    state = sw_quoted;
                    break;
                }

                if(ch == '+') {
                    *d++ = ' ';
                    break;
                }

                *d++ = ch;
                break;

            case sw_quoted:
                if(ch >= '0' && ch <= '9') {
                    decoded = (u_char)(ch - '0');
                    state = sw_quoted_second;
                    break;
                }

                c = (u_char)(ch | 0x20);

                if(c >= 'a' && c <= 'f') {
                    decoded = (u_char)(c - 'a' + 10);
                    state = sw_quoted_second;
                    break;
                }

                /* the invalid quoted character */
                state = sw_usual;
                *d++ = ch;
                break;

            case sw_quoted_second:
                state = sw_usual;

                if(ch >= '0' && ch <= '9') {
                    ch = (u_char)((decoded << 4) + ch - '0');

                    if(type & UNESCAPE_REDIRECT) {
                        if(ch > '%' && ch < 0x7f) {
                            *d++ = ch;
                            break;
                        }

                        *d++ = '%';
                        *d++ = * (s - 2);
                        *d++ = * (s - 1);
                        break;
                    }

                    *d++ = ch;
                    break;
                }

                c = (u_char)(ch | 0x20);

                if(c >= 'a' && c <= 'f') {
                    ch = (u_char)((decoded << 4) + c - 'a' + 10);

                    if(type & UNESCAPE_URI) {
                        if(ch == '?') {
                            *d++ = ch;
                            goto done;
                        }

                        *d++ = ch;
                        break;
                    }

                    if(type & UNESCAPE_REDIRECT) {
                        if(ch == '?') {
                            *d++ = ch;
                            goto done;
                        }

                        if(ch > '%' && ch < 0x7f) {
                            *d++ = ch;
                            break;
                        }

                        *d++ = '%';
                        *d++ = * (s - 2);
                        *d++ = * (s - 1);
                        break;
                    }

                    *d++ = ch;
                    break;
                }

                /* the invalid quoted character */
                break;
        }
    }

done:
    *dst = d;
    *src = s;
}

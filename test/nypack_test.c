#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#include "nypack.h"

int main(void)
{
    #define PAYLOAD_TYPE    55
    #define PAYLOAD         '1', '2', '3', '4', '5'
    #define PAYLOAD_LEN     5

    static uint8_t nypack_data[] = {
        NYPACK_HEAD_BYTE1, NYPACK_HEAD_BYTE2,
        PAYLOAD_LEN + 7, 0,
        PAYLOAD_TYPE,
        PAYLOAD,
        0x35, 0xA1
    };
    static uint8_t payload[] = { PAYLOAD };
    static uint8_t paser_buff[1024];
    struct nypack_parser parser = NYPACK_PARSER_INIT(paser_buff, sizeof(paser_buff));

    for (int i = 0; i < sizeof(nypack_data); i++) {
        nypack_parser_evt_t evt = nypack_parser_byte(&parser, nypack_data[i]);
        switch (evt) {
        case NYPACK_PARSER_EVT_DONE:
            assert(parser.payload_len == PAYLOAD_LEN);
            assert(0 == memcmp(parser.payload, payload, PAYLOAD_LEN));
            assert(parser.type == PAYLOAD_TYPE);
            break;
        case NYPACK_PARSER_EVT_NO:
            break;
        default: // error
            printf("error=%d on byte %d\n", evt, i);
            abort();
            break;
        }
    }
    printf("nypack paser test done!!!\n");

    static uint8_t gen_nypack[128];
    size_t out_len;

    // from different buffer
    out_len = 0;
    assert(0 == nypack_generate(payload, sizeof(payload), gen_nypack, sizeof(gen_nypack), PAYLOAD_TYPE, &out_len));
    assert(out_len == sizeof(nypack_data));
    assert(0 == memcmp(gen_nypack, nypack_data, sizeof(nypack_data)));
    // from same buffer
    out_len = 0;
    memcpy(gen_nypack, payload, sizeof(payload));
    assert(0 == nypack_generate(gen_nypack, sizeof(payload), gen_nypack, sizeof(gen_nypack), PAYLOAD_TYPE, &out_len));
    assert(out_len == sizeof(nypack_data));
    assert(0 == memcmp(gen_nypack, nypack_data, sizeof(nypack_data)));

    printf("nypack generate test done!!!\n");

    return 0;
}

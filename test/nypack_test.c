#include <stdio.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include <stdarg.h>

#include "cmocka.h"

#include "nypack.h"

#define PAYLOAD_TYPE    55
#define PAYLOAD         '1', '2', '3', '4', '5'
#define PAYLOAD_LEN     5

static uint8_t data_without_crc[] = {
    NYPACK_HEAD_BYTE1, NYPACK_HEAD_BYTE2,
    PAYLOAD_LEN + 7, 0,
    PAYLOAD_TYPE,
    PAYLOAD
};
static uint8_t nypack_data[sizeof(data_without_crc) + 2];
static uint8_t payload[] = { PAYLOAD };

static void test_nypack_parser(void **state)
{
    static uint8_t paser_buff[1024];
    struct nypack_parser parser = NYPACK_PARSER_INIT(paser_buff, sizeof(paser_buff));

    for (int i = 0; i < sizeof(nypack_data); i++) {
        nypack_parser_evt_t evt = nypack_parser_byte(&parser, nypack_data[i]);
        switch (evt) {
        case NYPACK_PARSER_EVT_DONE:
            assert_true(parser.payload_len == PAYLOAD_LEN);
            assert_true(0 == memcmp(parser.payload, payload, PAYLOAD_LEN));
            assert_int_equal(parser.type, PAYLOAD_TYPE);
            break;
        case NYPACK_PARSER_EVT_NO:
            break;
        default: // error
            printf("error=%d on byte %d\n", evt, i);
            fail();
            break;
        }
    }
}

static void test_nypack_generate(void **state)
{
    static uint8_t gen_nypack[128];
    size_t out_len;

    // from different buffer
    out_len = 0;
    assert_true(0 == nypack_generate(payload, sizeof(payload), gen_nypack, sizeof(gen_nypack), PAYLOAD_TYPE, &out_len));
    assert_int_equal(sizeof(nypack_data), out_len);
    assert_true(0 == memcmp(gen_nypack, nypack_data, sizeof(nypack_data)));
    // from same buffer
    out_len = 0;
    memcpy(gen_nypack, payload, sizeof(payload));
    assert_true(0 == nypack_generate(gen_nypack, sizeof(payload), gen_nypack, sizeof(gen_nypack), PAYLOAD_TYPE, &out_len));
    assert_int_equal(sizeof(nypack_data), out_len);
    assert_true(0 == memcmp(gen_nypack, nypack_data, sizeof(nypack_data)));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_nypack_parser),
        cmocka_unit_test(test_nypack_generate),
    };

    // fill data
    int ofst = 0;
    memcpy(nypack_data, data_without_crc, sizeof(data_without_crc));
    uint16_t crc = crc_16bits_compute(data_without_crc, sizeof(data_without_crc));
    nypack_data[ofst++] = crc & 0xFF;
    nypack_data[ofst++] = (crc >> 8) & 0xFF;

    return cmocka_run_group_tests(tests, NULL, NULL);
}

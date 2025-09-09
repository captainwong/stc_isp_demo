#include <assert.h>

#include <QtWidgets/QApplication>

#include "../../common/protocol.h"
#include "programmer.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    if (1) {
        uint8_t buf[] = {
            0x23,
            0x06,
            0xA0,
            0x00,
            0x00,
            0x00,
            0x00,
            0x00,
            0x24,
            0x13,
        };

        assert(isp_pkt_end(buf) == ISP_PKT_END);
        assert(isp_pkt_sum(buf) == 0x13);
        assert(isp_pkt_len(buf) == sizeof(buf));
        assert(isp_pkt_calc_sum((isp_packet_t *)buf) == isp_pkt_sum(buf));
        isp_pkt_parse_context_t ctx;
        isp_packet_t rx;
        for (auto c : buf) {
            isp_parse(&ctx, &rx, c);
        }
        assert(isp_parse_ok);
    }

    if (1) {
        uint8_t buf[] = {0x40, 0x00, 0x02, 0x01, 0x00, 0x24, 0x99};
        assert(ldr_pkt_end(buf) == LDR_PKT_END);
        assert(ldr_pkt_sum(buf) == 0x99);
        assert(ldr_pkt_len(buf) == sizeof(buf));
        assert(ldr_pkt_calc_sum((ldr_packet_t *)buf) == ldr_pkt_sum(buf));
        ldr_pkt_parse_context_t ctx;
        ldr_packet_t rx;
        for (auto c : buf) {
            ldr_parse(&ctx, &rx, c);
        }
        assert(ldr_parse_ok);
    }

    programmer window;
    window.show();
    return app.exec();
}

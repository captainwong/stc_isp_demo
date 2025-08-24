#include <QtWidgets/QApplication>

#include "../../bootloader/src/protocol.h"
#include "programmer.h"
#include <assert.h>

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
        assert(isp_pkt_calc_sum((isp_packet_t*)buf) == isp_pkt_sum(buf));
        isp_pkt_parse_context_t ctx;
        isp_packet_t rx;
        for (auto c : buf) {
            isp_parse(&ctx, &rx, c);
        }
        assert(isp_parse_ok);
    }
    programmer window;
    window.show();
    return app.exec();
}

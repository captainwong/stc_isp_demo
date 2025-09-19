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

    if (1) {
        uint32_t size = 64 * 1024;  // 64KB
        struct {
            const char *name;
            uint32_t b;
        } partitions[] = {
            {"ldr ", 14 * 1024},  // 14KB
            {"meta", 512},
            {"app ", 0},
        };

        uint32_t start = 0, end;
        for (auto &partition : partitions) {
            if (partition.b == 0) {
                partition.b = (size - start);
            }
            end = start + partition.b - 1;
            assert(end < size);
            printf("%s: 0x%06X - 0x%06X (%d B)\n", partition.name, start, end, partition.b);
            start += partition.b;
        }
    }

    if (1) {
        uint32_t size = 4 * 1024 * 1024;  // 4MB
        struct {
            const char *name;
            uint32_t kb;
        } partitions[] = {
            {"ota     ", 4},
            {"ota_bak ", 4},
            {"factory ", 60},
            {"app1    ", 60},
            {"app2    ", 60},
            {"app_data", 0},
        };

        uint32_t start = 0, end;
        for (auto &partition : partitions) {
            if (partition.kb == 0) {
                partition.kb = (size - start) / 1024;
            }
            end = start + partition.kb * 1024 - 1;
            assert(end < size);
            printf("%s: 0x%06X - 0x%06X (%d KB)\n", partition.name, start, end, partition.kb);
            start += partition.kb * 1024;
        }
    }

    programmer window;
    window.show();
    return app.exec();
}

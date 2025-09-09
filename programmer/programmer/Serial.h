#pragma once

#include <QList>
#include <QSerialPort>
#include <stdio.h>

#include "../../common/protocol.h"

#define qdebug_qbytes(origin)                               \
    do {                                                    \
        auto arr = origin;                                  \
        char tmp[8];                                        \
        QStringList sl;                                     \
        for (int i = 0; i < arr.length(); i++) {            \
            auto c = arr[i];                                \
            snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c); \
            sl.append(tmp);                                 \
        }                                                   \
        MYQDEBUG << arr.length() << sl.join(" ");           \
    } while (0);

inline QString bytes2string(const uint8_t* buf, size_t len) {
    char tmp[8];
    QStringList sl;
    for (size_t i = 0; i < len; i++) {
        uint8_t c = buf[i];
        snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c);
        sl.append(tmp);
    }
    return sl.join(" ");
}

inline QString bytes2string(const QByteArray& arr) {
    return bytes2string((const uint8_t*)arr.constData(), arr.size());
}

class Serial : public QSerialPort {
    Q_OBJECT
public:
    Serial(QObject* parent);

    void send_reboot();
    void connect_ldr();
    void read_ldr_version();
    void read_chip_info();
    void read_chip_version();
    void read_rom(uint16_t addr, uint8_t size);
    void erase_all();
    void program_bin(uint16_t addr, const QByteArray& bin);

    void send_tx(isp_packet_t* tx);

signals:
    void sig_parsed(QByteArray data);
    void sig_parse_error(uint8_t b);

public slots:
    void slot_serial_on_read();

private:
    QByteArray out{};
    ldr_pkt_parse_context_t ctx{};
    ldr_packet_t rx{};
};

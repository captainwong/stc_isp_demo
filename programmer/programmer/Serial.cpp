#include "Serial.h"

#include <jlib/jlib/qt/QtDebug.h>

Serial::Serial(QObject* parent) : QSerialPort(parent) {
    connect(this, &QSerialPort::readyRead, this, &Serial::slot_serial_on_read);
}

void Serial::slot_serial_on_read() {
    auto _all = readAll();
    // qdebug_qbytes(_all);

    for (auto c : _all) {
        ldr_parse(&ctx, &rx, c);
        if (ldr_parse_ok) {
            emit sig_parsed(QByteArray((const char*)rx.pkt.dat, ldr_pkt_len(&rx)));
        }
    }
}

void Serial::send_reboot() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.cmd = ISP_CMD_REBOOT;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_e2() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.cmd = ISP_CMD_READ;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_ldr_version() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.cmd = ISP_CMD_CONNECT;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_chipid() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.cmd = ISP_CMD_READ_CHIP_INFO;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::send_tx(isp_packet_t* tx) {
    isp_pkt_sum(tx) = isp_pkt_calc_sum(tx);
    write((const char*)tx->buf, isp_pkt_len(tx));
}

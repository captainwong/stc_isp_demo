#include "Serial.h"

#include <jlib/jlib/qt/QtDebug.h>

Serial::Serial(QObject* parent) : QSerialPort(parent) {
    connect(this, &QSerialPort::readyRead, this, &Serial::slot_serial_on_read);
}

void Serial::slot_serial_on_read() {
    auto _all = readAll();
    qdebug_qbytes(_all);

    for (auto c : _all) {
        ldr_parse(&ctx, &rx, c);
        if (ldr_parse_ok) {
            emit sig_parsed(QByteArray((const char*)rx.buf, ldr_pkt_len(&rx)));
        }
    }
}

void Serial::send_reboot() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_REBOOT;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_rom(uint16_t addr, uint8_t size) {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_READ;
    tx.pkt.addr = addr;
    tx.pkt.size = size;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::connect_ldr() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_CONNECT;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_ldr_version() {
    isp_packet_t tx = { 0 };
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_READ_LDR_VERSION;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_chip_info() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_READ_CHIP_INFO;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::read_chip_version() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_READ_CHIP_VERSION;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::erase_all() {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6;
    tx.pkt.cmd = ISP_CMD_ERASE;
    isp_pkt_end(&tx) = ISP_PKT_END;
    send_tx(&tx);
}

void Serial::program_bin(uint16_t addr, const QByteArray& bin) {
    isp_packet_t tx = {0};
    tx.pkt.head = ISP_PKT_HEAD;
    tx.pkt.len = 6 + bin.size();
    tx.pkt.cmd = ISP_CMD_PROGRAM;
    tx.pkt.addr = addr;
    tx.pkt.size = bin.size() & 0xFF;
    isp_pkt_end(&tx) = ISP_PKT_END;
    memcpy(tx.pkt.dat, bin.constData(), bin.size());
    send_tx(&tx);
}

void Serial::send_tx(isp_packet_t* tx) {
    isp_pkt_sum(tx) = isp_pkt_calc_sum(tx);
    auto dat = QByteArray((const char*)tx->buf, isp_pkt_len(tx));
    qdebug_qbytes(dat);
    write(dat);
}

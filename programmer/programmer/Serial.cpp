#include "Serial.h"

#include <jlib/qt/QtDebug.h>

enum {
    STATE_IDLE = 0,
    STATE_EB_F1,
    STATE_N,
    STATE_DATA,
    STATE_SUM,
};

static struct {
    uint8_t state;
    uint8_t buf[256];
    uint8_t len;
} ctx = {0};

Serial::Serial(QObject* parent) : QSerialPort(parent) {
    connect(this, &QSerialPort::readyRead, this, &Serial::slot_serial_on_read);
}

void Serial::slot_serial_on_read() {
    auto _all = readAll();
    // qdebug_qbytes(_all);
    in += _all;
    rx_count += _all.size();

    while (!in.isEmpty()) {
        uint8_t c = in[0];
        in = in.mid(1);
        twifi2pc_cmd_t cmd = parse_eb_f1(c, &in);
        if (cmd != TWIFI2PC_NONE) {
            emit sig_parsed(cmd, QByteArray((const char*)in.dat, in.len));
        }
    }
}

void Serial::send_reboot() {
    send_buf(nullptr, 0, PC2TWIFI_REBOOT);
}

void Serial::send_reset_wb2() {
    send_buf(nullptr, 0, PC2TWIFI_RESET_WB2);
}

void Serial::send_to_wb2(const QString& str) {
    // 一次最多发送255-6字节
    auto cpy = str;
    while (!cpy.isEmpty()) {
        auto msg = cpy.left(255 - 6);
        cpy = cpy.mid(msg.size());
        send_buf((const uint8_t*)msg.toLatin1().constData(), msg.size(), PC2TWIFI_TO_WB2);
    }
}

void Serial::wb2_power(uint8_t on) {
    send_buf(&on, 1, PC2TWIFI_WB2_POWER);
}

void Serial::read_e2() {
    send_buf(nullptr, 0, PC2TWIFI_READ_E2);
}

void Serial::read_chipid() {
    send_buf(nullptr, 0, PC2TWIFI_READ_CHIPID);
}

void Serial::send_buf(const uint8_t* buf, size_t len, pc2twifi_cmd_t cmd) {
    if (len > 255 - 6) {
        MYQDEBUG << "buf too long";
        return;
    }
    uint8_t cmd_buf[255];
    cmd_buf[0] = 0xEB;
    cmd_buf[1] = 0xF0;
    cmd_buf[2] = (uint8_t)(len + 6);
    cmd_buf[3] = cmd >> 8;
    cmd_buf[4] = cmd & 0xFF;
    memcpy(&cmd_buf[5], buf, len);
    hb_sum(cmd_buf, cmd_buf[2]);
    write((const char*)cmd_buf, cmd_buf[2]);
    MYQDEBUG3 << "<<<"
              << bytes2string(cmd_buf, cmd_buf[2]);
}

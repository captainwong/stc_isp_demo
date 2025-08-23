#pragma once

#include <QList>
#include <QSerialPort>

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
    void send_reset_wb2();
    void read_e2();
    void read_chipid();
    void send_buf(const uint8_t* buf, size_t len, pc2twifi_cmd_t cmd);

signals:
    void sig_parsed(uint16_t cmd, QByteArray data);
    void sig_parse_error(uint8_t b);

public slots:
    void slot_serial_on_read();

private:
    QByteArray in{};
    QByteArray out{};
};

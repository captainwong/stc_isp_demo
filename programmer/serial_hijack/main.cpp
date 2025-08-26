/**
 * physical connection example:
 * 
 * STC CHIP TX <--> COM_REAL CH340 RX
 * STC CHIP RX <--> COM_REAL CH340 TX
 * 
 * FAKE_A CH340 TX <--> FAKE_B CH340 RX
 * FAKE_A CH340 RX <--> FAKE_B CH340 TX
 * 
 * FAKE_B CH340: opened by STC-ISP(用户版)
 */

#include <QByteArray>
#include <QDebug>
#include <QSerialPort>
#include <QStringList>
#include <QtCore/QCoreApplication>
#include <stdio.h>

// #define COM_REAL "COM9"
// #define COM_FAKE_A "COM3"
#define BAUD 115200

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

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    QSerialPort serial1, serial2;

    // GET 2 COM PORTS from command line
    if (argc < 3) {
        qWarning("Usage: %s <COM_REAL> <COM_FAKE_A>", argv[0]);
        return 1;
    }

    const char* COM_REAL = argv[1];
    const char* COM_FAKE_A = argv[2];

    serial1.setPortName(COM_REAL);
    serial1.setBaudRate(BAUD);
    if (!serial1.open(QIODevice::ReadWrite)) {
        qWarning("Failed to open port %s", COM_REAL);
        return 1;
    } else {
        qDebug("Open %s success", COM_REAL);
    }

    serial2.setPortName(COM_FAKE_A);
    serial2.setBaudRate(BAUD);
    if (!serial2.open(QIODevice::ReadWrite)) {
        qWarning("Failed to open port %s", COM_FAKE_A);
        return 1;
    } else {
        qDebug("Open %s success", COM_FAKE_A);
    }

    a.connect(&serial1, &QSerialPort::readyRead, [&serial1, &serial2]() {
        auto all = serial1.readAll();
        qDebug().noquote() << "B:" << bytes2string(all);
        serial2.write(all);
    });

    a.connect(&serial2, &QSerialPort::readyRead, [&serial1, &serial2]() {
        auto all = serial2.readAll();
        qDebug().noquote() << "A:" << bytes2string(all);
        serial1.write(all);
    });

    return a.exec();
}

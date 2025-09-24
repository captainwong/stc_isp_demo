#pragma once

#include <stdio.h>

#include <QList>
#include <QSerialPort>

#include "stcutil.h"

class Serial : public QSerialPort {
    Q_OBJECT
public:
    Serial(QObject* parent);

    void send_reboot();
    void connect_ldr();
    void read_ldr_version();
    void read_app_version();
    void read_chip_info();
    void read_chip_version();
    void read_rom(uint16_t addr, uint8_t size);
    void erase_all();
    void erase_page(uint16_t addr);
    void program_bin(uint16_t addr, const QByteArray& bin);
    void calc_crc32(const QByteArray& data);

    void read_flash_size();
    void erase_flash();
    void erase_flash_sector(uint32_t addr);
    void read_flash(uint32_t addr, uint8_t size);
    void program_flash(uint32_t addr, const QByteArray& bin);

    void reply_latest_ota_app_info(const latest_app_info_t& info);
    void reply_app_data(const get_app_data_res_t* res);

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

#include "flash.h"

#include <jlib/jlib/qt/QtDebug.h>

#include "QHexView/model/buffer/qmemorybuffer.h"
#include "QHexView/qhexview.h"
#include "Serial.h"
#include "stcutil.h"

void flash::createOtaGroup(OtaInfoGroupWidget* wdt, const QString& title) {
    wdt->grp = new QGroupBox(title, this);
    wdt->grp = new QGroupBox(tr("Master OTA info"), this);
    wdt->lblSeq = new QLabel(tr("Sequence:"), wdt->grp);
    wdt->leSeq = new QLineEdit(wdt->grp);
    wdt->lblCurrentApp = new QLabel(tr("Current App ID:"), wdt->grp);
    wdt->leCurrentApp = new QLineEdit(wdt->grp);
    wdt->lblDlState = new QLabel(tr("Download State:"), wdt->grp);
    wdt->leDlState = new QLineEdit(wdt->grp);
    wdt->lblDlAppID = new QLabel(tr("Download App ID:"), wdt->grp);
    wdt->leDlAppID = new QLineEdit(wdt->grp);
    wdt->lblDlReceived = new QLabel(tr("Download Received:"), wdt->grp);
    wdt->leDlReceived = new QLineEdit(wdt->grp);
    wdt->lblDlCRC = new QLabel(tr("Download CRC32:"), wdt->grp);
    wdt->leDlCRC = new QLineEdit(wdt->grp);
    wdt->factory.lblSize = new QLabel(tr("Factory Size (Hex):"), wdt->grp);
    wdt->factory.leSize = new QLineEdit(wdt->grp);
    wdt->factory.lblCRC = new QLabel(tr("Factory CRC32:"), wdt->grp);
    wdt->factory.leCRC = new QLineEdit(wdt->grp);
    wdt->factory.lblTimestamp = new QLabel(tr("Factory Timestamp:"), wdt->grp);
    wdt->factory.leTimestamp = new QLineEdit(wdt->grp);
    wdt->factory.lblVersion = new QLabel(tr("Factory Version:"), wdt->grp);
    wdt->factory.leVersion = new QLineEdit(wdt->grp);
    wdt->app1.lblSize = new QLabel(tr("App1 Size (Hex):"), wdt->grp);
    wdt->app1.leSize = new QLineEdit(wdt->grp);
    wdt->app1.lblCRC = new QLabel(tr("App1 CRC32:"), wdt->grp);
    wdt->app1.leCRC = new QLineEdit(wdt->grp);
    wdt->app1.lblTimestamp = new QLabel(tr("App1 Timestamp:"), wdt->grp);
    wdt->app1.leTimestamp = new QLineEdit(wdt->grp);
    wdt->app1.lblVersion = new QLabel(tr("App1 Version:"), wdt->grp);
    wdt->app1.leVersion = new QLineEdit(wdt->grp);
    wdt->app2.lblSize = new QLabel(tr("App2 Size (Hex):"), wdt->grp);
    wdt->app2.leSize = new QLineEdit(wdt->grp);
    wdt->app2.lblCRC = new QLabel(tr("App2 CRC32:"), wdt->grp);
    wdt->app2.leCRC = new QLineEdit(wdt->grp);
    wdt->app2.lblTimestamp = new QLabel(tr("App2 Timestamp:"), wdt->grp);
    wdt->app2.leTimestamp = new QLineEdit(wdt->grp);
    wdt->app2.lblVersion = new QLabel(tr("App2 Version:"), wdt->grp);
    wdt->app2.leVersion = new QLineEdit(wdt->grp);

    wdt->btnRead = new QPushButton(tr("Read"), wdt->grp);
    wdt->btnReadFactroyAndCalcCrc32 = new QPushButton(tr("Read Factory"), wdt->grp);
    wdt->btnReadApp1AndCalcCrc32 = new QPushButton(tr("Read App1"), wdt->grp);
    wdt->btnReadApp2AndCalcCrc32 = new QPushButton(tr("Read App2"), wdt->grp);

    {
        auto grid = new QGridLayout();
        int row = 0;
        grid->addWidget(wdt->lblSeq, row, 0);
        grid->addWidget(wdt->leSeq, row, 1);
        row++;
        grid->addWidget(wdt->lblCurrentApp, row, 0);
        grid->addWidget(wdt->leCurrentApp, row, 1);
        row++;
        grid->addWidget(wdt->lblDlState, row, 0);
        grid->addWidget(wdt->leDlState, row, 1);
        row++;
        grid->addWidget(wdt->lblDlAppID, row, 0);
        grid->addWidget(wdt->leDlAppID, row, 1);
        row++;
        grid->addWidget(wdt->lblDlReceived, row, 0);
        grid->addWidget(wdt->leDlReceived, row, 1);
        row++;
        grid->addWidget(wdt->lblDlCRC, row, 0);
        grid->addWidget(wdt->leDlCRC, row, 1);
        row++;

        grid->addWidget(wdt->factory.lblSize, row, 0);
        grid->addWidget(wdt->factory.leSize, row, 1);
        row++;
        grid->addWidget(wdt->factory.lblCRC, row, 0);
        grid->addWidget(wdt->factory.leCRC, row, 1);
        row++;
        grid->addWidget(wdt->factory.lblTimestamp, row, 0);
        grid->addWidget(wdt->factory.leTimestamp, row, 1);
        row++;
        grid->addWidget(wdt->factory.lblVersion, row, 0);
        grid->addWidget(wdt->factory.leVersion, row, 1);
        row++;

        grid->addWidget(wdt->app1.lblSize, row, 0);
        grid->addWidget(wdt->app1.leSize, row, 1);
        row++;
        grid->addWidget(wdt->app1.lblCRC, row, 0);
        grid->addWidget(wdt->app1.leCRC, row, 1);
        row++;
        grid->addWidget(wdt->app1.lblTimestamp, row, 0);
        grid->addWidget(wdt->app1.leTimestamp, row, 1);
        row++;
        grid->addWidget(wdt->app1.lblVersion, row, 0);
        grid->addWidget(wdt->app1.leVersion, row, 1);
        row++;

        grid->addWidget(wdt->app2.lblSize, row, 0);
        grid->addWidget(wdt->app2.leSize, row, 1);
        row++;
        grid->addWidget(wdt->app2.lblCRC, row, 0);
        grid->addWidget(wdt->app2.leCRC, row, 1);
        row++;
        grid->addWidget(wdt->app2.lblTimestamp, row, 0);
        grid->addWidget(wdt->app2.leTimestamp, row, 1);
        row++;
        grid->addWidget(wdt->app2.lblVersion, row, 0);
        grid->addWidget(wdt->app2.leVersion, row, 1);
        row++;

        {
            auto line = new QHBoxLayout();
            line->addStretch(1);
            line->addWidget(wdt->btnRead);
            line->addWidget(wdt->btnReadFactroyAndCalcCrc32);
            line->addWidget(wdt->btnReadApp1AndCalcCrc32);
            line->addWidget(wdt->btnReadApp2AndCalcCrc32);
            line->addStretch(1);
            grid->addLayout(line, row, 0, 1, 2);
        }
        wdt->grp->setLayout(grid);
    }
}

flash::flash(QWidget* parent, Serial* pserial) : QDialog(parent), serial(pserial) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setMinimumSize(1500, 800);

    //////////////////////////////////// e2 ////////////////////////////////////
    grpE2 = new QGroupBox(tr("FLASH"), this);
    grpE2->setFixedWidth(750);
    view = new QHexView(grpE2);
    e2.reserve(FLASH_EMPTY_SIZE);
    e2.fill((char)0xFF, FLASH_EMPTY_SIZE);
    doc = QHexDocument::fromMemory<QMemoryBuffer>(e2, this);
    view->setDocument(doc);
    view->setAddressWidth(3);
    view->setScrollSteps(3);
    pb = new QProgressBar(grpE2);
    pb->setRange(0, FLASH_EMPTY_SIZE);

    auto e2layout = new QVBoxLayout();
    {
        e2layout->addWidget(view, 1);
        e2layout->addWidget(pb);
        grpE2->setLayout(e2layout);
    }

    //////////////////////////////////// basic ////////////////////////////////////
    basic.grpBasic = new QGroupBox(tr("Basic Operations"), this);
    basic.lblFlashSize = new QLabel(tr("Flash Size:"), basic.grpBasic);
    basic.leFlashSize = new QLineEdit(basic.grpBasic);
    basic.btnReadFlashSize = new QPushButton(tr("Read"), basic.grpBasic);
    connect(basic.btnReadFlashSize, &QPushButton::clicked, this, &flash::slotReadFlashSize);
    basic.btnEraseAll = new QPushButton(tr("Erase All"), basic.grpBasic);
    connect(basic.btnEraseAll, &QPushButton::clicked, this, &flash::slotEraseAll);

    basic.lblOffset = new QLabel(tr("Offset (Hex):"), basic.grpBasic);
    basic.leOffset = new QLineEdit("0", basic.grpBasic);
    basic.btnRead = new QPushButton(tr("Read"), basic.grpBasic);
    connect(basic.btnRead, &QPushButton::clicked, this, &flash::slotRead);
    basic.btnEraseSector = new QPushButton(tr("Erase Sector"), basic.grpBasic);
    connect(basic.btnEraseSector, &QPushButton::clicked, this, &flash::slotEraseSector);
    basic.lblSize = new QLabel(tr("Size (Hex):"), basic.grpBasic);
    basic.leSize = new QLineEdit("100", basic.grpBasic);
    basic.btnGen = new QPushButton(tr("Generate"), basic.grpBasic);
    connect(basic.btnGen, &QPushButton::clicked, this, &flash::generateData);
    basic.btnProgram = new QPushButton(tr("Program"), basic.grpBasic);
    connect(basic.btnProgram, &QPushButton::clicked, this, &flash::slotProgram);

    {
        auto grid = new QGridLayout();
        int row = 0;
        grid->addWidget(basic.lblFlashSize, row, 0);
        grid->addWidget(basic.leFlashSize, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(basic.btnReadFlashSize);
            line->addWidget(basic.btnEraseAll);
            grid->addLayout(line, row, 2);
        }
        row++;

        grid->addWidget(basic.lblOffset, row, 0);
        grid->addWidget(basic.leOffset, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(basic.btnRead);
            line->addWidget(basic.btnEraseSector);
            grid->addLayout(line, row, 2);
        }
        row++;

        grid->addWidget(basic.lblSize, row, 0);
        grid->addWidget(basic.leSize, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(basic.btnGen);
            line->addWidget(basic.btnProgram);
            grid->addLayout(line, row, 2);
        }
        row++;

        basic.grpBasic->setLayout(grid);
    }

    //////////////////////////////////// master ////////////////////////////////////
    createOtaGroup(&master, tr("Master OTA info"));
    connect(master.btnRead, &QPushButton::clicked, this, &flash::slotReadMasterOtaInfo);
    //////////////////////////////////// backup ////////////////////////////////////
    createOtaGroup(&backup, tr("Backup OTA info"));
    connect(backup.btnRead, &QPushButton::clicked, this, &flash::slotReadBackupOtaInfo);

    auto mainLayout = new QHBoxLayout();
    mainLayout->addWidget(grpE2);
    mainLayout->addWidget(basic.grpBasic, 1);
    mainLayout->addWidget(master.grp, 1);
    mainLayout->addWidget(backup.grp, 1);
    setLayout(mainLayout);

    showMaximized();
    slotReadFlashSize();
}

flash::~flash() {
}

void flash::slotReadFlashSize() {
    serial->read_flash_size();
}

void flash::slotEraseAll() {
    serial->erase_flash();
}

void flash::slotEraseSector() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    serial->erase_flash_sector(offset);
}

void flash::slotRead() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    e2recv = basic.leSize->text().toUInt(nullptr, 16);
    uint32_t size = e2recv;
    if (size > PKT_DAT_MAX_LEN) {  // read at most PKT_DAT_MAX_LEN bytes each time
        size = PKT_DAT_MAX_LEN;
    }
    serial->read_flash(offset, size);
    e2recvd = 0;
    pb->setRange(0, e2recv);
    pb->setValue(0);
}

void flash::read_next_chunk() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    uint32_t size = (e2recv - e2recvd);
    if (size > PKT_DAT_MAX_LEN) {  // read at most PKT_DAT_MAX_LEN bytes each time
        size = PKT_DAT_MAX_LEN;
    }
    serial->read_flash(offset, size);
}

void flash::generateData() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    uint32_t size = basic.leSize->text().toUInt(nullptr, 16);
    for (uint32_t i = 0; i < size; i++) {
        if (offset + i < (uint32_t)e2.size()) {
            e2[offset + i] = (char)(i & 0xFF);
        }
    }
    doc->setData(e2);
    view->scrollToAddress(offset);
}

void flash::slotProgram() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    e2send = basic.leSize->text().toUInt(nullptr, 16);
    if ((uint32_t)e2.size() < offset + e2send) {
        QMessageBox::warning(this, tr("Error"), tr("Data size exceeds flash size!"));
        return;
    }
    uint32_t size = e2send;
    if (size > PKT_DAT_MAX_LEN) {  // program at most PKT_DAT_MAX_LEN bytes each time
        size = PKT_DAT_MAX_LEN;
    }
    serial->program_flash(offset, e2.mid(offset, size));
    e2sent = 0;
    pb->setRange(0, e2send);
    pb->setValue(0);
}

void flash::slotReadMasterOtaInfo() {
    basic.leOffset->setText(QString::number(NORFLASH_OTA_MASTER_ADDR, 16));
    basic.leSize->setText(QString::number(sizeof(ota_info_t), 16));
    slotRead();
}

void flash::slotReadBackupOtaInfo() {
    basic.leOffset->setText(QString::number(NORFLASH_OTA_BACKUP_ADDR, 16));
    basic.leSize->setText(QString::number(sizeof(ota_info_t), 16));
    slotRead();
}

void flash::slotReadFactoryAndCalcCrc32ByMaster() {
}

void flash::slotReadApp1AndCalcCrc32ByMaster() {
}

void flash::slotReadApp2AndCalcCrc32ByMaster() {
}

void flash::slotReadFactoryAndCalcCrc32ByBackup() {
}

void flash::slotReadApp1AndCalcCrc32ByBackup() {
}

void flash::slotReadApp2AndCalcCrc32ByBackup() {
}

void flash::program_next_chunk() {
    uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
    uint32_t size = (e2send - e2sent);
    if (size > PKT_DAT_MAX_LEN) {  // program at most PKT_DAT_MAX_LEN bytes each time
        size = PKT_DAT_MAX_LEN;
    }
    serial->program_flash(offset + e2sent, e2.mid(offset + e2sent, size));
}

void flash::slot_serial_parsed(const QByteArray& buf) {
    auto pkt = reinterpret_cast<const ldr_packet_t*>(buf.constData());
    char sbuf[128];
    switch (pkt->pkt.status) {
        case LDR_STATUS_W25Q_SIZE: {
            uint16_t type = (pkt->pkt.dat[0] << 8) | pkt->pkt.dat[1];
            QString typeStr = norflashType2String(type);
            basic.leFlashSize->setText(QString::number(norflashType2SizeInMBytes(type)) + "MB (" + typeStr + ")");
            e2.resize(norflashType2SizeInBytes(type));
            e2.fill((char)0xFF, e2.size());
            pb->setRange(0, e2.size());
            doc->setData(e2);
            break;
        }
        case LDR_STATUS_W25Q_ERASE_ALL_RES:
            MYQDEBUG3_NOQUOTE << "Flash erase all OK";
            break;

        case LDR_STATUS_W25Q_ERASE_SECTOR_RES: {
            uint32_t addr = *(uint32_t*)&pkt->pkt.dat[0];
            snprintf(sbuf, sizeof(sbuf), "Flash erase sector 0x%06X OK", addr);
            MYQDEBUG3_NOQUOTE << sbuf;
            break;
        }

        case LDR_STATUS_W25Q_DATA: {
            uint32_t addr = *(uint32_t*)&pkt->pkt.dat[0];
            uint8_t size = pkt->pkt.size - 4;  // first 4 bytes are address
            if ((size_t)addr + size > (size_t)e2.size()) {
                MYQCRITICAL3_NOQUOTE << "Received data exceeds flash size!";
                return;
            }
            memcpy(e2.data() + addr, &pkt->pkt.dat[4], size);
            doc->setData(e2);
            view->scrollToAddress(addr);
            e2recvd += size;
            pb->setValue(e2recvd);
            if (e2recvd < e2recv) {
                read_next_chunk();
            } else {
                MYQDEBUG3_NOQUOTE << "Flash read OK";
                uint32_t offset = basic.leOffset->text().toUInt(nullptr, 16);
                uint32_t size = basic.leSize->text().toUInt(nullptr, 16);
                OtaInfoGroupWidget* wdt = nullptr;

                if (size == sizeof(ota_info_t)) {
                    ota_info_t info;
                    if (offset == NORFLASH_OTA_MASTER_ADDR) {
                        wdt = &master;
                        info = *(ota_info_t*)(e2.constData() + NORFLASH_OTA_MASTER_ADDR);
                    } else if (offset == NORFLASH_OTA_BACKUP_ADDR) {
                        wdt = &backup;
                        info = *(ota_info_t*)(e2.constData() + NORFLASH_OTA_BACKUP_ADDR);
                    }
                    ota_info_to_little_endian(info);

                    if (wdt) {
                        wdt->leSeq->setText(QString::number(info.seq));
                        wdt->leCurrentApp->setText(QString::number(info.current_app));
                        wdt->leDlState->setText(QString::number(info.dlctx.state));
                        wdt->leDlAppID->setText(QString::number(info.dlctx.appid));
                        wdt->leDlReceived->setText(QString::number(info.dlctx.received));
                        wdt->leDlCRC->setText(QString::number(info.dlctx.crc, 16).toUpper());
                        wdt->factory.leSize->setText(QString::number(info.factory.size, 16).toUpper());
                        wdt->factory.leCRC->setText(QString::number(info.factory.crc, 16).toUpper());
                        wdt->factory.leTimestamp->setText(timestamp2String(info.factory.timestamp));
                        wdt->factory.leVersion->setText(version2String(info.factory.version));
                        wdt->app1.leSize->setText(QString::number(info.app1.size, 16).toUpper());
                        wdt->app1.leCRC->setText(QString::number(info.app1.crc, 16).toUpper());
                        wdt->app1.leTimestamp->setText(timestamp2String(info.app1.timestamp));
                        wdt->app1.leVersion->setText(version2String(info.app1.version));
                        wdt->app2.leSize->setText(QString::number(info.app2.size, 16).toUpper());
                        wdt->app2.leCRC->setText(QString::number(info.app2.crc, 16).toUpper());
                        wdt->app2.leTimestamp->setText(timestamp2String(info.app2.timestamp));
                        wdt->app2.leVersion->setText(version2String(info.app2.version));
                    }
                }
            }
            break;
        }

        case LDR_STATUS_W25Q_PROGRAM_RES: {
            uint32_t addr = *(uint32_t*)&pkt->pkt.dat[0];
            if (addr == basic.leOffset->text().toUInt(nullptr, 16) + e2sent) {
                uint8_t size = pkt->pkt.size - 4;  // first 4 bytes are address

                // check if the data is correct
                if (memcmp(e2.constData() + addr, &pkt->pkt.dat[4], size) != 0) {
                    MYQCRITICAL3_NOQUOTE << "Program data verification failed!";
                    return;
                }

                e2sent += size;
                pb->setValue(e2sent);
                if (e2sent < e2send) {
                    program_next_chunk();
                } else {
                    MYQDEBUG3_NOQUOTE << "Flash program OK";
                }
            } else {
                MYQCRITICAL3_NOQUOTE << "Program address mismatch!";
            }
            break;
        }
    }
}

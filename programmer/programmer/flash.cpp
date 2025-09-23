#include "flash.h"

#include <jlib/jlib/qt/QtDebug.h>

#include "QHexView/model/buffer/qmemorybuffer.h"
#include "QHexView/qhexview.h"
#include "Serial.h"
#include "stcutil.h"

flash::flash(QWidget* parent, Serial* pserial) : QDialog(parent), serial(pserial) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setMinimumSize(1500, 800);

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

    grpBasic = new QGroupBox(tr("Basic Operations"), this);
    lblFlashSize = new QLabel(tr("Flash Size:"), grpBasic);
    leFlashSize = new QLineEdit(grpBasic);
    btnReadFlashSize = new QPushButton(tr("Read"), grpBasic);
    connect(btnReadFlashSize, &QPushButton::clicked, this, &flash::slotReadFlashSize);
    btnEraseAll = new QPushButton(tr("Erase All"), grpBasic);
    connect(btnEraseAll, &QPushButton::clicked, this, &flash::slotEraseAll);

    lblOffset = new QLabel(tr("Offset (Hex):"), grpBasic);
    leOffset = new QLineEdit("0", grpBasic);
    btnRead = new QPushButton(tr("Read"), grpBasic);
    connect(btnRead, &QPushButton::clicked, this, &flash::slotRead);
    btnEraseSector = new QPushButton(tr("Erase Sector"), grpBasic);
    connect(btnEraseSector, &QPushButton::clicked, this, &flash::slotEraseSector);
    lblSize = new QLabel(tr("Size (Hex):"), grpBasic);
    leSize = new QLineEdit("100", grpBasic);
    btnGen = new QPushButton(tr("Generate"), grpBasic);
    connect(btnGen, &QPushButton::clicked, this, &flash::generateData);
    btnProgram = new QPushButton(tr("Program"), grpBasic);
    connect(btnProgram, &QPushButton::clicked, this, &flash::slotProgram);

    {
        auto grid = new QGridLayout();
        int row = 0;
        grid->addWidget(lblFlashSize, row, 0);
        grid->addWidget(leFlashSize, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(btnReadFlashSize);
            line->addWidget(btnEraseAll);
            grid->addLayout(line, row, 2);
        }
        row++;

        grid->addWidget(lblOffset, row, 0);
        grid->addWidget(leOffset, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(btnRead);
            line->addWidget(btnEraseSector);
            grid->addLayout(line, row, 2);
        }
        row++;

        grid->addWidget(lblSize, row, 0);
        grid->addWidget(leSize, row, 1);
        {
            auto line = new QHBoxLayout();
            line->addWidget(btnGen);
            line->addWidget(btnProgram);
            grid->addLayout(line, row, 2);
        }
        row++;

        grpBasic->setLayout(grid);
    }

    auto mainLayout = new QHBoxLayout();
    mainLayout->addWidget(grpE2);
    mainLayout->addWidget(grpBasic, 1);
    setLayout(mainLayout);
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
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
    serial->erase_flash_sector(offset);
}

void flash::slotRead() {
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
    e2recv = leSize->text().toUInt(nullptr, 16);
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
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
    uint32_t size = (e2recv - e2recvd);
    if (size > PKT_DAT_MAX_LEN) {  // read at most PKT_DAT_MAX_LEN bytes each time
        size = PKT_DAT_MAX_LEN;
    }
    serial->read_flash(offset, size);
}

void flash::generateData() {
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
    uint32_t size = leSize->text().toUInt(nullptr, 16);
    for (uint32_t i = 0; i < size; i++) {
        if (offset + i < (uint32_t)e2.size()) {
            e2[offset + i] = (char)(i & 0xFF);
        }
    }
    doc->setData(e2);
    view->scrollToAddress(offset);
}

void flash::slotProgram() {
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
    e2send = leSize->text().toUInt(nullptr, 16);
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

void flash::program_next_chunk() {
    uint32_t offset = leOffset->text().toUInt(nullptr, 16);
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
            leFlashSize->setText(QString::number(norflashType2SizeInMBytes(type)) + "MB (" + typeStr + ")");
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
            }
            break;
        }

        case LDR_STATUS_W25Q_PROGRAM_RES: {
            uint32_t addr = *(uint32_t*)&pkt->pkt.dat[0];
            if (addr == leOffset->text().toUInt(nullptr, 16) + e2sent) {
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

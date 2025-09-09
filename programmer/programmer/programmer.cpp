#include "programmer.h"

#include <jlib/jlib/qt/QtDebug.h>
#include <jlib/jlib/qt/QtPathHelper.h>
#include <jlib/jlib/qt/darkmode.h>
#include <libemb/emb_bitrev.h>
#include <libstc/stc8h.h>

#include <QSerialPort>
#include <QSerialPortInfo>

#include "QHexView/model/buffer/qmemorybuffer.h"
#include "QHexView/qhexview.h"
#include "Serial.h"
#include <libstc/disassembler/hex80.h>
#include <libstc/disassembler/intel8051is.h>
#include "stcutil.h"

constexpr const int COMMON_BAUDS[] = {
    1200,
    2400,
    4800,
    9600,
    19200,
    38400,
    57600,
    115200,
    100000,
    200000,
    230400,
    250000,
    500000,
    1000000,
    1500000,
    2000000,
    2500000,
    3000000,
};

#define DEFAULT_COM "COM22"
#define DEFAULT_BAUD 115200

programmer::programmer(QWidget* parent)
    : QDialog(parent) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setStyleSheet(jlib::qt::dark_mode_stylesheet);
    setMinimumSize(1200, 800);

    serial = new Serial(this);

    cmbPort = new QComboBox(this);
    btnRefresh = new QPushButton(tr("Refresh"), this);
    connect(btnRefresh, &QPushButton::clicked, this, &programmer::slotRefresh);
    cmbBaud = new QComboBox(this);
    cmbBaud->setEditable(true);
    for (auto b : COMMON_BAUDS) {
        cmbBaud->addItem(QString::number(b), b);
    }
    cmbBaud->setCurrentText(QString::number(DEFAULT_BAUD));
    btnConnect = new QPushButton(tr("Connect"), this);
    connect(btnConnect, &QPushButton::clicked, this, &programmer::slotConnect);

    auto topLayout = new QHBoxLayout();
    topLayout->addWidget(cmbPort, 1);
    topLayout->addWidget(cmbBaud);
    topLayout->addWidget(btnRefresh);
    topLayout->addWidget(btnConnect);

    grpE2 = new QGroupBox(tr("ROM"), this);
    grpE2->setFixedWidth(750);
    view = new QHexView(grpE2);
    e2.reserve(E2_EMPTY_SIZE);
    e2.fill((char)0xFF, E2_EMPTY_SIZE);
    doc = QHexDocument::fromMemory<QMemoryBuffer>(e2, this);
    view->setDocument(doc);
    view->setAddressWidth(3);
    view->setScrollSteps(3);
    pb = new QProgressBar(grpE2);
    pb->setRange(0, E2_EMPTY_SIZE);

    lblRomSize = new QLabel(tr("ROM Size:"), this);
    leRomSize = new QLineEdit(this);
    leRomSize->setReadOnly(true);
    lblFiller = new QLabel(tr("Filler:"), this);
    cmbFiller = new QComboBox(this);
    cmbFiller->setEditable(true);
    cmbFiller->addItem(("00"), 0x00);
    cmbFiller->addItem(("FF"), 0xFF);
    lblBootloaderSize = new QLabel(tr("Bootloader:"), this);
    cmbBootloaderSize = new QComboBox(this);
    cmbBootloaderSize->setEditable(true);
    for (int i = 1; i < 63; i++) {
        cmbBootloaderSize->addItem(QString::number(i), i);
    }
    cmbBootloaderSize->setCurrentText("4");
    lblTotalRomSize = new QLabel("/64KB", this);

    btnOpen = new QPushButton(tr("Open"), grpE2);
    btnOpen->setToolTip(tr("Open USER_APP hex file"));
    connect(btnOpen, &QPushButton::clicked, this, &programmer::slotOpen);
    btnPatch = new QPushButton(tr("Patch"), grpE2);
    btnPatch->setToolTip(tr("Patch USER_APP hex file"));
    connect(btnPatch, &QPushButton::clicked, this, &programmer::slotPatch);
    btnMerge = new QPushButton(tr("Merge"), grpE2);
    btnMerge->setToolTip(tr("Merge BOOTLOADER and USER_APP to one hex file for AiCube-ISP"));
    connect(btnMerge, &QPushButton::clicked, this, &programmer::slotMerge);

    auto e2BtnLine = new QHBoxLayout();
    e2BtnLine->addWidget(lblRomSize);
    e2BtnLine->addWidget(leRomSize);
    e2BtnLine->addWidget(lblFiller);
    e2BtnLine->addWidget(cmbFiller);
    e2BtnLine->addWidget(lblBootloaderSize);
    e2BtnLine->addWidget(cmbBootloaderSize);
    e2BtnLine->addWidget(lblTotalRomSize);
    e2BtnLine->addWidget(btnOpen);
    e2BtnLine->addWidget(btnPatch);
    e2BtnLine->addWidget(btnMerge);

    auto e2layout = new QVBoxLayout();
    e2layout->addWidget(view, 1);
    e2layout->addWidget(pb);
    e2layout->addLayout(e2BtnLine);
    grpE2->setLayout(e2layout);

    grpDisasm = new QGroupBox(tr("Disassembly"), this);
    disasmOutput = new QPlainTextEdit(grpDisasm);
    disasmOutput->setReadOnly(true);
    auto disasmLayout = new QVBoxLayout();
    disasmLayout->addWidget(disasmOutput);
    grpDisasm->setLayout(disasmLayout);

    grpLdr = new QGroupBox(tr("Bootloader"), this);
    lblLdrVersion = new QLabel(tr("Version:"), this);
    leLdrVersion = new QLineEdit(this);
    lblLdrOutput = new QLabel(tr("Output:"), this);
    leLdrOutput = new QPlainTextEdit(this);
    leLdrOutput->setReadOnly(true);
    btnClearOutput = new QPushButton(tr("Clear Output"), this);
    connect(btnClearOutput, &QPushButton::clicked, this, &programmer::slotClearOutput);
    btnReadVersion = new QPushButton(tr("Read Version"), this);
    connect(btnReadVersion, &QPushButton::clicked, this, &programmer::slotReadVersion);
    btnReadChipInfo = new QPushButton(tr("Read Chip Info"), this);
    connect(btnReadChipInfo, &QPushButton::clicked, this, &programmer::slotReadChipInfo);
    btnEraseAll = new QPushButton(tr("Erase All"), this);
    connect(btnEraseAll, &QPushButton::clicked, this, &programmer::slotEraseAll);
    btnProgram = new QPushButton(tr("Program"), this);
    connect(btnProgram, &QPushButton::clicked, this, &programmer::slotProgram);
    btnReboot = new QPushButton(tr("Reboot"), this);
    connect(btnReboot, &QPushButton::clicked, this, &programmer::slotReboot);
    lblReadOffset = new QLabel(tr("Offset:"), this);
    leReadOffset = new QLineEdit("0", this);
    lblReadLen = new QLabel(tr("Length:"), this);
    leReadLen = new QLineEdit(this);
    btnReadRom = new QPushButton(tr("Read ROM"), this);
    connect(btnReadRom, &QPushButton::clicked, this, &programmer::slotReadRom);
    {
        auto grid = new QGridLayout();
        int row = 0;
        grid->addWidget(lblLdrVersion, row, 0);
        grid->addWidget(leLdrVersion, row, 1);
        row++;
        grid->addWidget(lblLdrOutput, row, 0);
        grid->addWidget(leLdrOutput, row, 1);
        row++;

        auto line = new QHBoxLayout();
        line->addWidget(btnClearOutput);
        line->addWidget(btnReadVersion);
        line->addWidget(btnReadChipInfo);

        auto line2 = new QHBoxLayout();
        line2->addWidget(btnEraseAll);
        line2->addWidget(btnProgram);
        line2->addWidget(btnReboot);

        auto line3 = new QHBoxLayout();
        line3->addWidget(lblReadOffset);
        line3->addWidget(leReadOffset);
        line3->addWidget(lblReadLen);
        line3->addWidget(leReadLen);
        line3->addWidget(btnReadRom);

        auto ldrLayout = new QVBoxLayout();
        ldrLayout->addLayout(grid);
        ldrLayout->addLayout(line);
        ldrLayout->addLayout(line2);
        ldrLayout->addLayout(line3);
        grpLdr->setLayout(ldrLayout);
    }

    auto bodyRLayout = new QVBoxLayout();
    bodyRLayout->addWidget(grpDisasm, 1);
    bodyRLayout->addWidget(grpLdr);

    auto bodyLayout = new QHBoxLayout();
    bodyLayout->addWidget(grpE2);
    bodyLayout->addLayout(bodyRLayout);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bodyLayout);
    setLayout(mainLayout);

    slotRefresh();
    connect(serial, &Serial::sig_parsed, this, &programmer::slot_serial_parsed, Qt::QueuedConnection);
    connSerialError = connect(serial,
                              static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                              this,
                              &programmer::slot_serial_error);
}

programmer::~programmer() {}

void programmer::slotRefresh() {
    cmbPort->clear();
    auto all = QSerialPortInfo::availablePorts();
    QString cur = all.isEmpty() ? QString() : all.first().portName() + " " + all.first().description();
    for (const auto& p : all) {
        QString txt = p.portName() + " " + p.description();
        cmbPort->addItem(txt, p.portName());
        if (DEFAULT_COM == p.portName()) {
            cur = txt;
        }
    }
    cmbPort->setCurrentText(cur);
}

void programmer::slotConnect() {
    if (!serial->isOpen()) {
        auto portName = cmbPort->currentData().toString();
        serial->setPortName(portName);
        int baud = cmbBaud->currentText().toInt();
        serial->setBaudRate(baud);
        if (connSerialError) {
            disconnect(connSerialError);
        }
        // serial->clearError();
        if (!serial->open(QIODevice::ReadWrite)) {
            QMessageBox::critical(this, tr("Error"), serial->errorString());
            return;
        }

        connSerialError = connect(serial,
                                  static_cast<void (QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
                                  this,
                                  &programmer::slot_serial_error);

        btnConnect->setText(tr("Disconnect"));
    } else {
        slotDisconnect();
    }
}

void programmer::slotDisconnect() {
    if (connSerialError) {
        disconnect(connSerialError);
    }
    serial->close();
    btnConnect->setText(tr("Connect"));
}

void programmer::slot_serial_error(QSerialPort::SerialPortError serialPortError) {
    if (serialPortError == QSerialPort::NoError) {
        return;
    }
    slotDisconnect();
    QMessageBox::critical(this, tr("COM Error"), serial->errorString());
}

void programmer::slot_serial_parsed(const QByteArray& buf) {
    auto pkt = reinterpret_cast<const ldr_packet_t*>(buf.constData());
    char sbuf[64];
    switch (pkt->pkt.status) {
        case LDR_STATUS_OK:
            if (pkt->pkt.size == 2) {
                snprintf(sbuf, sizeof(sbuf), "0x%04X, %d.%d",
                         (pkt->pkt.dat[0] << 8) | pkt->pkt.dat[1],
                         pkt->pkt.dat[0], pkt->pkt.dat[1]);
                leLdrVersion->setText(sbuf);
            }
            if (e2sent && e2sent < (size_t)e2.size()) {
                program();
            } else {
                leLdrOutput->moveCursor(QTextCursor::End);
                leLdrOutput->insertPlainText("OK\n");
                leLdrOutput->moveCursor(QTextCursor::End);
            }
            break;
        case LDR_STATUS_UNKNOWN_CMD:
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText("Unknow CMD\n");
            leLdrOutput->moveCursor(QTextCursor::End);
            break;
        case LDR_STATUS_ADDR_OUT_OF_RANGE:
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText("Address Out Of Range\n");
            leLdrOutput->moveCursor(QTextCursor::End);
            break;
        case LDR_STATUS_PROGRAM_FAILED:
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText("Program Failed\n");
            leLdrOutput->moveCursor(QTextCursor::End);
            break;
        case LDR_STATUS_CHIP_INFO:
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText(chipInfo2String((stc_chipid_t*)&pkt->pkt.dat[0]));
            leLdrOutput->moveCursor(QTextCursor::End);
            serial->read_chip_version();
            break;
        case LDR_STATUS_CHIP_VERSION:
            snprintf(sbuf, sizeof(sbuf), "Chip Version: %c\n", pkt->pkt.dat[0]);
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText(sbuf);
            leLdrOutput->moveCursor(QTextCursor::End);
            break;
        case LDR_STATUS_LOG: {
            QString log = QString::fromLatin1((const char*)pkt->pkt.dat, pkt->pkt.size) + "\n";
            leLdrOutput->moveCursor(QTextCursor::End);
            leLdrOutput->insertPlainText(log);
            leLdrOutput->moveCursor(QTextCursor::End);
            break;
        }
        case LDR_STATUS_ROM: {
            size_t offset = e2recv;
            size_t total = offset + (pkt->pkt.size);
            if ((size_t)e2.size() < total) {
                e2.resize(total);
            }
            e2.replace(offset, (pkt->pkt.size), QByteArray((const char*)pkt->pkt.dat, (pkt->pkt.size)));
            doc->setData(e2);

            e2recv += (pkt->pkt.size);
            pb->setValue(e2recv);
            if (e2recv < leReadLen->text().toUInt()) {
                read_rom();
            }
            break;
        }
    }
}

static bool tryParseHex80File(const std::string& file_content, std::vector<hex80_code_snippet_t>& snippets) {
    std::vector<hex80_record_t> records;
    if (hex80_to_records(file_content, records)) {
        return false;
    }
    merge_hex80_records(records, snippets);
    return true;
}

void programmer::slotOpen() {
    // *.hex
    const QString supportedFormats = "Hex80 Files (*.hex);;All Files (*.*)";
    QString dir{};
#ifdef _DEBUG
    QDir d(jlib::qt::PathHelperLocalWithoutBin().program());
    MYQDEBUG << d.absolutePath();
    d.cdUp();
    MYQDEBUG << d.absolutePath();
    d.cdUp();
    MYQDEBUG << d.absolutePath();
    dir = d.absolutePath() + "/app/output";
    MYQDEBUG << dir;
#endif

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), dir, supportedFormats);
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open file"));
        return;
    }
    QByteArray dat = file.readAll();
    if (dat.length() > E2_MAX_SIZE) {
        QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
        return;
    }

    // for .bin files, read the content and update the view
    if (fileName.endsWith(".bin", Qt::CaseInsensitive)) {
        e2 = dat;
        doc->setData(e2);
        pb->setRange(0, e2.size());
        pb->setValue(0);
        leRomSize->setText(QString::number(e2.size()));
    } else if (fileName.endsWith(".hex", Qt::CaseInsensitive)) {
        std::vector<hex80_code_snippet_t> snippets;
        if (!tryParseHex80File(dat.toStdString(), snippets)) {
            QMessageBox::critical(this, tr("Error"), tr("Failed to parse Hex80 file"));
            return;
        }
        std::string result;
        size_t e2size = 0;
        for (const auto& snippet : snippets) {
            int r = disasm(snippet.addr, snippet.dat.data(), snippet.dat.size(), result);
            if (r < 0) {
                QMessageBox::critical(this, tr("Error"), tr("Disassembly failed"));
                return;
            }
            if (e2size < snippet.addr + snippet.dat.size()) {
                e2size = snippet.addr + snippet.dat.size();
            }
        }

        if (e2size > E2_MAX_SIZE) {
            QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
            return;
        }

        // copy snippets to e2
        e2.resize(e2size);
        e2.fill(cmbFiller->currentData().toUInt() & 0xFF);
        for (const auto& snippet : snippets) {
            std::copy(snippet.dat.begin(), snippet.dat.end(), e2.begin() + snippet.addr);
        }

        doc->setData(e2);
        disasmOutput->setPlainText(QString::fromStdString(result));
        pb->setRange(0, e2.size());
        pb->setValue(0);
        leRomSize->setText(QString::number(e2.size()));
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Unsupported file format"));
    }
}

void programmer::slotPatch() {
    uint16_t ldr_size = (cmbBootloaderSize->currentText().toUInt() * 1024) & 0xFFFF;
    if (ldr_size >= (size_t)e2.size()) {
        QMessageBox::critical(this, tr("Error"), tr("Bootloader size exceeds or equals to ROM size"));
        return;
    }
    size_t valid_size = e2.size() - ldr_size;

    // move first 3 op codes to ldr_size
    e2[ldr_size + 0] = e2[0];
    e2[ldr_size + 1] = e2[1];
    e2[ldr_size + 2] = e2[2];

    // move valid data to the beginning
    for (size_t i = 0; i < valid_size; i++) {
        e2[i] = e2[i + ldr_size];
    }
    e2.resize(valid_size);
    doc->setData(e2);
    pb->setRange(0, e2.size());
    pb->setValue(0);
    leRomSize->setText(QString::number(e2.size()));

    std::string result;
    int r = disasm(0, (const uint8_t*)e2.constData(), e2.size(), result);
    if (r < 0) {
        QMessageBox::warning(this, tr("Error"), tr("Disassembly failed"));
    } else {
        disasmOutput->setPlainText(QString::fromStdString(result));
    }
}

void programmer::slotMerge() {
    uint16_t ldr_size = (cmbBootloaderSize->currentText().toUInt() * 1024) & 0xFFFF;

    const QString supportedFormats = "Hex80 Files (*.hex);;All Files (*.*)";
    QString dir{};
#ifdef _DEBUG
    {
        QDir d(jlib::qt::PathHelperLocalWithoutBin().program());
        d.cdUp();
        d.cdUp();
        dir = d.absolutePath() + "/bootloader/output";
    }
#endif

    QString bootloader, userapp, allin1;
    std::vector<hex80_code_snippet_t> ldr_snippets, user_snippets, allin1_snippets;

    bootloader = QFileDialog::getOpenFileName(this, tr("Open Bootloader Hex File"), dir, supportedFormats);
    if (bootloader.isEmpty()) {
        return;
    }
    QFile file1(bootloader);
    if (!file1.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open bootloader hex file"));
        return;
    }
    QByteArray dat1 = file1.readAll();
    if (dat1.length() > E2_MAX_SIZE) {
        QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
        return;
    }
    if (!tryParseHex80File(dat1.toStdString(), ldr_snippets) ||
        ldr_snippets.empty() ||
        ldr_snippets.back().addr + ldr_snippets.back().dat.size() > ldr_size) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to parse Hex80 file"));
        return;
    }

#ifdef _DEBUG
    {
        QDir d(jlib::qt::PathHelperLocalWithoutBin().program());
        d.cdUp();
        d.cdUp();
        dir = d.absolutePath() + "/app/output";
    }
#endif
    userapp = QFileDialog::getOpenFileName(this, tr("Open User Application Hex File"), dir, supportedFormats);
    if (userapp.isEmpty()) {
        return;
    }

    QFile file2(userapp);
    if (!file2.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open user application hex file"));
        return;
    }
    QByteArray dat2 = file2.readAll();
    if (dat2.length() > E2_MAX_SIZE) {
        QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
        return;
    }
    if (!tryParseHex80File(dat2.toStdString(), user_snippets) ||
        user_snippets.size() < 2 ||            // at least 2 snippets
        user_snippets[0].addr != 0x0000 ||     // make sure first snippet starts at 0x0000
        (user_snippets[0].dat.size() != 3) ||  // make sure first instruction is LJMP addr16
        (user_snippets[0].dat[0] != 0x02) ||   // make sure first instruction is LJMP, and addr16 is bigger than ldr_size + 3
        ((user_snippets[0].dat[1] << 8) | (user_snippets[0].dat[2])) < ldr_size + 3) {
        QMessageBox::critical(this, tr("Error"), tr("Failed to parse Hex80 file"));
        return;
    }

    user_snippets[0].addr += ldr_size;  // move the first user app instruction to user rom space
    std::copy(ldr_snippets.begin(), ldr_snippets.end(), std::back_inserter(allin1_snippets));
    std::copy(user_snippets.begin(), user_snippets.end(), std::back_inserter(allin1_snippets));
    std::string result;
    size_t e2size = 0;
    for (const auto& snippet : allin1_snippets) {
        int r = disasm(snippet.addr, snippet.dat.data(), snippet.dat.size(), result);
        if (r < 0) {
            QMessageBox::critical(this, tr("Error"), tr("Disassembly failed"));
            return;
        }
        if (e2size < snippet.addr + snippet.dat.size()) {
            e2size = snippet.addr + snippet.dat.size();
        }
    }

    if (e2size > E2_MAX_SIZE) {
        QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
        return;
    }

    // copy snippets to e2
    e2.resize(e2size);
    e2.fill(cmbFiller->currentData().toUInt() & 0xFF);
    for (const auto& snippet : allin1_snippets) {
        std::copy(snippet.dat.begin(), snippet.dat.end(), e2.begin() + snippet.addr);
    }

    doc->setData(e2);
    disasmOutput->setPlainText(QString::fromStdString(result));
    pb->setRange(0, e2.size());
    pb->setValue(0);
    leRomSize->setText(QString::number(e2.size()));

    allin1 = QFileDialog::getSaveFileName(this, tr("Save Merged Hex File"), dir + "/allin1.hex", supportedFormats);
    if (allin1.isEmpty()) {
        return;
    }

    QFile file(allin1);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, tr("Error"), tr("Failed to open all-in-one hex file for writing"));
        return;
    }
    file.write(e2);
    file.close();

    QMessageBox::information(this, tr("Success"), tr("All-in-one hex file saved successfully"));
}

void programmer::slotClearOutput() {
    leLdrOutput->clear();
}

void programmer::slotReadVersion() {
    leLdrVersion->clear();
    serial->read_ldr_version();
}

void programmer::slotReadChipInfo() {
    serial->read_chip_info();
}

void programmer::slotEraseAll() {
    serial->erase_all();
}

void programmer::slotProgram() {
    e2sent = 0;
    pb->setRange(0, e2.size());
    program();
}

void programmer::slotReboot() {
    serial->send_reboot();
}

void programmer::slotReadRom() {
    e2recv = 0;
    pb->setRange(0, leReadLen->text().toUInt());
    pb->setValue(0);
    read_rom();
}

void programmer::program() {
    auto bin = e2.mid(e2sent, 128);
    MYQDEBUG << "Programming" << QString::number(e2sent, 16) << "size=" << bin.size();
    serial->program_bin(rev16(e2sent), bin);
    e2sent += bin.size();
    pb->setValue(e2sent);
}

void programmer::read_rom() {
    uint16_t addr = leReadOffset->text().toUInt();
    size_t size = leReadLen->text().toUInt();
    size -= addr + e2recv;
    if (size > 128) {
        size = 128;
    }
    serial->read_rom((uint16_t)(addr + e2recv), size & 0xFF);
}

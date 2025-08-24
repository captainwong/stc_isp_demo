#include "programmer.h"

#include <jlib/jlib/qt/QtDebug.h>
#include <jlib/jlib/qt/QtPathHelper.h>
#include <jlib/jlib/qt/darkmode.h>

#include <QSerialPort>
#include <QSerialPortInfo>

#include "QHexView/model/buffer/qmemorybuffer.h"
#include "QHexView/qhexview.h"
#include "disassembler/hex80.h"
#include "disassembler/intel8051is.h"

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

#define DEFAULT_BAUD 115200

programmer::programmer(QWidget* parent)
    : QDialog(parent) {
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMinimizeButtonHint);
    setWindowFlags(windowFlags() | Qt::WindowMaximizeButtonHint);
    setStyleSheet(jlib::qt::dark_mode_stylesheet);
    setMinimumSize(1200, 800);

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
    btnFlash = new QPushButton(tr("Flash"), grpE2);
    connect(btnFlash, &QPushButton::clicked, this, &programmer::slotFlash);
    btnFlash->setToolTip(tr("Flash USER_APP hex file"));

    auto e2BtnLine = new QHBoxLayout();
    e2BtnLine->addWidget(lblFiller);
    e2BtnLine->addWidget(cmbFiller);
    e2BtnLine->addWidget(lblBootloaderSize);
    e2BtnLine->addWidget(cmbBootloaderSize);
    e2BtnLine->addWidget(lblTotalRomSize);
    e2BtnLine->addWidget(btnOpen);
    e2BtnLine->addWidget(btnPatch);
    e2BtnLine->addWidget(btnFlash);

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

    auto bodyLayout = new QHBoxLayout();
    bodyLayout->addWidget(grpE2);
    bodyLayout->addWidget(grpDisasm);

    auto mainLayout = new QVBoxLayout();
    mainLayout->addLayout(topLayout);
    mainLayout->addLayout(bodyLayout);
    setLayout(mainLayout);

    slotRefresh();
}

programmer::~programmer() {}

void programmer::slotRefresh() {
    cmbPort->clear();
    auto all = QSerialPortInfo::availablePorts();
    for (const auto& p : all) {
        QString txt = p.portName() + " " + p.description();
        cmbPort->addItem(txt, p.portName());
    }
}

void programmer::slotConnect() {
}

/*
std::string file_content;
    std::vector<uint8_t> bin_data;
    std::ifstream ifs(input_file, std::ios::binary);
    if (!ifs) {
        std::cerr << "Failed to open input file: " << input_file << std::endl;
        return 1;
    }
    ifs.seekg(0, std::ios::end);
    std::streamsize size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    if (format == "hex80") {
        file_content.resize((size_t)size);
        if (!ifs.read(&file_content[0], size)) {
            std::cerr << "Failed to read input file: " << input_file << std::endl;
            return 1;
        }
        std::vector<hex80_record_t> records;
        if (hex80_to_records(file_content, records)) {
            std::cerr << "Failed to convert hex80 to records." << std::endl;
            return 1;
        }
        std::string result;
        std::vector<hex80_code_snippet_t> snippets;
        merge_hex80_records(records, snippets);
        for (const auto& snippet : snippets) {
            int r = disasm(snippet.addr, snippet.dat.data(), snippet.dat.size(), result);
            if (r < 0) {
                std::cerr << "Disassembly failed with error code: " << r << std::endl;
                std::cout << "The recognized ASM instructions are:\n"
                          << result;
                return 1;
            }
        }
        if (output_file.empty()) {
            std::cout << result << std::endl;
        } else {
            std::ofstream ofs(output_file);
            if (!ofs) {
                std::cerr << "Failed to open output file: " << output_file << std::endl;
                return 1;
            }
            ofs << result;
        }
        return 0;
    } else if (format == "hex") {
        file_content.resize((size_t)size);
        if (!ifs.read(&file_content[0], size)) {
            std::cerr << "Failed to read input file: " << input_file << std::endl;
            return 1;
        }
        if (hex_to_bin(file_content, bin_data)) {
            std::cerr << "Failed to convert hex to binary." << std::endl;
            return 1;
        }
    } else {
        bin_data.resize((size_t)size);
        if (!ifs.read(reinterpret_cast<char*>(bin_data.data()), size)) {
            std::cerr << "Failed to read input file: " << input_file << std::endl;
            return 1;
        }
    }

    std::string result;
    int r = disasm(0, bin_data.data(), bin_data.size(), result);
    if (r < 0) {
        std::cerr << "Disassembly failed with error code: " << r << std::endl;
        std::cout << "The recognized ASM instructions are:\n"
                  << result;
        return 1;
    } else {
        if (output_file.empty()) {
            std::cout << result << std::endl;
        } else {
            std::ofstream ofs(output_file);
            if (!ofs) {
                std::cerr << "Failed to open output file: " << output_file << std::endl;
                return 1;
            }
            ofs << result;
        }
    }
*/

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
    dir = d.absolutePath() + "/demo_app/output";
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
    QByteArray data = file.readAll();
    if (data.length() > E2_MAX_SIZE) {
        QMessageBox::critical(this, tr("Error"), tr("ROM space exceeded 64KB"));
        return;
    }

    // for .bin files, read the content and update the view
    if (fileName.endsWith(".bin", Qt::CaseInsensitive)) {
        e2 = data;
        doc->setData(e2);
        pb->setRange(0, e2.size());
    } else if (fileName.endsWith(".hex", Qt::CaseInsensitive)) {
        std::vector<hex80_code_snippet_t> snippets;
        if (!tryParseHex80File(data.toStdString(), snippets)) {
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
    } else {
        QMessageBox::warning(this, tr("Error"), tr("Unsupported file format"));
    }
}

void programmer::slotPatch() {
    size_t ldr_size = cmbBootloaderSize->currentText().toUInt() * 1024;
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

    std::string result;
    int r = disasm(0, (const uint8_t*)e2.constData(), e2.size(), result);
    if (r < 0) {
        QMessageBox::warning(this, tr("Error"), tr("Disassembly failed"));
    } else {
        disasmOutput->setPlainText(QString::fromStdString(result));
    }
}

void programmer::slotFlash() {
}

#pragma once

#include <QtWidgets>
#include <QSerialPort>

#define E2_MAX_SIZE 0x10000
#define E2_EMPTY_SIZE 0x100

class QHexDocument;
class QHexView;
class Serial;
class programmer : public QDialog {
    Q_OBJECT

public:
    programmer(QWidget* parent = nullptr);
    ~programmer();

private slots:
    void slotRefresh();
    void slotConnect();
    void slotDisconnect();    
    void slot_serial_error(QSerialPort::SerialPortError serialPortError);
    void slot_serial_parsed(const QByteArray& pkt);
    void slotOpen();
    void slotPatch();
    void slotClearOutput();
    void slotReadVersion();
    void slotReadChipInfo();
    void slotEraseAll();
    void slotProgram();
    void slotReboot();

    void program();

private:
    QComboBox* cmbPort{};
    QPushButton* btnRefresh{};
    QComboBox* cmbBaud{};
    QPushButton* btnConnect{};

    QGroupBox* grpE2{};
    QHexView* view{};
    QProgressBar* pb{};
    QLabel* lblRomSize{};
    QLineEdit* leRomSize{};
    QLabel* lblFiller{};
    QComboBox* cmbFiller{};
    QLabel* lblBootloaderSize{};
    QComboBox* cmbBootloaderSize{};
    QLabel* lblTotalRomSize{};
    QPushButton* btnOpen{};
    QPushButton* btnPatch{};

    QGroupBox* grpDisasm{};
    QPlainTextEdit* disasmOutput{};

    QGroupBox* grpLdr{};
    QLabel* lblLdrVersion{};
    QLineEdit* leLdrVersion{};
    QLabel* lblLdrOutput{};
    QPlainTextEdit* leLdrOutput{};
    QPushButton* btnClearOutput{};
    QPushButton* btnReadVersion{};
    QPushButton* btnReadChipInfo{};
    QPushButton* btnEraseAll{};
    QPushButton* btnProgram{};
    QPushButton* btnReboot{};

    QByteArray e2{};
    size_t e2sent = 0;
    QHexDocument* doc{};
    Serial* serial{};
    QMetaObject::Connection connSerialError{};
};

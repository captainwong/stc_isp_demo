#pragma once

#include <QSerialPort>
#include <QtWidgets>

#include "../../common/protocol.h"

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
    void slotOpenFlash();
    void slot_serial_error(QSerialPort::SerialPortError serialPortError);
    void slot_serial_parsed(const QByteArray& pkt);
    void slotOpen();
    void slotPatch();
    void slotMerge();
    // void slotClearOutput();
    void slotReadLdrVersion();
    void slotReadAppVersion();
    void slotReadChipInfo();
    void slotEraseAll();
    void slotProgram();
    void slotReboot();
    void slotReadRom();
    void slotErasePage();
    void slotRandomCrcData();
    void slotCalcCrc32();

    void slotLoadOtaConfig();
    void slotSaveOtaConfig();
    void slotAddOtaApp();
    void slotRemoveOtaApp();
    void slotSetOtaAppAsLatest();

    void program();
    void read_rom();
    bool loadOtaConfig(const QString& path);
    bool saveOtaConfig(const QString& path);
    void updateOtaAppList();

private:
    QComboBox* cmbPort{};
    QPushButton* btnRefresh{};
    QComboBox* cmbBaud{};
    QPushButton* btnConnect{};
    QPushButton* btnOpenFlash{};

    ///////////////// rom /////////////////
    QGroupBox* grpE2{};
    QHexView* view{};
    QProgressBar* pb{};
    QLabel* lblRomSize{};
    QLineEdit* leRomSize{};
    QLabel* lblFiller{};
    QComboBox* cmbFiller{};
    QLabel* lblBootloaderSize{};
    QComboBox* cmbBootloaderSize{};
    QLabel* lblMetaSize{};
    QComboBox* cmbMetaSize{};
    QLabel* lblTotalRomSize{};
    QPushButton* btnOpen{};
    QPushButton* btnPatch{};
    QPushButton* btnMerge{};

    ///////////////// disasm /////////////////
    QGroupBox* grpDisasm{};
    QPlainTextEdit* disasmOutput{};

    ///////////////// bootloader /////////////////
    QGroupBox* grpLdr{};
    QLabel* lblLdrVersion{};
    QLineEdit* leLdrVersion{};
    QPushButton* btnReadVersion{};
    QPushButton* btnReadChipInfo{};
    QPushButton* btnEraseAll{};
    QPushButton* btnProgram{};
    QPushButton* btnReboot{};
    QLabel* lblReadOffset{};
    QLineEdit* leReadOffset{};
    QLabel* lblReadLen{};
    QLineEdit* leReadLen{};
    QPushButton* btnReadRom{};
    QPushButton* btnErasePage{};
    QLabel* lblCrcData{};
    QLineEdit* leCrcData{};
    QLineEdit* leCrc32{};
    QPushButton* btnRandomCrcData{};
    QPushButton* btnCalcCrc32{};

    ///////////////// app /////////////////
    struct {
        QGroupBox* grp{};
        QLabel* lblAppVersion{};
        QLineEdit* leAppVersion{};
        QPushButton* btnReadVersion{};
        QPushButton* btnReadChipInfo{};

    } app{};

    ///////////////// ota-server /////////////////
    struct {
        QGroupBox* grp{};
        QListWidget* lstApps{};
        QLabel* lblConfPath{};
        QLineEdit* leConfPath{};
        QPushButton* btnLoadConf{};
        QPushButton* btnSaveConf{};
        QPushButton* btnAddApp{};
        QPushButton* btnRemoveApp{};
        QPushButton* btnSetAppAsLatest{};
    } ota{};

    QByteArray e2{};
    size_t e2sent = 0, e2recv = 0;
    QHexDocument* doc{};
    Serial* serial{};
    QMetaObject::Connection connSerialError{};

    typedef struct {
        app_info_t info;
        QString path;
    } ota_app_t;
    QMap<uint32_t, ota_app_t> ota_apps{}; // version -> app
    uint32_t ota_latest_version = 0;
    QString ota_conf_path{};
};

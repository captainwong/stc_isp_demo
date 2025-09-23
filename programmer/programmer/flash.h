#pragma once

#include <QtWidgets>

#define FLASH_EMPTY_SIZE 0x100
#define FLASH_MAX_SIZE (1024 * 1024 * 4)  // 4MB

class QHexDocument;
class QHexView;
class Serial;

typedef struct OtaInfoGroupWidget_s {
    QGroupBox* grp{};
    QLabel* lblSeq{};
    QLineEdit* leSeq{};
    QLabel* lblCurrentApp{};
    QLineEdit* leCurrentApp{};

    QLabel* lblDlState{};
    QLineEdit* leDlState{};
    QLabel* lblDlAppID{};
    QLineEdit* leDlAppID{};
    QLabel* lblDlReceived{};
    QLineEdit* leDlReceived{};
    QLabel* lblDlCRC{};
    QLineEdit* leDlCRC{};

    struct {
        QLabel* lblSize{};
        QLineEdit* leSize{};
        QLabel* lblCRC{};
        QLineEdit* leCRC{};
        QLabel* lblTimestamp{};
        QLineEdit* leTimestamp{};
        QLabel* lblVersion{};
        QLineEdit* leVersion{};
    } factory{}, app1{}, app2{};

    QPushButton* btnRead{};
    QPushButton* btnReadFactroyAndCalcCrc32{};
    QPushButton* btnReadApp1AndCalcCrc32{};
    QPushButton* btnReadApp2AndCalcCrc32{};

} OtaInfoGroupWidget;

class flash : public QDialog {
    Q_OBJECT

public:
    flash(QWidget* parent, Serial* pserial);
    ~flash();

public slots:
    void slot_serial_parsed(const QByteArray& pkt);
    void slotReadFlashSize();
    void slotEraseAll();
    void slotEraseSector();
    void slotRead();
    void slotProgram();
    void slotReadMasterOtaInfo();
    void slotReadBackupOtaInfo();
    void slotReadFactoryAndCalcCrc32ByMaster();
    void slotReadApp1AndCalcCrc32ByMaster();
    void slotReadApp2AndCalcCrc32ByMaster();
    void slotReadFactoryAndCalcCrc32ByBackup();
    void slotReadApp1AndCalcCrc32ByBackup();
    void slotReadApp2AndCalcCrc32ByBackup();
    
private:
    void read_next_chunk();
    void generateData();
    void program_next_chunk();
    void createOtaGroup(OtaInfoGroupWidget* wdt, const QString& title);

private:
    QGroupBox* grpE2{};
    QHexView* view{};
    QProgressBar* pb{};

    struct {
        QGroupBox* grpBasic{};
        QLabel* lblFlashSize{};
        QLineEdit* leFlashSize{};
        QPushButton* btnReadFlashSize{};
        QPushButton* btnEraseAll{};
        QLabel* lblOffset{};
        QLineEdit* leOffset{};
        QPushButton* btnRead{};
        QPushButton* btnEraseSector{};
        QLabel* lblSize{};
        QLineEdit* leSize{};
        QPushButton* btnGen{};
        QPushButton* btnProgram{};
    } basic{};

    OtaInfoGroupWidget master{}, backup{};

    QByteArray e2{};
    size_t e2send = 0;   // how many bytes to send
    size_t e2sent = 0;   // how many bytes have been sent
    size_t e2recv = 0;   // how many bytes to receive
    size_t e2recvd = 0;  // how many bytes have been received
    QHexDocument* doc{};
    Serial* serial{};
};

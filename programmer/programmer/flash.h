#pragma once

#include <QtWidgets>

#define FLASH_EMPTY_SIZE 0x100
#define FLASH_MAX_SIZE (1024 * 1024 * 4)  // 4MB

class QHexDocument;
class QHexView;
class Serial;
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
    void read_next_chunk();
    void generateData();
    void program_next_chunk();

private:
    QGroupBox* grpE2{};
    QHexView* view{};
    QProgressBar* pb{};

    QLabel* lblFlashSize{};
    QLineEdit* leFlashSize{};
    QPushButton* btnReadFlashSize{};
    QPushButton* btnEraseAll{};

    QGroupBox* grpBasic{};
    QLabel* lblOffset{};
    QLineEdit* leOffset{};
    QPushButton* btnRead{};
    QPushButton* btnEraseSector{};
    QLabel* lblSize{};
    QLineEdit* leSize{};
    QPushButton* btnGen{};
    QPushButton* btnProgram{};

    QByteArray e2{};
    size_t e2send = 0;   // how many bytes to send
    size_t e2sent = 0;   // how many bytes have been sent
    size_t e2recv = 0;   // how many bytes to receive
    size_t e2recvd = 0;  // how many bytes have been received
    QHexDocument* doc{};
    Serial* serial{};
};

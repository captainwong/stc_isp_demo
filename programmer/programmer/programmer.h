#pragma once

#include <QtWidgets>

#define E2_MAX_SIZE 0x10000
#define E2_EMPTY_SIZE 0x100

class QHexDocument;
class QHexView;
class programmer : public QDialog {
    Q_OBJECT

public:
    programmer(QWidget* parent = nullptr);
    ~programmer();

private slots:
    void slotRefresh();
    void slotConnect();
    void slotOpen();
    void slotPatch();
    void slotFlash();

private:
    QComboBox* cmbPort{};
    QPushButton* btnRefresh{};
    QComboBox* cmbBaud{};
    QPushButton* btnConnect{};

    QGroupBox* grpE2{};
    QHexView* view{};
    QProgressBar* pb{};
    QLabel* lblFiller{};
    QComboBox* cmbFiller{};
    QLabel* lblBootloaderSize{};
    QComboBox* cmbBootloaderSize{};
    QLabel* lblTotalRomSize{};
    QPushButton* btnOpen{};
    QPushButton* btnPatch{};
    QPushButton* btnFlash{};

    QGroupBox* grpDisasm{};
    QPlainTextEdit* disasmOutput{};

    QByteArray e2{};
    QHexDocument* doc{};
};

#pragma once

#include <libemb/emb_bitrev.h>
#include <libstc/stc8h.h>

#include <QString>

#include "../../common/protocol.h"

#define qdebug_qbytes(origin)                               \
    do {                                                    \
        auto arr = origin;                                  \
        char tmp[8];                                        \
        QStringList sl;                                     \
        for (int i = 0; i < arr.length(); i++) {            \
            auto c = arr[i];                                \
            snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c); \
            sl.append(tmp);                                 \
        }                                                   \
        MYQDEBUG3 << arr.length() << sl.join(" ");           \
    } while (0);

inline QString bytes2string(const uint8_t* buf, size_t len) {
    char tmp[8];
    QStringList sl;
    for (size_t i = 0; i < len; i++) {
        uint8_t c = buf[i];
        snprintf(tmp, sizeof(tmp), "%02X", (uint8_t)c);
        sl.append(tmp);
    }
    return sl.join(" ");
}

inline QString bytes2string(const QByteArray& arr) {
    return bytes2string((const uint8_t*)arr.constData(), arr.size());
}

inline QString version2String(uint32_t version) {
    return QString::number(version, 16).toUpper() + QString(" %1.%2.%3")
                                                        .arg((version >> 24) & 0xFF)
                                                        .arg((version >> 16) & 0xFF)
                                                        .arg(version & 0xFFFF);
}

inline QString mcuid2String(uint16_t mcuid) {
    switch (mcuid) {
#define XX(id, chip) \
    case id:         \
        return QString(#chip);
        STC_MCU_ID_MAP(XX);
#undef XX
        default:
            return QString("Unknown chipid: ") + QString::number(mcuid, 16);
    }
}

inline QString package2String(uint8_t pkg) {
    switch (pkg) {
#define XX(id, name) \
    case id:         \
        return QString(#name);
        STC_PACKAGE_MAP(XX);
#undef XX
        default:
            return QString("Unknown package: ") + QString::number(pkg, 16);
    }
}

inline QString chipInfo2String(stc_chipid_t* id) {
    QString str;
    char buf[1024];
    snprintf(buf, sizeof(buf), "CPU ID: %02X%02X%02X%02X%02X%02X%02X\n",
             id->st.unique_id.b[0], id->st.unique_id.b[1], id->st.unique_id.b[2],
             id->st.unique_id.b[3], id->st.unique_id.b[4], id->st.unique_id.b[5],
             id->st.unique_id.b[6]);
    str += buf;

    snprintf(buf, sizeof(buf), "  MCU ID: 0x%04X %s\n", rev16(id->st.unique_id.st.mcu), mcuid2String(rev16(id->st.unique_id.st.mcu)).toUtf8().constData());
    str += buf;

    snprintf(buf, sizeof(buf), "  Test Machine No: 0x%04X\n", rev16(id->st.unique_id.st.test_machine_no));
    str += buf;

    snprintf(buf, sizeof(buf), "  Test No: 0x%02X%02X%02X\n",
             id->st.unique_id.st.test_no[0], id->st.unique_id.st.test_no[1], id->st.unique_id.st.test_no[2]);
    str += buf;

    snprintf(buf, sizeof(buf), "1.19V BGV: %d mV\n", rev16(id->st.bgv));
    str += buf;

    snprintf(buf, sizeof(buf), "Internal 32K IRC Frequency: %d Hz\n", rev16(id->st.inner_32k_irc_freq));
    str += buf;

    snprintf(buf, sizeof(buf), "Test Date: %02X-%02X-%02X\n",  // BCD
             id->st.test_date.year, id->st.test_date.month, id->st.test_date.day);
    str += buf;

    snprintf(buf, sizeof(buf), "Package: %02X, %s\n", id->st.package, package2String(id->st.package).toUtf8().constData());
    str += buf;

    return str;
}

//     id      type   size(MB)
#define NORFLASH_TYPES_MAP(XX) \
    XX(0xEF13, W25Q80, 1)      \
    XX(0xEF14, W25Q16, 2)      \
    XX(0xEF15, W25Q32, 4)      \
    XX(0xEF16, W25Q64, 8)      \
    XX(0xEF17, W25Q128, 16)    \
    XX(0xEF18, W25Q256, 32)

inline QString norflashType2String(uint16_t type) {
    switch (type) {
#define XX(id, name, sizem) \
    case id:                \
        return QString(#name);
        NORFLASH_TYPES_MAP(XX);
#undef XX
        default:
            return QString("Unknown NOR Flash type: ") + QString::number(type, 16);
    }
}

inline size_t norflashType2SizeInBytes(uint16_t type) {
    switch (type) {
#define XX(id, name, sizem) \
    case id:                \
        return sizem * 1024 * 1024;
        NORFLASH_TYPES_MAP(XX);
#undef XX
        default:
            return 0;
    }
}

inline size_t norflashType2SizeInMBytes(uint16_t type) {
    switch (type) {
#define XX(id, name, sizem) \
    case id:                \
        return sizem;
        NORFLASH_TYPES_MAP(XX);
#undef XX
        default:
            return 0;
    }
}

inline QString timestamp2String(uint32_t t) {
    time_t tt = (time_t)t;
    struct tm* tm = localtime(&tt);
    if (tm) {
        char buf[64];
        strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
        return QString::number(t, 16).toUpper() + " " + QString(buf);
    } else {
        return QString("Invalid timestamp: ") + QString::number(t, 16).toUpper();
    }
}

inline QString flashAppDlStateToString(uint8_t state) {
    switch (state) {
        case FLASH_APP_DL_STATE_INVALID:
            return QString::number(state, 16).toUpper() + " invalid";
        case FLASH_APP_DL_STATE_IDLE:
            return QString::number(state, 16).toUpper() + " idle";
        case FLASH_APP_DL_STATE_ERASING:
            return QString::number(state, 16).toUpper() + " erasing";
        case FLASH_APP_DL_STATE_DOWNLOADING:
            return QString::number(state, 16).toUpper() + " downloading";
        case FLASH_APP_DL_STATE_VERIFYING:
            return QString::number(state, 16).toUpper() + " verifying";
        case FLASH_APP_DL_STATE_APPLYING:
            return QString::number(state, 16).toUpper() + " applying";
        default:
            return QString::number(state, 16).toUpper() + QString(" UNKNOWN_FLASH_APP_DL_STATE");
    }
}

/*
typedef union {
    uint8_t b;
    struct {
        uint8_t dfu : 1;                // whether to enter DFU mode
        uint8_t ldr : 1;                // whether running in bootloader mode
        uint8_t onchip_app_valid : 1;   // whether on-chip application is valid
        uint8_t onchip_meta_valid : 1;  // whether on-chip factory metadata is valid
        uint8_t appid : 2;              // current running application id, 0: factory, 1: app1, 2: app2
        uint8_t otaid : 1;              // current old ota info id (which is ready to overwrite), 0: master, 1: backup
        uint8_t resv : 1;               // reserved
    } st;
} system_context_t;
*/
inline QString sysctx2String(uint8_t b){
    QString str;
    system_context_t ctx;
    ctx.b = b;
    str += QString("DFU: ") + (ctx.st.dfu ? "Yes" : "No") + ", \n";
    str += QString("Mode: ") + (ctx.st.ldr ? "Bootloader" : "Application") + ", \n";
    str += QString("On-chip App Valid: ") + (ctx.st.onchip_app_valid ? "Yes" : "No") + ", \n";
    str += QString("On-chip Meta Valid: ") + (ctx.st.onchip_meta_valid ? "Yes" : "No") + ", \n";
    str += QString("App ID: ");
    switch (ctx.st.appid) {
        case 0:
            str += "Factory";
            break;
        case 1:
            str += "App1";
            break;
        case 2:
            str += "App2";
            break;
        default:
            str += "Unknown";
            break;
    }
    str += ", \n";
    str += QString("Ota ID: ") + (ctx.st.otaid ? "Backup" : "Master");
    return str;
}
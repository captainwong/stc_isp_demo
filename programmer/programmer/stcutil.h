#pragma once

#include <libemb/emb_bitrev.h>
#include <libstc/stc8h.h>

#include <QString>

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

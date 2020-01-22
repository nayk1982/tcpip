#ifndef CONVERT_H
#define CONVERT_H

#include <QtCore>
#include <QString>
#include <QDateTime>
#include <QLocale>

namespace convert { //==========================================================

typedef union Value8bits {
    qint8 val_int;
    quint8 val_uint;
} ByteUnion;

typedef union Value16bits {
    qint16 val_int;
    quint16 val_uint;
    quint8 val_byte[2];
    qint8 val_sbyte[2];
} ShortUnion;

typedef union Value32bits {
    qint32 val_int;
    quint32 val_uint;
    float val_float;
    quint8 val_byte[4];
    qint8 val_sbyte[4];
} SingleUnion;

typedef union Value64bits {
    qint64 val_int;
    quint64 val_uint;
    double val_float;
    quint8 val_byte[8];
    qint8 val_sbyte[8];
} DoubleUnion;

//==============================================================================

quint8 bcdEncode(qint8 value);
quint16 bcdEncode(qint16 value);
quint32 bcdEncode(qint32 value);
quint64 bcdEncode(qint64 value);
quint8 bcdEncode(quint8 value);
quint16 bcdEncode(quint16 value);
quint32 bcdEncode(quint32 value);
quint64 bcdEncode(quint64 value);
qint8 bcdDecode(quint8 bcdValue, bool *ok = nullptr);
qint16 bcdDecode(quint16 bcdValue, bool *ok = nullptr);
qint32 bcdDecode(quint32 bcdValue, bool *ok = nullptr);
qint64 bcdDecode(quint64 bcdValue, bool *ok = nullptr);
quint8 bcdDecodeUnsigned(quint8 bcdValue, bool *ok = nullptr);
quint16 bcdDecodeUnsigned(quint16 bcdValue, bool *ok = nullptr);
quint32 bcdDecodeUnsigned(quint32 bcdValue, bool *ok = nullptr);
quint64 bcdDecodeUnsigned(quint64 bcdValue, bool *ok = nullptr);
QString intToHex(qint8  val, int len = 2, bool withPrefix = false);
QString intToHex(quint8  val, int len = 2, bool withPrefix = false);
QString intToHex(qint16 val, int len = 4, bool withPrefix = false);
QString intToHex(quint16 val, int len = 4, bool withPrefix = false);
QString intToHex(qint32 val, int len = 8, bool withPrefix = false);
QString intToHex(quint32 val, int len = 8, bool withPrefix = false);
QString intToHex(qint64 val, int len = 16, bool withPrefix = false);
QString intToHex(quint64 val, int len = 16, bool withPrefix = false);
qint8 strToIntDef(const QString &str, qint8 defaultValue);
quint8 strToIntDef(const QString &str, quint8 defaultValue);
qint16 strToIntDef(const QString &str, qint16 defaultValue);
quint16 strToIntDef(const QString &str, quint16 defaultValue);
qint32 strToIntDef(const QString &str, qint32 defaultValue);
quint32 strToIntDef(const QString &str, quint32 defaultValue);
qint64 strToIntDef(const QString &str, qint64 defaultValue);
quint64 strToIntDef(const QString &str, quint64 defaultValue);
QString doubleToStr(qreal val, int precise = 2,
                    QLocale::NumberOption numberOptions = QLocale::OmitGroupSeparator);
qreal strToDouble(const QString &str, bool *ok = nullptr );
QDateTime strToDateTime(const QString &str);
quint8  lo(quint16 val);
quint16 lo(quint32 val);
quint32 lo(quint64 val);
qint8  lo(qint16 val);
qint16 lo(qint32 val);
qint32 lo(qint64 val);
quint8  hi(quint16 val);
quint16 hi(quint32 val);
quint32 hi(quint64 val);
qint8  hi(qint16 val);
qint16 hi(qint32 val);
qint32 hi(qint64 val);
quint16 reverseBytes(quint16 val);
quint32 reverseBytes(quint32 val);
quint64 reverseBytes(quint64 val);
qint16 reverseBytes(qint16 val);
qint32 reverseBytes(qint32 val);
qint64 reverseBytes(qint64 val);
QString bytesToHex(const QByteArray &data, const QString &splitter = QString(" "));

} // namespace convert //=======================================================
#endif // CONVERT_H

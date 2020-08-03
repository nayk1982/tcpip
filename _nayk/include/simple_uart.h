/****************************************************************************
** Copyright (c) 2020 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
** All right reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
** NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
** LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
** OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
** WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
****************************************************************************/
#ifndef SIMPLE_UART_H
#define SIMPLE_UART_H

#include <QObject>
#include <QString>
#include <QStringList>

#if defined (QT_GUI_LIB)
#    include <QComboBox>
#endif

#if defined (Q_OS_WIN32)
#   include <windows.h>
#endif

#include "AbstractPort"

namespace nayk { //=============================================================

//==============================================================================
class SimpleUart: public AbstractPort
{
    Q_OBJECT

public:
    explicit SimpleUart(QObject *parent = nullptr);
    explicit SimpleUart(const QString &portName, QObject *parent = nullptr);
    virtual ~SimpleUart();
    bool setPortName(const QString &portName) override;
    bool setBaudRate(qint32 baudRate);
    qint32 baudRate() const;
    bool open(bool readOnly = false) override;
    bool open(const QString &portName, qint32 baudRate, bool readOnly = false);
    bool open(const QString &portName, bool readOnly = false);
    void close() override;
    bool isOpen() const override;
    bool isReady() override;
    QString portSettings();
    static QStringList availablePorts();
#if defined (QT_GUI_LIB)
    static void fillComboBoxPortName(QComboBox *comboBox, const QVariant &defaultValue = QVariant());
#endif

private:
    bool m_readOnly {false};
    qint32 m_baudRate {9600};

#if defined (Q_OS_WIN32)
    HANDLE m_handle {INVALID_HANDLE_VALUE};
#else
    int m_handle {-1};
#endif

    int baudRateConst(qint32 baudRate);
    qint64 writeData(const char *bytes, qint64 bytesCount) override;
    qint64 readData(char *bytes, qint64 count) override;
};
//==============================================================================

} // namespace nayk //==========================================================
#endif // SIMPLE_UART_H

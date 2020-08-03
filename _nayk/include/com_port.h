/****************************************************************************
** Copyright (c) 2019 Evgeny Teterin (nayk) <sutcedortal@gmail.com>
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
#ifndef COM_PORT_H
#define COM_PORT_H

#include <QObject>
#include <QVariant>
#include <QtSerialPort/QSerialPortInfo>
#include <QtSerialPort/QSerialPort>
#include <QByteArray>
#include <QTimer>

#if defined (QT_GUI_LIB)
#    include <QComboBox>
#endif

#include "AbstractPort"

namespace nayk { //=============================================================

//==============================================================================
class ComPort : public AbstractPort
{
    Q_OBJECT

public:
#if defined (QT_GUI_LIB)
    enum PortProperty {
        PortName = 0,
        PortBaudRate,
        PortDataBits,
        PortStopBits,
        PortParity,
        PortFlowControl
    };
    Q_ENUM(PortProperty)
#endif
    explicit ComPort(QObject *parent = nullptr);
    explicit ComPort(const QString &portName, QObject *parent = nullptr);
    explicit ComPort(const QSerialPortInfo &portInfo, QObject *parent = nullptr);
    virtual ~ComPort();
    bool setPortName(const QString &portName) override;
    bool setBaudRate(qint32 baudRate);
    bool setDataBits(QSerialPort::DataBits dataBits);
    bool setStopBits(QSerialPort::StopBits stopBits);
    bool setParity(QSerialPort::Parity parity);
    bool setFlowControl(QSerialPort::FlowControl flowControl);
    qint32 baudRate() const;
    QSerialPort::DataBits dataBits() const;
    QSerialPort::StopBits stopBits() const;
    QSerialPort::Parity parity() const;
    QSerialPort::FlowControl flowControl() const;
    bool open(bool readOnly = false) override;
    void close() override;
    bool isOpen() const override;
    bool isReady() override;
    qint64 bufferSize() const;
    void setBufferSize(qint64 bufferSize);
    bool isRts();
    bool isDtr();
    bool isCts();
    bool isDsr();
    void clearAllBuffers();
    void clearReadBuffer();
    void clearWriteBuffer();
    QString portSettings();

#if defined (QT_GUI_LIB)
    static void fillComboBoxPortProperty(QComboBox *comboBox, PortProperty portProperty,
                                         const QVariant &defaultValue = QVariant());
#endif
    static QString baudRateToStr(QSerialPort::BaudRate baudRate);
    static QString dataBitsToStr(QSerialPort::DataBits dataBits);
    static QString stopBitsToStr(QSerialPort::StopBits stopBits);
    static QString parityToStr(QSerialPort::Parity parity);
    static QString flowControlToStr(QSerialPort::FlowControl flowControl);
    static QSerialPort::BaudRate strToBaudRate(const QString &value);
    static QSerialPort::DataBits strToDataBits(const QString &value);
    static QSerialPort::StopBits strToStopBits(const QString &value);
    static QSerialPort::Parity strToParity(const QString &value);
    static QSerialPort::FlowControl strToFlowControl(const QString &value);

signals:
    void rts(bool on);
    void dtr(bool on);
    void cts(bool on);
    void dsr(bool on);

private:
    bool m_busy {true};
    bool m_cts {false};
    bool m_dsr {false};
    QSerialPort m_port;
    QTimer m_timer;

    void startInit();

protected:
    qint64 writeData(const char *bytes, qint64 bytesCount) override;
    qint64 readData(char *bytes, qint64 count) override;

private slots:
    void serialPort_error(QSerialPort::SerialPortError error);
    void serialPort_requestToSendChanged(bool set);
    void serialPort_dataTerminalReadyChanged(bool set);
    void serialPort_readyRead();
    void serialPort_bytesWriten(qint64 count);
    void on_timerTimeOut();
};
//==============================================================================

} // namespace nayk //==========================================================
#endif // COM_PORT_H

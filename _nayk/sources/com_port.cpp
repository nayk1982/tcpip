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
#include <QMetaEnum>
#include "Convert"
#include "SystemUtils"

#include "com_port.h"

namespace nayk { //=============================================================

#if defined (QT_GUI_LIB)
//------------------------------------------------------------------------------
void fillComboBoxPortName(QComboBox *comboBox, const QString &defaultValue)
{
    int index = -1;
    QList<QSerialPortInfo> portList = QSerialPortInfo::availablePorts();
    for(const QSerialPortInfo &portInfo: portList) {

        QString name = portInfo.portName();

        comboBox->addItem( name, name );
        if(defaultValue == name) index = comboBox->count()-1;
    }
    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
void fillComboBoxBaudRate(QComboBox *comboBox, QSerialPort::BaudRate defaultValue)
{
    int index = -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::BaudRate>();

    for(int i=0; i<metaEnum.keyCount(); ++i) {

        QSerialPort::BaudRate value = static_cast<QSerialPort::BaudRate>( metaEnum.value(i) );
        if(value != QSerialPort::UnknownBaud) {
            comboBox->addItem( ComPort::baudRateToStr(value), value );
            if(value == defaultValue) index = comboBox->count()-1;
        }
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
void fillComboBoxDataBits(QComboBox *comboBox, QSerialPort::DataBits defaultValue)
{
    int index = -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::DataBits>();

    for(int i=0; i<metaEnum.keyCount(); ++i) {

        QSerialPort::DataBits value = static_cast<QSerialPort::DataBits>( metaEnum.value(i) );
        if(value != QSerialPort::UnknownDataBits) {
            comboBox->addItem( ComPort::dataBitsToStr(value), value );
            if(value == defaultValue) index = comboBox->count()-1;
        }
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
void fillComboBoxStopBits(QComboBox *comboBox, QSerialPort::StopBits defaultValue)
{
    int index = -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::StopBits>();

    for(int i=0; i<metaEnum.keyCount(); ++i) {

        QSerialPort::StopBits value = static_cast<QSerialPort::StopBits>( metaEnum.value(i) );
        if(value != QSerialPort::UnknownStopBits) {
            comboBox->addItem( ComPort::stopBitsToStr(value), value );
            if(value == defaultValue) index = comboBox->count()-1;
        }
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
void fillComboBoxParity(QComboBox *comboBox, QSerialPort::Parity defaultValue)
{
    int index = -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::Parity>();

    for(int i=0; i<metaEnum.keyCount(); ++i) {

        QSerialPort::Parity value = static_cast<QSerialPort::Parity>( metaEnum.value(i) );
        if(value != QSerialPort::UnknownParity) {
            comboBox->addItem( ComPort::parityToStr(value), value );
            if(value == defaultValue) index = comboBox->count()-1;
        }
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
void fillComboBoxFlowControl(QComboBox *comboBox, QSerialPort::FlowControl defaultValue)
{
    int index = -1;
    QMetaEnum metaEnum = QMetaEnum::fromType<QSerialPort::FlowControl>();

    for(int i=0; i<metaEnum.keyCount(); ++i) {

        QSerialPort::FlowControl value = static_cast<QSerialPort::FlowControl>( metaEnum.value(i) );
        if(value != QSerialPort::UnknownFlowControl) {
            comboBox->addItem( ComPort::flowControlToStr(value), value );
            if(value == defaultValue) index = comboBox->count()-1;
        }
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
//------------------------------------------------------------------------------
#endif
//==============================================================================
ComPort::ComPort(QObject *parent) : AbstractPort(parent)
{
    if(!QSerialPortInfo::availablePorts().isEmpty()) {
        m_port.setPort(QSerialPortInfo::availablePorts().first());
    }

    startInit();
}
//==============================================================================
ComPort::ComPort(const QString &portName, QObject *parent) : AbstractPort(parent)
{
    m_port.setPortName(portName);

    startInit();
}
//==============================================================================
ComPort::ComPort(const QSerialPortInfo &portInfo, QObject *parent) : AbstractPort(parent)
{
    m_port.setPort(portInfo);

    startInit();
}
//==============================================================================
ComPort::~ComPort()
{
    close();
}
//==============================================================================
bool ComPort::setPortName(const QString &portName)
{
    if(m_port.portName() == portName) return true;

    m_port.setPortName(portName);

    if(m_port.portName() != portName) {

        m_port.setPortName(m_portName);
        m_lastError = tr("%1: Failed to set port name: %2")
                    .arg(m_portName)
                    .arg(portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
    }

#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Port name set: %2")
                    .arg(m_portName)
                    .arg(portName), Log::LogDbg );
#endif
    m_portName = m_port.portName();
    return true;
}
//==============================================================================
bool ComPort::setBaudRate(qint32 baudRate)
{
    if(m_port.baudRate() == baudRate) return true;

    if (m_port.setBaudRate(baudRate)) {
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Baudrate set: %2")
                    .arg(m_port.portName())
                    .arg(m_port.baudRate()), Log::LogDbg );
#endif
        return true;
    }

    m_lastError = tr("%1: Failed to set baudrate: %2")
            .arg(m_port.portName())
            .arg(baudRate);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
    return false;
}
//==============================================================================
bool ComPort::setDataBits(QSerialPort::DataBits dataBits)
{
    if(m_port.dataBits() == dataBits) return true;

    if (m_port.setDataBits(dataBits)) {
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Databits set: %2")
                    .arg(m_port.portName())
                    .arg(dataBitsToStr(m_port.dataBits())), Log::LogDbg );
#endif
        return true;
    }

    m_lastError = tr("%1: Failed to set databits: %2")
            .arg(m_port.portName())
            .arg(dataBitsToStr(dataBits));
#if !defined (WITHOUT_LOG)
    emit toLog( m_lastError, Log::LogError);
#endif
    return false;
}
//==============================================================================
bool ComPort::setStopBits(QSerialPort::StopBits stopBits)
{
    if(m_port.stopBits() == stopBits) return true;

    if (m_port.setStopBits(stopBits)) {
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Stopbits set: %2")
                    .arg(m_port.portName())
                    .arg(stopBitsToStr(m_port.stopBits())), Log::LogDbg );
#endif
        return true;
    }

    m_lastError = tr("%1: Failed to set stopbits: %2")
            .arg(m_port.portName())
            .arg(stopBitsToStr(stopBits));
#if !defined (WITHOUT_LOG)
    emit toLog( m_lastError, Log::LogError);
#endif
    return false;
}
//==============================================================================
bool ComPort::setParity(QSerialPort::Parity parity)
{
    if(m_port.parity() == parity) return true;

    if (m_port.setParity(parity)) {
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Parity set: %2")
                    .arg(m_port.portName())
                    .arg(parityToStr(m_port.parity())), Log::LogDbg );
#endif
        return true;
    }

    m_lastError = tr("%1: Failed to set parity: %2")
            .arg(m_port.portName())
            .arg(parityToStr(parity));
#if !defined (WITHOUT_LOG)
    emit toLog( m_lastError, Log::LogError);
#endif
    return false;
}
//==============================================================================
bool ComPort::setFlowControl(QSerialPort::FlowControl flowControl)
{
    if(m_port.flowControl() == flowControl) return true;

    if (m_port.setFlowControl(flowControl)) {
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Flowcontrol set: %2")
                    .arg(m_port.portName())
                    .arg(flowControlToStr(m_port.flowControl())), Log::LogDbg );
#endif
        return true;
    }

    m_lastError = tr("%1: Failed to set flowcontrol: %2")
            .arg(m_port.portName())
            .arg(flowControlToStr(flowControl));
#if !defined (WITHOUT_LOG)
    emit toLog( m_lastError, Log::LogError);
#endif
    return false;
}
//==============================================================================
qint32 ComPort::baudRate() const
{
    return m_port.baudRate();
}
//==============================================================================
QSerialPort::DataBits ComPort::dataBits() const
{
    return m_port.dataBits();
}
//==============================================================================
QSerialPort::StopBits ComPort::stopBits() const
{
    return m_port.stopBits();
}
//==============================================================================
QSerialPort::Parity ComPort::parity() const
{
    return m_port.parity();
}
//==============================================================================
QSerialPort::FlowControl ComPort::flowControl() const
{
    return m_port.flowControl();
}
//==============================================================================
bool ComPort::open(bool readOnly)
{
    QIODevice::OpenMode openMode = readOnly ? QIODevice::ReadOnly : QIODevice::ReadWrite;
    if (m_port.isOpen() && (m_port.openMode() == openMode)) return true;

    close();

    if (m_port.open( openMode)) {

        m_buffer.clear();
        m_cts = isCts();
        m_dsr = isDsr();

#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: Port is open").arg(m_port.portName()), Log::LogInfo );
        emit toLog( QString("%1: %2")
                    .arg(m_port.portName())
                    .arg(portSettings())
                    , Log::LogDbg );
        emit toLog( tr("%1: RTS signal is %2")
                    .arg(m_port.portName())
                    .arg(m_port.isRequestToSend() ? tr("ON") : tr("OFF")), Log::LogDbg );
        emit toLog( tr("%1: CTS signal is %2")
                    .arg(m_port.portName())
                    .arg(m_cts ? tr("ON") : tr("OFF")), Log::LogDbg );
        emit toLog( tr("%1: DTR signal is %2")
                    .arg(m_port.portName())
                    .arg(m_port.isDataTerminalReady() ? tr("ON") : tr("OFF")), Log::LogDbg );
        emit toLog( tr("%1: DSR signal is %2")
                    .arg(m_port.portName())
                    .arg(m_dsr ? tr("ON") : tr("OFF")), Log::LogDbg );
#endif

        m_port.clear();
        m_timer.start();
        m_busy = false;
        emit portOpen();
        emit cts(m_cts);
        emit dsr(m_dsr);
        emit rts(m_port.isRequestToSend());
        emit dtr(m_port.isDataTerminalReady());
        return true;
    }
    else {

        m_lastError = tr("%1: Failed to open port: %2")
                        .arg(m_port.portName())
                        .arg(m_port.errorString());
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }
}
//==============================================================================
void ComPort::close()
{
    m_timer.stop();
    m_busy = true;

    if(!isOpen()) return;

    m_port.clear();
    system_utils::pause(100);
    m_port.close();
    m_cts = false;
    m_dsr = false;

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port is closed").arg(m_port.portName()), Log::LogInfo );
#endif
    emit portClose();
}
//==============================================================================
bool ComPort::isOpen() const
{
    return m_port.isOpen();
}
//==============================================================================
bool ComPort::isReady()
{
    if(!m_port.isOpen()) return false;

    if(m_port.flowControl() == QSerialPort::HardwareControl)
        return isCts();
    else
        return true;
}
//==============================================================================
qint64 ComPort::writeData(const char *bytes, qint64 bytesCount)
{
    if (!isOpen()) {
        m_lastError = tr("%1: Port is not open").arg(m_port.portName());

#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

    if(bytesCount < 1) return 0;

    qint64 count = m_port.write(bytes, bytesCount);

    if(count < 0) {
        m_lastError = tr("%1: Write error: %2")
                .arg(m_port.portName())
                .arg(m_port.errorString());
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

#if !defined (WITHOUT_LOG)
    if (count > 0) {

        emit toLog( QString("%1: %2")
                    .arg(m_port.portName())
                    .arg(convert::bytesToHex( bytes, count, " ")),
                    Log::LogOut );
        emit toLog( tr("%1: %2 bytes written to buffer")
                    .arg(m_port.portName())
                    .arg(count), Log::LogDbg );
    }
#endif

    return count;
}
//==============================================================================
qint64 ComPort::readData(char *bytes, qint64 count)
{
    m_buffer.clear();

    if (!isOpen()) {
        m_lastError = tr("%1: Port is not open").arg(m_port.portName());
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

    m_buffer = (count < 0) ? m_port.readAll() : m_port.read(count);
    if(m_buffer.isEmpty()) return 0;

    for(auto i=0; i<m_buffer.size(); ++i)
        *(bytes + i) = m_buffer.at(i);

    for(auto i=m_buffer.size(); i<count; ++i)
        *(bytes + i) = 0;

#if !defined (WITHOUT_LOG)
        emit toLog( QString("%1: %2")
                    .arg(m_port.portName())
                    .arg(convert::bytesToHex(m_buffer, " ")), Log::LogIn );
        emit toLog( tr("%1: Read %2 bytes")
                    .arg(m_port.portName())
                    .arg(m_buffer.size()), Log::LogDbg );
#endif

    emit bytesRead( m_buffer.size() );
    return m_buffer.size();
}
//==============================================================================
#if defined (QT_GUI_LIB)
void ComPort::fillComboBoxPortProperty(QComboBox *comboBox, ComPort::PortProperty portProperty,
                                       const QVariant &defaultValue)
{
    if(!comboBox) return;
    comboBox->clear();

    switch (portProperty) {
    case ComPort::PortName:
        fillComboBoxPortName(comboBox, defaultValue.toString());
        break;
    case ComPort::PortBaudRate:
        fillComboBoxBaudRate(comboBox, qvariant_cast<QSerialPort::BaudRate>(defaultValue));
        break;
    case ComPort::PortDataBits:
        fillComboBoxDataBits(comboBox, qvariant_cast<QSerialPort::DataBits>(defaultValue));
        break;
    case ComPort::PortStopBits:
        fillComboBoxStopBits(comboBox, qvariant_cast<QSerialPort::StopBits>(defaultValue));
        break;
    case ComPort::PortParity:
        fillComboBoxParity(comboBox, qvariant_cast<QSerialPort::Parity>(defaultValue));
        break;
    case ComPort::PortFlowControl:
        fillComboBoxFlowControl(comboBox, qvariant_cast<QSerialPort::FlowControl>(defaultValue));
        break;
    }
}
#endif
//==============================================================================
QString ComPort::baudRateToStr(QSerialPort::BaudRate baudRate)
{
    if(baudRate == QSerialPort::UnknownBaud) {
        return tr("Unknown");
    }
    else {
        return QString::number( static_cast<int>(baudRate) );
    }
}
//==============================================================================
QString ComPort::dataBitsToStr(QSerialPort::DataBits dataBits)
{
    if(dataBits == QSerialPort::UnknownDataBits) {
        return tr("Unknown");
    }
    else {
        return QString::number( static_cast<int>(dataBits) );
    }
}
//==============================================================================
QString ComPort::stopBitsToStr(QSerialPort::StopBits stopBits)
{
    switch (stopBits) {
    case QSerialPort::OneStop:
        return "1";
    case QSerialPort::OneAndHalfStop:
        return "1.5";
    case QSerialPort::TwoStop:
        return "2";
    default:
        return tr("Unknown");
    }
}
//==============================================================================
QString ComPort::parityToStr(QSerialPort::Parity parity)
{
    switch (parity) {
    case QSerialPort::NoParity:
        return tr("No");
    case QSerialPort::EvenParity:
        return tr("Even");
    case QSerialPort::OddParity:
        return tr("Odd");
    case QSerialPort::SpaceParity:
        return tr("Space");
    case QSerialPort::MarkParity:
        return tr("Mark");
    default:
        return tr("Unknown");
    }
}
//==============================================================================
QString ComPort::flowControlToStr(QSerialPort::FlowControl flowControl)
{
    switch (flowControl) {
    case QSerialPort::NoFlowControl:
        return tr("No");
    case QSerialPort::HardwareControl:
        return tr("Hardware");
    case QSerialPort::SoftwareControl:
        return tr("Software");
    default:
        return tr("Unknown");
    }
}
//==============================================================================
QSerialPort::BaudRate ComPort::strToBaudRate(const QString &value)
{
    bool ok;
    int br = value.toInt(&ok);
    if(ok) {
        return static_cast<QSerialPort::BaudRate>(br);
    }
    else {
        return QSerialPort::Baud9600;
    }
}
//==============================================================================
QSerialPort::DataBits ComPort::strToDataBits(const QString &value)
{
    bool ok;
    int db = value.toInt(&ok);
    if(ok && (db > 4) && (db < 9)) {
        return static_cast<QSerialPort::DataBits>(db);
    }
    else {
        return QSerialPort::Data8;
    }
}
//==============================================================================
QSerialPort::StopBits ComPort::strToStopBits(const QString &value)
{
    if(value == "2" || value.toLower().contains(tr("two"))) {
        return QSerialPort::TwoStop;
    }
    else if(value == "1.5" || value == "1,5" || value.toLower().contains(tr("oneandhalf"))) {
        return QSerialPort::OneAndHalfStop;
    }
    else {
        return QSerialPort::OneStop;
    }
}
//==============================================================================
QSerialPort::Parity ComPort::strToParity(const QString &value)
{
    if(value.toLower().contains(tr("even")) || (value.toLower() == "e")) {
        return QSerialPort::EvenParity;
    }
    else if(value.toLower().contains(tr("odd")) || (value.toLower() == "o")) {
        return QSerialPort::OddParity;
    }
    else if(value.toLower().contains(tr("space")) || (value.toLower() == "s")) {
        return QSerialPort::SpaceParity;
    }
    else if(value.toLower().contains(tr("mark")) || value.toLower() == "m") {
        return QSerialPort::MarkParity;
    }
    else {
        return QSerialPort::NoParity;
    }
}
//==============================================================================
QSerialPort::FlowControl ComPort::strToFlowControl(const QString &value)
{
    if(value.toLower().contains(tr("hardware"))) {
        return QSerialPort::HardwareControl;
    }
    else if(value.toLower().contains(tr("software"))) {
        return QSerialPort::SoftwareControl;
    }
    else {
        return QSerialPort::NoFlowControl;
    }
}
//==============================================================================
void ComPort::startInit()
{
    m_portName = m_port.portName();
    m_timer.setSingleShot(false);
    m_timer.setInterval(100);

    connect(&m_port, static_cast<void(QSerialPort::*)(QSerialPort::SerialPortError)>(&QSerialPort::error),
            this, static_cast<void(ComPort::*)(QSerialPort::SerialPortError)>(&ComPort::serialPort_error));
    connect(&m_port, &QSerialPort::requestToSendChanged,
            this, &ComPort::serialPort_requestToSendChanged);
    connect(&m_port, &QSerialPort::dataTerminalReadyChanged,
            this, &ComPort::serialPort_dataTerminalReadyChanged);
    connect(&m_port, &QSerialPort::readyRead,
            this, &ComPort::serialPort_readyRead);
    connect(&m_port, &QSerialPort::bytesWritten,
            this, &ComPort::serialPort_bytesWriten);
    connect(&m_timer, &QTimer::timeout,
            this, &ComPort::on_timerTimeOut);
}
//==============================================================================
void ComPort::serialPort_error(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::NoError) return;
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: %2")
                .arg(m_port.portName())
                .arg(m_port.errorString()), Log::LogError );
#endif
    emit portError();
}
//==============================================================================
bool ComPort::isRts()
{
    return m_port.isRequestToSend();
}
//==============================================================================
bool ComPort::isDtr()
{
    return m_port.isDataTerminalReady();
}
//==============================================================================
bool ComPort::isCts()
{
    return ((m_port.pinoutSignals() & QSerialPort::ClearToSendSignal)
                == QSerialPort::ClearToSendSignal);
}
//==============================================================================
bool ComPort::isDsr()
{
    return ((m_port.pinoutSignals() & QSerialPort::DataSetReadySignal)
                == QSerialPort::DataSetReadySignal);
}
//==============================================================================
void ComPort::clearAllBuffers()
{
    m_port.clear(QSerialPort::AllDirections);
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Clear inbound and outbound buffers")
                .arg(m_port.portName()), Log::LogDbg );
#endif
}
//==============================================================================
void ComPort::clearReadBuffer()
{
    m_port.clear(QSerialPort::Input);
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Clear inbound buffer")
                .arg(m_port.portName()), Log::LogDbg );
#endif
}
//==============================================================================
void ComPort::clearWriteBuffer()
{
    m_port.clear(QSerialPort::Output);
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Clear outbound buffer")
                .arg(m_port.portName()), Log::LogDbg );
#endif
}
//==============================================================================
QString ComPort::portSettings()
{
    return QString("%1,%2,%3,%4,%5")
            .arg(m_port.baudRate())
            .arg(parityToStr(m_port.parity()))
            .arg(m_port.dataBits())
            .arg(stopBitsToStr(m_port.stopBits()))
            .arg(flowControlToStr(m_port.flowControl()));
}
//==============================================================================
qint64 ComPort::bufferSize() const
{
    return m_port.readBufferSize();
}
//==============================================================================
void ComPort::setBufferSize(qint64 bufferSize)
{
    m_port.setReadBufferSize(bufferSize);

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Read buffer size set: %2")
                .arg(m_port.portName())
                .arg(m_port.readBufferSize()), Log::LogDbg );
#endif
}
//==============================================================================
void ComPort::serialPort_requestToSendChanged(bool set)
{
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: RTS signal is %2")
                .arg(m_port.portName())
                .arg(set ? tr("ON") : tr("OFF")), Log::LogDbg );
#endif
    emit rts(set);
}
//==============================================================================
void ComPort::serialPort_dataTerminalReadyChanged(bool set)
{
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: DTR signal is %2")
                .arg(m_port.portName())
                .arg(set ? tr("ON") : tr("OFF")), Log::LogDbg );
#endif
    emit dtr(set);
}
//==============================================================================
void ComPort::serialPort_readyRead()
{
    if(m_busy) {
        m_port.clear(QSerialPort::Input);
        return;
    }

    if(m_autoRead) {
        read(-1);
    }
    else {
        emit readyRead();
    }
}
//==============================================================================
void ComPort::serialPort_bytesWriten(qint64 count)
{
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: %2 bytes written to the port from the buffer")
                .arg(m_port.portName())
                .arg(count), Log::LogDbg );
#endif
    emit bytesWrite(count);
}
//==============================================================================
void ComPort::on_timerTimeOut()
{
    bool on = isCts();

    if(m_cts != on) {
        m_cts = on;
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: CTS signal is %2")
                    .arg(m_port.portName())
                    .arg(m_cts ? tr("ON") : tr("OFF")), Log::LogDbg );
#endif
        emit cts(m_cts);
    }

    on = isDsr();

    if(m_dsr != on) {
        m_dsr = on;
#if !defined (WITHOUT_LOG)
        emit toLog( tr("%1: DSR signal is %2")
                    .arg(m_port.portName())
                    .arg(m_dsr ? tr("ON") : tr("OFF")), Log::LogDbg );
#endif
        emit dsr(m_dsr);
    }
}
//==============================================================================

} // namespace nayk //==========================================================

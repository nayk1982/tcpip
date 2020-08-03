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
#include <QDir>
#include <stdio.h>
#include "Convert"
#include "SystemUtils"
#include "simple_uart.h"

#if defined (Q_OS_LINUX)
#   include <unistd.h>
#   include <termios.h>
#   include <fcntl.h>
#endif

namespace nayk { //=============================================================

const int portWaitTimeout {5000};

#ifdef Q_OS_WIN32
const int bufferInSize {4096};
const int bufferOutSize {4096};
#endif

//==============================================================================
SimpleUart::SimpleUart(QObject *parent) : AbstractPort(parent)
{
    QStringList ports = availablePorts();
    if(!ports.isEmpty()) m_portName = ports.first();
}
//==============================================================================
SimpleUart::SimpleUart(const QString &portName, QObject *parent)
    : AbstractPort(parent)
{
    m_portName = portName;
}
//==============================================================================
SimpleUart::~SimpleUart()
{
    close();
}
//==============================================================================
bool SimpleUart::setPortName(const QString &portName)
{
    if(portName == m_portName) return true;

    if( isOpen() ) {

        close();
        QString savePortName = m_portName;
        m_portName = portName;

        if(!open(m_readOnly)) {

            m_portName = savePortName;
            return false;
        }
    }
    else {

        m_portName = portName;
    }

    return true;
}
//==============================================================================
bool SimpleUart::setBaudRate(qint32 baudRate)
{
    if(m_baudRate == baudRate) return true;

    if(!isOpen()) {
        m_baudRate = baudRate;
        return true;
    }
#ifdef Q_OS_LINUX
    struct termios options;

    if( tcgetattr(m_handle, &options) < 0) {

        m_lastError = tr("%1: Failed to get termios")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if(cfsetispeed(&options, baudRateConst(baudRate)) < 0) {

        m_lastError = tr("%1: Failed to set baudrate")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if(cfsetospeed(&options, baudRateConst(baudRate)) < 0) {

        m_lastError = tr("%1: Failed to set baudrate")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if (tcsetattr(m_handle, TCSANOW, &options) < 0) {

        m_lastError = tr("%1: Failed to set termios")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }
#endif // LINUX

#ifdef Q_OS_WIN32
    DCB dcb;

    if (!GetCommState(m_handle, &dcb)) {

        m_lastError = tr("%1: Failed to GetCommState")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    dcb.BaudRate = baudRateConst(baudRate);	// current baud rate

    if (!SetCommState(m_handle, &dcb)) {

        m_lastError = tr("%1: Failed to SetCommState")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }
#endif

    m_baudRate = baudRate;
    return true;
}
//==============================================================================
qint32 SimpleUart::baudRate() const
{
    return m_baudRate;
}
//==============================================================================
bool SimpleUart::open(bool readOnly)
{
    close();

#ifdef Q_OS_LINUX
    QString portName = "/dev/" + m_portName;
    int fd = ::open(portName.toLatin1().constData(),
                    (readOnly ? O_RDONLY : O_RDWR) | O_NOCTTY | O_NDELAY);

    if (fd < 0) {

        m_lastError = tr("%1: Failed to open port")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if( lockf(fd, F_TEST, 0) < 0 ) {

        m_lastError = tr("%1: Failed to locked")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        ::close(fd);
        return false;
    }
    else if( lockf(fd, F_LOCK, 0) < 0 ) {

        m_lastError = tr("%1: Failed to locked")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        ::close(fd);
        return false;
    }

    if(fcntl(fd, F_SETFL, 0) < 0) {

        m_lastError = tr("%1: Failed to setfl")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(fd, F_ULOCK, 0);
#pragma GCC diagnostic pop

        ::close(fd);
        return false;
    }

    struct termios options;

    if (tcgetattr(fd, &options) < 0) {

        m_lastError = tr("%1: Failed to get termios")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(fd, F_ULOCK, 0);
#pragma GCC diagnostic pop

        ::close(fd);
        return false;
    }

    // Baudrate
    if(cfsetispeed(&options, baudRateConst(m_baudRate)) < 0) {

        m_lastError = tr("%1: Failed to set baudrate")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(fd, F_ULOCK, 0);
#pragma GCC diagnostic pop

        ::close(fd);
        return false;
    }

    if(cfsetospeed(&options, baudRateConst(m_baudRate)) < 0) {

        m_lastError = tr("%1: Failed to set baudrate")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(fd, F_ULOCK, 0);
#pragma GCC diagnostic pop

        ::close(fd);
        return false;
    }

    // Bit size
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    // Parity
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~PARODD;

    // Stop bits
    options.c_cflag &= ~CSTOPB;

    // Flow Control
    options.c_cflag &= ~CRTSCTS;
    options.c_iflag &= ~(IXON | IXOFF | IXANY);

    options.c_cflag |= (CLOCAL | CREAD);


    options.c_iflag &= ~INLCR;
    options.c_iflag &= ~ICRNL;
    options.c_iflag &= ~IGNCR;
    options.c_oflag &= ~OPOST;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    options.c_cc[VMIN] = 0;
    options.c_cc[VTIME] = 1;

    if (tcsetattr(fd, TCSANOW, &options) < 0) {

        m_lastError = tr("%1: Failed to set termios")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(fd, F_ULOCK, 0);
#pragma GCC diagnostic pop

        ::close(fd);
        return false;
    }

    m_handle = fd;
#endif //LINUX

#ifdef Q_OS_WIN32
    DCB dcb;
    QString portName = "\\\\.\\" + m_portName;

    m_handle = CreateFileA(reinterpret_cast<LPCSTR>(portName.toLatin1().data()),
                        GENERIC_READ | GENERIC_WRITE,
                        0,			// comm devices must be opened w/exclusive-access
                        NULL,			// no security attrs
                        OPEN_EXISTING,          // comm devices must use OPEN_EXISTING
                        0,			// FILE_FLAG_OVERLAPPED, not overlapped I/O
                        NULL			// hTemplate must be NULL for comm devices
                        );


    if ( m_handle == INVALID_HANDLE_VALUE) {

        m_lastError = tr("%1: Invalid Handle")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if (!GetCommState( m_handle, &dcb)) {

        m_lastError = tr("%1: Failed to GetCommState")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    dcb.BaudRate = baudRateConst(m_baudRate);	// current baud rate
    dcb.fBinary = 1;			// binary mode, no EOF check
    dcb.fParity = 0;			// disable parity checking
    dcb.fOutxCtsFlow = 0;		// CTS output flow control
    dcb.fOutxDsrFlow = 0;		// DSR output flow control
    dcb.fDtrControl = DTR_CONTROL_DISABLE;
    // DTR flow control type
    dcb.fDsrSensitivity = 0;	// DSR sensitivity
    dcb.fTXContinueOnXoff = 0;	// XOFF continues Tx
    dcb.fOutX = 0;		// XON/XOFF out flow control
    dcb.fInX = 0;		// XON/XOFF in flow control
    //	dcb.XoffLim=128;
    //	dcb.XonLim=256;
    dcb.fErrorChar = 0;		// enable error replacement
    dcb.fNull = 0;		// enable null stripping
    dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
    // RTS flow control
    dcb.fAbortOnError = 1;	// abort reads/writes on error
    dcb.ByteSize = 8;           // number of bits/byte, 4-8
    dcb.Parity = 0;             // 0-4=no,odd,even,mark,space
    dcb.StopBits = 0;           // 0,1,2 = 1, 1.5, 2

    if (!SetCommState( m_handle, &dcb)) {

        m_lastError = tr("%1: Failed to SetCommState")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    if (!SetupComm( m_handle, bufferInSize, bufferOutSize)) {

        m_lastError = tr("%1: Failed to SetupComm")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    COMMTIMEOUTS tOut;
    tOut.ReadIntervalTimeout = 3;
    tOut.ReadTotalTimeoutMultiplier = 2;
    tOut.ReadTotalTimeoutConstant = 50;
    tOut.WriteTotalTimeoutMultiplier = 2;
    tOut.WriteTotalTimeoutConstant = 50;

    if(!SetCommTimeouts(m_handle, &tOut )) {

        m_lastError = tr("%1: Failed to SetCommTimeouts")
                        .arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return false;
    }

    PurgeComm( m_handle, PURGE_TXCLEAR | PURGE_RXCLEAR );

#endif //WINDOWS

    m_readOnly = readOnly;

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port is open").arg(m_portName), Log::LogInfo );
    emit toLog( QString("%1: %2")
                .arg(m_portName)
                .arg(portSettings())
                , Log::LogDbg );
#endif

    emit portOpen();
    return true;
}
//==============================================================================
bool SimpleUart::open(const QString &portName, qint32 baudRate, bool readOnly)
{
    close();

    return setPortName(portName) && setBaudRate(baudRate) && open(readOnly);
}
//==============================================================================
bool SimpleUart::open(const QString &portName, bool readOnly)
{
    return open(portName, m_baudRate, readOnly);
}
//==============================================================================
void SimpleUart::close()
{
    if(!isOpen()) return;

#ifdef Q_OS_LINUX

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-result"
        lockf(m_handle, F_ULOCK, 0);
#pragma GCC diagnostic pop

    ::close(m_handle);
    m_handle = -1;
#endif
#ifdef Q_OS_WIN32
    PurgeComm( m_handle, PURGE_TXCLEAR | PURGE_RXCLEAR );
    CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
#endif

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port is closed").arg(m_portName), Log::LogInfo );
#endif
    emit portClose();
}
//==============================================================================
bool SimpleUart::isOpen() const
{
#ifdef Q_OS_LINUX
    return (m_handle != -1);
#endif
#ifdef Q_OS_WIN32
    return (m_handle != INVALID_HANDLE_VALUE);
#endif
}
//==============================================================================
bool SimpleUart::isReady()
{
    return isOpen();
}
//==============================================================================
qint64 SimpleUart::writeData(const char *bytes, qint64 bytesCount)
{
    if (!isOpen()) {
        m_lastError = tr("%1: Port is not open").arg(m_portName);

#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

    if(bytesCount < 1) return 0;
    qint64 result;

#ifdef Q_OS_LINUX
    result = ::write(m_handle, bytes, bytesCount);
#endif
#ifdef Q_OS_WIN32
    DWORD rp;
    WriteFile(m_handle, bytes, bytesCount, &rp, NULL);
    result = rp;
#endif

    if(result < 0) {
        m_lastError = tr("%1: Write error").arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

    if (result > 0) {
#if !defined (WITHOUT_LOG)

        emit toLog( QString("%1: %2")
                    .arg(m_portName)
                    .arg(convert::bytesToHex( bytes, result, " ")),
                    Log::LogOut );
        emit toLog( tr("%1: %2 bytes written to buffer")
                    .arg(m_portName)
                    .arg(result), Log::LogDbg );
#endif

        emit bytesWrite(result);
    }

    return result;
}
//==============================================================================
qint64 SimpleUart::readData(char *bytes, qint64 count)
{
    m_buffer.clear();

    if (!isOpen()) {
        m_lastError = tr("%1: Port is not open").arg(m_portName);
#if !defined (WITHOUT_LOG)
        emit toLog( m_lastError, Log::LogError);
#endif
        return 0;
    }

    qint64 result {0};
    qint64 realRead {0};
    char buffer[count];
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    int timeMs {0};

    do
    {
#ifdef Q_OS_LINUX
        result = ::read(m_handle, buffer, (count - realRead));
#endif //LINUX
#ifdef Q_OS_WIN32
        DWORD rp;
        ReadFile(m_handle, buffer, (count - realRead), &rp, NULL);
        result = rp;
#endif //WINDOWS

        if (result < 1 ) {

            system_utils::pause( 50 );
        }
        else {
            for (int i=0; i < result; ++i)
                *(bytes + realRead + i) = buffer[i];

            realRead += result;
            startTime = QDateTime::currentMSecsSinceEpoch();
        }

        timeMs = QDateTime::currentMSecsSinceEpoch() - startTime;

    } while ((realRead < count) && (timeMs < portWaitTimeout) );

    if(realRead > 0)
        m_buffer = QByteArray(bytes, realRead);

    for (int i = realRead; i < count; ++i)
        *(bytes + i) = 0;

    if(realRead > 0) {
#if !defined (WITHOUT_LOG)
        emit toLog( QString("%1: %2")
                    .arg(m_portName)
                    .arg(convert::bytesToHex(m_buffer, " ")), Log::LogIn );
        emit toLog( tr("%1: Read %2 bytes")
                    .arg(m_portName)
                    .arg(realRead), Log::LogDbg );
#endif

        emit bytesRead( realRead );
    }

    return realRead;
}
//==============================================================================
QString SimpleUart::portSettings()
{
    return QString("%1,%2,%3,%4,%5")
            .arg(m_baudRate)
            .arg("NoParity")
            .arg(8)
            .arg(1)
            .arg("NoFlowControl");
}
//==============================================================================
QStringList SimpleUart::availablePorts()
{
#ifdef Q_OS_LINUX
    static const QStringList deviceFileNameFilterList = QStringList()
            << QString("ttyS*")
            << QString("ttyUSB*");
    QStringList result;
    QDir deviceDir(QString("/dev"));

    if (deviceDir.exists()) {

        deviceDir.setNameFilters(deviceFileNameFilterList);
        deviceDir.setFilter(QDir::Files | QDir::System | QDir::NoSymLinks);

        QStringList deviceFilePaths;
        const auto deviceFileInfos = deviceDir.entryInfoList();

        for (const QFileInfo &deviceFileInfo : deviceFileInfos) {

            const QString deviceAbsoluteFilePath = deviceFileInfo.absoluteFilePath();

            if (!deviceFilePaths.contains(deviceAbsoluteFilePath)) {

                int res = ::open(deviceAbsoluteFilePath.toLatin1().constData(),
                                 O_RDWR | O_NOCTTY | O_NDELAY);
                if (res != -1) {

                    struct termios t_port;

                    if (tcgetattr(res, &t_port) >= 0) {

                        deviceFilePaths.append(deviceAbsoluteFilePath);
                        result.append(deviceAbsoluteFilePath.mid(5));
                    }

                    ::close(res);
                }
            }
        }
    }

    return result;
#endif

#ifdef Q_OS_WIN32
    HKEY hKey = nullptr;
    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return QStringList();

    QStringList result;
    DWORD index = 0;
    enum { MaximumValueNameInChars = 16383 };
    std::vector<wchar_t> outputValueName(MaximumValueNameInChars, 0);
    std::vector<wchar_t> outputBuffer(MAX_PATH + 1, 0);
    DWORD bytesRequired = MAX_PATH;

    for (;;) {
        DWORD requiredValueNameChars = MaximumValueNameInChars;
        const LONG ret = ::RegEnumValue(hKey, index, &outputValueName[0], &requiredValueNameChars,
                nullptr, nullptr, reinterpret_cast<PBYTE>(&outputBuffer[0]), &bytesRequired);
        if (ret == ERROR_MORE_DATA) {
            outputBuffer.resize(bytesRequired / sizeof(wchar_t) + 2, 0);
        } else if (ret == ERROR_SUCCESS) {

            QString str = QString::fromWCharArray(&outputBuffer[0]);
            int n = str.toLower().indexOf("com");
            result.append( n < 0 ? str : str.mid(n) );
            ++index;
        } else {
            break;
        }
    }
    ::RegCloseKey(hKey);

    return result;
#endif
}
//==============================================================================
#if defined (QT_GUI_LIB)
void SimpleUart::fillComboBoxPortName(QComboBox *comboBox, const QVariant &defaultValue)
{
    if(!comboBox) return;

    comboBox->clear();
    QStringList ports = availablePorts();

    int index = -1;
    for(auto i=0; i<ports.size(); ++i) {

        QString portName = ports.at(i);
        comboBox->addItem( portName, portName );
        if(portName == defaultValue.toString()) index = i;
    }

    if(index >= 0) comboBox->setCurrentIndex(index);
}
#endif
//==============================================================================
int SimpleUart::baudRateConst(qint32 baudRate)
{
    switch (baudRate) {
    case 300:
#ifdef Q_OS_WIN32
        return CBR_300;
#else
        return B300;
#endif
    case 600:
#ifdef Q_OS_WIN32
        return CBR_600;
#else
        return B600;
#endif
    case 1200:
#ifdef Q_OS_WIN32
        return CBR_1200;
#else
        return B1200;
#endif
    case 2400:
#ifdef Q_OS_WIN32
        return CBR_2400;
#else
        return B2400;
#endif
    case 4800:
#ifdef Q_OS_WIN32
        return CBR_4800;
#else
        return B4800;
#endif
    case 9600:
#ifdef Q_OS_WIN32
        return CBR_9600;
#else
        return B9600;
#endif
    case 19200:
#ifdef Q_OS_WIN32
        return CBR_19200;
#else
        return B19200;
#endif
    case 38400:
#ifdef Q_OS_WIN32
        return CBR_38400;
#else
        return B38400;
#endif
    case 57600:
#ifdef Q_OS_WIN32
        return CBR_57600;
#else
        return B57600;
#endif
    case 115200:
#ifdef Q_OS_WIN32
        return CBR_115200;
#else
        return B115200;
#endif
    default:
#ifdef Q_OS_WIN32
        return 0;
#else
        return B0;
#endif
    }
}
//==============================================================================

} // namespace nayk //==========================================================

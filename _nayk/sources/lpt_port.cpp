#include "Convert"
#include "SystemUtils"
#include "lpt_port.h"

#ifdef Q_OS_WIN32
#   include <vector>
#   include <minwindef.h>
#   include <winreg.h>
#endif

namespace nayk { //=============================================================

//==============================================================================

#ifdef Q_OS_WIN32
static QStringList portNamesFromHardwareDeviceMap()
{
    HKEY hKey = nullptr;
    QStringList result;

    if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"HARDWARE\\DEVICEMAP\\PARALLEL PORTS", 0, KEY_QUERY_VALUE, &hKey) != ERROR_SUCCESS)
        return result;

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
        }
        else if (ret == ERROR_SUCCESS) {

            QString value = QString::fromWCharArray(&outputBuffer[0]);
            int n = value.lastIndexOf("\\");
            if(n >= 0) value.remove(0, n + 1);
            if(!value.isEmpty()) {
                result.append(value);
            }
            ++index;
        } else {
            break;
        }
    }
    ::RegCloseKey(hKey);
    return result;
}
#endif
//==============================================================================
LptPort::LptPort(QObject *parent) : AbstractPort(parent)
{
    QStringList portList = availablePorts();
    if(!portList.isEmpty()) m_portName = portList.first();
}
//==============================================================================
LptPort::LptPort(const QString &portName, QObject *parent) : AbstractPort(parent)
{
    m_portName = portName;
}
//==============================================================================
LptPort::~LptPort()
{
    close();
}
//==============================================================================
bool LptPort::open(bool readOnly)
{
    if(isOpen()) return true;

#ifdef Q_OS_WIN32
    LPCWSTR fileName = reinterpret_cast<LPCWSTR>(m_portName.utf16());
    DWORD mode = readOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
    m_handle = CreateFile( fileName, mode, 0, NULL,
                           OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

    if(m_handle == INVALID_HANDLE_VALUE) {

        m_lastError = tr("Failed to open port: %1")
                .arg(GetLastError());
        port_error();
        return false;
    }
#else
    if(m_portName.left(5) != "/dev/")
        m_deviceFile.setFileName("/dev/" + m_portName);
    else
        m_deviceFile.setFileName(m_portName);

    if(!m_deviceFile.open( readOnly ? QIODevice::ReadOnly : QIODevice::ReadWrite )) {

        m_lastError = tr("Failed to open port: %1")
                .arg(m_deviceFile.errorString());
        port_error();
        return false;
    }
#endif

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port is open").arg(m_portName), Log::LogInfo );
#endif
    emit portOpen();
    return true;
}
//==============================================================================
void LptPort::close()
{
    if(!isOpen()) return;

#ifdef Q_OS_WIN32
    FlushFileBuffers(m_handle);
    system_utils::pause(100);
    CloseHandle(m_handle);
    m_handle = INVALID_HANDLE_VALUE;
#else
    m_deviceFile.flush();
    system_utils::pause(100);
    m_deviceFile.close();
#endif

#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port is closed").arg(m_portName), Log::LogInfo );
#endif

    emit portClose();
}
//==============================================================================
bool LptPort::isOpen() const
{
#ifdef Q_OS_WIN32
    return m_handle != INVALID_HANDLE_VALUE;
#else
    return m_deviceFile.isOpen();
#endif
}
//==============================================================================
qint64 LptPort::writeData(const char *bytes, qint64 bytesCount)
{
    if (!isOpen()) {
        m_lastError = tr("Port is not open");
        port_error();
        return 0;
    }

    if(bytesCount < 1) return 0;
    qint64 count = 0;

#ifdef Q_OS_WIN32
    DWORD dw = 0;

    if( !WriteFile(m_handle,
                   reinterpret_cast<LPCVOID>( bytes ),
                   bytesCount, &dw, NULL) ) {
        m_lastError = tr("Write error: %1")
                .arg(GetLastError());
        port_error();
        return 0;
    }
    FlushFileBuffers(m_handle);
    count = static_cast<qint64>( dw );

#else
    count = m_deviceFile.write(bytes, bytesCount);

    if( (count < 0) || !m_deviceFile.flush()) {
        m_lastError = tr("Write error: %1").arg(m_deviceFile.errorString());
        port_error();
        return 0;
    }
#endif

#if !defined (WITHOUT_LOG)
    if (count > 0) {

        emit toLog( QString("%1: %2")
                    .arg(m_portName)
                    .arg(convert::bytesToHex(bytes, count, " ")), Log::LogOut );
        emit toLog( tr("%1: %2 bytes written to buffer")
                    .arg(m_portName)
                    .arg(count), Log::LogDbg );
    }
#endif

    port_bytesWriten(count);
    return count;
}
//==============================================================================
bool LptPort::setPortName(const QString &portName)
{
    if(m_portName == portName) return true;

    close();
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: Port name set: %2")
                .arg(m_portName)
                .arg(portName), Log::LogDbg );
#endif

    m_portName = portName;
    return true;
}
//==============================================================================
qint64 LptPort::readData(char *bytes, qint64 count)
{
    m_buffer.clear();

    if (!isOpen()) {
        m_lastError = tr("Port is not open");
        port_error();
        return 0;
    }

#ifdef Q_OS_WIN32
    DWORD lpSize = (count < 0) ? MAXDWORD : static_cast<DWORD>(count);
    DWORD rp;

    if(!ReadFile(m_handle, reinterpret_cast<LPVOID>(bytes),
                 lpSize, &rp, NULL )) {

        m_lastError = tr("Read error: %1")
                .arg(GetLastError());
        port_error();
        return 0;
    }

    qint64 result = rp;

#else
    qint64 result = m_deviceFile.read(bytes, (count < 0) ? 0x7FFFFFFF : count);
#endif

    if(result < 1) return 0;
    m_buffer = QByteArray(bytes, result);

#if !defined (WITHOUT_LOG)
    emit toLog( QString("%1: %2")
                .arg(m_portName)
                .arg(convert::bytesToHex(m_buffer, " ")), Log::LogIn );
    emit toLog( tr("%1: Read %2 bytes")
                .arg(m_portName)
                .arg(result), Log::LogDbg );
#endif

    emit bytesRead( result );
    return result;
}
//==============================================================================
QStringList LptPort::availablePorts()
{
    QStringList list;

#ifdef Q_OS_WIN32

    list = portNamesFromHardwareDeviceMap();
#else

    QStringList namesList;
    namesList << "lpt%1" << "lp%1" << "parport%1";

    for(const QString &portName: namesList) {

        for(int i=0; i<10; ++i) {

            QString port = portName.arg(i);

            if(QFile::exists( "/dev/" + port )) {
                list.append( port );
            }

            port = "usb/" + portName.arg(i);

            if(QFile::exists( "/dev/" + port )) {

                list.append( port );
            }
        }
    }

#endif
    list.sort();
    return list;
}
//==============================================================================
#if defined (QT_GUI_LIB)
void LptPort::fillComboBoxAvailablePorts(QComboBox *comboBox, const QVariant &defaultValue)
{
    if(!comboBox) return;
    int index = -1;
    QStringList portList = availablePorts();
    for(const QString &portName: portList) {

        comboBox->addItem( portName, portName );
        if(defaultValue == portName) index = comboBox->count()-1;
    }
    if(index >= 0) comboBox->setCurrentIndex(index);
}
#endif
//==============================================================================
void LptPort::port_error()
{
#if !defined (WITHOUT_LOG)
    emit toLog( QString("%1: %2")
                .arg(m_portName)
                .arg(m_lastError), Log::LogError );
#endif

    emit portError();
}
//==============================================================================
void LptPort::port_bytesWriten(qint64 count)
{
#if !defined (WITHOUT_LOG)
    emit toLog( tr("%1: %2 bytes written to the port from the buffer")
                .arg(m_portName)
                .arg(count), Log::LogDbg );
#endif

    emit bytesWrite(count);
}
//==============================================================================

//==============================================================================

} // namespace //===============================================================

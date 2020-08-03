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
#include <QtGlobal>
#include <QTimer>
#include <QEventLoop>
#include <QProcess>
#include <QSysInfo>

#if defined (Q_OS_WIN32)
#   include <windows.h>
#   if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
#       include <versionhelpers.h>
#   endif
#elif defined (Q_OS_LINUX)
#   include <unistd.h>
#   include <sys/utsname.h>
#endif

#include "Convert"
#include "system_utils.h"

namespace system_utils { //=====================================================

void pause(int ms)
{
    QTimer timer;
    timer.setInterval(ms == 0 ? 1 : qAbs(ms));
    timer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timer.start();
    loop.exec();
}
//==============================================================================
int dayOfWeek(int year, int month, int day)
{
    if(year <= 1752)
        return -1;

    if((month < 1) || (month > 12))
        return -1;

    if((day < 1) || (day > 31))
        return -1;

    static int t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
    year -= month < 3;
    int dow = (year + year/4 - year/100 + year/400 + t[month-1] + day) % 7;

    if(dow == 0)
        dow = 7;
    else if(( dow < 0 ) || ( dow > 7 ))
        return -1;

    return dow;
}
//==============================================================================
bool osCmd(const QString &cmd, QByteArray &out, int timeout)
{
    QProcess process;

#if defined (Q_OS_WIN32)
    process.start(QString("%1").arg(cmd));
    //process.start(QString("cmd -c \"%1\"").arg(cmd));
#else
    process.start(QString("bash -c \"%1\"").arg(cmd));
#endif

    timeout = qBound(300, timeout, 300000);
    if (!process.waitForStarted(timeout)) {
        out = QObject::tr("Timeout wait for started").toUtf8();
        return false;
    }
    if (!process.waitForFinished(timeout)) {
        out = QObject::tr("Timeout wait for finished").toUtf8();
        return false;
    }
    out = process.readAllStandardOutput();
    return true;
}
//==============================================================================
bool osCmd(const QString &cmd, QString &out, int timeout)
{
    QByteArray buf;
    bool result = osCmd(cmd, buf, timeout);
    out = QString(buf);
    return result;
}
//==============================================================================
QString osName()
{
#if defined (Q_OS_WIN32)
    return "Windows";
#elif defined (Q_OS_FREEBSD)
    return "FreeBSD";
#elif defined (Q_OS_LINUX)
    return "Linux";
#elif defined (Q_OS_IOS)
    return "iOS";
#elif defined (Q_OS_MACOS)
    return "MacOS";
#elif defined (Q_OS_ANDROID)
    return "Android"
#elif defined (Q_OS_UNIX)
    return "Unix";
#else
    return QString();
#endif
}
//==============================================================================
QString osVersion()
{
    QString kernelVersion = QSysInfo::kernelVersion();

#if defined (Q_OS_WIN32)

#   if (QT_VERSION >= QT_VERSION_CHECK(5, 10, 0))
    if (IsWindowsServer()) return "Server";
    if (IsWindows10OrGreater()) return "10";
    if (IsWindows8Point1OrGreater()) return "8.1";
    if (IsWindows8OrGreater())return "8";
    if (IsWindows7SP1OrGreater()) return "7 SP1";
    if (IsWindows7OrGreater()) return "7";
    if (IsWindowsVistaOrGreater()) return "Vista";
    if (IsWindowsXPOrGreater()) return "XP";
#   else

    QStringList verList = kernelVersion.split('.', QString::SkipEmptyParts);
    qint64 ver {0};

    for(int i=verList.size()-1; i >= 0; --i) {
        ver = ver | ( static_cast<qint64>(convert::strToIntDef(verList.at(i), 0))
                      << (16 * (verList.size()-1-i)));
    }

    if(ver >= 0x0A00000000)
        return "10";
    if(ver >= 0x0600030000)
        return "8.1";
    if(ver >= 0x0600020000)
        return "8";
    if(ver >= 0x0600011DB1)
        return "7 SP1";
    if(ver >= 0x0600010000)
        return "7";
    if(ver >= 0x0600000000)
        return "Vista";
    if(ver >= 0x0500010000)
        return "XP";
    if(ver >= 0x0500000000)
        return "2k";

#   endif

#elif defined (Q_OS_LINUX)
    utsname kernelInfo;
    uname(&kernelInfo);
    return QString(kernelInfo.release);
#endif

    return kernelVersion;
}
//==============================================================================
QString hostName()
{
    return QSysInfo::machineHostName();
}
//==============================================================================

} // namespace system_utils //==================================================


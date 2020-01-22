#include <QTimer>
#include <QEventLoop>
#include <QProcess>
#include <QTextCodec>

#if defined (Q_OS_WIN32)
#include <windows.h>
#elif defined (Q_OS_LINUX)
#include <unistd.h>
#include <sys/utsname.h>
#endif

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
        out = QByteArray("Process start timeout");
        return false;
    }
    if (!process.waitForFinished(timeout)) {
        out = QByteArray("Timeout waiting for process to complete");
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
    QString res = "";

#if defined (Q_OS_WIN32)
    res = "Windows";
#elif defined (Q_OS_LINUX)
    res = "Linux";
#elif defined (Q_OS_MACOS)
    res = "MacOS";
#elif defined (Q_OS_UNIX)
    res = "Unix";
#endif

    return res;
}
//==============================================================================

} // namespace system_utils //==================================================


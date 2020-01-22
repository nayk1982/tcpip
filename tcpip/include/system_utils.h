#ifndef SYSTEM_UTILS_H
#define SYSTEM_UTILS_H

#include <QObject>
#include <QDateTime>

namespace system_utils { //=====================================================

void pause(int ms = 1000);
int dayOfWeek(int year, int month, int day);
bool osCmd(const QString &cmd, QString &out, int timeout=5000);
bool osCmd(const QString &cmd, QByteArray &out, int timeout=5000);
QString osName();

} // namespace system_utils //==================================================
#endif // SYSTEM_UTILS_H

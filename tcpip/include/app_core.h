#ifndef APP_CORE_H
#define APP_CORE_H

#include <QtCore>

namespace app_core { //=========================================================

bool initializeApplication(const QString &applicationName,
                           const QString &organizationName = QString(),
                           const QString &organizationDomain = QString());
void installTranslations(const QString &translationsDir = QString("translations"),
                         const QString &language = QString());
QString applicationVersion();
QString applicationBuildDate();
QString applicationFullPath();
QString applicationRootPath();
QString applicationProfilePath();
bool parameterExists(const QString &name, const QString &shortName = QString(),
                     QString *value = nullptr);
bool checkVersionQuery();

} // namespace app_core //======================================================
#endif // APP_CORE_H

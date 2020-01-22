#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include "FileSys"
#include "app_core.h"

namespace app_core { //=========================================================

using namespace file_sys;

//==============================================================================
bool initializeApplication(const QString &applicationName,
                           const QString &organizationName,
                           const QString &organizationDomain)
{
    QCoreApplication *application = QCoreApplication::instance();
    if(!application) return false;

    application->setApplicationName(applicationName);
    if(!organizationName.isEmpty()) application->setOrganizationName(organizationName);
    if(!organizationDomain.isEmpty()) application->setOrganizationDomain(organizationDomain);

#if defined (APP_VERSION)
    QString version = QString(APP_VERSION);
#if defined (APP_BUILD_DATE)
    QString buildDate = APP_BUILD_DATE;
    application->setProperty("buildDate", buildDate);
    version += QString(".%1.%2%3")
            .arg( buildDate.mid(2,2) )
            .arg( buildDate.mid(5,2) )
            .arg( buildDate.mid(8,2) );
#endif
    application->setApplicationVersion(version);
#endif

    if (checkVersionQuery()) return false;

    QString profilePath = applicationProfilePath();
    application->setProperty("profilePath", profilePath);

    installTranslations(applicationRootPath() + "translations", "ru");
    return true;
}
//==============================================================================
void installTranslations(const QString &translationsDir, const QString &language)
{
    QCoreApplication *application = QCoreApplication::instance();
    if(!application) return;

    QString lang = language.isEmpty() ? QLocale::system().name() : language;
    QDir dir(translationsDir);
    QStringList filesList = dir.entryList(QStringList("*_" + lang + ".qm"),
                                          QDir::Files);

    for(const QString &fileName: filesList) {

        QTranslator *translator = new QTranslator(application);
        if(!translator->load(translationsDir + directorySeparator + fileName)) {
            delete translator;
            continue;
        }
        application->installTranslator(translator);
    }
}
//==============================================================================
QString applicationVersion()
{
    return QCoreApplication::applicationVersion();
}
//==============================================================================
QString applicationBuildDate()
{
#if defined (APP_BUILD_DATE)
    return QString(APP_BUILD_DATE);
#else
    return tr("Unknown");
#endif
}
//==============================================================================
QString applicationFullPath()
{
    QString res = QDir::fromNativeSeparators( QCoreApplication::applicationDirPath() );
    if( res.right(1) != directorySeparator ) res += directorySeparator;
    return res;
}
//==============================================================================
QString applicationProfilePath()
{
    QString path = applicationRootPath();
    QString profileDir;

    if((fileExists( path + "portable" ) || parameterExists("portable"))
            && makePath(path + "profile")) {

        profileDir = path + "profile" + directorySeparator;
    }
    else {

        profileDir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation)
                + directorySeparator;
        QString appDirectory = QCoreApplication::organizationName()
                + directorySeparator
                + QCoreApplication::applicationName()
                + directorySeparator;

        if(!profileDir.contains( appDirectory )) {
            profileDir += appDirectory;
        }
    }

    return profileDir;
}
//==============================================================================
bool parameterExists(const QString &name, const QString &shortName, QString *value)
{
    QStringList params = QCoreApplication::arguments();
    int index = 1;

    while(index < params.size()) {

        QString param = params.at(index);
        index++;

        while(!param.isEmpty() && ((param[0] == '/') || (param[0] == '-'))) {
            param.remove(0,1);
        }

        if(param.isEmpty()) continue;

        if(!shortName.isEmpty() && (param == shortName)) {

            if(value && index < params.size()) *value = params.at(index);
            return true;
        }
        else {

            int n = param.indexOf('=');

            if(n > 0) {
                QString val = param.mid(n+1);
                param = param.left(n);

                if(param == name) {
                    if(value) *value = val;
                    return true;
                }
            }
            else if(param == name) {

                if(value && index < params.size()) *value = params.at(index);
                return true;
            }
        }
    }

    return false;
}
//==============================================================================
bool checkVersionQuery()
{
    if(!parameterExists("version", "ver")) return false;

    QTextStream standardOutput(stdout);
    standardOutput << QCoreApplication::applicationName() << endl;
    standardOutput << QObject::tr("Version: %1").arg(QCoreApplication::applicationVersion()) << endl;
#if defined (APP_BUILD_DATE)
    standardOutput << QObject::tr("Release Date: %1").arg(QString( APP_BUILD_DATE )) << endl;
#endif
    QCoreApplication::quit();
    return true;
}
//==============================================================================
QString applicationRootPath()
{
    QString path = applicationFullPath();

    if(path.right(5) == QString("%1bin%1").arg(directorySeparator)) {
        path.remove( path.length()-4, 4 );
    }

    return path;
}
//==============================================================================

} // namespace app_core //======================================================

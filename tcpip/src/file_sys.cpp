#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include "file_sys.h"

namespace file_sys { //=========================================================

bool directoryExists(const QString &dirName)
{
    QFileInfo dir(dirName);
    return dir.exists() && dir.isDir();
}
//==============================================================================
bool fileExists(const QString &fileName)
{
    QFileInfo file(fileName);
    return file.exists() && file.isFile();
}
//==============================================================================
bool makePath(const QString &path)
{
    QDir dir;
    return dir.mkpath(path);
}
//==============================================================================
QString changeFileExt(const QString &fileName, const QString &newExt)
{
    int i = fileName.lastIndexOf('.');

    if( newExt.length() > 0 ) {
        if(newExt[0] == '.') {
            return ((i>0) ? fileName.left( i ) : fileName) + newExt;
        }
        else {
            return ((i>0) ? fileName.left( i ) : fileName) + "." + newExt;
        }
    }
    else {
        return fileName;
    }
}
//==============================================================================
QString extractFileExt(const QString &fileName, bool withDot)
{
    QString f = extractFileName(fileName);
    int i = f.lastIndexOf('.');

    if(i >= 0) {
        if(withDot) {
            return f.right( f.length() - i );
        }
        else {
            return f.right( f.length() - i - 1 );
        }
    }
    else {
        return "";
    }
}
//==============================================================================
QString extractFileName(const QString &fileName)
{
    QString f = QDir::fromNativeSeparators(fileName);

    while((f.length()>0) && (f[f.length()-1] == directorySeparator)) {
        f.remove(f.length()-1,1);
    }

    int i;

#if defined (Q_OS_WIN32)
    i = f.indexOf(':');
    if(i >= 0) f.remove(0, i+1);
#endif

    i = f.lastIndexOf( directorySeparator );
    if(i>=0) f.remove(0, i+1);
    return f;
}
//==============================================================================
QString extractFilePath(const QString &fileName)
{
   QString f = QDir::fromNativeSeparators(fileName);
   int i = f.lastIndexOf( directorySeparator );
   return (i >= 0) ? f.left(i+1) : "";
}
//==============================================================================
bool readJsonFromFile(const QString &fileName, QJsonDocument &json, QString &errorString)
{
    QByteArray data;

    if(readFile(fileName, data, errorString)) {

        QJsonParseError parseError;
        json = QJsonDocument::fromJson( data, &parseError );

        if(parseError.error != QJsonParseError::NoError) {
            errorString = QObject::tr("Ошибка рабора файла \"%1\":")
                    .arg( extractFileName( fileName ) ) + "\n" + parseError.errorString();
            return false;
        }
    }
    else {
        return false;
    }

    return true;
}
//==============================================================================
bool readJsonFromFile(const QString &fileName, QJsonArray &json, QString &errorString)
{
    QJsonDocument doc;
    if(!readJsonFromFile(fileName, doc, errorString)) return false;

    if(!doc.isArray()) {
        errorString = QObject::tr("Файл \"%1\" не массив JSON")
                .arg( extractFileName( fileName ) );
        return false;
    }

    json = doc.array();
    return true;
}
//==============================================================================
bool readJsonFromFile(const QString &fileName, QJsonObject &json, QString &errorString)
{
    QJsonDocument doc;
    if(!readJsonFromFile(fileName, doc, errorString)) return false;

    if(!doc.isObject()) {
        errorString = QObject::tr("Файл \"%1\" не объект JSON")
                .arg( extractFileName( fileName ) );
        return false;
    }

    json = doc.object();
    return true;
}
//==============================================================================
bool writeJsonToFile(const QString &fileName, const QJsonDocument &json, QString &errorString)
{
    return writeFile(fileName, json.toJson(), errorString);
}
//==============================================================================
bool writeJsonToFile(const QString &fileName, const QJsonArray &json, QString &errorString)
{
    return writeJsonToFile(fileName, QJsonDocument(json), errorString);
}
//==============================================================================
bool writeJsonToFile(const QString &fileName, const QJsonObject &json, QString &errorString)
{
    return writeJsonToFile(fileName, QJsonDocument(json), errorString);
}
//==============================================================================
bool readFile(const QString &fileName, QByteArray &data, QString &errorString)
{
    errorString = "";
    QFile file( fileName );

    if(!file.exists()) {
        errorString =  QObject::tr("Файл \"%1\" не существует")
                .arg(extractFileName( fileName ));
        return false;
    }

    if(!file.open(QFile::ReadOnly)) {
        errorString = QObject::tr("Не удалось открыть файл \"%1\" для чтения")
                .arg( extractFileName( fileName ) );
        return false;
    }

    data = file.readAll();
    file.close();
    return true;
}
//==============================================================================
bool writeFile(const QString &fileName, const QByteArray &data, QString &errorString)
{
    errorString = "";

    QFile file( fileName );

    if(!file.open(QFile::WriteOnly)) {
        errorString = QObject::tr("Не удалось открыть файл \"%1\" для записи")
                .arg( extractFileName( fileName ) );
        return false;
    }

    int n = static_cast<int>(file.write( data ));
    bool res = file.flush();
    file.close();

    if(res && (n < data.size())) {
        errorString = QObject::tr("Не удалось записать все данные в файл \"%1\"")
                .arg( extractFileName( fileName ) );
        return false;
    }

    if(!res) {
        errorString = QObject::tr("Не удалось сохранить данные в файл \"%1\"")
                .arg( extractFileName( fileName ) );
        return false;
    }

    return true;
}
//==============================================================================

} // namespace file_sys //======================================================


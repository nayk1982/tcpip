#ifndef FILE_SYS_H
#define FILE_SYS_H

#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

namespace file_sys { //=========================================================

const QChar directorySeparator = '/';

//==============================================================================

bool directoryExists(const QString &dirName);
bool fileExists(const QString &fileName);
bool makePath(const QString &path);
QString changeFileExt(const QString &fileName, const QString &newExt);
QString extractFileExt(const QString &fileName, bool withDot = true);
QString extractFileName(const QString &fileName);
QString extractFilePath(const QString &fileName);
bool readJsonFromFile(const QString &fileName, QJsonDocument &json, QString &errorString);
bool readJsonFromFile(const QString &fileName, QJsonArray &json, QString &errorString);
bool readJsonFromFile(const QString &fileName, QJsonObject &json, QString &errorString);
bool writeJsonToFile(const QString &fileName, const QJsonDocument &json, QString &errorString);
bool writeJsonToFile(const QString &fileName, const QJsonArray &json, QString &errorString);
bool writeJsonToFile(const QString &fileName, const QJsonObject &json, QString &errorString);
bool readFile(const QString &fileName, QByteArray &data, QString &errorString);
bool writeFile(const QString &fileName, const QByteArray &data, QString &errorString);

} // namespace file_sys //======================================================
#endif // FILE_SYS_H

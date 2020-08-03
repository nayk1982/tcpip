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
#include <QDateTime>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlField>
#include "database_client.h"

namespace nayk { //=============================================================

const QString sqlErrorString = QObject::tr("SQL error: %1");

//==============================================================================
DataBaseClient::DataBaseClient(QObject *parent) : QObject(parent)
{
    initialize();
}
//==============================================================================
DataBaseClient::DataBaseClient(const QString &sqlDriver, QObject *parent)
    : QObject(parent)
    , m_sqlDriver(sqlDriver)
{
    initialize();
}
//==============================================================================
DataBaseClient::~DataBaseClient()
{
    close();
}
//==============================================================================
QString DataBaseClient::sqlDriver() const
{
    return m_sqlDriver;
}
//==============================================================================
void DataBaseClient::setSqlDriver(const QString &sqlDriver)
{
    if(m_sqlDriver != sqlDriver) {
        m_sqlDriver = sqlDriver;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change SQL-driver: %1").arg(m_sqlDriver), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
QString DataBaseClient::host() const
{
    return m_host;
}
//==============================================================================
void DataBaseClient::setHost(const QString &host)
{
    if(m_host != host) {
        m_host = host;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change connection host: %1").arg(m_host), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
int DataBaseClient::port() const
{
    return m_port;
}
//==============================================================================
void DataBaseClient::setPort(const int &port)
{
    if(m_port != port) {
        m_port = port;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change connection port: %1").arg(m_port), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
QString DataBaseClient::user() const
{
    return m_user;
}
//==============================================================================
void DataBaseClient::setUser(const QString &user)
{
    if(m_user != user) {
        m_user = user;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change connection username: %1").arg(m_user), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
QString DataBaseClient::password() const
{
    return m_password;
}
//==============================================================================
void DataBaseClient::setPassword(const QString &password)
{
    if(m_password != password) {
        m_password = password;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change connection password"), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
bool DataBaseClient::open(bool reconnect)
{
    if(reconnect)
        close();

    if(isOpen())
        return !reconnect;

    QSqlDatabase db = QSqlDatabase::addDatabase(m_sqlDriver, m_connectionName);

    if(!db.isValid()) {
        m_lastError = tr("Failed to create database connection");
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }

    db.setHostName(m_host);
    db.setDatabaseName(m_dataBase);
    db.setUserName(m_user);
    db.setPassword(m_password);
    db.setPort(m_port);

#if !defined(WITHOUT_LOG)
    emit toLog(tr("Database connection settings: host=%1; port=%2; base=%3; user=%4")
               .arg(db.hostName())
               .arg(db.port())
               .arg(db.databaseName())
               .arg(db.userName())
               , Log::LogDbg
               );
#endif

    if (!db.open()) {
        m_lastError = db.lastError().text();
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Failed to connect database: %1").arg( m_lastError ), Log::LogError);
#endif
        emit error();
        return false;
    }

#if !defined(WITHOUT_LOG)
    emit toLog(tr("Established database connection"), Log::LogInfo);
#endif
    emit opened();
    return true;
}
//==============================================================================
bool DataBaseClient::close()
{
    if(!isOpen())
        return true;

    db().close();
#if !defined(WITHOUT_LOG)
    emit toLog(tr("Database connection closed"), Log::LogInfo);
#endif
    emit closed();
    return true;
}
//==============================================================================
bool DataBaseClient::isOpen(bool reconnect)
{
    return QSqlDatabase::database(m_connectionName, reconnect).isOpen();
}
//==============================================================================
QString DataBaseClient::lastError() const
{
    return m_lastError;
}
//==============================================================================
QString DataBaseClient::dataBase() const
{
    return m_dataBase;
}
//==============================================================================
void DataBaseClient::setDataBase(const QString &dataBase)
{
    if(m_dataBase != dataBase) {
        m_dataBase = dataBase;
#if !defined(WITHOUT_LOG)
        emit toLog(tr("Change database name: %1").arg(m_dataBase), Log::LogDbg);
#endif
        if(isOpen()) open(true);
    }
}
//==============================================================================
QSqlDatabase DataBaseClient::db() const
{
    return QSqlDatabase::database(m_connectionName, false);
}
//==============================================================================
bool DataBaseClient::execSQL(const QString &sqlText, QSqlQuery *query, bool withTransaction)
{
    if(!isOpen(true)) {
        m_lastError = sqlErrorString.arg(tr("Database connection not established"));
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }

    if(!query) {
        m_lastError = sqlErrorString.arg(tr("Incorrect QSqlQuery"));
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }

    QSqlDatabase dataBase = db();
    if(withTransaction) dataBase.transaction();

    if(!query->exec(sqlText)) {
        m_lastError = sqlErrorString.arg(query->lastError().text());
        if(withTransaction) dataBase.rollback();
#if !defined(WITHOUT_LOG)
        emit toLog("SQL: " + sqlText, Log::LogDbg);
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }
    if(withTransaction) dataBase.commit();
    return true;
}
//==============================================================================
bool DataBaseClient::execSQL(const QString &sqlText)
{
    QSqlQuery query(db());
    return execSQL(sqlText, &query, true);
}
//==============================================================================
bool DataBaseClient::tableExist(const QString &tableName)
{
    return isOpen() && db().tables().contains( tableName );
}
//==============================================================================
bool DataBaseClient::getTables(QStringList &list)
{
    if(!isOpen()) {
        m_lastError = sqlErrorString.arg(tr("Database connection not established"));
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }

    list = db().tables(QSql::Tables);
    return true;
}
//==============================================================================
bool DataBaseClient::getTableFields(const QString &tableName, QStringList &list)
{
    if(!isOpen()) {
        m_lastError = sqlErrorString.arg(tr("Database connection not established"));
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        emit error();
        return false;
    }

    if(!tableExist(tableName)) {
        m_lastError = tr("Table '%1' does not exist").arg(tableName);
#if !defined(WITHOUT_LOG)
        emit toLog(m_lastError, Log::LogError);
#endif
        return false;
    }

    QString sql = QString("SELECT * FROM \"%1\" LIMIT 1;").arg(tableName);
    QSqlQuery query(db());

    if(!execSQL(sql, &query)) return false;

    QSqlRecord rec = query.record();

    list.clear();

#if !defined(WITHOUT_LOG)
    emit toLog(tr("'%1' table fields:"), Log::LogDbg);
#endif

    for(auto i = 0; i < rec.count(); ++i) {

        QString fieldName = rec.field(i).name();
#if !defined(WITHOUT_LOG)
        emit toLog(QString("'%1'").arg(fieldName), Log::LogDbg);
#endif
        list.append(fieldName);
    }

    return true;
}
//==============================================================================
void DataBaseClient::initialize()
{
    m_connectionName = QString("connect_%1")
            .arg(QDateTime::currentMSecsSinceEpoch());
}
//==============================================================================

} // namespace nayk //==========================================================

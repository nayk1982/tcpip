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
#ifndef DATABASE_CLIENT_H
#define DATABASE_CLIENT_H

#include <QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

#if !defined(WITHOUT_LOG)
#   include "Log"
#endif

namespace nayk { //=============================================================

class DataBaseClient : public QObject
{
    Q_OBJECT
public:
    explicit DataBaseClient(QObject *parent = nullptr);
    explicit DataBaseClient(const QString &sqlDriver, QObject *parent = nullptr);
    virtual ~DataBaseClient();

    QString sqlDriver() const;
    void setSqlDriver(const QString &sqlDriver);
    QString host() const;
    void setHost(const QString &host);
    int port() const;
    void setPort(const int &port);
    QString user() const;
    void setUser(const QString &user);
    QString password() const;
    void setPassword(const QString &password);
    bool open(bool reconnect = false);
    bool close();
    bool isOpen(bool reconnect = false);
    QString lastError() const;
    QString dataBase() const;
    void setDataBase(const QString &dataBase);
    QSqlDatabase db() const;
    bool execSQL(const QString &sqlText, QSqlQuery *query, bool withTransaction = false);
    bool execSQL(const QString &sqlText);
    bool tableExist(const QString &tableName);
    bool getTables(QStringList &list);
    bool getTableFields(const QString &tableName, QStringList &list);

signals:
#if !defined(WITHOUT_LOG)
    void toLog(const QString &text, Log::LogType logType = Log::LogInfo);
#endif
    void opened();
    void closed();
    void error();

private:
    QString m_lastError {""};
    QString m_sqlDriver {"QPSQL"};
    QString m_host {"localhost"};
    int m_port {5432};
    QString m_user {""};
    QString m_password {""};
    QString m_dataBase {""};
    QString m_connectionName {"default"};

    void initialize();
};

} // namespace nayk //==========================================================
#endif // DATABASE_CLIENT_H

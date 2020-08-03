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
#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <QObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QUrl>
#include <QVariant>
#include <QVariantMap>
#include <QMap>
#include <QString>
#include <QStringList>

#if !defined (WITHOUT_LOG)
#   include "Log"
#endif

namespace nayk { //=============================================================

class HttpServer : public QObject
{
    Q_OBJECT
public:
    explicit HttpServer(QObject *parent = nullptr);
    virtual ~HttpServer();
    bool readRequest();
    bool writeResponse();
    int requestTimeOut() const;
    void setRequestTimeOut(int requestTimeOut);
    QString lastError() const;
    QVariant cookie(const QString &cookieName) const;
    bool requestParameterExist(const QString &parameterName) const;
    QVariant getParameter(const QString &parameterName) const;
    QVariant postParameter(const QString &parameterName) const;
    QVariant requestParameter(const QString &parameterName) const;
    QVariant requestHeader(const QString &headerName) const;
    QVariant responseHeader(const QString &headerName) const;
    void setResponseHeader(const QString &headerName, const QVariant &headerValue);
    QByteArray responseContent() const;
    void setResponseContent(const QByteArray &responseContent);
    QByteArray requestContent() const;

signals:
#if !defined (WITHOUT_LOG)
    void toLog(const QString &text, Log::LogType logType = Log::LogInfo);
#endif
    void error();
    void requestReading();
    void responseWriting();

private:
    int m_requestTimeOut {300000};
    QString m_lastError {""};
    QVariantMap m_cookies;
    QVariantMap m_getParameters;
    QVariantMap m_postParameters;
    QVariantMap m_requestHeaders;
    QVariantMap m_responseHeaders;
    QByteArray m_requestContent;
    QByteArray m_responseContent;

    bool processCookies();
    bool processHeaders();
    bool processGet();
    bool processPost();
    bool readContent();
    QVariantMap decodeQuery(const QString &queryStr, const QString &pairSeparator = QString("&")) const;
    QString encodeQuery(QVariantMap queryMap, const QString &pairGlue = QString("&")) const;
    void sendLastError();
};

} // namespace nayk //==========================================================
#endif // HTTP_SERVER_H

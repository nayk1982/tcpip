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
#ifndef NAYK_HTTP_CLIENT_H
#define NAYK_HTTP_CLIENT_H

#include <QObject>
#include <QtNetwork/QtNetwork>
#include <QtNetwork/QHttpMultiPart>
#include <QByteArray>
#include <QString>
#include <QJsonDocument>
#include "HttpConst"

namespace nayk { //=============================================================


//==============================================================================
class HttpClient : public QObject
{
    Q_OBJECT

public:
    enum ProxyType { ProxyHTTP, ProxyHTTPS, ProxySOCKS4, ProxySOCKS5 };
    Q_ENUM(ProxyType)

    explicit HttpClient(QObject *parent = nullptr);
    virtual ~HttpClient();
    virtual bool sendRequest();
    virtual bool sendRequestAPI();
    virtual bool sendRequestMultipart(QHttpMultiPart *multiPart);
    virtual bool sendRequestHttp(bool getRequest = false);
    //
    QString lastError() const;
    void setProxySettings(bool useProxy = false, const QString &addr = "127.0.0.1", quint16 port = 3128,
                          ProxyType proxyType = ProxyHTTP, const QString &login = QString(), const QString &pas = QString() );
    void setRequestTimeOut(qint64 timeOut);
    void setURL(const QString &url);
    void setFileNameForSave(const QString &fileName);
    void setRequestData(const QByteArray &data);
    void setContentType(const QString &contentType);
    QString contentType() const;
    QString url() const;
    QByteArray replyData() const;
    QByteArray requestData() const;
    bool sendRequest(qint64 maxWaitTime);
    bool sendRequest(const QByteArray &jsonData);
    bool sendRequest(const QByteArray &jsonData, qint64 maxWaitTime);
    bool sendRequest(const QString &url, const QByteArray &jsonData, qint64 maxWaitTime);
    bool sendRequest(const QString &url);
    bool sendRequestAPI(const QString &cmd);
    bool sendRequestAPI(const QStringList &paramsList);
    bool sendRequestAPI(const QByteArray &jsonData);
    void addParam(const QString &paramName, const QString &paramValue);
    void addParam(const QString &paramName, qint32 paramValue);
    void addParam(const QString &paramName, qint64 paramValue);
    void addParam(const QString &paramName, double paramValue);
    void addParam(const QString &paramName, QDate paramValue);
    void addParam(const QString &paramName, QDateTime paramValue);
    QJsonDocument jsonAnswer();
    void clearParams();
    //
    static bool downloadData(const QString &url, QByteArray &data, qint64 maxWaitTime = 10000,
                             const QMap<QString, QString> &headers = QMap<QString,QString>(),
                             const QMap<QString,QString> &post = QMap<QString,QString>());
    static bool downloadFile(const QString &url, const QString &fileName, qint64 maxWaitTime=0);
    static bool downloadData(const QString &url, const QJsonObject &jsonRequest, QJsonObject &jsonAnswer,
                             qint64 maxWaitTime = 10000, const QMap<QString, QString> &headers = QMap<QString,QString>());

signals:
    void abortRequest();

private:
    QString m_url {""};
    QString m_fileName {""};
    QByteArray m_requestData;
    qint64 m_requestTimeOut {10000};
    bool m_useProxy {false};
    QNetworkProxy m_proxy;
    QString m_contentType {contentTypeJSON};
    QMap<QString, QString> m_params;
    //
    QByteArray requestParamsData() const;

protected:
    QString m_lastError {""};
    QByteArray m_answer;

};
//===============================================================================
} // namespace nayk
#endif // NAYK_HTTP_CLIENT_H

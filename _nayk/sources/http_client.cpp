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
#include <QTimer>
#include <QEventLoop>
#include <QFile>
#include <QTextStream>
#include "Convert"
#include "http_client.h"

namespace nayk { //=============================================================

//==============================================================================
HttpClient::HttpClient(QObject *parent) : QObject(parent)
{
    setProxySettings();
}
//==============================================================================
HttpClient::~HttpClient()
{
    emit abortRequest();
}
//==============================================================================
void HttpClient::setProxySettings(bool useProxy, const QString &addr, quint16 port, ProxyType proxyType, const QString &login, const QString &pas)
{
    m_useProxy = useProxy;
    m_proxy.setHostName( addr );
    m_proxy.setPort( port );
    m_proxy.setType( m_useProxy ?
                      ((proxyType == ProxyHTTP) ? QNetworkProxy::HttpProxy : QNetworkProxy::Socks5Proxy)
                      : QNetworkProxy::NoProxy );
    m_proxy.setUser( login );
    m_proxy.setPassword( pas );
}
//==============================================================================
bool HttpClient::sendRequestMultipart(QHttpMultiPart *multiPart)
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval( static_cast<int>(m_requestTimeOut));
    timer.setSingleShot(true);

    m_answer.clear();
    m_lastError = "";

    QNetworkRequest request{ QUrl { m_url } };
    request.setSslConfiguration( QSslConfiguration::defaultConfiguration() );

    QNetworkAccessManager manager;

    if(m_useProxy) { // настройки прокси если включен
        manager.setProxy(m_proxy);
    }

    QNetworkReply* reply = manager.post(request, multiPart);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, &timer, &QTimer::stop);
    QObject::connect(this, &HttpClient::abortRequest, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
    {
        m_answer = reply->readAll();
    }
    else
    {
        m_lastError = reply->errorString();
    }

    if(!m_fileName.isEmpty()) {
        QFile file(m_fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream stream( &file );
            if(m_lastError.isEmpty())
                stream << QString::fromUtf8( m_answer );
            else
                stream << m_lastError << "\n";
            file.close();
        }
    }

    reply->deleteLater();

    return m_lastError.isEmpty();
}
//==============================================================================
bool HttpClient::sendRequest()
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(static_cast<int>(m_requestTimeOut));
    timer.setSingleShot(true);

    m_answer.clear();
    m_lastError = "";

    QNetworkRequest request{ QUrl { m_url } };
    request.setHeader(QNetworkRequest::ContentTypeHeader, m_contentType);
    request.setSslConfiguration( QSslConfiguration::defaultConfiguration() );

    QNetworkAccessManager manager;

    if(m_useProxy) { // настройки прокси если включен
        manager.setProxy(m_proxy);
    }

    QNetworkReply* reply = manager.post(request, m_requestData);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, &timer, &QTimer::stop);
    QObject::connect(this, &HttpClient::abortRequest, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
    {
        m_answer = reply->readAll();
    }
    else
    {
        m_lastError = reply->errorString();
    }

    if(!m_fileName.isEmpty()) {
        QFile file(m_fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream stream( &file );
            if(m_lastError.isEmpty())
                stream << QString::fromUtf8( m_answer );
            else
                stream << m_lastError << "\n";
            file.close();
        }
    }

    reply->deleteLater();

    return m_lastError.isEmpty();
}
//==============================================================================
QByteArray HttpClient::requestParamsData() const
{
    auto b = m_params.begin();
    auto e = m_params.end();

    QByteArray byteArrayData;

    while (b != e) {

        byteArrayData.append(b.key());
        byteArrayData.append('=');
        byteArrayData.append( QUrl::toPercentEncoding( b.value() )  );
        byteArrayData.append('&');

        b++;
    }

    byteArrayData.chop(1);
    return byteArrayData;
}
//==============================================================================
bool HttpClient::sendRequestHttp(bool getRequest)
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(static_cast<int>(m_requestTimeOut));
    timer.setSingleShot(true);

    m_answer.clear();
    m_lastError = "";

    QUrl url(m_url);
    if(getRequest) url.setQuery( requestParamsData() );

    QNetworkRequest request( url );
    if(!getRequest) request.setHeader(QNetworkRequest::ContentTypeHeader, contentTypeWWWForm);
    request.setSslConfiguration( QSslConfiguration::defaultConfiguration() );

    QNetworkAccessManager manager;

    if(m_useProxy) { // настройки прокси если включен
        manager.setProxy(m_proxy);
    }

    QNetworkReply* reply = getRequest ? manager.get(request) : manager.post(request, requestParamsData());

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, &timer, &QTimer::stop);
    QObject::connect(this, &HttpClient::abortRequest, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError)
    {
        m_answer = reply->readAll();
    }
    else
    {
        m_lastError = reply->errorString();
    }

    if(!m_fileName.isEmpty()) {
        QFile file(m_fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream stream( &file );
            if(m_lastError.isEmpty())
                stream << QString::fromUtf8( m_answer );
            else
                stream << m_lastError << "\n";
            file.close();
        }
    }

    reply->deleteLater();

    return m_lastError.isEmpty();
}
//==============================================================================
bool HttpClient::sendRequest(qint64 maxWaitTime)
{
    m_requestTimeOut = maxWaitTime;
    return sendRequest();
}
//==============================================================================
bool HttpClient::sendRequest(const QByteArray &jsonData)
{
    m_requestData = jsonData;
    return sendRequest();
}
//==============================================================================
bool HttpClient::sendRequest(const QByteArray &jsonData, qint64 maxWaitTime)
{
    m_requestTimeOut = maxWaitTime;
    m_requestData = jsonData;
    return sendRequest();
}
//==============================================================================
bool HttpClient::sendRequest(const QString &url, const QByteArray &jsonData, qint64 maxWaitTime)
{
    m_requestTimeOut = maxWaitTime;
    m_requestData = jsonData;
    m_url = url;
    return sendRequest();
}
//==============================================================================
bool HttpClient::sendRequest(const QString &url)
{
    m_url = url;
    return sendRequest();
}
//==============================================================================
bool HttpClient::sendRequestAPI(const QByteArray &jsonData)
{
    m_params.clear();
    m_requestData = jsonData;
    return sendRequestAPI();
}
//==============================================================================
bool HttpClient::sendRequestAPI(const QString &cmd)
{
    m_params.clear();
    addParam("cmd", cmd);
    return sendRequestAPI();
}
//==============================================================================
bool HttpClient::sendRequestAPI(const QStringList &paramsList)
{
    m_params.clear();
    for(int i=0; i<paramsList.size(); i++) {
        QString str = paramsList.at(i);
        QStringList sl = str.split("=", QString::SkipEmptyParts);
        if(sl.size() == 2) addParam( sl.first(), sl.last() );
    }
    return sendRequestAPI();
}
//==============================================================================
bool HttpClient::sendRequestAPI()
{
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(static_cast<int>(m_requestTimeOut));
    timer.setSingleShot(true);

    m_answer.clear();
    m_lastError = "";

    QNetworkRequest request{ QUrl { m_url } };
    request.setSslConfiguration( QSslConfiguration::defaultConfiguration() );

    QNetworkAccessManager manager;

    if(m_useProxy) { // настройки прокси если включен
        manager.setProxy(m_proxy);
    }

    if(m_params.count()) {
        m_requestData.clear();
        QMap<QString, QString>::iterator itr;

        for (itr = m_params.begin(); itr != m_params.end(); ++itr) {
            m_requestData.append(itr.key());
            m_requestData.append('=');
            m_requestData.append( QUrl::toPercentEncoding( itr.value() )  );
            m_requestData.append('&');
        }
        m_requestData.chop(1);
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentTypeWWWForm);
    }
    else {
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentTypeJSON);
    }

    QNetworkReply* reply = manager.post(request, m_requestData);

    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, reply, &QNetworkReply::abort);
    QObject::connect(this, &HttpClient::abortRequest, &timer, &QTimer::stop);
    QObject::connect(this, &HttpClient::abortRequest, &loop, &QEventLoop::quit);

    timer.start();
    loop.exec();

    if (reply->isFinished() && (reply->error() == QNetworkReply::NoError))
    {
        m_answer = reply->readAll();
    }
    else
    {
        m_lastError = reply->errorString();
        if(m_lastError.isEmpty()) m_lastError = QObject::tr("Неизвестная ошибка.");
    }

    if(!m_fileName.isEmpty()) {
        QFile file(m_fileName);
        if(file.open(QIODevice::WriteOnly)) {
            QTextStream stream( &file );
            if(m_lastError.isEmpty())
                stream << QString::fromUtf8( m_answer );
            else
                stream << m_lastError << "\n";
            file.close();
        }
    }

    reply->deleteLater();

    if(m_lastError.isEmpty()) {
        QJsonDocument doc = QJsonDocument::fromJson( m_answer );
        if(doc.isNull() || doc.isEmpty() || !doc.isObject()) {
            m_lastError = QObject::tr("Ошибка при разборе ответа JSON.");
            return false;
        }
        QJsonObject obj = doc.object();
        if(!obj.contains("error") || !obj.contains("server") || !obj.value("error").isObject() || !obj.value("server").isObject()) {
            m_lastError = QObject::tr("Некорректный ответ JSON.");
            return false;
        }
        QJsonObject err = obj.value("error").toObject();
        if(err.value("code").toInt(0) > 0) {
            m_lastError = err.value("text").toString();
            if(m_lastError.isEmpty()) m_lastError = QObject::tr("Неизвестная ошибка.");
            return false;
        }
    }

    return m_lastError.isEmpty();
}
//==============================================================================
QJsonDocument HttpClient::jsonAnswer()
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson( m_answer, &error );

    if (error.error != QJsonParseError::NoError) {
        m_lastError = error.errorString();
        return QJsonDocument();
    }
    return doc;
}
//==============================================================================
bool HttpClient::downloadFile(const QString &url, const QString &fileName, qint64 maxWaitTime)
{
    QByteArray data;
    if(!downloadData( url, data, maxWaitTime )) return false;

    QFile file( fileName );
    if(!file.open( QIODevice::WriteOnly )) return false;

    file.write(data);

    file.close();
    return true;
}
//==============================================================================
bool HttpClient::downloadData(const QString &url, QByteArray &data, qint64 maxWaitTime,
                            const QMap<QString,QString> &headers, const QMap<QString,QString> &post)
{
    bool res = false;
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(static_cast<int>((maxWaitTime == 0) ? 2000 : maxWaitTime));
    timer.setSingleShot(true);

    data.clear();
    QNetworkRequest request{ QUrl { url } };

    if(!headers.isEmpty()) {
        foreach (QString strHeader, headers.keys()) {
            request.setRawHeader( strHeader.toUtf8(), QString(headers.value(strHeader)).toUtf8() );
        }
    }
    QByteArray postData;
    if(!post.isEmpty()) {
        request.setHeader(QNetworkRequest::ContentTypeHeader, contentTypeWWWForm);
        foreach (QString strPar, post.keys()) {
            postData.append( strPar );
            postData.append( "=" );
            postData.append( QUrl::toPercentEncoding( post.value(strPar) ) );
            postData.append( "&" );
        }
        postData.chop(1);
    }

    QNetworkAccessManager manager;

    QNetworkReply* reply = post.isEmpty() ? manager.get(request) : manager.post(request, postData);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    timer.start();
    loop.exec();
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {

        data = reply->readAll();
        res = true;
    }
    reply->deleteLater();
    return res;
}
//==============================================================================
bool HttpClient::downloadData(const QString &url, const QJsonObject &jsonRequest, QJsonObject &jsonAnswer,
                             qint64 maxWaitTime, const QMap<QString, QString> &headers)
{
    bool res = false;
    QEventLoop loop;
    QTimer timer;
    timer.setInterval(static_cast<int>((maxWaitTime == 0) ? 2000 : maxWaitTime));
    timer.setSingleShot(true);

    QNetworkRequest request{ QUrl { url } };

    if(!headers.isEmpty()) {
        foreach (QString strHeader, headers.keys()) {
            request.setRawHeader( strHeader.toUtf8(), QString(headers.value(strHeader)).toUtf8() );
        }
    }
    QByteArray postData = QJsonDocument(jsonRequest).toJson(QJsonDocument::Compact);
    request.setHeader(QNetworkRequest::ContentTypeHeader, contentTypeJSON);

    QNetworkAccessManager manager;

    QNetworkReply* reply = manager.post(request, postData);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);
    timer.start();
    loop.exec();
    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {

        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson( reply->readAll(), &err );
        res = (err.error == QJsonParseError::NoError);
        if(res && !doc.isNull() && doc.isObject()) jsonAnswer = doc.object();
        else res = false;
    }
    reply->deleteLater();
    return res;
}
//==============================================================================
QString HttpClient::lastError() const
{
    return m_lastError;
}
//==============================================================================
void HttpClient::setRequestTimeOut(qint64 timeOut)
{
    m_requestTimeOut = timeOut;
}
//==============================================================================
void HttpClient::setURL(const QString &url)
{
    m_url = url;
}
//==============================================================================
void HttpClient::setFileNameForSave(const QString &fileName)
{
    m_fileName = fileName;
}
//==============================================================================
void HttpClient::setRequestData(const QByteArray &data)
{
    m_requestData = data;
}
//==============================================================================
void HttpClient::setContentType(const QString &contentType)
{
    m_contentType = contentType;
}
//==============================================================================
QString HttpClient::contentType() const
{
    return m_contentType;
}
//==============================================================================
QString HttpClient::url() const
{
    return m_url;
}
//==============================================================================
QByteArray HttpClient::replyData() const
{
    return m_answer;
}
//==============================================================================
QByteArray HttpClient::requestData() const
{
    return m_requestData;
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, const QString &paramValue)
{
    m_params[paramName] = paramValue;
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, qint32 paramValue)
{
    m_params[paramName] = QString::number( paramValue );
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, qint64 paramValue)
{
    m_params[paramName] = QString::number( paramValue );
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, double paramValue)
{
    m_params[paramName] = convert::doubleToStr(paramValue, 8);
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, QDate paramValue)
{
    m_params[paramName] = paramValue.toString("yyyy-MM-dd");
}
//==============================================================================
void HttpClient::addParam(const QString &paramName, QDateTime paramValue)
{
    m_params[paramName] = paramValue.toString("yyyy-MM-dd HH:mm:ss");
}
//==============================================================================
void HttpClient::clearParams()
{
    m_params.clear();
}
//==============================================================================

} // namespace nayk //==========================================================

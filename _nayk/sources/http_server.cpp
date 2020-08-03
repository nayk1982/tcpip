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
#include <QCoreApplication>
#include <QTextCodec>
#include <QIODevice>

#include <string>
#include <iostream>
#include <stdio.h>

#ifdef Q_OS_WIN
#include <fcntl.h>
#include <io.h>
#endif

#include "Convert"
#include "SystemUtils"
#include "HttpConst"
#include "http_server.h"

//==============================================================================

extern char** environ;

namespace nayk { //=============================================================

HttpServer::HttpServer(QObject *parent) : QObject(parent)
{
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF-8") );
}
//==============================================================================
HttpServer::~HttpServer()
{

}
//==============================================================================
bool HttpServer::readRequest()
{
#if !defined(WITHOUT_LOG)
    emit toLog(tr("Read HTTP request"), Log::LogInfo);
#endif

    if(processHeaders()) {
#if !defined(WITHOUT_LOG)
        if(!m_requestHeaders.isEmpty()) {
            emit toLog(tr("HTTP headers:"), Log::LogDbg);
            for(auto itr = m_requestHeaders.begin(); itr != m_requestHeaders.end(); ++itr) {
                if(!itr.value().toString().isNull())
                    emit toLog(QString("%1: %2").arg(itr.key()).arg(itr.value().toString()), Log::LogDbg);
            }
        }
#endif
    }
    else {
        sendLastError();
        return false;
    }

    if(processCookies()) {
#if !defined(WITHOUT_LOG)
        if(!m_cookies.isEmpty()) {
            emit toLog(tr("Cookies:"), Log::LogDbg);
            for(auto itr = m_cookies.begin(); itr != m_cookies.end(); ++itr) {
                if(!itr.value().toString().isNull())
                    emit toLog(QString("%1: %2").arg(itr.key()).arg(itr.value().toString()), Log::LogDbg);
            }
        }
#endif
    }
    else {
        sendLastError();
        return false;
    }

    if(processGet()) {
#if !defined(WITHOUT_LOG)
        if(!m_getParameters.isEmpty()) {
            emit toLog(tr("GET parameters:"), Log::LogDbg);
            for(auto itr = m_getParameters.begin(); itr != m_getParameters.end(); ++itr) {
                if(!itr.value().toString().isNull())
                    emit toLog(QString("%1: %2").arg(itr.key()).arg(itr.value().toString()), Log::LogDbg);
            }
        }
#endif
    }
    else {
        sendLastError();
        return false;
    }

    if(processPost()) {
#if !defined(WITHOUT_LOG)
        if(!m_postParameters.isEmpty()) {
            emit toLog(tr("POST parameters:"), Log::LogDbg);
            for(auto itr = m_postParameters.begin(); itr != m_postParameters.end(); ++itr) {
                if(!itr.value().toString().isNull())
                    emit toLog(QString("%1: %2").arg(itr.key()).arg(itr.value().toString()), Log::LogDbg);
            }
        }
#endif
    }
    else {
        sendLastError();
        return false;
    }

    emit requestReading();
    return true;
}
//==============================================================================
bool HttpServer::writeResponse()
{
    if(!m_responseHeaders.contains(headerContentLength))
        m_responseHeaders[headerContentLength] = m_responseContent.size();

    QByteArray headers;
#if !defined(WITHOUT_LOG)
    emit toLog(tr("Response headers:"), Log::LogDbg);
#endif

    for (auto itr = m_responseHeaders.begin(); itr != m_responseHeaders.end(); ++itr) {
        headers.append( QString(itr.key() + ": " + itr.value().toString() + "\r\n").toUtf8() );
#if !defined(WITHOUT_LOG)
        emit toLog(QString("%1: %2").arg(itr.key()).arg(itr.value().toString()), Log::LogDbg);
#endif
    }
    headers.append( QString("\r\n").toUtf8() );

#ifdef Q_OS_WIN
    _setmode(_fileno(stdout), _O_BINARY);
#endif

    QFile standardOutput;
    if(!standardOutput.open(stdout, QIODevice::WriteOnly)) {
        m_lastError = tr("Failed to open stdout");
        sendLastError();
        return false;
    }

#if !defined(WITHOUT_LOG)
    emit toLog(tr("Send HTTP response headers"), Log::LogInfo);
#endif

    int index = 0;
    qint64 startTime = QDateTime::currentMSecsSinceEpoch();
    bool isTimeOut = false;

    while(!isTimeOut && (index < headers.size())) {
        qint64 n = standardOutput.write( headers );
        index += static_cast<int>(n);
        isTimeOut = (QDateTime::currentMSecsSinceEpoch() - startTime) > m_requestTimeOut;
    }

    if(isTimeOut) {
        m_lastError = tr("Response header timeout");
        sendLastError();
        return false;
    }

    if(index < headers.size()) {
        m_lastError = tr("Failed to send response headers");
        sendLastError();
        return false;
    }

#if !defined(WITHOUT_LOG)
    emit toLog(tr("Send HTTP response content"), Log::LogInfo);
#endif

    index = 0;

    while(!isTimeOut && (index < m_responseContent.size())) {
        qint64 n = standardOutput.write( m_responseContent.mid(index) );
        index += static_cast<int>(n);
        isTimeOut = (QDateTime::currentMSecsSinceEpoch() - startTime) > m_requestTimeOut;
    }

    if(isTimeOut) {
        m_lastError = tr("Response content timeout");
        sendLastError();
        return false;
    }

    if(index < m_responseContent.size()) {
        m_lastError = tr("Failed to send response content");
        sendLastError();
        return false;
    }

    emit responseWriting();
    return true;
}
//==============================================================================
int HttpServer::requestTimeOut() const
{
    return m_requestTimeOut;
}
//==============================================================================
void HttpServer::setRequestTimeOut(int requestTimeOut)
{
    m_requestTimeOut = qBound(5, requestTimeOut, 1800000);
}
//==============================================================================
QString HttpServer::lastError() const
{
    return m_lastError;
}
//==============================================================================
QVariant HttpServer::cookie(const QString &cookieName) const
{
    return m_cookies.value(cookieName);
}
//==============================================================================
bool HttpServer::requestParameterExist(const QString &parameterName) const
{
    return m_getParameters.contains(parameterName)
            || m_postParameters.contains(parameterName);
}
//==============================================================================
QVariant HttpServer::getParameter(const QString &parameterName) const
{
    return m_getParameters.value(parameterName);
}
//==============================================================================
QVariant HttpServer::postParameter(const QString &parameterName) const
{
    return m_postParameters.value(parameterName);
}
//==============================================================================
QVariant HttpServer::requestParameter(const QString &parameterName) const
{
    return m_getParameters.contains(parameterName)
            ? m_getParameters.value(parameterName)
            : m_postParameters.value(parameterName);
}
//==============================================================================
QVariant HttpServer::requestHeader(const QString &headerName) const
{
    return m_requestHeaders.value(headerName);
}
//==============================================================================
QVariant HttpServer::responseHeader(const QString &headerName) const
{
    return m_responseHeaders.value(headerName);
}
//==============================================================================
void HttpServer::setResponseHeader(const QString &headerName, const QVariant &headerValue)
{
    m_responseHeaders[headerName] = headerValue;
}
//==============================================================================
QByteArray HttpServer::responseContent() const
{
    return m_responseContent;
}
//==============================================================================
void HttpServer::setResponseContent(const QByteArray &responseContent)
{
    m_responseContent = responseContent;
}
//==============================================================================
QByteArray HttpServer::requestContent() const
{
    return m_requestContent;
}
//==============================================================================
bool HttpServer::processCookies()
{
    QVariant cookies = m_requestHeaders.value(serverHeaderHttpCookie);

    if (cookies.isValid()) {
        m_cookies = decodeQuery(cookies.toString(), ";");
    }

    return true;
}
//==============================================================================
bool HttpServer::processHeaders()
{
    for( int i=0; environ[i]; ++i ) {
        QString envStr = QString(environ[i]);
        int n = envStr.indexOf("=");
        if(n<1) continue;
        QString key = envStr.left(n).trimmed().toUpper();
        QString val = envStr.mid(n + 1).trimmed();
        if(val.isEmpty()) continue;
        m_requestHeaders[key] = val;
    }
    return true;
}
//==============================================================================
bool HttpServer::processGet()
{
    QString query = m_requestHeaders.value(serverHeaderQueryString).toString();

    if (!query.isEmpty()) {
        m_getParameters = decodeQuery(query);
    }

    return true;
}
//==============================================================================
bool HttpServer::processPost()
{
    if (m_requestHeaders.value(serverHeaderRequestMethod).toString().toUpper()
            != methodPost)
        return true;

    bool ok = false;
    int contentLength = m_requestHeaders.value(serverHeaderContentLength).toInt(&ok);

    if(!ok || contentLength < 1) {
        m_lastError = tr("Incorrect ContentLength value");
        return false;
    }

    QString contentType = m_requestHeaders.value(serverHeaderContentType).toString();

    if(contentType.isEmpty()) {
        m_lastError = tr("Incorrect ContentType value");
        return false;
    }

    QStringList sList =  contentType.split(";", QString::SkipEmptyParts);
    contentType = sList.first().toUpper().trimmed();

    if(contentType == contentTypeWWWForm.toUpper().trimmed()) {

        std::string strPostData;
        std::getline(std::cin, strPostData);

        if (!QString::fromStdString(strPostData).isEmpty()) {
            m_postParameters = decodeQuery(QString(strPostData.c_str()));
        }
        return true;
    }

    if(!readContent()) {
        return false;
    }

    if(contentType != contentTypeMultipartForm.toUpper().trimmed())
        return true;

    QString boundary = "";

    for(int i=1; i<sList.count(); ++i) {

        QString strVal = QString(sList.at(i)).trimmed();

        if (strVal.toUpper().left(9) == "BOUNDARY=") {
            strVal.remove(0,9);
            boundary = strVal.trimmed();
            break;
        }
    }

    if(boundary.isEmpty()) {
        m_lastError = tr("Boundary not found");
        return false;
    }

    QString endl = "\r\n";

    if( m_requestContent.contains( QString("\r\n--"+boundary+"\r\n").toLocal8Bit() ) )
        endl = "\r\n";
    else if( m_requestContent.contains( QString("\r--"+boundary+"\r").toLocal8Bit() ) )
        endl = "\r";
    else if( m_requestContent.contains( QString("\n--"+boundary+"\n").toLocal8Bit() ) )
        endl = "\n";

    QString boundaryMiddle = "--" + boundary + endl;
    QString boundaryEnd = "--" + boundary + "--";
    bool itsEnd = false;

    while(!m_requestContent.isEmpty() && !itsEnd) {

        int ii = m_requestContent.indexOf( boundaryMiddle.toLocal8Bit() );
        if(ii < 0) break;
        m_requestContent.remove(0, ii + boundaryMiddle.length());
        if(m_requestContent.isEmpty()) break;

        ii = m_requestContent.indexOf( QString(endl + boundaryMiddle).toLocal8Bit() );

        if(ii < 0) {
            itsEnd = true;
            ii = m_requestContent.indexOf( QString(endl + boundaryEnd).toLocal8Bit() );
            if (ii < 0)
                ii = m_requestContent.indexOf( QString(boundaryEnd).toLocal8Bit() );
            if(ii < 0)
                ii = m_requestContent.size();
        }

        if(ii < 1) break;

        QByteArray bVal = m_requestContent.left(ii);
        m_requestContent.remove(0, ii);
        ii = bVal.indexOf( QString(endl+endl).toLocal8Bit() );

        if(ii < 1) continue;

        sList = QString(bVal).left(ii).split(endl, QString::SkipEmptyParts);
        bVal.remove(0, ii + 2 * endl.length() );
        if(bVal.isEmpty()) continue;

        QString sName = "";
        QString sFileName = "";
        QString sType = "BINARY";
        bool binType = false;
        bool jsonType = false;

        for(int i = 0; i < sList.count(); ++i) {

            if( QString( sList.at(i) ).toUpper().contains("CONTENT-DISPOSITION:") ) {
                ii = QString( sList.at(i) ).toUpper().indexOf(" NAME=\"");
                if(ii < 0)
                    ii = QString( sList.at(i) ).toUpper().indexOf(";NAME=\"");
                if(ii >= 0) {
                    sName = QString( sList.at(i) ).right( QString(sList.at(i)).length() - ii - 7 );
                    ii = sName.indexOf("\"");
                    if(ii > 0)
                        sName = sName.left(ii); else sName = "";
                }
                ii = QString( sList.at(i) ).toUpper().indexOf(" FILENAME=\"");
                if(ii < 0)
                    ii = QString( sList.at(i) ).toUpper().indexOf(";FILENAME=\"");
                if(ii >= 0) {
                    sFileName = QString( sList.at(i) ).right( QString(sList.at(i)).length() - ii - 11 );
                    ii = sFileName.indexOf("\"");
                    if(ii > 0)
                        sFileName = sFileName.left(ii); else sFileName = "";
                }
            }
            else if( QString( sList.at(i) ).toUpper().contains("CONTENT-TYPE:") ) {
                binType = QString( sList.at(i) ).toLower().contains(contentTypeBinary);
                jsonType = QString( sList.at(i) ).toLower().contains(contentTypeJSON);
            }
            else {
                ii = QString( sList.at(i) ).toUpper().indexOf("CONTENT-TRANSFER-ENCODING:");
                if(ii >= 0) {
                    sType = QString( sList.at(i) ).right(
                                QString(sList.at(i)).length() - ii - 26 ).trimmed().toUpper();
                    binType = (sType == "BINARY");
                }
            }
        }

        if(sName.isEmpty()) continue;

        if(binType || !sFileName.isEmpty()) {
            m_postParameters[sName] = bVal;
        }
        else if(jsonType) {

            QJsonDocument doc = QJsonDocument::fromJson(bVal);
            if(doc.isNull() || doc.isEmpty()) {
                m_lastError = tr("Incorrect Json format");
                return false;
            }
            m_postParameters[sName] = doc.toJson();
        }
        else {
            QString val = QString(bVal).trimmed();
            if(val.isEmpty()) val = "1";
            m_postParameters[sName] = val;
        }
    }

    return true;
}
//==============================================================================
bool HttpServer::readContent()
{
    m_requestContent.clear();
#if !defined(WITHOUT_LOG)
    emit toLog( tr("%1 - Begin request reading")
                .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")),
               Log::LogDbg);
#endif

#ifdef Q_OS_WIN
    _setmode(_fileno(stdin), _O_BINARY);
#endif
    QFile standardInput;
    if(!standardInput.open( stdin, QIODevice::ReadOnly )) {
        m_lastError = tr("Failed to open stdin");
        return false;
    }

    int readTimeOutMax = 60000;
    int contentLength = m_requestHeaders.value(serverHeaderContentLength).toInt();
    bool readTimeOut = false;
    bool queryTimeOut = false;
#if !defined(WITHOUT_LOG)
    int blockCnt = 0;
    QString queStr = "";
#endif
    qint64 startTimeMSec = QDateTime::currentMSecsSinceEpoch();
    qint64 curInTimeMSec = startTimeMSec;
    qint64 curMS, timeMS, sumTime = 0;

    while(!standardInput.atEnd() && !readTimeOut && !queryTimeOut) {

        curMS = QDateTime::currentMSecsSinceEpoch();

        if((curMS - curInTimeMSec) > readTimeOutMax) {
            readTimeOut = true;
            break;
        }
        if((curMS - startTimeMSec) > m_requestTimeOut) {
            queryTimeOut = true;
            break;
        }

        // засекаем текущее время:
        curMS = QDateTime::currentMSecsSinceEpoch();
        QByteArray readBuf = standardInput.read(1024);
        // вычисляем разницу:
        timeMS = QDateTime::currentMSecsSinceEpoch() - curMS;
        sumTime += timeMS;
#if !defined(WITHOUT_LOG)
        if(!queStr.isEmpty()) queStr += ", ";
        queStr += tr("%1 = %2 msec").arg(readBuf.size()).arg(timeMS);
#endif
        if(readBuf.isEmpty()) {
            system_utils::pause(20);
            continue;
        }

        m_requestContent.append(readBuf);

#if !defined(WITHOUT_LOG)
        QString logStr = "";
        for(int i=0; i<readBuf.size(); ++i) {
            logStr += " " + QString(readBuf.mid(i,1).toHex()).toUpper();
        }
        logStr = QString::number(blockCnt) + " =" + logStr;
        if(blockCnt < 10) logStr = "0" + logStr;
        ++blockCnt;

        emit toLog( tr("%1 BLOCK %2 [%3 bytes]")
                    .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz"))
                    .arg(logStr)
                    .arg(readBuf.size())
                    , Log::LogDbg );
#endif
        if(m_requestContent.count() >= contentLength) break;
        curInTimeMSec = QDateTime::currentMSecsSinceEpoch();
    }
    standardInput.close();

#if !defined(WITHOUT_LOG)
    if(readTimeOut) {
        emit toLog("... READ-TIME-OUT ...", Log::LogError);
    }
    else if(queryTimeOut) {
        emit toLog("... QUERY-TIME-OUT ...", Log::LogError);
    }

    emit toLog(tr("%1 - End request reading")
               .arg(QDateTime::currentDateTime().toString("HH:mm:ss.zzz")), Log::LogDbg );
    emit toLog(tr("Read time and bytes count: %1").arg(queStr), Log::LogDbg );
    emit toLog(tr("Total bytes read = %1; Unread: content_length-read_count = %2")
               .arg(m_requestContent.size())
               .arg(contentLength - m_requestContent.size()), Log::LogDbg );
    emit toLog(tr("Total time read: %1 msec").arg(sumTime), Log::LogDbg );
#endif

    m_lastError.clear();

    if(readTimeOut) {
        m_lastError = tr("Reading timeout");
    }
    if(queryTimeOut) {
        m_lastError = tr("Request timeout");
    }

    if(!m_lastError.isEmpty()) {
        return false;
    }
#if !defined(WITHOUT_LOG)
    if(m_requestContent.size() != contentLength) {
        emit toLog(tr("Content-Length does not match size: %1")
                   .arg(m_requestContent.size()), Log::LogWarning );
    }
#endif
    return true;
}
//==============================================================================
QVariantMap HttpServer::decodeQuery(const QString &queryStr, const QString &pairSeparator) const
{
    QVariantMap result;
    QStringList parameters = queryStr.split(pairSeparator, QString::SkipEmptyParts);

    for (const QString &strPair: parameters) {

        QStringList pairList = strPair.split("=", QString::SkipEmptyParts);

        if(!pairList.isEmpty()) {

            QString val = (pairList.size()>1) ? QString(pairList.at(1)) : QString("");
            result[ QUrl::fromPercentEncoding(pairList.at(0).toLatin1()).replace("+", " ") ]
                    = QUrl::fromPercentEncoding(val.toLatin1()).replace("+", " ");
        }
    }

    return result;
}
//==============================================================================
QString HttpServer::encodeQuery(QVariantMap queryMap, const QString &pairGlue) const
{
    QStringList result;

    for (const QString &key: queryMap.keys()) {

        QStringList pairList;
        pairList.append(QString(QUrl::toPercentEncoding(key)).replace(" ", "+"));
        pairList.append(QString(QUrl::toPercentEncoding(queryMap.value(key).toString())).replace(" ", "+"));
        result.append(pairList.join("="));
    }

    return result.join(pairGlue);
}
//==============================================================================
void HttpServer::sendLastError()
{
#if !defined(WITHOUT_LOG)
   emit toLog(m_lastError, Log::LogError);
#endif
    emit error();
}
//==============================================================================

} // namespace nayk //==========================================================

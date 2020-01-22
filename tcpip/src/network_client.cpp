#include <QEventLoop>
#include <QTimer>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/QNetworkInterface>
#include <QStringList>
#include <QTextCodec>
#include "Convert"
#include "SystemUtils"
#include "network_client.h"

namespace network_client { //===================================================

constexpr int requestTimeOut = 4000;

//==============================================================================
bool simpleGet(const QString &url, QString &resultString)
{
    bool res { false };
    QEventLoop loop;
    QTimer timer;
    timer.setInterval( requestTimeOut );
    timer.setSingleShot(true);

    QNetworkRequest request { QUrl { url } };
    QNetworkAccessManager manager;
    QNetworkReply* reply = manager.get(request);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timer, &QTimer::timeout, reply, &QNetworkReply::abort);

    timer.start();
    loop.exec();
    timer.stop();

    if (reply->isFinished() && reply->error() == QNetworkReply::NoError) {

        resultString = QString( reply->readAll() ).trimmed();
        res = true;
    }
    else {

        resultString = QString( reply->errorString() ).trimmed();
        res = false;
    }

    reply->deleteLater();
    return res;
}
//==============================================================================
bool externalIP(QString &ip)
{
    static QStringList urls = { "https://icanhazip.com", "https://api.ipify.org",
                                "http://smart-ip.net/myip", "http://api.ipify.org",
                                "http://grio.ru/myip.php", "http://ifconfig.me/ip" };
    for(auto i=0; i<urls.size(); ++i) {
        if(simpleGet( urls.at(i), ip )) {

            QStringList ip4 = ip.split('.', QString::SkipEmptyParts);
            if(ip4.size() != 4) continue;
            bool okIP = true;
            for(auto j=0; j<ip4.size(); j++) {
                bool ok;
                int n = QString(ip4.at(j)).toInt(&ok, 10);
                okIP = okIP && ok && (n>=0) && (n<=255);
                if(!okIP) break;
            }
            if(okIP) return true;
        }
    }
    return false;
}
//==============================================================================
QString hostAddress()
{
    QList<QHostAddress> myIpAddresses = QNetworkInterface::allAddresses();
    for(const QHostAddress &addr: myIpAddresses) {

        if(addr.isNull() || addr.isLoopback() || addr.isMulticast()) continue;
        if(addr.protocol() == QAbstractSocket::IPv4Protocol) {

            return addr.toString();
        }
    }

    return QString();
}
//==============================================================================
QString connectionName(const QString &ip)
{
    if(ip.isEmpty()) return QString();
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();

    for(const QNetworkInterface &iface: list) {

        if(!iface.isValid()) continue;

        QList<QHostAddress> myIpAddresses = iface.allAddresses();

        for(const QHostAddress &addr: myIpAddresses) {

            if(addr.isNull()) continue;
            if(addr.toString() == ip) {

                return iface.humanReadableName();
            }
        }
    }

    return QString();
}
//==============================================================================
bool addressEntry(const QString &ip, QNetworkAddressEntry &addressEntry)
{
    if(ip.isEmpty()) return false;

    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface &iface: list) {

        if(!iface.isValid()) continue;

        QList<QNetworkAddressEntry> addrList = iface.addressEntries();

        for(const QNetworkAddressEntry &addr: addrList) {

            if(addr.ip().isNull()) continue;
            if(addr.ip().toString() == ip) {

                addressEntry = addr;
                return true;
            }
        }
    }

    return false;
}
//==============================================================================
bool isCorrectIp(const QString &ip)
{
    QStringList list = ip.split(".", QString::SkipEmptyParts);
    if(list.size() != 4) return false;

    for(const QString &str: list) {

        bool ok;
        int value = str.toInt(&ok);
        if(!ok || (value < 0) || (value > 255)) return false;
    }

    return true;
}
//==============================================================================
bool hostSettings(TcpIp4Struct &host, QString &err)
{
    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    for(const QNetworkInterface &iface: list) {

        if(!iface.isValid()) continue;

        QList<QNetworkAddressEntry> addrList = iface.addressEntries();

        for(const QNetworkAddressEntry &addr: addrList) {

            const QHostAddress &hostAddr = addr.ip();
            if(hostAddr.isNull() || hostAddr.isLoopback() || hostAddr.isMulticast()) continue;
            if(hostAddr.protocol() == QAbstractSocket::IPv4Protocol) {

                host.name = iface.humanReadableName();
                host.addr = hostAddr.toString();
                host.mask = addr.netmask().toString();
                host.broadcast = addr.broadcast().toString();

                QByteArray cmdOut;
                if(system_utils::osCmd(
                            QString("netsh interface ip show config name=\"%1\"")
                            .arg(host.name),
                            cmdOut, 6000)) {

                    QTextCodec *codec = QTextCodec::codecForName("cp866");
                    QString utfStr = codec->toUnicode(cmdOut);
                    QStringList strList = utfStr.split("\n", QString::SkipEmptyParts);

                    for(int i=0; i<strList.size(); ++i) {

                        QString str = strList.at(i).trimmed().toLower();
                        int n = str.indexOf(":", -1);
                        err += str + "\n";

                        if(str.contains("dhcp ") && (n > 0)) {

                            QString s = str.mid(n+1).trimmed();
                            host.dhcp = s.contains("да") || s.contains("yes");
                        }
                        else if((n > 0) && (str.contains(" шлюз:") || str.contains("gateway:")) ) {

                            QString s = str.mid(n+1).trimmed();
                            if(isCorrectIp(s)) host.gateway = s;
                        }
                        else if((n>0) && (str.contains(" dns-"))) {

                            host.autoDns = !str.contains("статич") && !str.contains("static");
                            QString s = str.mid(n+1).trimmed();
                            QStringList tmpList = s.split(" ", QString::SkipEmptyParts);
                            if(!tmpList.isEmpty()) {
                                s = tmpList.first().trimmed();
                                if(isCorrectIp(s)) host.dns1 = s;

                                if(tmpList.size() > 1) {
                                    s = tmpList.at(1).trimmed();
                                    if(isCorrectIp(s)) host.dns2 = s;
                                }
                                else if(i < strList.size()-1) {
                                    s = strList.at(i+1).toLower().trimmed();
                                    if(isCorrectIp(s)) host.dns2 = s;
                                }
                            }
                        }
                    }
                }
                else {
                    err = cmdOut;
                }

                return true;
            }
        }
    }

    err = QObject::tr("Интерфейс не найден");
    return false;
}
//==============================================================================

} // namespace//================================================================

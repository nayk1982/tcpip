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
    err = "";
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

                err += QString("Name: %1\n"
                               "Address: %2\n"
                               "Mask: %3\n"
                               "Gateway: %4\n"
                               "Broadcast: %5\n")
                        .arg(host.name)
                        .arg(host.addr)
                        .arg(host.mask)
                        .arg(host.gateway)
                        .arg(host.broadcast);

                QByteArray cmdOut;
                if(system_utils::osCmd(
                            QString("netsh interface ip show config name=\"%1\"")
                            .arg(host.name),
                            cmdOut, 6000)) {

                    QTextCodec *codec = QTextCodec::codecForName("cp866");
                    QString utfStr = codec->toUnicode(cmdOut);
                    QStringList strList = utfStr.split("\n", QString::SkipEmptyParts);

                    for(int i=1; i<strList.size(); ++i) {

                        QString str = strList.at(i).trimmed().toLower();
                        int n = str.indexOf(':');
                        if(n < 0) continue;

                        if(str.contains("dhcp включен")
                                || str.contains("dhcp разрешен") || str.contains("dhcp enable")) {

                            QString s = str.mid(n+1).trimmed();
                            host.dhcp = s.contains("да") || s.contains("yes");
                        }
                        else if(str.contains("шлюз:") || str.contains("gateway:")) {

                            QString s = str.mid(n+1).trimmed();
                            if(isCorrectIp(s)) host.gateway = s;
                        }
                        else if(str.contains("метрик") || str.contains("metric")) {

                            QString s = str.mid(n+1).trimmed();
                            int val = convert::strToIntDef(s, -1);
                            if(val >= 0) {

                                if(str.contains("шлюз") || str.contains("gateway")) {

                                    host.gatewayMetric = val;
                                }
                                else if(str.contains("интерфейс") || str.contains("interface")) {

                                    host.ifaceMetric = val;
                                }
                            }
                        }
                        else if(str.contains("dns-")) {

                            host.autoDns = str.contains("dhcp");
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

                    err += QString("Use DHCP: %1\n"
                                   "Static DNS: %2\n"
                                   "DNS1: %3\n"
                                   "DNS2: %4\n"
                                   "Gateway metric: %5\n"
                                   "Iterface metric: %6\n")
                            .arg(host.dhcp ? "yes" : "no")
                            .arg(host.autoDns ? "no" : "yes")
                            .arg(host.dns1)
                            .arg(host.dns2)
                            .arg(host.gatewayMetric)
                            .arg(host.ifaceMetric);
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
bool setHostSettings(const TcpIp4Struct &host, QString &err)
{
    QString cmd = QString("netsh interface ip set address name=\"%1\" ")
            .arg(host.name);

    if(host.dhcp) {

        cmd += "dhcp";
    }
    else {

        cmd += QString("static %1 %2 %3 %4")
                .arg(host.addr)
                .arg(host.mask)
                .arg(host.gateway)
                .arg(host.gatewayMetric);
    }

    if(!system_utils::osCmd( cmd, err, 30000)) {

        return false;
    }

    cmd = QString("netsh interface ip set dns name=\"%1\" ")
                .arg(host.name);

    if(host.autoDns) {

        cmd += "dhcp";
    }
    else {

        cmd += QString("static %1").arg(host.dns1.isEmpty() ? "none" : host.dns1);
    }

    if(!system_utils::osCmd( cmd, err, 30000)) {

        return false;
    }

    if(!host.autoDns && !host.dns1.isEmpty() && !host.dns2.isEmpty()) {

        cmd = QString("netsh interface ip add dns name=\"%1\" %2")
                    .arg(host.name).arg(host.dns2);

        if(!system_utils::osCmd( cmd, err, 30000)) {

            return false;
        }
    }

    err = QString("Name: %1\n"
                  "Address: %2\n"
                  "Mask: %3\n"
                  "Gateway: %4\n"
                  "Broadcast: %5\n")
            .arg(host.name)
            .arg(host.addr)
            .arg(host.mask)
            .arg(host.gateway)
            .arg(host.broadcast);

    err += QString("Use DHCP: %1\n"
                   "Static DNS: %2\n"
                   "DNS1: %3\n"
                   "DNS2: %4\n"
                   "Gateway metric: %5\n"
                   "Iterface metric: %6\n")
            .arg(host.dhcp ? "yes" : "no")
            .arg(host.autoDns ? "no" : "yes")
            .arg(host.dns1)
            .arg(host.dns2)
            .arg(host.gatewayMetric)
            .arg(host.ifaceMetric);

    return true;
}
//==============================================================================

} // namespace//================================================================

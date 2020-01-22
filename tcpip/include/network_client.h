#ifndef NETWORK_CLIENT_H
#define NETWORK_CLIENT_H

#include <QObject>
#include <QtNetwork/QNetworkAddressEntry>

namespace network_client { //===================================================

typedef struct TcpIp4Struct {
    bool dhcp {false};
    bool autoDns {false};
    QString name {""};
    QString addr {""};
    QString mask {""};
    QString broadcast {""};
    QString gateway {""};
    QString dns1 {""};
    QString dns2 {""};
} TcpIp4Struct;

bool simpleGet(const QString &url, QString &resultString);
bool externalIP(QString &ip);
QString hostAddress();
QString connectionName(const QString &ip);
bool addressEntry(const QString &ip, QNetworkAddressEntry &addressEntry);
bool isCorrectIp(const QString &ip);
bool hostSettings(TcpIp4Struct &host, QString &err);

}; // namespace //==============================================================
#endif // NETWORK_CLIENT_H

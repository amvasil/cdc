#ifndef NETSERVER_H
#define NETSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include "netclient.h"
class NetServer : public NetClient
{
    Q_OBJECT
public:
    explicit NetServer(QObject *parent, QHostAddress own_ip, quint16 own_port);
    QTcpServer *server;
    QHostAddress ip;
    quint16 port;
    QHostAddress client_ip;
    quint16 client_port;
    void listen();
signals:
    void connected();
public slots:
    void slot_connected();
};

#endif // NETSERVER_H

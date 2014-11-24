#include "netserver.h"

NetServer::NetServer(QObject *parent,QHostAddress own_ip,quint16 own_port) :
    NetClient(parent,own_ip,own_port)
{
    server = new QTcpServer(this);
    port = own_port;
    ip = own_ip;
    connect(server,SIGNAL(newConnection()),this,SLOT(slot_connected()));
}

void NetServer::listen()
{
    server->listen(ip,port);
}



void NetServer::slot_connected()
{
    socket = server->nextPendingConnection();
    connect(socket,SIGNAL(readyRead()),this,SLOT(slot_received_data()));
    emit connected();
}

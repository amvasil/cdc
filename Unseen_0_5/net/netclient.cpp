#include "netclient.h"
#include <QDebug>
#include <QTimer>
#include <stdio.h>


NetClient::NetClient(QObject *parent, QHostAddress own_ip, quint16 own_port) :
    QObject(parent)
{
    ip = own_ip;
    port = own_port;
    received_parts = 0;
    count_parts = 0;
    tx_queue.clear();
    tx_busy_flag = 0;
}

void NetClient::bind()
{
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(connected()),this,SLOT(slot_connected()));
    connect(socket,SIGNAL(readyRead()),this,SLOT(slot_received_data()));
    socket->bind(ip,port);

}

void NetClient::connectToServer(QHostAddress s_ip, quint16 s_port)
{
    server_ip = s_ip;
    server_port = s_port;
    qDebug()<<"trying to connect to"<<server_port;
    socket->connectToHost(server_ip,server_port);
}

void NetClient::sendConfiguration(const QString& data)
{
    enqueueData(data,"{CONFIG}");
}

void NetClient::sendTest(const QString& data)
{
    enqueueData(data,"{TEST}");
}

void NetClient::sendVariable(const QString& data)
{
    enqueueData(data,"{VAR}");
}

void NetClient::sendCommand(QString command)
{
    enqueueData(command,"");
}

void NetClient::sendRequest(QString type, QString data_name)
{
    enqueueData(data_name,type.prepend("{").append("}"));
}


void NetClient::sendDataFromQueue()
{

    if(tx_queue.isEmpty())
        return;
    if(tx_busy_flag)
        return;
    sendDataSimply(*tx_queue.first());
    tx_busy_flag=1;
    delete tx_queue.takeFirst();
}

QString &NetClient::getReceivedType()
{
    return received_type;
}

QString &NetClient::getReceivedData()
{
    return data_received;
}

//considered deprecated
void NetClient::clear()
{
    tx_queue.clear();
}

void NetClient::slot_connected()
{
    emit connected();
}

void NetClient::slot_received_data()
{
    QString data(socket->readAll());
    if(data.isEmpty())
        return;
    buffer.append(data);
    if(buffer=="$READY!")
    {
        tx_busy_flag = 0;
        sendDataFromQueue();
        data_received.clear();
        buffer.clear();
        return;
    }
    if(data == "$ERROR!")
    {
        tx_busy_flag = 0;
        data_received.clear();
        buffer.clear();
        return;
    }
    if(!data.endsWith("!"))
    {
        return;
    }

    if(!buffer.startsWith("$"))
    {
        qDebug()<<"wrong parcel"<<buffer;
        exit(0);
    }
    parseData(buffer);
    buffer.clear();

}

void NetClient::parseData(QString &data)
{
    data.remove("!");
    //command parcels
    if(data.startsWith("${VAR}"))
    {
        data_received = data.remove("${VAR}");
        received_type = "VAR";
    }
    else if(data.startsWith("${TEST}"))
    {
        data_received = data.remove("${TEST}");
        received_type = "TEST";
    }
    else if(data.startsWith("${CONFIG}"))
    {
        data_received = data.remove("${CONFIG}");
        received_type = "CONFIG";

    }
    else if(data == "$TESTDONE")
    {
        data_received.clear();
        received_type = "TESTDONE";

    }
    else if(data.startsWith("${GETVAR}"))
    {
        data_received = data.remove("${GETVAR}");
        received_type = "GETVAR";

    }
    else if(data == "$START")
    {
        data_received.clear();
        received_type = "START";
    }
    else
    {
        qDebug()<<"unknown parcel";
        confirm();
        return;
    }
    emit received_data();
    confirm();
}

void NetClient::sendDataSimply(QString& data)
{
    int sent = socket->write(data.toLocal8Bit());
    if(sent!=data.size())
    {
        qDebug()<<"SOME DATA NOT SENT";
    }
}

void NetClient::enqueueData(const QString &data,QString full_header)
{
    QString *str = new QString(data);
    str->prepend(full_header).prepend("$").append("!");
    tx_queue.append(str);
    if(!tx_busy_flag)
        sendDataFromQueue();

}

void NetClient::confirm()
{
    socket->write(QString("$READY!").toLocal8Bit());
}

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
    rx_queue.clear();
    tx_busy_flag = 0;
}

void NetClient::bind()
{
    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(connected()),this,SLOT(slot_connected()));
    //connect(socket,SIGNAL(bytesWritten(qint64)),this,SLOT(data_sent(qint64)));
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

void NetClient::sendConfiguration(QString data)
{
    qDebug()<<"config "<<data.size();
    sendData(data.prepend("${CONFIG}"));
}

void NetClient::sendTest(QString data)
{
    sendData(data.prepend("${TEST}"));
}

void NetClient::sendVariable(QString data)
{
    bool to_send=0;
    if(tx_queue.isEmpty())
        to_send=1;
    tx_queue<<createParcels(data,"VAR");
    if(to_send)
        sendDataFromQueue();
}

void NetClient::sendCommand(QString command)
{
    sendData(command.prepend("$"));
}

void NetClient::sendData(QString &buffer)
{
    if(tx_queue.isEmpty() && !tx_busy_flag){
        tx_busy_flag = 1;
        socket->write(buffer.toLocal8Bit());
    }
    else{
        QString *str = new QString(buffer);
        tx_queue<<(str);
    }
}

QLinkedList<QString *> NetClient::createParcels(QString &data,QString header)
{
    QLinkedList<QString*> temp;
    int parts = data.size()/buffer_size;
    int start = 0;
    int len;
    for(int i=0;i<=parts;i++)
    {
        len = start+buffer_size>data.size()?data.size()-start:buffer_size;
        temp.append(new QString(data.mid(start,len).prepend("${%1,%2,%3}").
                                arg(header,QString::number(i+1),QString::number(parts+1))));
        start = start+len;
    }
    return temp;
}

void NetClient::sendDataFromQueue()
{
    if(tx_queue.isEmpty())
        return;
    socket->write(tx_queue.first()->toLocal8Bit());
    tx_busy_flag=1;
    tx_queue.removeFirst();
}

QString &NetClient::getReceivedType()
{
    return received_type;
}

QString &NetClient::getReceivedData()
{
    return data_received;
}

void NetClient::clear()
{
    qDebug()<<tx_queue.size()<<rx_queue.size();
    tx_queue.clear();
    rx_queue.clear();
}

void NetClient::waitForConnection()
{
    if(socket->isOpen())
        return;
    QTimer::singleShot(0,this,SLOT(waitForConnection()));

}

void NetClient::slot_connected()
{
    emit connected();
}



void NetClient::slot_received_data()
{
    qDebug()<<"got some data";
    QString data(socket->readAll());
    if(data.isEmpty())
        return;
    if(data == "$READY")
    {
        tx_busy_flag = 0;
        sendDataFromQueue();
        return;
    }
    if(data == "$ERROR")
    {
        tx_busy_flag = 0;
        qDebug()<<"error message received";
        return;
    }
    qDebug()<<"data: "<<data.left(15)<<data.size();
    if(!data.startsWith("$"))
    {
        qDebug()<<"wrong parcel";
    }
    if(!data.contains("{"))
    {
        if(data=="$START")
        {
            data_received.clear();
            received_type = "START";
            emit received_data();
        }
        socket->write(QString("$READY").toLocal8Bit());
        return;
    }
    QString header = data.mid(data.indexOf("{")+1,data.indexOf("}")-data.indexOf("{")-1);
    qDebug()<<header;
    if(header.contains("GETVAR"))
    {
        received_type = "GETVAR";
        data_received = header.mid(header.indexOf(",")+1);
        emit received_data();
    }
    else if(header == "CONFIG")
    {
        received_type = "CONFIG";
        data_received = data.mid(data.indexOf("}")+1);
        emit received_data();
    }
    else if(header == "TEST")
    {
        received_type = "TEST";
        data_received = data.mid(data.indexOf("}")+1);
        emit received_data();
    }
    else if(header.contains("VAR"))
    {
        char *str = header.remove("VAR,").toLocal8Bit().data();
        int first=0,second=0;
        int res = sscanf(str,"%d,%d",&first,&second);
        if(res==EOF)
        {
            qDebug()<<"wrong int format in var";
            socket->write(QString("$ERROR").toLocal8Bit());
            return;
        }
        int curr_part = first;
        count_parts = second;



        if(curr_part==1)
        {
            received_type = "VAR";
            data_received.clear();
        }
        data_received.append(data.mid(data.indexOf("}")+1));
        if(curr_part==count_parts)
        {
            emit received_data();
        }
    }
    socket->write(QString("$READY").toLocal8Bit());
}

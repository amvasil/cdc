#ifndef NETCLIENT_H
#define NETCLIENT_H

#include <QObject>
#include <QTcpSocket>

#include <QHostAddress>
#include <QLinkedList>
class NetClient : public QObject
{
    Q_OBJECT
protected:
    QTcpSocket *socket;
    QHostAddress ip;
    quint16 port;
    QHostAddress server_ip;
    quint16 server_port;
    QLinkedList<QString*> tx_queue,rx_queue;
    static const int buffer_size = 8000;
    QString received_type;
    QString data_received;
    int count_parts;
    int received_parts;
    bool tx_busy_flag;
public:
    explicit NetClient(QObject *parent, QHostAddress own_ip, quint16 own_port);
    void bind();
    void connectToServer(QHostAddress s_ip,quint16 s_port);
    void sendConfiguration(QString data);
    void sendTest(QString data);
    void sendVariable(QString data);
    void sendCommand(QString command);
    void sendData(QString& buffer);
    QLinkedList<QString *> createParcels(QString& data, QString header);
    void sendDataFromQueue();
    QString& getReceivedType();
    QString& getReceivedData();
    void clear();
signals:
    void connected();
    void received_data();
public slots:
    void waitForConnection();
private slots:
    void slot_connected();
    //void data_sent(qint64 sent);
    void slot_received_data();
};

#endif // NETCLIENT_H

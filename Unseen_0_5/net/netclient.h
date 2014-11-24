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
    QLinkedList<QString*> tx_queue;
    static const int buffer_size = 8000;
    QString received_type;
    QString data_received;
    QString buffer;
    int count_parts;
    int received_parts;
    bool tx_busy_flag;
    void parseData(QString& data);
    void sendDataSimply(QString& data);
    void enqueueData(const QString &data, QString full_header);
    void sendDataFromQueue();
    void confirm();
public:
    explicit NetClient(QObject *parent, QHostAddress own_ip, quint16 own_port);
    void bind();
    void connectToServer(QHostAddress s_ip,quint16 s_port);
    void sendConfiguration(const QString &data);
    void sendTest(const QString &data);
    void sendVariable(const QString &data);
    void sendCommand(QString command);
    void sendRequest(QString type,QString data_name);
    QString& getReceivedType();
    QString& getReceivedData();
    void clear();
signals:
    void connected();
    void received_data();
public slots:
private slots:
    void slot_connected();
    void slot_received_data();
};

#endif // NETCLIENT_H

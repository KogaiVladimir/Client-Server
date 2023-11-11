#ifndef SERVER_H
#define SERVER_H
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QMap>
#include <QTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>

class Server : public QTcpServer {

    Q_OBJECT
public:
    Server();
    QTcpSocket* m_socket;

private:
    QMap<QTcpSocket*, QVector<QString>> m_sockets_info;
    QByteArray m_data;

    quint16 m_next_block_size;

    int m_counter;

    QFile *m_file;
    QByteArray *m_data_json;
    QJsonDocument *m_doc_json;
    QJsonObject *m_obj_json;
    QJsonValue *m_value_json;
    QTextStream *m_stream_json;

    QJsonObject *m_client_info;
    QJsonObject *m_sett_power_sensor;

public slots:
    void sendToClient(QString str);
    void incomingConnection(qintptr socket_descriptor);
    void sltReadyRead();
    void sltReedClientInfo(QString client_info);
    void disconnectClient(QString str);
    void sltClientInfoToJson();
};

#endif // SERVER_H

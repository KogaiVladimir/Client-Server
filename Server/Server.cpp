#include "Server.h"

Server::Server()
    :m_next_block_size(0), m_counter(0) {
    m_file = new QFile();
    m_file->setFileName("client_info.json");

    m_data_json = new QByteArray();
    m_doc_json = new QJsonDocument();
    m_obj_json = new QJsonObject();
    m_value_json = new QJsonValue();

    m_client_info = new QJsonObject();
    m_sett_power_sensor = new QJsonObject();
    m_stream_json = new QTextStream();

    if(this->listen(QHostAddress::Any, 2323)) {
        qDebug() << "Start!";
    } else {
        qDebug() << "Error!";
    }
}

void Server::sendToClient(QString str) {
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof(qint16));
    for(auto socket : m_sockets_info.keys()) {
        if(socket->state() == QAbstractSocket::ConnectedState &&
                socket != m_socket) socket->write(m_data);
    }
}

void Server::incomingConnection(qintptr socket_descriptor) {
    m_socket = new QTcpSocket;
    m_socket->setSocketDescriptor(socket_descriptor);
    connect(m_socket, &QTcpSocket::readyRead, this, &Server::sltReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, m_socket, &QTcpSocket::deleteLater);
    m_sockets_info.insert(m_socket, {});

    QString str = "Connect Success";
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << QTime::currentTime() << str;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof(qint16));
    m_socket->write(m_data);
}

void Server::sltReadyRead() {
    m_socket = (QTcpSocket*)sender();
    QDataStream in(m_socket);
    in.setVersion(QDataStream::Qt_6_2);
    if(in.status() == QDataStream::Ok){
        while(true) {
            if(m_next_block_size == 0) {
                if(m_socket->bytesAvailable() < 2)
                    break;
                in >> m_next_block_size;
            }
            if(m_socket->bytesAvailable() < m_next_block_size)
                break;

            QString str;
            QTime time;
            in >> time >> str;
            m_next_block_size = 0;
            if(str.contains("ClientInfo")) {
                sltReedClientInfo(str);
                break;
            } else if(str.contains("Disconnect")) {
                disconnectClient(str);
                break;
            } else if(str.contains("screenshot")) {
                str += "_" + QString::number(m_counter);
                ++m_counter;
            }
            sendToClient(str);
            break;
        }
    } else {
        qDebug() << "DataStream error";
    }
}

void Server::sltReedClientInfo(QString client_info) {
    client_info = client_info.remove("ClientInfo");
    QVector<QString> client_info_vec;
    QStringList elements = client_info.split('\n', Qt::SkipEmptyParts);
    for (const QString& element : elements) {
        client_info_vec.append(element.trimmed());
    }
    m_sockets_info.insert(m_socket, client_info_vec);
}

void Server::disconnectClient(QString str) {
    str = str.remove("Disconnect");
    QStringList elements = str.split('\n', Qt::SkipEmptyParts);
    m_sockets_info[m_socket].push_back(elements.first());
    m_socket->disconnectFromHost();
    sltClientInfoToJson();
}

void Server::sltClientInfoToJson() {
    m_file->open(QIODevice::WriteOnly | QIODevice::Text);

    for(auto client : m_sockets_info) {
        for(auto info : client){
            auto key = info.left(info.indexOf(": "));
            m_client_info->insert(key, info.remove(key + ": "));
        }
        m_obj_json->insert(m_client_info->value("User").toString(),
                           *m_client_info);
    }
    m_doc_json->setObject(*m_obj_json);
    *m_data_json = m_doc_json->toJson(QJsonDocument::Indented);
    m_stream_json->setDevice(m_file);
    *m_stream_json << *m_data_json;
    m_file->close();
}

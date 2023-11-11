#include "Client.h"

Client::Client(QWidget *parent)
    : QMainWindow(parent), m_next_block_size(0)
{
    m_socket = new QTcpSocket();
    connect(m_socket, &QTcpSocket::readyRead, this, &Client::sltReadyRead);
    connect(m_socket, &QTcpSocket::disconnected, m_socket, &QTcpSocket::deleteLater);

    m_connect_btn = new QPushButton("Connect");
    m_send_message_btn = new QPushButton("->");

    m_log = new QTextBrowser();
    m_message_edit = new QLineEdit();

    m_horizontal_layout = new QHBoxLayout();
    m_horizontal_layout->addWidget(m_message_edit);
    m_horizontal_layout->addWidget(m_send_message_btn);

    m_screen_btn = new QPushButton("Screen");

    m_vertical_layout = new QVBoxLayout();
    m_vertical_layout->addWidget(m_connect_btn);
    m_vertical_layout->addWidget(m_log);
    m_vertical_layout->addLayout(m_horizontal_layout);
    m_vertical_layout->addWidget(m_screen_btn);

    QWidget* central_widget = new QWidget();
    central_widget->setLayout(m_vertical_layout);

    setCentralWidget(central_widget);

    connect(m_connect_btn, &QPushButton::clicked, this, &Client::sltConnect);
    connect(m_send_message_btn, &QPushButton::clicked, this, &Client::sltSendSlicked);
    connect(m_message_edit, &QLineEdit::returnPressed, this, &Client::sltSendSlicked);
    connect(m_screen_btn, &QPushButton::clicked, this, &Client::sltScreenMessage);
}

Client::~Client() {
}

void Client::sltConnect() {
    m_socket->connectToHost("127.0.0.1", 2323);
}

void Client::sltReadyRead() {
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
            if(str.contains("Connect Success")) {
                sltSendClientInfo();
            } else if(str.contains("screenshot")) {
                sltScreen(str);
            } else {
                m_log->append(time.toString() + ": " + str);
            }
            break;
        }
    } else {
        m_log->append("DataStream error");
    }
}

void Client::sltSendSlicked() {
    sendMessage(m_message_edit->text());
}

void Client::sendMessage(QString message) {
    m_data.clear();
    QDataStream out(&m_data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_2);
    out << quint16(0) << QTime::currentTime() << message;
    out.device()->seek(0);
    out << quint16(m_data.size() - sizeof(qint16));
    m_socket->write(m_data);
}

void Client::sltScreenMessage() {
    sendMessage("screenshot");
}

void Client::sltScreen(QString screen_name) {
    QList<QScreen*> screens = QGuiApplication::screens();
    for (int i = 0; i < screens.size(); ++i) {
        QScreen* screen = screens.at(i);
        QPixmap screenshot = screen->grabWindow(0);
        QString filename = QString(screen_name + ".png").arg(i);
        screenshot.save(filename);
    }
}

void Client::sltSendClientInfo() {
    QString time_input = QTime::currentTime().toString();
    QString domain = QHostInfo::localDomainName();
    QString computer_name = QHostInfo::localHostName();
    QString ip_address = QHostInfo::fromName(QHostInfo::localHostName()).addresses().first().toString();

    wchar_t user_name[UNLEN + 1];
    DWORD user_name_size = UNLEN + 1;
    GetUserName(user_name, &user_name_size);
    QString user_name_str = QString::fromWCharArray(user_name);

    QString client_info_str = QString("ClientInfo"
                                 "\nDomain: %1"
                                 "\nComputer: %2"
                                 "\nIP: %3"
                                 "\nUser: %4"
                                 "\nTime Input: %5")
                            .arg(domain)
                            .arg(computer_name)
                            .arg(ip_address)
                            .arg(user_name_str)
                            .arg(time_input);

    sendMessage(client_info_str);
}

void Client::closeEvent(QCloseEvent *event) {
    QString time_output = "Disconnect\nTime Output: " + QTime::currentTime().toString();
    sendMessage(time_output);
    m_socket->waitForBytesWritten();
    QMainWindow::closeEvent(event);
}

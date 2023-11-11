#ifndef CLIENT_H
#define CLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>
#include <QLineEdit>
#include <QTextBrowser>
#include <QLayout>
#include <QTime>
#include <QScreen>
#include <QPixmap>
#include <QString>
#include <QApplication>

#include <Windows.h>
#include <QHostInfo>
#include <QByteArray>
#include <lmcons.h>

class Client : public QMainWindow
{
    Q_OBJECT

public:
    Client(QWidget *parent = nullptr);
    ~Client();

private:
    QTcpSocket* m_socket;
    QByteArray m_data;

    QPushButton* m_connect_btn;
    QTextBrowser* m_log;

    QLineEdit* m_message_edit;
    QPushButton* m_send_message_btn;

    QHBoxLayout* m_horizontal_layout;
    QVBoxLayout* m_vertical_layout;

    quint16 m_next_block_size;

    QPushButton* m_screen_btn;
private slots:
    void sendMessage(QString message);
    void sltConnect();
    void sltReadyRead();
    void sltSendSlicked();
    void sltScreenMessage();
    void sltScreen(QString screen_name);
    void sltSendClientInfo();
    void closeEvent(QCloseEvent* event);
};
#endif // CLIENT_H

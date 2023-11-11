#include "Client.h"

#include <QApplication>

//#include <QCoreApplication>
//#include <QScreen>
//#include <QPixmap>
//#include <QString>
//#include <QDateTime>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Client w;
    w.show();

//------------------------------------------------------------------
//    QCoreApplication app(argc, argv);

//    QList<QScreen*> screens = QGuiApplication::screens();

//    qDebug() << "screens.size" << screens.size();
//    for (int i = 0; i < screens.size(); ++i) {
//        QScreen* screen = screens.at(i);

//        QPixmap screenshot = screen->grabWindow(0);

//        QString filename = QString("screenshot_%1.png").arg(i);
//        screenshot.save(filename);

//        qDebug() << "Скриншот сохранен в файл:" << filename;
//    }
//    app.exec();
//-------------------------------------------------------------------
    return a.exec();
}

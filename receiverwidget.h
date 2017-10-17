#ifndef RECEIVERWIDGET_H
#define RECEIVERWIDGET_H

#include <QWidget>

enum {Fail = 0, Success = 1};

class QTcpServer;
class QTcpSocket;

namespace Ui {
class ReceiverWidget;
}

class ReceiverWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ReceiverWidget(QWidget *parent = 0);
    ~ReceiverWidget();

private slots:
    void acceptConnection();
    void receiveFromClient();
    void clientDiconnected();
    void handleReceivedData(qint64 len);

signals:
    void receiveFinished();

private:
    Ui::ReceiverWidget *ui;
    QTcpServer *tcpServer;
    QTcpSocket *tcpConnection;

    QStringList receivedFilesList;

   // QDataStream in;
    QByteArray wholeByteArray;
    QString fileName;
    qint64 fileSize, chunksSize;
    uchar *uCharData;

    bool tryToListen();
    void startListening();
};

#endif // RECEIVERWIDGET_H

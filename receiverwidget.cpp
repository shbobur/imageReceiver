#include "receiverwidget.h"
#include "ui_receiverwidget.h"

#include <QTcpServer>
#include <QHostAddress>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QPixmap>
#include <QStringListModel>
#include <QBuffer>

ReceiverWidget::ReceiverWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ReceiverWidget)
{
    ui->setupUi(this);

    fileSize = 84369;
    chunksSize = 0;

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(receivedFilesList);
    ui->receivedFileListView->setModel(model);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), SLOT(acceptConnection()));

   // connect(this, SIGNAL(receiveFinished()), SLOT(handleReceivedData()));

    startListening();
}

ReceiverWidget::~ReceiverWidget()
{
    tcpServer->close();
    tcpServer->deleteLater();
    //delete tcpServer;
    delete ui;
}

void ReceiverWidget::acceptConnection()
{
    tcpConnection = tcpServer->nextPendingConnection();
    ui->sourceLineEdit->setText(tcpConnection->localAddress().toString() + "/" + QString::number(tcpConnection->localPort()));
    connect(tcpConnection, SIGNAL(readyRead()), SLOT(receiveFromClient()));
    connect(tcpConnection, SIGNAL(disconnected()), SLOT(clientDiconnected()));
}

void ReceiverWidget::receiveFromClient()
{
    qDebug() << "stream size " << tcpConnection->bytesAvailable();

    if (tcpConnection->bytesAvailable() < fileSize)
        return ;
    /*
    if (fileSize == 0) {
        QDataStream in(tcpConnection);

        in >> fileName;
        qDebug() << fileName;
        receivedFilesList.append(fileName);
        QString str;
        in >> str;
        fileSize = str.toInt();
        qDebug() << fileSize;
        return;
    }
*/
    fileName = "Famous-characters-Troll-face-Troll-face-poker-269483.jpg";
    fileSize = 84369;

    //QDataStream stream(tcpConnection);
    QByteArray ba;
    ba = tcpConnection->readAll();
    qDebug() << "received data " << ba.size() << " bytes";

    QPixmap pixmap;
    pixmap.loadFromData(ba);
    if (pixmap.isNull())
        qDebug() << "The pixmap is null";
    else
        qDebug() << "Converted to pixmap succesfully.";
    ui->playerLabel->setPixmap(pixmap);



   /* uCharData = (uchar*)malloc(fileSize + 256);

    quint64 len = tcpConnection->read((char*)uCharData, fileSize);

    qDebug() << "received " << len;

    QImage image;                 // Construct a new QImage
    image.loadFromData(uCharData, len, "jpeg"); // Load the image from the receive buffer
    if (image.isNull())           // Check if the image was indeed received
            qDebug() << "The image is null. Something failed.";
    else
        qDebug() << "Converted to image succesfully.";

    QPixmap pixmap;
    pixmap.loadFromData(uCharData, len, "jpeg");
    if (pixmap.isNull())
        qDebug() << "The pixmap is null";
    else
        qDebug() << "Converted to pixmap succesfully.";
    ui->playerLabel->setPixmap(pixmap);
*/
   // qDebug() << "wholeByteArray size: " << wholeByteArray.size();

    //handleReceivedData(len);

}

void ReceiverWidget::clientDiconnected()
{
    ui->sourceLineEdit->setText("");
}

void ReceiverWidget::handleReceivedData(qint64 len)
{
    /*if (wholeByteArray.size() > fileSize.size())
        wholeByteArray.chop(wholeByteArray.size() - fileSize.toInt());
*/
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    //file.write(wholeByteArray);
    file.write((char*)uCharData, len);
    file.close();


    qDebug() << "Whole received data " << wholeByteArray.size() << " bytes";
    QPixmap pixmap;
    //pixmap.loadFromData(wholeByteArray, "JPEG");
    pixmap.loadFromData(uCharData, len, "JPEG");
    ui->playerLabel->setPixmap(pixmap);
    wholeByteArray.clear();
}

bool ReceiverWidget::tryToListen()
{

    return tcpServer->listen(QHostAddress("127.0.0.1"), 3030);
}

void ReceiverWidget::startListening()
{
    while (tryToListen() != Success) {
        if (QMessageBox::question(this, tr("Start the server"), tr("Could not start the server. Try again?"), QMessageBox::Ok | QMessageBox::Close) != QMessageBox::Ok) {
            this->close();
        }

        ui->stateLabel->setText(tcpServer->errorString());
    }

    ui->stateLabel->setText(tr("Listenning to ") + tcpServer->serverAddress().toString() + "/" + QString::number(tcpServer->serverPort()));
    ui->connectButton->setEnabled(false);
}

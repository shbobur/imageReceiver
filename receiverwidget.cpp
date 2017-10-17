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

    QStringListModel *model = new QStringListModel(this);
    model->setStringList(receivedFilesList);
    ui->receivedFileListView->setModel(model);

    tcpServer = new QTcpServer(this);
    connect(tcpServer, SIGNAL(newConnection()), SLOT(acceptConnection()));

    connect(this, SIGNAL(receiveFinished()), SLOT(handleReceivedData()));

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
    QDataStream in(tcpConnection);
    QByteArray tmpByteArray;
    //bool isInfoGot = false;

    in >> fileName;
    qDebug() << fileName;
    receivedFilesList.append(fileName);
    in >> fileSize;
    qDebug() << fileSize;

    int receivedDataSize = 0;

   while (tcpConnection->waitForReadyRead(1000))
   {
            while (tcpConnection->bytesAvailable())
            {
                qDebug() << "bytesAvailable:" << tcpConnection->bytesAvailable();
                //in >> tmpByteArray;
                tmpByteArray = tcpConnection->readAll();

                qDebug() << "adding... " << wholeByteArray;
                wholeByteArray.insert(wholeByteArray.size(), tmpByteArray);

                qDebug() << "tmp size " << tmpByteArray.size();

                receivedDataSize += tmpByteArray.size();
            }
           // break;
    }

    qDebug() << "received " << receivedDataSize;
    qDebug() << "wholeByteArray size: " << wholeByteArray.size();

    handleReceivedData();
}

void ReceiverWidget::clientDiconnected()
{
    ui->sourceLineEdit->setText("");
}

void ReceiverWidget::handleReceivedData()
{
    if (wholeByteArray.size() > fileSize.size())
        wholeByteArray.chop(wholeByteArray.size() - fileSize.toInt());

    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(wholeByteArray);
    file.close();


    qDebug() << "Whole received data " << wholeByteArray.size() << " bytes";
    QPixmap pixmap;
    pixmap.loadFromData(wholeByteArray, "JPEG");
    ui->playerLabel->setPixmap(pixmap);
    wholeByteArray.clear();
}

bool ReceiverWidget::tryToListen()
{

    return tcpServer->listen(QHostAddress("127.0.0.1"), 2020);
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

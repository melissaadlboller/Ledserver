#include "ledserver.h"
#include "config.h"
#include <QDebug>

LedServer::LedServer(quint16 port, QObject *parent) : QObject(parent), m_port(port)
{
    m_server = new QTcpServer(this);
    connect(m_server, &QTcpServer::newConnection, this, &LedServer::myNewConnection);
}

void LedServer::start()
{
    m_server->listen(QHostAddress::Any, PORT);
    qDebug() << "Server lauscht auf Port" << PORT;

    if(!m_server->listen(QHostAddress::Any, PORT))
        throw m_server->errorString();
}

// Client hat Verbindung zum Server aufgebaut
void LedServer::myNewConnection()
{
    qDebug() << "Bitte Zahl von 0 bis 15 eingeben:";

    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &LedServer::myServerRead);
    connect(m_socket, &QTcpSocket::disconnected, this, &LedServer::myClientDisconnect);
}

// Client hat Verbindung zum Server getrennt
void LedServer::myClientDisconnect()
{
    m_socket->close();
    qDebug() << "Client hat Verbindung getrennt";
}

// Client hat eine Zahl (0...15) zum Server gesendet
void LedServer::myServerRead()
{
    while(m_socket->bytesAvailable())
    {
        QString message = m_socket->readAll();
        int x = message.toInt();
        qDebug() << x;
        if (x >= 0 && x <= 15)
        {
            qDebug() << "Okay";
            m_gpio->set(x);
        }
        qDebug() << "Bitte Zahl von 0 bis 15 eingeben:";
    }
}

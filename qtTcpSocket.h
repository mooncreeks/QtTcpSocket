#ifndef QtTcpSocket_H
#define QtTcpSocket_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QHostAddress>
#include <QTextCodec>
#include <QDataStream>
#include <QFile>
#include <QDateTime>

class QtTcpSocket : public QObject
{
    Q_OBJECT
public:
    enum G_SOCKET_LOG_TYPE
    {
        G_SOCKET_MSG = 0,
        G_SOCKET_ERR
    };
    explicit QtTcpSocket(QObject *parent = 0);
    ~QtTcpSocket();
    void doConnect();
    void sendCommand(QString cmd);
    QHash<QString, QString> getSocketState();
    void log(quint16 type, QString msg);
    void updateSocketConnectionState(bool connected);

    void setPort(quint16 port);
    void setAddress(QString address);

    QString getAddress();
    quint16 getPort();
signals:
    
public slots:
    void self_socket_connected();
    void self_socket_disconnected();
    void self_socket_bytesWritten(qint64 bytes);
    void self_socket_readyRead();
    void self_socket_aboutToClose();
    void self_socket_error(QAbstractSocket::SocketError socketError);
    void self_socket_stateChanged(QAbstractSocket::SocketState socketState);
private:
    bool m_isConnected;
    int file_tag;
    QHostAddress *m_address;
    quint16 m_port;
    QString m_command;
    QTcpSocket *m_pSocket;
    QString m_current_file_name ;
    QByteArray m_temp_storage;
    
};

#endif // QtTcpSocket_H

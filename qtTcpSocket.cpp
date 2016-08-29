#include "qtTcpSocket.h"

QtTcpSocket::QtTcpSocket(QObject *parent) :
    QObject(parent)
{
    m_pSocket = new QTcpSocket(this);
    m_address = new QHostAddress();
    m_isConnected = false;
    m_port = 5000;
    connect(m_pSocket, SIGNAL(connected()), this, SLOT(self_socket_connected()));
    connect(m_pSocket, SIGNAL(disconnected()), this, SLOT(self_socket_disconnected()));
    connect(m_pSocket, SIGNAL(readyRead()), this, SLOT(self_socket_readyRead()));
    connect(m_pSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(self_socket_connected()));
    connect(m_pSocket, SIGNAL(aboutToClose()), this, SLOT(self_socket_aboutToClose()));
    connect(m_pSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(self_socket_error(QAbstractSocket::SocketError)));
    //connect(m_pSocket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(self_socket_stateChanged(QAbstractSocket::SocketState)));
}

QtTcpSocket::~QtTcpSocket()
{
    delete m_pSocket;
    m_pSocket = NULL;
    delete m_address;
    m_address = NULL;
}

void QtTcpSocket::self_socket_connected()
{
    updateSocketConnectionState(true);
    QHash<QString,QString> hash = getSocketState();
    //quint8 state_key = hash.value("state_key").toUInt();
    QString state_value = hash.value("state_value");
    log(G_SOCKET_MSG, "[slot:self_socket_connected:" + state_value + "]");
}

void QtTcpSocket::self_socket_disconnected()
{
    updateSocketConnectionState(false);
    QHash<QString,QString> hash = getSocketState();
    //quint8 state_key = hash.value("state_key").toUInt();
    QString state_value = hash.value("state_value");
    log(G_SOCKET_MSG, "[slot:self_socket_disconnected:" + state_value + "]");
}

void QtTcpSocket::self_socket_bytesWritten(qint64 bytes)
{
    log(G_SOCKET_MSG, "slot:self_socket_bytesWritten [" + QString::number(bytes) + "]");
}

void QtTcpSocket::self_socket_readyRead()
{
    while (m_pSocket->bytesAvailable()) {
        QByteArray arr = m_pSocket->readAll();
        static quint8 block_num = 0;
        quint16 buf_size = m_pSocket->readBufferSize();
        qDebug()<<"buffer size: "<<buf_size;
        qDebug()<<"#################"<<block_num++;
        log(G_SOCKET_MSG, "socket_readyRead [" + QString::number(arr.size()) + "] :" + arr);
        m_temp_storage.append(arr);
    }
    qDebug()<< m_temp_storage.size();
    qDebug()<< m_temp_storage;
}
void QtTcpSocket::self_socket_aboutToClose()
{
    log(G_SOCKET_MSG, "slot:self_socket_aboutToClose");
}
void QtTcpSocket::self_socket_error(QAbstractSocket::SocketError socketError)
{
    QString err_msg = "";
    switch (socketError) {
        case QAbstractSocket::ConnectionRefusedError:
            err_msg = "The connection was refused by the peer (or timed out).";
            break;
        case QAbstractSocket::RemoteHostClosedError:
            err_msg = "The remote host closed the connection.";
            break;
        case QAbstractSocket::HostNotFoundError:
            err_msg = "The host address was not found.";
            break;
        case QAbstractSocket::SocketAccessError:
            err_msg = "The socket operation failed because the application lacked the required privileges.";
            break;
        case QAbstractSocket::SocketResourceError:
            err_msg = "The local system ran out of resources (e.g., too many sockets).";
            break;
        case QAbstractSocket::SocketTimeoutError:
            err_msg = "The socket operation timed out.";
            break;
        case QAbstractSocket::DatagramTooLargeError:
            err_msg = "The datagram was larger than the operating system's limit (which can be as low as 8192 bytes).";
            break;
        case QAbstractSocket::NetworkError:
            err_msg = "An error occurred with the network (e.g., the network cable was accidentally plugged out).";
            break;
        case QAbstractSocket::AddressInUseError:
            err_msg = "The address specified to QAbstractSocket::bind() is already in use and was set to be exclusive.";
            break;
        case QAbstractSocket::SocketAddressNotAvailableError:
            err_msg = "The address specified to QAbstractSocket::bind() does not belong to the host.";
            break;
        case QAbstractSocket::UnsupportedSocketOperationError:
            err_msg = "The requested socket operation is not supported by the local operating system (e.g., lack of IPv6 support).";
            break;
        case QAbstractSocket::ProxyAuthenticationRequiredError:
            err_msg = "The socket is using a proxy, and the proxy requires authentication.";
            break;
        case QAbstractSocket::SslHandshakeFailedError:
            err_msg = "The SSL/TLS handshake failed, so the connection was closed (only used in QSslSocket)";
            break;
        case QAbstractSocket::UnfinishedSocketOperationError:
            err_msg = "Used by QAbstractSocketEngine only, The last operation attempted has not finished yet (still in progress in the background).";
            break;
        case QAbstractSocket::ProxyConnectionRefusedError:
            err_msg = "Could not contact the proxy server because the connection to that server was denied";
            break;
        case QAbstractSocket::ProxyConnectionClosedError:
            err_msg = "The connection to the proxy server was closed unexpectedly (before the connection to the final peer was established)";
            break;
        case QAbstractSocket::ProxyConnectionTimeoutError:
            err_msg = "The connection to the proxy server timed out or the proxy server stopped responding in the authentication phase.";
            break;
        case QAbstractSocket::ProxyNotFoundError:
            err_msg = "The proxy address set with setProxy() (or the application proxy) was not found.";
            break;
        case QAbstractSocket::ProxyProtocolError:
            err_msg = "The connection negotiation with the proxy server failed, because the response from the proxy server could not be understood.";
            break;
        case QAbstractSocket::OperationError:
            err_msg = "An operation was attempted while the socket was in a state that did not permit it.";
            break;
        case QAbstractSocket::SslInternalError:
            err_msg = "The SSL library being used reported an internal error. This is probably the result of a bad installation or misconfiguration of the library.";
            break;
        case QAbstractSocket::SslInvalidUserDataError:
            err_msg = "Invalid data (certificate, key, cypher, etc.) was provided and its use resulted in an error in the SSL library.";
            break;
        case QAbstractSocket::TemporaryError:
            err_msg = "A temporary error occurred (e.g., operation would block and socket is non-blocking).";
            break;
        case QAbstractSocket::UnknownSocketError:
            err_msg = "An unidentified error occurred.";
            break;
        default:
            err_msg = "Socket Error";
            break;
    }

    log(G_SOCKET_ERR,"[slot:self_socket_error]" + err_msg);
}
void QtTcpSocket::self_socket_stateChanged(QAbstractSocket::SocketState socketState)
{
    log(G_SOCKET_MSG, "[slot:self_socket_stateChanged:]" + socketState);
}

QString QtTcpSocket::getAddress()
{
    return m_address->toString();
}

quint16 QtTcpSocket::getPort()
{
    return m_port;
}

void QtTcpSocket::setAddress(QString address)
{
    m_address->setAddress(address);
}

void QtTcpSocket::setPort(quint16 port)
{
    m_port = port;
}

void QtTcpSocket::log(quint16 type, QString msg)
{
    QDateTime local(QDateTime::currentDateTime());
    QString currentTime = local.toString("yyyy-MM-dd hh:mm:ss");

    QString log_type = "MSG";
    switch (type) {
    case G_SOCKET_MSG:
        log_type = "MSG";
        break;
    case G_SOCKET_ERR:
        log_type = "ERROR";
        break;
    }

    qDebug()<<"[" + currentTime + "]" + "[" + log_type + "]" + msg;
}

void QtTcpSocket::updateSocketConnectionState(bool connected)
{
    m_isConnected = connected;

    if(connected)
    {
        // connection enabled
        log(G_SOCKET_MSG, "[updateSocketConnectionState:true]");
    }
    else
    {
        // connection disabled
        log(G_SOCKET_MSG, "[updateSocketConnectionState:false]");
    }
}

QHash<QString, QString> QtTcpSocket::getSocketState()
{
    /* QAbstractSocket::UnconnectedState   0 The socket is not connected.
       QAbstractSocket::HostLookupState    1 The socket is performing a host name lookup.
       QAbstractSocket::ConnectingState    2 The socket has started establishing a connection.
       QAbstractSocket::ConnectedState     3 A connection is established.
       QAbstractSocket::BoundState         4 The socket is bound to an address and port.
       QAbstractSocket::ClosingState       6 The socket is about to close (data may still be waiting to be written).
       QAbstractSocket::ListeningState     5 For internal use only.
     */
    QAbstractSocket::SocketState state = m_pSocket->state();
    QHash<QString, QString> hash;
    QString code="";
    switch (state) {
    case QAbstractSocket::UnconnectedState:
        code = "UnconnectedState";
        break;
    case QAbstractSocket::HostLookupState:
        code = "HostLookupState";
        break;
    case QAbstractSocket::ConnectingState:
        code = "ConnectingState";
        break;
    case QAbstractSocket::ConnectedState:
        code = "ConnectedState";
        break;
    case QAbstractSocket::BoundState:
        code = "BoundState";
        break;
    case QAbstractSocket::ClosingState:
        code = "ClosingState";
        break;
    case QAbstractSocket::ListeningState:
        code = "ListeningState";
        break;
    }
    hash.insert("state_key", QString::number(state,10));
    hash.insert("state_value", code);

    return hash;
}

void QtTcpSocket::doConnect()
{
    QHash<QString,QString> hash = getSocketState();
    quint8 state_key = hash.value("state_key").toUInt();
    QString state_value = hash.value("state_value");
    log(G_SOCKET_MSG, "[SocketState_BeforeConnect:" + state_value + "]");
    if (m_isConnected) {
        log(G_SOCKET_MSG, "[m_isConnected_BeforeConnect:true]");
    } else {
        log(G_SOCKET_MSG, "[m_isConnected_BeforeConnect:false]");
    }
    if((state_key == QAbstractSocket::ConnectedState) && m_isConnected) {
    } else {
        m_pSocket->abort();
        setAddress("192.168.208.202");
        setPort(5000);
        log(G_SOCKET_MSG, "Connecting to server: " + m_address->toString() + " ; within port: " + QString::number(m_port,10));
        m_pSocket->connectToHost(m_address->toString(), m_port);
    }

}

void QtTcpSocket::sendCommand(QString cmd)
{
    QHash<QString,QString> hash = getSocketState();
    quint8 state_key = hash.value("state_key").toUInt();
    QString state_value = hash.value("state_value");

    log(G_SOCKET_MSG, "[SocketState_BeforeSendCommand:" + state_value + "]");

    if((state_key == QAbstractSocket::ConnectedState) && m_isConnected) {
        QByteArray command = cmd.toLatin1();
        m_pSocket->write(command.data());
        log(G_SOCKET_MSG, command.data());
    } else {
        log(G_SOCKET_ERR, "[emit signal:self_send_socket_error:unConnected,command not sent]");
    }
}

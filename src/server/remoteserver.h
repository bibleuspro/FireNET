// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#ifndef REMOTESERVER_H
#define REMOTESERVER_H

#include <QObject>
#include <QThread>
#include <QTcpServer>
#include <QSslSocket>
#include <QMutex>

#include "global.h"

class RemoteConnection;

class RemoteServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit RemoteServer(QObject *parent = 0);
public:
	void Clear();
public slots:
	void Update();
	void CloseConnection();
signals:
	void close();
public:
	void run();
	int GetClientCount();
	QStringList GetServerList();
	SGameServer* GetGameServer(QString name, QString map, QString gamerules);

	void AddNewClient(SRemoteClient client);
	void RemoveClient(SRemoteClient client);
	void UpdateClient(SRemoteClient* client);

	bool CheckGameServerExists(QString name, QString ip, int port);
private:
	bool CreateServer();
public:
    virtual void incomingConnection(qintptr socketDescriptor);
	void sendMessageToRemoteClient(QSslSocket* socket, QByteArray &data);
public:
	bool bHaveAdmin;
private:
	QThread* m_Thread;
	QTcpServer* m_Server;
	QList<RemoteConnection*> m_connections;
private:
	QVector<SRemoteClient> m_Clients;
	QMutex m_Mutex;
};

#endif // REMOTESERVER_H
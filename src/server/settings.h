// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QVector>
#include <QVariant>
#include <QMutex>

struct SVariable
{
	QString key;
	QVariant value;
};

class SettingsManager : public QObject
{
	Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = 0);
public:
	void Clear();
public:
	QVariant GetVariable(QString key);
	QStringList GetVariablesList();
public:
	void SetVariable(QString key, QVariant value);
public:
	void RegisterVariable(QString key, QVariant value);
private:
	QVector<SVariable> m_Variables;
	QMutex m_Mutex;
};

#endif // SETTINGS_H
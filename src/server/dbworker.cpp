// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#include "global.h"
#include "dbworker.h"
#include "redisconnector.h"
#include "mysqlconnector.h"
#include "httpconnector.h"
#include "clientquerys.h"
#include "settings.h"

#include <QRegExp>
#include <QSqlQuery>

DBWorker::DBWorker(QObject *parent) : QObject(parent)
{
	pRedis = nullptr;
	pMySql = nullptr;
	pHTTP = nullptr;
}

void DBWorker::Update()
{
}

void DBWorker::Init()
{
	// Create Redis connection
	if (gEnv->pSettings->GetVariable("bUseRedis").toBool())
	{
		qInfo() << "Start Redis service...";
		pRedis = new RedisConnector;
		pRedis->run();
	}

	// Create MySQL connection
	if (gEnv->pSettings->GetVariable("bUseMySQL").toBool())
	{
		qInfo() << "Start MySql service...";
		gEnv->pSettings->SetVariable("redis_bg_saving", true);
		pMySql = new MySqlConnector;
		pMySql->run();
	}

	// Create HTTP connector
	if (gEnv->pSettings->GetVariable("bUseHttpAuth").toBool())
	{
		qWarning() << "Authorization mode set to HTTP, this can slows server";
		pHTTP = new HttpConnector;
	}
}

void DBWorker::Clear()
{
	qInfo() << "~DBWorker";

	if (pRedis != nullptr)
	{
		pRedis->Disconnect();
		pRedis->deleteLater();
	}
	if (pMySql != nullptr)
	{
		pMySql->Disconnect();
		pMySql->deleteLater();
	}

	if (pHTTP != nullptr)
	{
		pHTTP->deleteLater();
	}
}

bool DBWorker::UserExists(QString login)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "users:" + login;
			QList<QByteArray> rawCmd = { "HEXISTS", key.toUtf8(), "password" };
			QString buff = pRedis->SendSyncQuery(rawCmd);

			if (buff.toInt() > 0)
			{
				qDebug() << "Login" << login << "finded in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Login" << login << "not found in Redis DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found user" << login << "in Redis DB because Redis DB not connected!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM users WHERE login=:login");
			query->bindValue(":login", login);

			if (query->exec())
			{
				if (query->next())
				{ 
					qDebug() << "Login" << login << "finded in MySql DB";
					result = true;
				}
				else
				{
					qDebug() << "Login" << login << "not found in MySql DB";
					return false;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found user" << login << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return result;
}

bool DBWorker::ProfileExists(int uid)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "profiles:" + QString::number(uid);
			QList<QByteArray> rawCmd = { "HEXISTS", key.toUtf8(), "nickname" };
			QString buff = pRedis->SendSyncQuery(rawCmd);

			if (buff.toInt() > 0)
			{
				qDebug() << "Profile" << uid << "finded in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Profile" << uid << "not found in Redis DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found profile" << uid << "in Redis DB because Redis DB not opened!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM profiles WHERE uid=:uid");
			query->bindValue(":uid", uid);

			if (query->exec())
			{
				if (query->next())
				{
					qDebug() << "Profile" << uid << "finded in MySql DB";
					result = true;
				}
				else
				{
					qDebug() << "Profile" << uid << "not found in MySql DB";
					return false;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found profile" << uid << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return result;
}

bool DBWorker::NicknameExists(QString nickname)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "nicknames:" + nickname;
			QString buff = pRedis->SendSyncQuery("GET", key);

			if (!buff.isEmpty())
			{
				qDebug() << "Nickname" << nickname << "finded in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Nickname" << nickname << "not found in Redis DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found nickname" << nickname << "in Redis DB because Redis DB not connected!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM profiles WHERE nickname=:nickname");
			query->bindValue(":nickname", nickname);

			if (query->exec())
			{
				if (query->next())
				{
					qDebug() << "Nickname" << nickname << "finded in MySql DB";
					result = true;
				}
				else
				{
					qDebug() << "Nickname" << nickname << "not found in MySql DB";
					return false;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found nickname" << nickname << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return result;
}

int DBWorker::GetFreeUID()
{
	SettingsManager* pSettings = gEnv->pSettings;
	int uid = -1;

	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			// Get uids row and create new uid if uids row are empty
			QString buff = pRedis->SendSyncQuery("GET", "uids");

			if (buff.isEmpty())
			{
				qDebug() << "Key 'uids' not found! Creating key 'uids'...";

				buff = pRedis->SendSyncQuery("SET", "uids", "100001");

				if (buff == "OK")
				{
					uid = 100001;
					return uid;
				}
				else
				{
					qCritical() << "Error creating key 'uids'!!!";
					return uid;
				}

				buff.clear();
			}
			else
			{
				int tmp = buff.toInt() + 1;

				qDebug() << "Key 'uids' found! Creating new uid = " << tmp;
				buff = pRedis->SendSyncQuery("SET", "uids", QString::number(tmp));

				if (buff == "OK")
				{
					uid = tmp;
					qDebug() << "New uid created =" << uid;
					return uid;
				}
				else
				{
					qCritical() << "Error creating uid!";
					return uid;
				}
			}
		}
		else
		{
			qCritical() << "Failed found free uid in Redis DB because Redis DB not opened!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM users WHERE uid=(SELECT MAX(uid) FROM users)");

			if (query->exec())
			{
				if (query->next())
				{			
					int last_uid = query->value(0).toInt();

					qDebug() << "Last uid from table = " << last_uid;
					uid = last_uid + 1;
					return uid;
				}
				else
				{
					qDebug() << "Not any users in table, get first uid";
					uid = 100001;
					return uid;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found free uid in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return uid;
}

int DBWorker::GetUIDbyNick(QString nickname)
{
	SettingsManager* pSettings = gEnv->pSettings;
	int uid = -1;

	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			// Get uids row and create new uid if uids row are empty
			QString buff = pRedis->SendSyncQuery("GET", "nicknames:" + nickname);

			if (!buff.isEmpty())
			{
				qDebug() << "UID for" << nickname << "found in Redis DB";

				uid = buff.toInt();
				return uid;
			}
			else
			{
				qDebug() << "UID for" << nickname << "not found in Redis DB";
				return uid;
			}
		}
		else
		{
			qCritical() << "Failed found UID for " << nickname << "in Redis DB because Redis DB not connected!";
			return uid;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM profiles WHERE nickname=:nickname");
			query->bindValue(":nickname", nickname);

			if (query->exec())
			{
				if (query->next())
				{
					qDebug() << "UID for" << nickname << "found in MySql DB";
					uid = query->value(0).toInt();
					return uid;
				}
				else
				{
					qDebug() << "UID for" << nickname << "not found in MySql DB";
					return uid;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return uid;
			}
		}
		else
		{
			qCritical() << "Failed found UID for " << nickname << "in MySql DB because MySql DB not opened!";
			return uid;
		}
	}

	return uid;
}

SUser* DBWorker::GetUserData(QString login)
{
	SettingsManager* pSettings = gEnv->pSettings;
	SUser *dbUser = new SUser;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "users:" + login;
			QList<QByteArray> rawCmd = { "HGETALL", key.toUtf8() };
			QString buff = pRedis->SendSyncQuery(rawCmd);

			int dbUid = GetValueFromRawString("uid", buff).toInt();
			QString dbLogin = GetValueFromRawString("login", buff);
			QString dbPassword = GetValueFromRawString("password", buff);
			int dbBanStatus = GetValueFromRawString("ban", buff).toInt();

			if (dbUid > 0 && !dbLogin.isEmpty() && !dbPassword.isEmpty())
			{
				qDebug() << "User data for" << login << "is found in Redis DB";

				dbUser->uid = dbUid;
				dbUser->login = dbLogin;
				dbUser->password = dbPassword;

				if (dbBanStatus > 0)
					dbUser->bBanStatus = true;
				else
					dbUser->bBanStatus = false;

				return dbUser;
			}
			else
			{
				qDebug() << "User data for" << login << "not found in Redis DB";
				return nullptr;
			}
		}
		else
		{
			qCritical() << "Failed found login" << login << "in Redis DB because Redis DB not opened!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM users WHERE login=:login");
			query->bindValue(":login", login);

			if (query->exec())
			{
				if (query->next())
				{
					qDebug() << "User data for" << login << "is found in MySql DB";
					dbUser->uid = query->value("uid").toInt(); // uid
					dbUser->login =  query->value("login").toString(); // login
					dbUser->password =  query->value("password").toString(); // password
					int dbBanStatus = query->value("ban").toInt(); // ban status

					if (dbBanStatus > 0)
						dbUser->bBanStatus = true;
					else
						dbUser->bBanStatus = false;

					return dbUser;
				}
				else
				{
					qDebug() << "User data for" << login << "not found in MySql DB";
					return nullptr;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found login" << login << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return nullptr;
}

SProfile* DBWorker::GetUserProfile(int uid)
{
	SettingsManager* pSettings = gEnv->pSettings;
	SProfile *dbProfile = new SProfile;
	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "profiles:" + QString::number(uid);
			QList<QByteArray> rawCmd = { "HGETALL", key.toUtf8() };
			QString buff = pRedis->SendSyncQuery(rawCmd);

			int dbUid = GetValueFromRawString("uid", buff).toInt();
			QString dbNickname = GetValueFromRawString("nickname", buff);
			QString dbModel = GetValueFromRawString("fileModel", buff);
			int dbLvl = GetValueFromRawString("lvl", buff).toInt();
			int dbXp = GetValueFromRawString("xp", buff).toInt();
			int dbMoney = GetValueFromRawString("money", buff).toInt();
			QString dbItems = GetValueFromRawString("items", buff);
			QString dbFriends = GetValueFromRawString("friends", buff);

			if (dbUid > 0 && !dbNickname.isEmpty() && !dbModel.isEmpty())
			{
				qDebug() << "Profile" << uid << "is found in Redis DB";

				dbProfile->uid = dbUid;
				dbProfile->nickname = dbNickname;
				dbProfile->fileModel = dbModel;
				dbProfile->lvl = dbLvl;
				dbProfile->xp = dbXp;
				dbProfile->money = dbMoney;
				dbProfile->items = dbItems;
				dbProfile->friends = dbFriends;

				return dbProfile;
			}
			else
			{
				qDebug() << "Profile" << uid << "not found in Redis DB";
				return nullptr;
			}
		}
		else
		{
			qCritical() << "Failed found profile" << uid << "in Redis DB because Redis DB not connected!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("SELECT * FROM profiles WHERE uid=:uid");
			query->bindValue(":uid", uid);

			if (query->exec())
			{
				if (query->next())
				{
					qDebug() << "Profile" << uid << "is found in MySql DB";

					dbProfile->uid = query->value("uid").toInt();
					dbProfile->nickname = query->value("nickname").toString();
					dbProfile->fileModel = query->value("fileModel").toString();
					dbProfile->lvl = query->value("lvl").toInt();
					dbProfile->xp = query->value("xp").toInt();
					dbProfile->money = query->value("money").toInt();
					dbProfile->items = query->value("items").toString();
					dbProfile->friends = query->value("friends").toString();

					return dbProfile;
				}
				else
				{
					qDebug() << "Profile" << uid << "not found in MySql DB";
					return nullptr;
				}
			}
			else
			{
				qWarning() << "Failed send query to MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed found profile" << uid << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	return nullptr;
}

bool DBWorker::CreateUser(int uid, QString login, QString password)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "users:" + login;
			QList<QByteArray> rawCmd = { "HMSET", key.toUtf8(), "uid", QString::number(uid).toUtf8(), "login", login.toUtf8(), "password", password.toUtf8(), "ban", "0" };
			QString buff = pRedis->SendSyncQuery(rawCmd);

			if (buff == "OK")
			{
				qDebug() << "User" << login << "created in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Failed create user" << login << " in Redis DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed create user" << login << "in Redis DB because Redis DB not connected!";
			return false;
		}
	}
	

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("INSERT INTO users (uid, login, password, ban) VALUES (:uid, :login, :password, :ban)");
			query->bindValue(":uid", uid);
			query->bindValue(":login", login);
			query->bindValue(":password", password);
			query->bindValue(":ban", 0);

			if (query->exec())
			{
				qDebug() << "User" << login << "created in MySql DB";
				result = true;
			}
			else
			{
				qWarning() << "Failed create user" << login << "in MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed create user" << login << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	// Redis background saving
	if (pSettings->GetVariable("bUseRedis").toBool() && pSettings->GetVariable("redis_bg_saving").toBool() && result && pRedis != nullptr)
	{
		QString save_buff = pRedis->SendSyncQuery("BGSAVE");
		if (!save_buff.isEmpty())
		{
			qDebug() << "Redis background saving complete";
			result = true;
		}
		else
		{
			qDebug() << "Redis background saving failed";
			return false;
		}
	}

	return result;
}

bool DBWorker::CreateProfile(SProfile *profile)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "profiles:" + QString::number(profile->uid);

			QList<QByteArray> rawCmd = { "HMSET", key.toUtf8(),
			"uid", QString::number(profile->uid).toUtf8(),
			"nickname", profile->nickname.toUtf8(),
			"fileModel", profile->fileModel.toUtf8(),
			"lvl", QString::number(profile->lvl).toUtf8(),
			"xp", QString::number(profile->xp).toUtf8(),
			"money", QString::number(profile->money).toUtf8(),
			"items", profile->items.toUtf8(),
			"friends", profile->friends.toUtf8() };

			QString buff = pRedis->SendSyncQuery(rawCmd);
			QString buff2 = pRedis->SendSyncQuery("SET", "nicknames:" + profile->nickname, QString::number(profile->uid));

			if (buff == "OK" && buff2 == "OK")
			{
				qDebug() << "Profile" << profile->nickname << "created in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Failed create" << profile->nickname << " profile in Redis DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed create profile" << profile->nickname << "in Redis DB because Redis DB not opened!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("INSERT INTO profiles (uid, nickname, fileModel, lvl, xp, money, items, friends) "
				"VALUES (:uid, :nickname, :fileModel, :lvl, :xp, :money, :items, :friends)");
			query->bindValue(":uid", profile->uid);
			query->bindValue(":nickname", profile->nickname);
			query->bindValue(":fileModel", profile->fileModel);
			query->bindValue(":lvl", profile->lvl);
			query->bindValue(":xp", profile->xp);
			query->bindValue(":money", profile->money);
			query->bindValue(":items", profile->items);
			query->bindValue(":friends", profile->friends);


			if (query->exec())
			{
				qDebug() << "Profile" << profile->nickname << "created in MySql DB";
				result = true;
			}
			else
			{
				qWarning() << "Failed create profile" << profile->nickname << "in MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed create profile" << profile->nickname << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	// Redis background saving
	if (pSettings->GetVariable("bUseRedis").toBool() && pSettings->GetVariable("redis_bg_saving").toBool() && result && pRedis != nullptr)
	{
		QString save_buff = pRedis->SendSyncQuery("BGSAVE");
		if (!save_buff.isEmpty())
		{
			qDebug() << "Redis background saving complete";
			result = true;
		}
		else
		{
			qDebug() << "Redis background saving failed";
			return false;
		}
	}

	return result;
}

bool DBWorker::UpdateProfile(SProfile *profile)
{
	SettingsManager* pSettings = gEnv->pSettings;
	bool result = false;

	// Redis
	if (pSettings->GetVariable("bUseRedis").toBool() && pRedis != nullptr)
	{
		if (pRedis->connectStatus)
		{
			QString key = "profiles:" + QString::number(profile->uid);

			QList<QByteArray> rawCmd = { "HMSET", key.toUtf8(),
				"uid", QString::number(profile->uid).toUtf8(),
				"nickname", profile->nickname.toUtf8(),
				"fileModel", profile->fileModel.toUtf8(),
				"lvl", QString::number(profile->lvl).toUtf8(),
				"xp", QString::number(profile->xp).toUtf8(),
				"money", QString::number(profile->money).toUtf8(),
				"items", profile->items.toUtf8(),
				"friends", profile->friends.toUtf8() };

			QString buff = pRedis->SendSyncQuery(rawCmd);

			if (buff == "OK")
			{
				qDebug() << "Profile" << profile->nickname << "updated in Redis DB";
				result = true;
			}
			else
			{
				qDebug() << "Failed update profile" << profile->nickname << " in Redis DB";

				return false;
			}
		}
		else
		{
			qCritical() << "Failed update profile" << profile->nickname << "in Redis DB because Redis DB not opened!";
			return false;
		}
	}

	// MySql
	if (pSettings->GetVariable("bUseMySQL").toBool() && pMySql != nullptr)
	{
		if (pMySql->connectStatus)
		{
			QSqlQuery *query = new QSqlQuery(pMySql->GetDatabase());
			query->prepare("UPDATE profiles SET nickname=:nickname, fileModel=:fileModel, lvl=:lvl, xp=:xp, money=:money, items=:items, friends=:friends WHERE uid=:uid");
			query->bindValue(":uid", profile->uid);
			query->bindValue(":nickname", profile->nickname);
			query->bindValue(":fileModel", profile->fileModel);
			query->bindValue(":lvl", profile->lvl);
			query->bindValue(":xp", profile->xp);
			query->bindValue(":money", profile->money);
			query->bindValue(":items", profile->items);
			query->bindValue(":friends", profile->friends);

			if (query->exec())
			{
				qDebug() << "Profile" << profile->nickname << "updated in MySql DB";
				result = true;
			}
			else
			{
				qWarning() << "Failed update profile" << profile->nickname << "in MySql DB";
				return false;
			}
		}
		else
		{
			qCritical() << "Failed update profile" << profile->nickname << "in MySql DB because MySql DB not opened!";
			return false;
		}
	}

	// Redis background saving
	if (pSettings->GetVariable("bUseRedis").toBool() && pSettings->GetVariable("redis_bg_saving").toBool() && result && pRedis != nullptr)
	{
		QString save_buff = pRedis->SendSyncQuery("BGSAVE");
		if (!save_buff.isEmpty())
		{
			qDebug() << "Redis background saving complete";
			result = true;
		}
		else
		{
			qDebug() << "Redis background saving failed";
			return false;
		}
	}

	return result;
}

QString DBWorker::GetValueFromRawString(const char * valuename, QString rawString)
{
	if (rawString.isEmpty())
		return QString();

	QRegExp RN("\r\n");
	QStringList strList;
	strList = rawString.split(RN);

	for (int i = 0; i < strList.size(); ++i)
	{
		QString value =  strList[i];

		if (value == valuename)
		{
			QString result = strList[i + 2];
			return result;
		}
	}

	return QString();
}
// Copyright � 2016 Ilya Chernetsov. All rights reserved. Contacts: <chernecoff@gmail.com>
// License: http://opensource.org/licenses/MIT

#ifndef _FireNET_Loader_
#define _FireNET_Loader_

#ifndef _WINDEF_
class HINSTANCE__;
typedef HINSTANCE__* HINSTANCE;
#endif

#include "StdAfx.h"

#if defined(DEDICATED_SERVER)
#define PLUGIN_NAME "FireNET_Dedicated.dll"
#else
#define PLUGIN_NAME "FireNET.dll"
#endif

#define PLUGIN_LOADED "[" PLUGIN_NAME "] " "Plugin loaded!"
#define PLUGIN_ERROR "[" PLUGIN_NAME "] " "Error loading plugin!"

struct SProfile
{
	int uid;
	const char* nickname;
	const char* fileModel;
	int lvl;
	int xp;
	int money;
	const char* items;
	const char* friends;
};


class CPLuginLoader
{
public:
	CPLuginLoader();
	~CPLuginLoader();

	void LoadPlugin();
	void ReloadPlugins();
	void FreePlugins();

	void(*RegisterFlowNodes)(void);
	void(*SendRequest)(const char*);
	int(*GetUID)(void);

#if defined(DEDICATED_SERVER)
	void(*GameServerUpdateInfo)(void);
	SProfile*(*GameServerGetOnlineProfile)(int);
	void(*GameServerUpdateOnlineProfile)(SProfile*);
#endif

	HINSTANCE hndl;
private:
	void(*InitModule)(SSystemGlobalEnvironment&);
};

extern CPLuginLoader* pPluginLoader;

#endif
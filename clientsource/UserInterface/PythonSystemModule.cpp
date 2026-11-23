#include "StdAfx.h"
#include "PythonSystem.h"

PyObject* systemGetWidth(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetWidth());
}

PyObject* systemGetHeight(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetHeight());
}

PyObject* systemSetInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	PyObject* poHandler;
	if (!PyTuple_GetObject(poArgs, 0, &poHandler))
		return Py_BuildException();

	CPythonSystem::Instance().SetInterfaceHandler(poHandler);
	return Py_BuildNone();
}

PyObject* systemDestroyInterfaceHandler(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().DestroyInterfaceHandler();
	return Py_BuildNone();
}

PyObject* systemReserveResource(PyObject* poSelf, PyObject* poArgs)
{
	std::set<std::string> ResourceSet;
	CResourceManager::Instance().PushBackgroundLoadingSet(ResourceSet);
	return Py_BuildNone();
}

PyObject* systemisInterfaceConfig(PyObject* poSelf, PyObject* poArgs)
{
	int isInterfaceConfig = CPythonSystem::Instance().isInterfaceConfig();
	return Py_BuildValue("i", isInterfaceConfig);
}

PyObject* systemSaveWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	int iVisible;
	if (!PyTuple_GetInteger(poArgs, 1, &iVisible))
		return Py_BuildException();

	int iMinimized;
	if (!PyTuple_GetInteger(poArgs, 2, &iMinimized))
		return Py_BuildException();

	int ix;
	if (!PyTuple_GetInteger(poArgs, 3, &ix))
		return Py_BuildException();

	int iy;
	if (!PyTuple_GetInteger(poArgs, 4, &iy))
		return Py_BuildException();

	int iHeight;
	if (!PyTuple_GetInteger(poArgs, 5, &iHeight))
		return Py_BuildException();

	CPythonSystem::Instance().SaveWindowStatus(iIndex, iVisible, iMinimized, ix, iy, iHeight);
	return Py_BuildNone();
}

PyObject* systemGetWindowStatus(PyObject* poSelf, PyObject* poArgs)
{
	int iIndex;
	if (!PyTuple_GetInteger(poArgs, 0, &iIndex))
		return Py_BuildException();

	const CPythonSystem::TWindowStatus& c_rWindowStatus = CPythonSystem::Instance().GetWindowStatusReference(iIndex);
	return Py_BuildValue("iiiii", c_rWindowStatus.isVisible,
		c_rWindowStatus.isMinimized,
		c_rWindowStatus.ixPosition,
		c_rWindowStatus.iyPosition,
		c_rWindowStatus.iHeight);
}

PyObject* systemGetConfig(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::TConfig* tmp = CPythonSystem::Instance().GetConfig();

	int iRes = CPythonSystem::Instance().GetResolutionIndex(tmp->width, tmp->height, tmp->bpp);
	int iFrequency = CPythonSystem::Instance().GetFrequencyIndex(iRes, tmp->frequency);

	return Py_BuildValue("iiiiiiii", iRes,
		iFrequency,
		tmp->is_software_cursor,
		tmp->is_object_culling,
		tmp->music_volume,
		tmp->voice_volume,
		tmp->gamma,
		tmp->iDistance);
}

PyObject* systemSetSaveID(PyObject* poSelf, PyObject* poArgs)
{
	int iValue;
	if (!PyTuple_GetInteger(poArgs, 0, &iValue))
		return Py_BuildException();

	char* szSaveID;
	if (!PyTuple_GetString(poArgs, 1, &szSaveID))
		return Py_BuildException();

	CPythonSystem::Instance().SetSaveID(iValue, szSaveID);
	return Py_BuildNone();
}

PyObject* systemisSaveID(PyObject* poSelf, PyObject* poArgs)
{
	int value = CPythonSystem::Instance().IsSaveID();
	return Py_BuildValue("i", value);
}

PyObject* systemGetSaveID(PyObject* poSelf, PyObject* poArgs)
{
	const char* c_szSaveID = CPythonSystem::Instance().GetSaveID();
	return Py_BuildValue("s", c_szSaveID);
}

PyObject* systemGetMusicVolume(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("f", CPythonSystem::Instance().GetMusicVolume());
}

PyObject* systemGetSoundVolume(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetSoundVolume());
}

PyObject* systemSetMusicVolume(PyObject* poSelf, PyObject* poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetMusicVolume(fVolume);
	return Py_BuildNone();
}

PyObject* systemSetSoundVolumef(PyObject* poSelf, PyObject* poArgs)
{
	float fVolume;
	if (!PyTuple_GetFloat(poArgs, 0, &fVolume))
		return Py_BuildException();

	CPythonSystem::Instance().SetSoundVolumef(fVolume);
	return Py_BuildNone();
}

PyObject* systemIsSoftwareCursor(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsSoftwareCursor());
}

PyObject* systemSetViewChatFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetViewChatFlag(iFlag);

	return Py_BuildNone();
}

PyObject* systemIsViewChat(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsViewChat());
}

PyObject* systemSetAlwaysShowNameFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetAlwaysShowNameFlag(iFlag);

	return Py_BuildNone();
}

PyObject* systemSetShowDamageFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowDamageFlag(iFlag);

	return Py_BuildNone();
}

PyObject* systemSetShowSalesTextFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowSalesTextFlag(iFlag);

	return Py_BuildNone();
}

#ifdef WJ_SHOW_MOB_INFO
#include "PythonCharacterManager.h"
#include "PythonNonPlayer.h"
#include "PythonSystem.h"
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBAIFLAG)
PyObject* systemIsShowMobAIFlag(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobAIFlag());
}
PyObject* systemSetShowMobAIFlag(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobAIFlagFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it = CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti = CPythonCharacterManager::Instance().CharacterInstanceEnd();
		it != ti;
		++it)
	{
		CInstanceBase* pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoAiFlagRefresh();
	}
	return Py_BuildNone();
}
#endif
#if defined(WJ_SHOW_MOB_INFO) && defined(ENABLE_SHOW_MOBLEVEL)
PyObject* systemIsShowMobLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowMobLevel());
}
PyObject* systemSetShowMobLevel(PyObject* poSelf, PyObject* poArgs)
{
	int iFlag;
	if (!PyTuple_GetInteger(poArgs, 0, &iFlag))
		return Py_BuildException();

	CPythonSystem::Instance().SetShowMobLevelFlag(iFlag);

	for (CPythonCharacterManager::CharacterIterator it = CPythonCharacterManager::Instance().CharacterInstanceBegin(), ti = CPythonCharacterManager::Instance().CharacterInstanceEnd();
		it != ti;
		++it)
	{
		CInstanceBase* pkInst = *it;
		if (pkInst && pkInst->IsEnemy())
			pkInst->MobInfoLevelRefresh();
	}
	return Py_BuildNone();
}
#endif
#endif

PyObject* systemIsAlwaysShowName(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsAlwaysShowName());
}

PyObject* systemIsShowDamage(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowDamage());
}

PyObject* systemIsShowSalesText(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsShowSalesText());
}

PyObject* systemSetConfig(PyObject* poSelf, PyObject* poArgs)
{
	int res_index;
	int width;
	int height;
	int bpp;
	int frequency_index;
	int frequency;
	int software_cursor;
	int shadow;
	int object_culling;
	int music_volume;
	int voice_volume;
	int gamma;
	int distance;

	if (!PyTuple_GetInteger(poArgs, 0, &res_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 2, &software_cursor))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 3, &shadow))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 4, &object_culling))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 5, &music_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 6, &voice_volume))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 7, &gamma))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 8, &distance))
		return Py_BuildException();

	if (!CPythonSystem::Instance().GetResolution(res_index, (DWORD*)&width, (DWORD*)&height, (DWORD*)&bpp))
		return Py_BuildNone();

	if (!CPythonSystem::Instance().GetFrequency(res_index, frequency_index, (DWORD*)&frequency))
		return Py_BuildNone();

	CPythonSystem::TConfig tmp;

	memcpy(&tmp, CPythonSystem::Instance().GetConfig(), sizeof(tmp));

	tmp.width = width;
	tmp.height = height;
	tmp.bpp = bpp;
	tmp.frequency = frequency;
	tmp.is_software_cursor = software_cursor ? true : false;
	tmp.is_object_culling = object_culling ? true : false;
	tmp.music_volume = (char)music_volume;
	tmp.voice_volume = (char)voice_volume;
	tmp.gamma = gamma;
	tmp.iDistance = distance;

	CPythonSystem::Instance().SetConfig(&tmp);
	return Py_BuildNone();
}

PyObject* systemApplyConfig(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::Instance().ApplyConfig();
	return Py_BuildNone();
}

PyObject* systemSaveConfig(PyObject* poSelf, PyObject* poArgs)
{
	int ret = CPythonSystem::Instance().SaveConfig();
	return Py_BuildValue("i", ret);
}

PyObject* systemGetResolutionCount(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetResolutionCount());
}

PyObject* systemGetFrequencyCount(PyObject* poSelf, PyObject* poArgs)
{
	int	index;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	return Py_BuildValue("i", CPythonSystem::Instance().GetFrequencyCount(index));
}

PyObject* systemGetResolution(PyObject* poSelf, PyObject* poArgs)
{
	int	index;
	DWORD width = 0, height = 0, bpp = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	CPythonSystem::Instance().GetResolution(index, &width, &height, &bpp);
	return Py_BuildValue("iii", width, height, bpp);
}

PyObject* systemGetCurrentResolution(PyObject* poSelf, PyObject* poArgs)
{
	CPythonSystem::TConfig* tmp = CPythonSystem::Instance().GetConfig();
	return Py_BuildValue("iii", tmp->width, tmp->height, tmp->bpp);
}

PyObject* systemGetFrequency(PyObject* poSelf, PyObject* poArgs)
{
	int	index, frequency_index;
	DWORD frequency = 0;

	if (!PyTuple_GetInteger(poArgs, 0, &index))
		return Py_BuildException();

	if (!PyTuple_GetInteger(poArgs, 1, &frequency_index))
		return Py_BuildException();

	CPythonSystem::Instance().GetFrequency(index, frequency_index, &frequency);
	return Py_BuildValue("i", frequency);
}

PyObject* systemGetShadowLevel(PyObject* poSelf, PyObject* poArgs)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetShadowLevel());
}

PyObject* systemSetShadowLevel(PyObject* poSelf, PyObject* poArgs)
{
	int level;

	if (!PyTuple_GetInteger(poArgs, 0, &level))
		return Py_BuildException();

	if (level > 0)
		CPythonSystem::Instance().SetShadowLevel(level);

	return Py_BuildNone();
}

PyObject* systemGetLanguage(PyObject*, PyObject*)
{
	FILE* file = fopen("locale.cfg", "rb");
	char line[128] = { 0 };
	char language[16] = "ro";
	if (file)
	{
		if (fgets(line, sizeof(line), file))
		{
			char* token = strrchr(line, ' ');
			if (token)
			{
				while (*token == ' ') ++token;
				sscanf(token, "%15s", language);
			}
		}
		fclose(file);
	}
	return Py_BuildValue("s", language);
}

PyObject* systemGetAnonymousCountryMode(PyObject*, PyObject*)
{
	return Py_BuildValue("i", CPythonSystem::Instance().GetAnonymousCountryMode() ? 1 : 0);
}

PyObject* systemSetAnonymousCountryMode(PyObject*, PyObject* poArgs)
{
	int enabled;
	if (!PyTuple_GetInteger(poArgs, 0, &enabled))
		return Py_BuildException();
	CPythonSystem::Instance().SetAnonymousCountryMode(enabled != 0);
	CPythonSystem::Instance().SaveConfig();
	return Py_BuildNone();
}

PyObject* systemGetFontName(PyObject*, PyObject*)
{
	return Py_BuildValue("s", "Tahoma");
}

PyObject* systemGetFontSize(PyObject*, PyObject*)
{
	return Py_BuildValue("i", 12);
}

PyObject* systemGetFont(PyObject*, PyObject*)
{
	return Py_BuildValue("s", "Tahoma:12");
}

PyObject* systemSetFont(PyObject*, PyObject*)
{
	return Py_BuildNone();
}

PyObject* systemIsScaleMode(PyObject*, PyObject*)
{
	return Py_BuildValue("i", 0);
}

PyObject* systemGetScaledResolution(PyObject*, PyObject*)
{
	return Py_BuildValue("ii", CPythonSystem::Instance().GetWidth(), CPythonSystem::Instance().GetHeight());
}

static float s_androidInterfaceScale = 1.0f;
PyObject* systemGetScaleInterface(PyObject*, PyObject*) { return Py_BuildValue("f", s_androidInterfaceScale); }
PyObject* systemSetScaleInterface(PyObject*, PyObject* args)
{
	float scale;
	if (!PyTuple_GetFloat(args, 0, &scale)) return Py_BuildException();
	s_androidInterfaceScale = scale;
	return Py_BuildNone();
}
PyObject* systemSetIsScaleInterface(PyObject*, PyObject*) { return Py_BuildNone(); }

// Android keeps these desktop-only options for script compatibility.  The
// renderer owns the real surface size; values are still persisted in config
// where the legacy scripts expect them.
static int s_samplingLevel = 0;
static int s_textureViewType = 0;
static int s_nightMode = 0;
static int s_questTextDelay = 0;
static int s_showFloatingText = 1;
static int s_showDamage = 2;
static int s_showNameFlag = 0;
static int s_shrinkMap = 0;
static int s_shopViewOnlyFound = 0;
static float s_shopRangeView = 1000.0f;
static int s_customWidth = 0;
static int s_customHeight = 0;

PyObject* systemIsWindowed(PyObject*, PyObject*)
{
	return Py_BuildValue("i", CPythonSystem::Instance().IsWindowed() ? 1 : 0);
}

PyObject* systemSetWindowed(PyObject*, PyObject* args)
{
	int value;
	if (!PyTuple_GetInteger(args, 0, &value)) return Py_BuildException();
	CPythonSystem::Instance().GetConfig()->bWindowed = value != 0;
	return Py_BuildNone();
}

PyObject* systemSetResolution(PyObject*, PyObject* args)
{
	int width, height, bpp;
	if (!PyTuple_GetInteger(args, 0, &width) ||
		!PyTuple_GetInteger(args, 1, &height) ||
		!PyTuple_GetInteger(args, 2, &bpp)) return Py_BuildException();
	if (width > 0 && height > 0)
	{
		CPythonSystem::TConfig* cfg = CPythonSystem::Instance().GetConfig();
		cfg->width = width; cfg->height = height; cfg->bpp = bpp;
	}
	return Py_BuildNone();
}

PyObject* systemGetCustomResolution(PyObject*, PyObject*) { return Py_BuildValue("ii", s_customWidth, s_customHeight); }
PyObject* systemSetCustomResolution(PyObject*, PyObject* args)
{
	if (!PyTuple_GetInteger(args, 0, &s_customWidth) || !PyTuple_GetInteger(args, 1, &s_customHeight))
		return Py_BuildException();
	return Py_BuildNone();
}

PyObject* systemSetLanguage(PyObject*, PyObject* args)
{
	char* language;
	if (!PyTuple_GetString(args, 0, &language)) return Py_BuildException();
	FILE* file = fopen("locale.cfg", "wb");
	if (file) { fprintf(file, "10002 1252 %s\n", language); fclose(file); }
	return Py_BuildNone();
}

#define SYSTEM_INT_GETTER(name, value) PyObject* name(PyObject*, PyObject*) { return Py_BuildValue("i", value); }
#define SYSTEM_INT_SETTER(name, value) PyObject* name(PyObject*, PyObject* args) { int v; if (!PyTuple_GetInteger(args, 0, &v)) return Py_BuildException(); value = v; return Py_BuildNone(); }
SYSTEM_INT_GETTER(systemGetSamplingLevel, s_samplingLevel)
SYSTEM_INT_SETTER(systemSetSamplingLevel, s_samplingLevel)
SYSTEM_INT_GETTER(systemGetTextureViewType, s_textureViewType)
SYSTEM_INT_SETTER(systemSetTextureViewType, s_textureViewType)
SYSTEM_INT_GETTER(systemGetNightMode, s_nightMode)
SYSTEM_INT_SETTER(systemSetNightMode, s_nightMode)
SYSTEM_INT_GETTER(systemGetQuestTextDelay, s_questTextDelay)
SYSTEM_INT_SETTER(systemSetQuestTextDelay, s_questTextDelay)
SYSTEM_INT_GETTER(systemGetShowFloatingText, s_showFloatingText)
SYSTEM_INT_SETTER(systemSetShowFloatingText, s_showFloatingText)
SYSTEM_INT_GETTER(systemGetShowDamage, s_showDamage)
SYSTEM_INT_SETTER(systemSetShowNameFlag, s_showNameFlag)
SYSTEM_INT_GETTER(systemIsGroupShowName, s_showNameFlag)
SYSTEM_INT_GETTER(systemIsShrinkMap, s_shrinkMap)
SYSTEM_INT_SETTER(systemSetShrinkMap, s_shrinkMap)
SYSTEM_INT_GETTER(systemIsShopViewOnlyFound, s_shopViewOnlyFound)
SYSTEM_INT_SETTER(systemSetShopViewOnlyFound, s_shopViewOnlyFound)
#undef SYSTEM_INT_GETTER
#undef SYSTEM_INT_SETTER

PyObject* systemGetShopRangeView(PyObject*, PyObject*) { return PyFloat_FromDouble(s_shopRangeView); }
PyObject* systemSetShopRangeView(PyObject*, PyObject* args)
{
	float value;
	if (!PyTuple_GetFloat(args, 0, &value)) return Py_BuildException();
	s_shopRangeView = value;
	return Py_BuildNone();
}

void initsystemSetting()
{
	static PyMethodDef s_methods[] =
	{
		{ "GetWidth",					systemGetWidth,					METH_VARARGS },
		{ "GetHeight",					systemGetHeight,				METH_VARARGS },

		{ "SetInterfaceHandler",		systemSetInterfaceHandler,		METH_VARARGS },
		{ "DestroyInterfaceHandler",	systemDestroyInterfaceHandler,	METH_VARARGS },
		{ "ReserveResource",			systemReserveResource,			METH_VARARGS },

		{ "isInterfaceConfig",			systemisInterfaceConfig,		METH_VARARGS },
		{ "SaveWindowStatus",			systemSaveWindowStatus,			METH_VARARGS },
		{ "GetWindowStatus",			systemGetWindowStatus,			METH_VARARGS },

		{ "GetResolutionCount",			systemGetResolutionCount,		METH_VARARGS },
		{ "GetFrequencyCount",			systemGetFrequencyCount,		METH_VARARGS },

		{ "GetCurrentResolution",		systemGetCurrentResolution,		METH_VARARGS },

		{ "GetResolution",				systemGetResolution,			METH_VARARGS },
		{ "GetFrequency",				systemGetFrequency,				METH_VARARGS },

		{ "ApplyConfig",				systemApplyConfig,				METH_VARARGS },
		{ "SetConfig",					systemSetConfig,				METH_VARARGS },
		{ "SaveConfig",					systemSaveConfig,				METH_VARARGS },
		{ "GetConfig",					systemGetConfig,				METH_VARARGS },

		{ "SetSaveID",					systemSetSaveID,				METH_VARARGS },
		{ "isSaveID",					systemisSaveID,					METH_VARARGS },
		{ "GetSaveID",					systemGetSaveID,				METH_VARARGS },

		{ "GetMusicVolume",				systemGetMusicVolume,			METH_VARARGS },
		{ "GetSoundVolume",				systemGetSoundVolume,			METH_VARARGS },

		{ "SetMusicVolume",				systemSetMusicVolume,			METH_VARARGS },
		{ "SetSoundVolumef",			systemSetSoundVolumef,			METH_VARARGS },
		{ "IsSoftwareCursor",			systemIsSoftwareCursor,			METH_VARARGS },

		{ "SetViewChatFlag",			systemSetViewChatFlag,			METH_VARARGS },
		{ "IsViewChat",					systemIsViewChat,				METH_VARARGS },

		{ "SetAlwaysShowNameFlag",		systemSetAlwaysShowNameFlag,	METH_VARARGS },
		{ "IsAlwaysShowName",			systemIsAlwaysShowName,			METH_VARARGS },

		{ "SetShowDamageFlag",			systemSetShowDamageFlag,		METH_VARARGS },
		{ "IsShowDamage",				systemIsShowDamage,				METH_VARARGS },

		{ "SetShowSalesTextFlag",		systemSetShowSalesTextFlag,		METH_VARARGS },
		{ "IsShowSalesText",			systemIsShowSalesText,			METH_VARARGS },

		{ "GetShadowLevel",				systemGetShadowLevel,			METH_VARARGS },
		{ "SetShadowLevel",				systemSetShadowLevel,			METH_VARARGS },

#ifdef WJ_SHOW_MOB_INFO
		{ "IsShowMobAIFlag",			systemIsShowMobAIFlag,			METH_VARARGS },
		{ "SetShowMobAIFlag",			systemSetShowMobAIFlag,			METH_VARARGS },

		{ "IsShowMobLevel",				systemIsShowMobLevel,			METH_VARARGS },
		{ "SetShowMobLevel",			systemSetShowMobLevel,			METH_VARARGS },
#endif

		{ "GetLanguage",				systemGetLanguage,				METH_VARARGS },
		{ "GetAnonymousCountryMode",	systemGetAnonymousCountryMode,	METH_VARARGS },
		{ "SetAnonymousCountryMode",	systemSetAnonymousCountryMode,	METH_VARARGS },
		{ "GetFontName",				systemGetFontName,				METH_VARARGS },
		{ "GetFontSize",				systemGetFontSize,				METH_VARARGS },
		{ "GetFont",					systemGetFont,					METH_VARARGS },
		{ "SetFont",					systemSetFont,					METH_VARARGS },
		{ "IsScaleMode",				systemIsScaleMode,				METH_VARARGS },
		{ "GetScaledResolution",		systemGetScaledResolution,		METH_VARARGS },
		{ "GetScaleInterface",		systemGetScaleInterface,		METH_VARARGS },
		{ "SetScaleInterface",		systemSetScaleInterface,		METH_VARARGS },
		{ "SetIsScaleInterface",		systemSetIsScaleInterface,		METH_VARARGS },
		{ "IsWindowed",				systemIsWindowed,				METH_VARARGS },
		{ "SetWindowed",				systemSetWindowed,				METH_VARARGS },
		{ "SetResolution",				systemSetResolution,			METH_VARARGS },
		{ "GetCustomResolution",		systemGetCustomResolution,		METH_VARARGS },
		{ "SetCustomResolution",		systemSetCustomResolution,		METH_VARARGS },
		{ "SetLanguage",				systemSetLanguage,				METH_VARARGS },
		{ "GetSamplingLevel",			systemGetSamplingLevel,			METH_VARARGS },
		{ "SetSamplingLevel",			systemSetSamplingLevel,			METH_VARARGS },
		{ "GetTextureViewType",			systemGetTextureViewType,		METH_VARARGS },
		{ "SetTextureViewType",			systemSetTextureViewType,		METH_VARARGS },
		{ "GetNightMode",				systemGetNightMode,				METH_VARARGS },
		{ "SetNightMode",				systemSetNightMode,				METH_VARARGS },
		{ "GetQuestTextDelay",			systemGetQuestTextDelay,		METH_VARARGS },
		{ "SetQuestTextDelay",			systemSetQuestTextDelay,		METH_VARARGS },
		{ "GetShowFloatingText",		systemGetShowFloatingText,		METH_VARARGS },
		{ "SetShowFloatingText",		systemSetShowFloatingText,		METH_VARARGS },
		{ "GetShowDamage",				systemGetShowDamage,				METH_VARARGS },
		{ "SetShowNameFlag",			systemSetShowNameFlag,			METH_VARARGS },
		{ "IsGroupShowName",			systemIsGroupShowName,			METH_VARARGS },
		{ "IsShrinkMap",				systemIsShrinkMap,				METH_VARARGS },
		{ "SetShrinkMap",				systemSetShrinkMap,				METH_VARARGS },
		{ "GetShopRangeView",			systemGetShopRangeView,			METH_VARARGS },
		{ "SetShopRangeView",			systemSetShopRangeView,			METH_VARARGS },
		{ "IsShopViewOnlyFound",		systemIsShopViewOnlyFound,		METH_VARARGS },
		{ "SetShopViewOnlyFound",		systemSetShopViewOnlyFound,		METH_VARARGS },
		{ NULL,							NULL,							NULL }
	};

	PyObject* poModule = Py_InitModule("systemSetting", s_methods);
	PyModule_AddIntConstant(poModule, "RESOLUTION_MAX_NUM", CPythonSystem::RESOLUTION_MAX_NUM);
	PyModule_AddIntConstant(poModule, "SHOW_DAMAGE_NONE", 0);
	PyModule_AddIntConstant(poModule, "SHOW_DAMAGE_TARGET", 1);
	PyModule_AddIntConstant(poModule, "SHOW_DAMAGE_ALL", 2);

	PyModule_AddIntConstant(poModule, "WINDOW_STATUS", CPythonSystem::WINDOW_STATUS);
	PyModule_AddIntConstant(poModule, "WINDOW_INVENTORY", CPythonSystem::WINDOW_INVENTORY);
	PyModule_AddIntConstant(poModule, "WINDOW_ABILITY", CPythonSystem::WINDOW_ABILITY);
	PyModule_AddIntConstant(poModule, "WINDOW_SOCIETY", CPythonSystem::WINDOW_SOCIETY);
	PyModule_AddIntConstant(poModule, "WINDOW_JOURNAL", CPythonSystem::WINDOW_JOURNAL);
	PyModule_AddIntConstant(poModule, "WINDOW_COMMAND", CPythonSystem::WINDOW_COMMAND);

	PyModule_AddIntConstant(poModule, "WINDOW_QUICK", CPythonSystem::WINDOW_QUICK);
	PyModule_AddIntConstant(poModule, "WINDOW_GAUGE", CPythonSystem::WINDOW_GAUGE);
	PyModule_AddIntConstant(poModule, "WINDOW_MINIMAP", CPythonSystem::WINDOW_MINIMAP);
	PyModule_AddIntConstant(poModule, "WINDOW_CHAT", CPythonSystem::WINDOW_CHAT);
}

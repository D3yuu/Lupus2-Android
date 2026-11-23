#pragma once

class CPlayerSettingsModule
{
	public:
		enum EConfig
		{
			RACE_WARRIOR_M = 0,
			RACE_ASSASSIN_W = 1,
			RACE_SURA_M = 2,
			RACE_SHAMAN_W = 3,
			RACE_WARRIOR_W = 4,
			RACE_ASSASSIN_M = 5,
			RACE_SURA_W = 6,
			RACE_SHAMAN_M = 7,
			COMBO_TYPE_1 = 0,
			COMBO_TYPE_2 = 1,
			COMBO_TYPE_3 = 2,
			COMBO_INDEX_1 = 0,
			COMBO_INDEX_2 = 1,
			COMBO_INDEX_3 = 2,
			COMBO_INDEX_4 = 3,
			COMBO_INDEX_5 = 4,
			COMBO_INDEX_6 = 5,
			FILE_MAX_NUM = 70,
		};

	private:
		typedef struct SMotion
		{
			int wMotionIndex;
			char stName[FILE_MAX_NUM];
			BYTE byPercentage;
		} TMotion;
		
	public:
		const static bool LoadInitData();
		const static bool LoadGameEffect();
		const static bool LoadGeneralMotion(char stFolder[FILE_MAX_NUM]);
		const static bool RegisterEmotionAnis(char stFolder[FILE_MAX_NUM]);
		const static bool LoadGameWarrior(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
		const static bool LoadGameAssassin(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
		const static bool LoadGameSura(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
		const static bool LoadGameShaman(DWORD dwRace, char stFolder[FILE_MAX_NUM]);
};


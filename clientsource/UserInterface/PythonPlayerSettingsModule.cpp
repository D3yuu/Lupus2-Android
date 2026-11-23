#include "StdAfx.h"
#include "PythonPlayerSettingsModule.h"
#include "InstanceBase.h"
#include "../EffectLib/EffectManager.h"
#include "PythonSkill.h"
#include "../gamelib/RaceManager.h"
#include "PythonItem.h"

const CRaceMotionData pkMotionManager;
static const DWORD GUILD_SKILL_DRAGONBLOOD = pkMotionManager.NAME_SKILL + 101;
static const DWORD GUILD_SKILL_DRAGONBLESS = pkMotionManager.NAME_SKILL + 102;
static const DWORD GUILD_SKILL_BLESSARMOR = pkMotionManager.NAME_SKILL + 103;
static const DWORD GUILD_SKILL_SPPEDUP = pkMotionManager.NAME_SKILL + 104;
static const DWORD GUILD_SKILL_DRAGONWRATH = pkMotionManager.NAME_SKILL + 105;
static const DWORD GUILD_SKILL_MAGICUP = pkMotionManager.NAME_SKILL + 106;

static const DWORD HORSE_SKILL_WILDATTACK = pkMotionManager.NAME_SKILL + 121;
static const DWORD HORSE_SKILL_CHARGE = pkMotionManager.NAME_SKILL + 122;
static const DWORD HORSE_SKILL_SPLASH = pkMotionManager.NAME_SKILL + 123;

const bool CPlayerSettingsModule::LoadInitData()
{
	struct TIntroRace
	{
		DWORD race;
		const char* msm;
		const char* introPath;
	};

	static const TIntroRace races[] =
	{
		{ RACE_WARRIOR_M,  "gamedata/warrior_m.msm",  "d:/ymir work/pc/warrior/intro/" },
		{ RACE_WARRIOR_W,  "gamedata/warrior_w.msm",  "d:/ymir work/pc2/warrior/intro/" },
		{ RACE_ASSASSIN_W, "gamedata/assassin_w.msm", "d:/ymir work/pc/assassin/intro/" },
		{ RACE_ASSASSIN_M, "gamedata/assassin_m.msm", "d:/ymir work/pc2/assassin/intro/" },
		{ RACE_SURA_M,     "gamedata/sura_m.msm",     "d:/ymir work/pc/sura/intro/" },
		{ RACE_SURA_W,     "gamedata/sura_w.msm",     "d:/ymir work/pc2/sura/intro/" },
		{ RACE_SHAMAN_W,   "gamedata/shaman_w.msm",   "d:/ymir work/pc/shaman/intro/" },
		{ RACE_SHAMAN_M,   "gamedata/shaman_m.msm",   "d:/ymir work/pc2/shaman/intro/" },
	};

	static const char* introFiles[] = { "wait.msa", "selected.msa", "not_selected.msa" };
	static const WORD introMotions[] =
	{
		pkMotionManager.NAME_INTRO_WAIT,
		pkMotionManager.NAME_INTRO_SELECTED,
		pkMotionManager.NAME_INTRO_NOT_SELECTED,
	};

	CRaceManager& manager = CRaceManager::Instance();
	for (size_t i = 0; i < sizeof(races) / sizeof(races[0]); ++i)
	{
		manager.CreateRace(races[i].race);
		manager.SelectRace(races[i].race);
		CRaceData* raceData = manager.GetSelectedRaceDataPointer();
		if (!raceData || !raceData->LoadRaceData(races[i].msm))
		{
			TraceError("PlayerSettings: failed to load intro race %u from %s", races[i].race, races[i].msm);
			return false;
		}

		raceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
		for (size_t motion = 0; motion < 3; ++motion)
		{
			char path[256];
			snprintf(path, sizeof(path), "%s%s", races[i].introPath, introFiles[motion]);
			raceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, introMotions[motion], path, 0);
		}
	}
	return true;
}

const bool CPlayerSettingsModule::LoadGameEffect()
{
	struct TEffectEntry
	{
		UINT type;
		const char* bone;
		const char* file;
		bool cache;
	};

	// This is the PC client's effect registry. The Android port used to bind
	// LoadGameEffect to a no-op, so timed affects (Strong Body, Aura, etc.),
	// refinement shines and the GM mark had valid assets but no registered
	// effect ID to attach to an actor.
	static const TEffectEntry effects[] =
	{
		{ CInstanceBase::EFFECT_DUST, "", "d:/ymir work/effect/etc/dust/dust.mse", true },
		{ CInstanceBase::EFFECT_HORSE_DUST, "", "d:/ymir work/effect/etc/dust/running_dust.mse", true },
		{ CInstanceBase::EFFECT_HIT, "", "d:/ymir work/effect/hit/blow_1/blow_1_low.mse", true },
		{ CInstanceBase::EFFECT_HPUP_RED, "", "d:/ymir work/effect/etc/recuperation/drugup_red.mse", true },
		{ CInstanceBase::EFFECT_SPUP_BLUE, "", "d:/ymir work/effect/etc/recuperation/drugup_blue.mse", true },
		{ CInstanceBase::EFFECT_SPEEDUP_GREEN, "", "d:/ymir work/effect/etc/recuperation/drugup_green.mse", true },
		{ CInstanceBase::EFFECT_DXUP_PURPLE, "", "d:/ymir work/effect/etc/recuperation/drugup_purple.mse", true },
		{ CInstanceBase::EFFECT_AUTO_HPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_red.mse", true },
		{ CInstanceBase::EFFECT_AUTO_SPUP, "", "d:/ymir work/effect/etc/recuperation/autodrugup_blue.mse", true },
		{ CInstanceBase::EFFECT_RAMADAN_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item1.mse", true },
		{ CInstanceBase::EFFECT_HALLOWEEN_CANDY_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item2.mse", true },
		{ CInstanceBase::EFFECT_HAPPINESS_RING_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item3.mse", true },
		{ CInstanceBase::EFFECT_LOVE_PENDANT_EQUIP, "", "d:/ymir work/effect/etc/buff/buff_item4.mse", true },
		{ CInstanceBase::EFFECT_PENETRATE, "Bip01", "d:/ymir work/effect/hit/gwantong.mse", true },
		{ CInstanceBase::EFFECT_FIRECRACKER, "", "d:/ymir work/effect/etc/firecracker/newyear_firecracker.mse", false },
		{ CInstanceBase::EFFECT_SPIN_TOP, "", "d:/ymir work/effect/etc/firecracker/paing_i.mse", false },
		{ CInstanceBase::EFFECT_SELECT, "", "d:/ymir work/effect/etc/click/click_select.mse", false },
		{ CInstanceBase::EFFECT_TARGET, "", "d:/ymir work/effect/etc/click/click_glow_select.mse", false },
		{ CInstanceBase::EFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun.mse", true },
		{ CInstanceBase::EFFECT_CRITICAL, "Bip01 R Hand", "d:/ymir work/effect/hit/critical.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_TARGET, "", "d:/ymir work/effect/affect/damagevalue/target.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_NOT_TARGET, "", "d:/ymir work/effect/affect/damagevalue/nontarget.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE, "", "d:/ymir work/effect/affect/damagevalue/damage.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_SELFDAMAGE2, "", "d:/ymir work/effect/affect/damagevalue/damage_1.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_POISON, "", "d:/ymir work/effect/affect/damagevalue/poison.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_MISS, "", "d:/ymir work/effect/affect/damagevalue/miss.mse", true },
		{ CInstanceBase::EFFECT_DAMAGE_TARGETMISS, "", "d:/ymir work/effect/affect/damagevalue/target_miss.mse", true },
		{ CInstanceBase::EFFECT_PERCENT_DAMAGE1, "", "d:/ymir work/effect/hit/percent_damage1.mse", true },
		{ CInstanceBase::EFFECT_PERCENT_DAMAGE2, "", "d:/ymir work/effect/hit/percent_damage2.mse", true },
		{ CInstanceBase::EFFECT_PERCENT_DAMAGE3, "", "d:/ymir work/effect/hit/percent_damage3.mse", true },

		{ CInstanceBase::EFFECT_SPAWN_APPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_appear.mse", false },
		{ CInstanceBase::EFFECT_SPAWN_DISAPPEAR, "Bip01", "d:/ymir work/effect/etc/appear_die/monster_die.mse", false },
		{ CInstanceBase::EFFECT_FLAME_ATTACK, "equip_right_hand", "d:/ymir work/effect/hit/blow_flame/flame_3_weapon.mse", false },
		{ CInstanceBase::EFFECT_FLAME_HIT, "", "d:/ymir work/effect/hit/blow_flame/flame_3_blow.mse", false },
		{ CInstanceBase::EFFECT_FLAME_ATTACH, "", "d:/ymir work/effect/hit/blow_flame/flame_3_body.mse", false },
		{ CInstanceBase::EFFECT_ELECTRIC_ATTACK, "equip_right", "d:/ymir work/effect/hit/blow_electric/light_1_weapon.mse", false },
		{ CInstanceBase::EFFECT_ELECTRIC_HIT, "", "d:/ymir work/effect/hit/blow_electric/light_1_blow.mse", false },
		{ CInstanceBase::EFFECT_ELECTRIC_ATTACH, "", "d:/ymir work/effect/hit/blow_electric/light_1_body.mse", false },
		{ CInstanceBase::EFFECT_LEVELUP, "", "d:/ymir work/effect/etc/levelup_1/level_up.mse", false },
		{ CInstanceBase::EFFECT_SKILLUP, "", "d:/ymir work/effect/etc/skillup/skillup_1.mse", false },
		{ CInstanceBase::EFFECT_EMPIRE + 1, "Bip01", "d:/ymir work/effect/etc/empire/empire_A.mse", false },
		{ CInstanceBase::EFFECT_EMPIRE + 2, "Bip01", "d:/ymir work/effect/etc/empire/empire_B.mse", false },
		{ CInstanceBase::EFFECT_EMPIRE + 3, "Bip01", "d:/ymir work/effect/etc/empire/empire_C.mse", false },

		{ CInstanceBase::EFFECT_WEAPON + CInstanceBase::WEAPON_ONEHAND, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_sword_loop.mse", false },
		{ CInstanceBase::EFFECT_WEAPON + CInstanceBase::WEAPON_TWOHAND, "equip_right_hand", "d:/ymir work/pc/warrior/effect/geom_spear_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_YMIR, "Bip01", "d:/ymir work/effect/gm/gm.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_POISON, "Bip01", "d:/ymir work/effect/hit/blow_poison/poison_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_SLOW, "", "d:/ymir work/effect/affect/slow.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_STUN, "Bip01 Head", "d:/ymir work/effect/etc/stun/stun_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_DUNGEON_READY, "", "d:/ymir work/effect/etc/ready/ready.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GYEONGGONG, "", "d:/ymir work/pc/assassin/effect/gyeonggong_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GWIGEOM, "Bip01 R Finger2", "d:/ymir work/pc/sura/effect/gwigeom_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GONGPO, "", "d:/ymir work/pc/sura/effect/fear_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JUMAGAP, "", "d:/ymir work/pc/sura/effect/jumagap_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HOSIN, "", "d:/ymir work/pc/shaman/effect/3hosin_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_BOHO, "", "d:/ymir work/pc/shaman/effect/boho_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_KWAESOK, "", "d:/ymir work/pc/shaman/effect/10kwaesok_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_HEUKSIN, "", "d:/ymir work/pc/sura/effect/heuksin_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_MUYEONG, "", "d:/ymir work/pc/sura/effect/muyeong_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FIRE, "Bip01", "d:/ymir work/effect/hit/blow_flame/flame_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_GICHEON, "Bip01 R Hand", "d:/ymir work/pc/shaman/effect/6gicheon_hand.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_JEUNGRYEOK, "Bip01 L Hand", "d:/ymir work/pc/shaman/effect/jeungryeok_hand.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_PABEOP, "Bip01 Head", "d:/ymir work/pc/sura/effect/pabeop_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_FALLEN_CHEONGEUN, "", "d:/ymir work/pc/warrior/effect/gyeokgongjang_loop.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG1, "", "d:/ymir work/effect/etc/guild_war_flag/flag_red.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG2, "", "d:/ymir work/effect/etc/guild_war_flag/flag_blue.mse", false },
		{ CInstanceBase::EFFECT_AFFECT + CInstanceBase::AFFECT_WAR_FLAG3, "", "d:/ymir work/effect/etc/guild_war_flag/flag_yellow.mse", false },

		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SWORD_REFINED7, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_7.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SWORD_REFINED8, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_8.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SWORD_REFINED9, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_9.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BOW_REFINED7, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_7_b.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BOW_REFINED8, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_8_b.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BOW_REFINED9, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_9_b.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_FANBELL_REFINED7, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_7_f.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_FANBELL_REFINED8, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_8_f.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_FANBELL_REFINED9, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_9_f.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED7, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED8, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED9, "PART_WEAPON", "d:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED7_LEFT, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_7_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED8_LEFT, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_8_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_SMALLSWORD_REFINED9_LEFT, "PART_WEAPON_LEFT", "d:/ymir work/pc/common/effect/sword/sword_9_s.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BODYARMOR_REFINED7, "Bip01", "d:/ymir work/pc/common/effect/armor/armor_7.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BODYARMOR_REFINED8, "Bip01", "d:/ymir work/pc/common/effect/armor/armor_8.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BODYARMOR_REFINED9, "Bip01", "d:/ymir work/pc/common/effect/armor/armor_9.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BODYARMOR_SPECIAL, "Bip01", "d:/ymir work/pc/common/effect/armor/armor-4-2-1.mse", false },
		{ CInstanceBase::EFFECT_REFINED + CInstanceBase::EFFECT_BODYARMOR_SPECIAL2, "Bip01", "d:/ymir work/pc/common/effect/armor/armor-4-2-2.mse", false },
	};

	bool allRegistered = true;
	for (size_t i = 0; i < sizeof(effects) / sizeof(effects[0]); ++i)
	{
		if (!CInstanceBase::RegisterEffect(effects[i].type, effects[i].bone, effects[i].file, effects[i].cache))
			allRegistered = false;
	}
	return allRegistered;
}

const bool CPlayerSettingsModule::LoadGeneralMotion(char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;
	
	std::vector<SMotion> m_vecBasicMotion =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_FLYING, "damage_flying.msa", 0 },
		{ pkMotionManager.NAME_STAND_UP, "falling_stand.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_FLYING_BACK, "back_damage_flying.msa", 0 },
		{ pkMotionManager.NAME_STAND_UP_BACK, "back_falling_stand.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ pkMotionManager.NAME_DIG, "dig.msa", 0 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecBasicMotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	return true;
}

const bool CPlayerSettingsModule::RegisterEmotionAnis(char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecEmotion =
	{
		{ pkMotionManager.NAME_CLAP, "clap.msa", 0 },
		{ pkMotionManager.NAME_CHEERS_1, "cheers_1.msa", 0 },
		{ pkMotionManager.NAME_CHEERS_2, "cheers_2.msa", 0 },
		{ pkMotionManager.NAME_DANCE_1, "dance_1.msa", 0 },
		{ pkMotionManager.NAME_DANCE_2, "dance_2.msa", 0 },
		{ pkMotionManager.NAME_DANCE_3, "dance_3.msa", 0 },
		{ pkMotionManager.NAME_DANCE_4, "dance_4.msa", 0 },
		{ pkMotionManager.NAME_DANCE_5, "dance_5.msa", 0 },
		{ pkMotionManager.NAME_DANCE_6, "dance_6.msa", 0 },
		{ pkMotionManager.NAME_CONGRATULATION, "congratulation.msa", 0 },
#ifdef ENABLE_NEW_EMOTIONS
		{ pkMotionManager.NAME_DANCE_7, "dance_7.msa", 0 },
		{ pkMotionManager.NAME_SELFIE, "selfie.msa", 0 },
		{ pkMotionManager.NAME_DOZE, "doze.msa", 0 },
		{ pkMotionManager.NAME_EXERCISE, "exercise.msa", 0 },
		{ pkMotionManager.NAME_PUSHUP, "pushup.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_1, "amin.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_2, "backflip.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_3, "break.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_4, "cocalar.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_5, "dab.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_6, "facepalm.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_7, "fuckyou.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_8, "laba.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_9, "oriental.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_10, "ridack_animoney.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_11, "pingu.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_12, "twerk.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_13, "samba.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_14, "shimmy.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_15, "slow.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_16, "bestmates.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_17, "flair.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_18, "horon.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_19, "robotdance.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_20, "snowball.msa", 0 },
		{ pkMotionManager.NAME_EMOTION_21, "surprised2.msa", 0 },
#endif
		{ pkMotionManager.NAME_FORGIVE, "forgive.msa", 0 },
		{ pkMotionManager.NAME_ANGRY, "angry.msa", 0 },
		{ pkMotionManager.NAME_ATTRACTIVE, "attractive.msa", 0 },
		{ pkMotionManager.NAME_SAD, "sad.msa", 0 },
		{ pkMotionManager.NAME_SHY, "shy.msa", 0 },
		{ pkMotionManager.NAME_CHEERUP, "cheerup.msa", 0 },
		{ pkMotionManager.NAME_BANTER, "banter.msa", 0 },
		{ pkMotionManager.NAME_JOY, "joy.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_WARRIOR, "french_kiss_with_warrior.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_ASSASSIN, "french_kiss_with_assassin.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_SURA, "french_kiss_with_sura.msa", 0 },
		{ pkMotionManager.NAME_FRENCH_KISS_WITH_SHAMAN, "french_kiss_with_shaman.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_WARRIOR, "kiss_with_warrior.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_ASSASSIN, "kiss_with_assassin.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_SURA, "kiss_with_sura.msa", 0 },
		{ pkMotionManager.NAME_KISS_WITH_SHAMAN, "kiss_with_shaman.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_WARRIOR, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_ASSASSIN, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_SURA, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HIT_WITH_SHAMAN, "slap_hit.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_WARRIOR, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_ASSASSIN, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_SURA, "slap_hurt.msa", 0 },
		{ pkMotionManager.NAME_SLAP_HURT_WITH_SHAMAN, "slap_hurt.msa", 0 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "action/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}
	
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_WEDDING_DRESS);
	for (const auto& it : m_vecEmotion)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "action/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}
	
	std::vector<SMotion> m_vecEmotionDress =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "walk.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_WEDDING_DRESS);
	for (const auto& it : m_vecEmotionDress)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "wedding/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_WEDDING_DRESS, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	return true;
}

const bool CPlayerSettingsModule::LoadGameWarrior(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	if (!pRaceData->SetMotionRandomWeight(pkMotionManager.MODE_GENERAL, pkMotionManager.NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
		{ 1, "samyeon" },
		{ 2, "palbang" },
		{ 3, "jeongwi" },
		{ 4, "geomgyeong" },
		{ 5, "tanhwan" },
		{ 6, "gihyeol" },	
		{ 16, "gigongcham" },
		{ 17, "gyeoksan" },
		{ 18, "daejin" },
		{ 19, "cheongeun" },
		{ 20, "geompung" },
		{ 21, "noegeom" },
	};

	
	char szSkillName[FILE_MAX_NUM];
	char szSkillAdd[4] = "";
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}
	
	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 50 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 50 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0},
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);	
	
	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);
	
	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionTwoHandSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_TWOHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "twohand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_TWOHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}
	
	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseTwoHandSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_TWOHAND_SWORD);
	for (const auto& it : m_vecMotionHorseTwoHandSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_twohand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_TWOHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right_hand");

#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}

const bool CPlayerSettingsModule::LoadGameAssassin(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	if (!pRaceData->SetMotionRandomWeight(pkMotionManager.MODE_GENERAL, pkMotionManager.NAME_WAIT, 0, 70))
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
		{ 1, "amseup" },
		{ 2, "gungsin" },
		{ 3, "charyun" },
		{ 4, "eunhyeong" },
		{ 5, "sangong" },
		{ 6, "seomjeon" },
		{ 16, "yeonsa" },
		{ 17, "gwangyeok" },
		{ 18, "hwajo" },
		{ 19, "gyeonggong" },
		{ 20, "dokgigung" },
		{ 21, "seomgwang" },
	};

	char szSkillName[FILE_MAX_NUM];
	char szSkillAdd[4] = "";
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa" , 0 },
		{ pkMotionManager.NAME_RUN, "run.msa" , 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionDagger =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_8, "combo_08.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_DUALHAND_SWORD);
	for (const auto& it : m_vecMotionDagger)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "dualhand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_DUALHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_8);	
	
	std::vector<SMotion> m_vecMotionBow =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 70 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 30},
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50},
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_BOW);
	for (const auto& it : m_vecMotionBow)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "bow/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_BOW, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_BOW, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BOW, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	std::vector<SMotion> m_vecMotionHorseDagger =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_DUALHAND_SWORD);
	for (const auto& it : m_vecMotionHorseDagger)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_dualhand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_DUALHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	
	std::vector<SMotion> m_vecMotionHorseBow =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
		{ HORSE_SKILL_SPLASH, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_BOW);
	for (const auto& it : m_vecMotionHorseBow)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_bow/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BOW, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BOW, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_BOW, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BOW, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");

#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif
	return true;
}

const bool CPlayerSettingsModule::LoadGameSura(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
		{ 1, "swaeryeong" },
		{ 2, "yonggwon" },
		{ 3, "gwigeom" },
		{ 4, "gongpo" },
		{ 5, "jumagap" },
		{ 6, "pabeop" },
		{ 16, "maryeong" },
		{ 17, "hwayeom" },
		{ 18, "muyeong" },
		{ 19, "heuksin" },
		{ 20, "tusok" },
		{ 21, "mahwan" },
	};

	char szSkillName[FILE_MAX_NUM];
	char szSkillAdd[4] = "";
	for (WORD i = 0; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			if (i > 0)
				snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			else
				snprintf(szSkillAdd, sizeof(szSkillAdd), "");

			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionSword =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa" , 0 },
		{ pkMotionManager.NAME_RUN, "run.msa" , 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa" , 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa" , 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_ONEHAND_SWORD, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorseSword =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_ONEHAND_SWORD);
	for (const auto& it : m_vecMotionHorseSword)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_onehand_sword/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_ONEHAND_SWORD, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");

#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}

const bool CPlayerSettingsModule::LoadGameShaman(DWORD dwRace, char stFolder[FILE_MAX_NUM])
{
	CRaceManager& pkManager = CRaceManager::Instance();
	pkManager.SelectRace(dwRace);

	if (!LoadGeneralMotion(stFolder))
		return false;

	CRaceData* pRaceData = pkManager.GetSelectedRaceDataPointer();
	if (!pRaceData)
		return false;

	std::vector<SMotion> m_vecMotionGeneral =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack.msa", 50 },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "attack_1.msa", 50 },
	};

	char szFileName[FILE_MAX_NUM];
	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionGeneral)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "general/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionSkill =
	{
		{ pkMotionManager.NAME_SKILL + 1, "bipabu.msa" },
		{ pkMotionManager.NAME_SKILL + 2, "yongpa.msa" },
		{ pkMotionManager.NAME_SKILL + 3, "paeryong.msa" },
		{ pkMotionManager.NAME_SKILL + 4, "hosin_target.msa" },
		{ pkMotionManager.NAME_SKILL + 5, "boho_target.msa" },
		{ pkMotionManager.NAME_SKILL + 6, "gicheon_target.msa" },		
		{ pkMotionManager.NAME_SKILL + 16, "noejeon.msa" },
		{ pkMotionManager.NAME_SKILL + 17, "byeorak.msa" },
		{ pkMotionManager.NAME_SKILL + 18, "pokroe.msa" },
		{ pkMotionManager.NAME_SKILL + 19, "jeongeop_target.msa" },
		{ pkMotionManager.NAME_SKILL + 20, "kwaesok_target.msa" },
		{ pkMotionManager.NAME_SKILL + 21, "jeungryeok_target.msa" },
		{ GUILD_SKILL_DRAGONBLOOD, "guild_yongsinuipi.msa", 0 },
		{ GUILD_SKILL_DRAGONBLESS, "guild_yongsinuichukbok.msa", 0 },
		{ GUILD_SKILL_BLESSARMOR, "guild_seonghwigap.msa", 0 },
		{ GUILD_SKILL_SPPEDUP, "guild_gasokhwa.msa", 0 },
		{ GUILD_SKILL_DRAGONWRATH, "guild_yongsinuibunno.msa", 0 },
		{ GUILD_SKILL_MAGICUP, "guild_jumunsul.msa", 0 },
	};

	std::vector<std::pair<BYTE, const char*>> m_vecSkillName = {
		{ 1, "bipabu" },
		{ 2, "yongpa" },
		{ 3, "paeryong" },
		{ 4, "hosin" },
		{ 5, "boho" },
		{ 6, "gicheon" },
		{ 16, "noejeon" },
		{ 17, "byeorak" },
		{ 18, "pokroe" },
		{ 19, "jeongeop" },
		{ 20, "kwaesok" },
		{ 21, "jeungryeok" },
	};

	char szSkillName[FILE_MAX_NUM];
	char szSkillAdd[4] = "";
	for (WORD i = 1; i < CPythonSkill::SKILL_EFFECT_COUNT; i++)
	{
		for (auto& it : m_vecSkillName)
		{
			SMotion motion;

			motion.wMotionIndex = pkMotionManager.NAME_SKILL + (i * CPythonSkill::SKILL_GRADEGAP) + it.first;
			motion.byPercentage = 0;

			snprintf(szSkillAdd, sizeof(szSkillAdd), "_%d", i + 1);
			snprintf(motion.stName, sizeof(motion.stName), "%s%s.msa", it.second, szSkillAdd);
			m_vecMotionSkill.push_back(motion);
		}
	}

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_GENERAL);
	for (const auto& it : m_vecMotionSkill)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "skill/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_GENERAL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, 1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_GENERAL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);

	RegisterEmotionAnis(stFolder);

	std::vector<SMotion> m_vecMotionFan =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50, },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FAN);
	for (const auto& it : m_vecMotionFan)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fan/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FAN, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_FAN, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_FAN, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);	
	
	std::vector<SMotion> m_vecMotionBell =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE, "damage_1.msa", 50, },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_2.msa", 50,},
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage_3.msa", 50, },
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_4, "combo_04.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_5, "combo_05.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_6, "combo_06.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_7, "combo_07.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_BELL);
	for (const auto& it : m_vecMotionBell)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "bell/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_BELL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_BELL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, 4);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_1, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_4);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, 5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_2, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_7);

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, 6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_4, pkMotionManager.NAME_COMBO_ATTACK_5);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_5, pkMotionManager.NAME_COMBO_ATTACK_6);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_BELL, COMBO_TYPE_3, COMBO_INDEX_6, pkMotionManager.NAME_COMBO_ATTACK_4);

	std::vector<SMotion> m_vecMotionFishing =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 0 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_FISHING_THROW, "throw.msa", 0 },
		{ pkMotionManager.NAME_FISHING_WAIT, "fishing_wait.msa", 0 },
		{ pkMotionManager.NAME_FISHING_STOP, "fishing_cancel.msa", 0 },
		{ pkMotionManager.NAME_FISHING_REACT, "fishing_react.msa", 0 },
		{ pkMotionManager.NAME_FISHING_CATCH, "fishing_catch.msa", 0 },
		{ pkMotionManager.NAME_FISHING_FAIL, "fishing_fail.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_FISHING);
	for (const auto& it : m_vecMotionFishing)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "fishing/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_FISHING, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorse =
	{
		{ pkMotionManager.NAME_WAIT, "wait.msa", 90 },
		{ pkMotionManager.NAME_WAIT, "wait_1.msa", 9 },
		{ pkMotionManager.NAME_WAIT, "wait_2.msa", 1 },
		{ pkMotionManager.NAME_WALK, "walk.msa", 0 },
		{ pkMotionManager.NAME_RUN, "run.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE, "damage.msa", 0 },
		{ pkMotionManager.NAME_DAMAGE_BACK, "damage.msa", 0 },
		{ pkMotionManager.NAME_DEAD, "dead.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_charge.msa", 0 },
		{ HORSE_SKILL_CHARGE, "skill_splash.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE);
	for (const auto& it : m_vecMotionHorse)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	std::vector<SMotion> m_vecMotionHorseFan =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_FAN);
	for (const auto& it : m_vecMotionHorseFan)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_fan/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_FAN, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_FAN, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_FAN, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);	
	
	std::vector<SMotion> m_vecMotionHorseBell =
	{
		{ pkMotionManager.NAME_COMBO_ATTACK_1, "combo_01.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_2, "combo_02.msa", 0 },
		{ pkMotionManager.NAME_COMBO_ATTACK_3, "combo_03.msa", 0 },
		{ HORSE_SKILL_WILDATTACK, "skill_wildattack.msa", 0 },
	};

	pRaceData->RegisterMotionMode(pkMotionManager.MODE_HORSE_BELL);
	for (const auto& it : m_vecMotionHorseBell)
	{
		snprintf(szFileName, sizeof(szFileName), "%s%s%s", stFolder, "horse_bell/", it.stName);
#ifdef ENABLE_PLAYERS_LOAD_FIX
		CGraphicThing* pkMotionThing = pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BELL, it.wMotionIndex, szFileName, it.byPercentage);
		
		if (pkMotionThing)
			CResourceManager::Instance().LoadStaticCache(pkMotionThing->GetFileName());
#else
		pRaceData->RegisterMotionData(pkMotionManager.MODE_HORSE_BELL, it.wMotionIndex, szFileName, it.byPercentage);
#endif
	}

	pRaceData->ReserveComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, 3);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_1, pkMotionManager.NAME_COMBO_ATTACK_1);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_2, pkMotionManager.NAME_COMBO_ATTACK_2);
	pRaceData->RegisterComboAttack(pkMotionManager.MODE_HORSE_BELL, COMBO_TYPE_1, COMBO_INDEX_3, pkMotionManager.NAME_COMBO_ATTACK_3);

	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON, "equip_right");
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_WEAPON_LEFT, "equip_left");

#ifdef ENABLE_SASH_SYSTEM
	pRaceData->RegisterAttachingBoneName(CRaceData::PART_SASH, "Bip01 Spine2");
#endif

	return true;
}

PyObject* cpsmLoadGameWarrior(PyObject* poSelf, PyObject* poArgs)
{
	CPlayerSettingsModule::LoadGameWarrior(CPlayerSettingsModule::RACE_WARRIOR_M, "d:/ymir work/pc/warrior/");
	CPlayerSettingsModule::LoadGameWarrior(CPlayerSettingsModule::RACE_WARRIOR_W, "d:/ymir work/pc2/warrior/");
	return Py_BuildNone();
}

PyObject* cpsmInitData(PyObject*, PyObject*)
{
	CPlayerSettingsModule::LoadInitData();
	return Py_BuildNone();
}

PyObject* cpsmLoadGameEffect(PyObject*, PyObject*)
{
	CPlayerSettingsModule::LoadGameEffect();
	return Py_BuildNone();
}

PyObject* cpsmNoop(PyObject*, PyObject*)
{
	return Py_BuildNone();
}

PyObject* cpsmLoadGameAssassin(PyObject* poSelf, PyObject* poArgs)
{
	CPlayerSettingsModule::LoadGameAssassin(CPlayerSettingsModule::RACE_ASSASSIN_W, "d:/ymir work/pc/assassin/");
	CPlayerSettingsModule::LoadGameAssassin(CPlayerSettingsModule::RACE_ASSASSIN_M, "d:/ymir work/pc2/assassin/");
	return Py_BuildNone();
}

PyObject* cpsmLoadGameSura(PyObject* poSelf, PyObject* poArgs)
{
	CPlayerSettingsModule::LoadGameSura(CPlayerSettingsModule::RACE_SURA_M, "d:/ymir work/pc/sura/");
	CPlayerSettingsModule::LoadGameSura(CPlayerSettingsModule::RACE_SURA_W, "d:/ymir work/pc2/sura/");
	return Py_BuildNone();
}

PyObject* cpsmLoadGameShaman(PyObject* poSelf, PyObject* poArgs)
{
	CPlayerSettingsModule::LoadGameShaman(CPlayerSettingsModule::RACE_SHAMAN_W, "d:/ymir work/pc/shaman/");
	CPlayerSettingsModule::LoadGameShaman(CPlayerSettingsModule::RACE_SHAMAN_M, "d:/ymir work/pc2/shaman/");
	return Py_BuildNone();
}

void initplayersettingsmodule()
{
	static PyMethodDef s_methods[] =
	{
		{ "InitData",                    cpsmInitData,                 METH_VARARGS },
		{ "LoadGameSound",               cpsmNoop,                     METH_VARARGS },
		{ "LoadGameEffect",              cpsmLoadGameEffect,           METH_VARARGS },
		{ "LoadGameWarrior",			cpsmLoadGameWarrior,				METH_VARARGS },
		{ "LoadGameAssassin",			cpsmLoadGameAssassin,				METH_VARARGS },
		{ "LoadGameSura",				cpsmLoadGameSura,					METH_VARARGS },
		{ "LoadGameShaman",				cpsmLoadGameShaman,					METH_VARARGS },
		{ "LoadGameSkill",               cpsmNoop,                     METH_VARARGS },
		{ "LoadNpcScale",                cpsmNoop,                     METH_VARARGS },
		{ "LoadRaceHeight",              cpsmNoop,                     METH_VARARGS },

		{ NULL,								NULL,								NULL		 },
	};

	PyObject* poModule = Py_InitModule("appPlayerSettingsModule", s_methods);
}



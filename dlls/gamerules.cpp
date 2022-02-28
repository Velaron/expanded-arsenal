/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//=========================================================
// GameRules.cpp
//=========================================================

#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"skill.h"
#include	"game.h"

extern edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );

DLL_GLOBAL CGameRules *g_pGameRules = NULL;
extern DLL_GLOBAL BOOL g_fGameOver;
extern int gmsgDeathMsg;	// client dll messages
extern int gmsgMOTD;

int g_teamplay = 0;

//=========================================================
//=========================================================
BOOL CGameRules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	int iAmmoIndex;

	if( pszAmmoName )
	{
		iAmmoIndex = pPlayer->GetAmmoIndex( pszAmmoName );

		if( iAmmoIndex > -1 )
		{
			if( pPlayer->AmmoInventory( iAmmoIndex ) < iMaxCarry )
			{
				// player has room for more of this type of ammo
				return TRUE;
			}
		}
	}

	return FALSE;
}

//=========================================================
//=========================================================
edict_t *CGameRules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
{
	edict_t *pentSpawnSpot = EntSelectSpawnPoint( pPlayer );

	pPlayer->pev->origin = VARS( pentSpawnSpot )->origin + Vector( 0, 0, 1 );
	pPlayer->pev->v_angle  = g_vecZero;
	pPlayer->pev->velocity = g_vecZero;
	pPlayer->pev->angles = VARS( pentSpawnSpot )->angles;
	pPlayer->pev->punchangle = g_vecZero;
	pPlayer->pev->fixangle = TRUE;

	return pentSpawnSpot;
}

//=========================================================
//=========================================================
BOOL CGameRules::CanHavePlayerItem( CBasePlayer *pPlayer, CBasePlayerItem *pWeapon )
{
	// only living players can have items
	if( pPlayer->pev->deadflag != DEAD_NO )
		return FALSE;

	if( pWeapon->pszAmmo1() )
	{
		if( !CanHaveAmmo( pPlayer, pWeapon->pszAmmo1(), pWeapon->iMaxAmmo1() ) )
		{
			// we can't carry anymore ammo for this gun. We can only 
			// have the gun if we aren't already carrying one of this type
			if( pPlayer->HasPlayerItem( pWeapon ) )
			{
				return FALSE;
			}
		}
	}
	else
	{
		// weapon doesn't use ammo, don't take another if you already have it.
		if( pPlayer->HasPlayerItem( pWeapon ) )
		{
			return FALSE;
		}
	}

	// note: will fall through to here if GetItemInfo doesn't fill the struct!
	return TRUE;
}

//=========================================================
// load the SkillData struct with the proper values based on the skill level.
//=========================================================
void CGameRules::RefreshSkillData ( void )
{
	int iSkill;

	iSkill = (int)CVAR_GET_FLOAT( "skill" );
	g_iSkillLevel = iSkill;

	if( iSkill < 1 )
	{
		iSkill = 1;
	}
	else if( iSkill > 3 )
	{
		iSkill = 3; 
	}

	gSkillData.iSkillLevel = iSkill;

	ALERT( at_console, "\nGAME SKILL LEVEL:%d\n",iSkill );

	//Agrunt		
	gSkillData.agruntHealth = GetSkillCvar( "sk_agrunt_health" );
	gSkillData.agruntDmgPunch = GetSkillCvar( "sk_agrunt_dmg_punch" );

	// Apache 
	gSkillData.apacheHealth = GetSkillCvar( "sk_apache_health" );

	// Barney
	gSkillData.barneyHealth = GetSkillCvar( "sk_barney_health" );

	// Anime?
	gSkillData.animeHealth = GetSkillCvar( "sk_anime_health" );

	// Big Momma
	gSkillData.bigmommaHealthFactor = GetSkillCvar( "sk_bigmomma_health_factor" );
	gSkillData.bigmommaDmgSlash = GetSkillCvar( "sk_bigmomma_dmg_slash" );
	gSkillData.bigmommaDmgBlast = GetSkillCvar( "sk_bigmomma_dmg_blast" );
	gSkillData.bigmommaRadiusBlast = GetSkillCvar( "sk_bigmomma_radius_blast" );

	// Bullsquid
	gSkillData.bullsquidHealth = GetSkillCvar( "sk_bullsquid_health" );
	gSkillData.bullsquidDmgBite = GetSkillCvar( "sk_bullsquid_dmg_bite" );
	gSkillData.bullsquidDmgWhip = GetSkillCvar( "sk_bullsquid_dmg_whip" );
	gSkillData.bullsquidDmgSpit = GetSkillCvar( "sk_bullsquid_dmg_spit" );

	// Gargantua
	gSkillData.gargantuaHealth = GetSkillCvar( "sk_gargantua_health" );
	gSkillData.gargantuaDmgSlash = GetSkillCvar( "sk_gargantua_dmg_slash" );
	gSkillData.gargantuaDmgFire = GetSkillCvar( "sk_gargantua_dmg_fire" );
	gSkillData.gargantuaDmgStomp = GetSkillCvar( "sk_gargantua_dmg_stomp" );

	// Hassassin
	gSkillData.hassassinHealth = GetSkillCvar( "sk_hassassin_health" );

	// Super?
	gSkillData.superHealth = GetSkillCvar( "sk_super_health" );

	// Headcrab
	gSkillData.headcrabHealth = GetSkillCvar( "sk_headcrab_health" );
	gSkillData.headcrabDmgBite = GetSkillCvar( "sk_headcrab_dmg_bite" );

	// Headcrab2
	gSkillData.headcrab2Health = GetSkillCvar( "sk_headcrab2_health" );
	gSkillData.headcrab2DmgBite = GetSkillCvar( "sk_headcrab2_dmg_bite" );

	// Hgrunt 
	gSkillData.hgruntHealth = GetSkillCvar( "sk_hgrunt_health" );
	gSkillData.hgruntDmgKick = GetSkillCvar( "sk_hgrunt_kick" );
	gSkillData.hgruntShotgunPellets = GetSkillCvar( "sk_hgrunt_pellets" );
	gSkillData.hgruntGrenadeSpeed = GetSkillCvar( "sk_hgrunt_gspeed" );

	// Space?
	gSkillData.spaceHealth = GetSkillCvar( "sk_space_health" );
	gSkillData.spaceDmgKick = GetSkillCvar( "sk_space_kick" );
	gSkillData.spaceShotgunPellets = GetSkillCvar( "sk_space_pellets" );
	gSkillData.spaceGrenadeSpeed = GetSkillCvar( "sk_space_gspeed" );

	// Boss?
	gSkillData.bossHealth = GetSkillCvar( "sk_boss_health" );
	gSkillData.bossDmgKick = GetSkillCvar( "sk_boss_kick" );
	gSkillData.bossShotgunPellets = GetSkillCvar( "sk_boss_pellets" );
	gSkillData.bossGrenadeSpeed = GetSkillCvar( "sk_boss_gspeed" );

	// Alpha?
	gSkillData.alphaHealth = GetSkillCvar( "sk_alpha_health" );
	gSkillData.alphaDmgKick = GetSkillCvar( "sk_alpha_kick" );
	gSkillData.alphaShotgunPellets = GetSkillCvar( "sk_alpha_pellets" );
	gSkillData.alphaGrenadeSpeed = GetSkillCvar( "sk_alpha_gspeed" );

	// Spforce
	gSkillData.spforceHealth = GetSkillCvar( "sk_spforce_health" );
	gSkillData.spforceDmgKick = GetSkillCvar( "sk_spforce_kick" );
	gSkillData.spforceShotgunPellets = GetSkillCvar( "sk_spforce_pellets" );
	gSkillData.spforceGrenadeSpeed = GetSkillCvar( "sk_spforce_gspeed" );

	// Agent
	gSkillData.agentHealth = GetSkillCvar( "sk_agent_health" );
	gSkillData.agentDmgKick = GetSkillCvar( "sk_agent_kick" );
	gSkillData.agentShotgunPellets = GetSkillCvar( "sk_agent_pellets" );
	gSkillData.agentGrenadeSpeed = GetSkillCvar( "sk_agent_gspeed" );

	// Massn
	gSkillData.massnHealth = GetSkillCvar( "sk_massn_health" );
	gSkillData.massnDmgKick = GetSkillCvar( "sk_massn_kick" );
	gSkillData.massnShotgunPellets = GetSkillCvar( "sk_massn_pellets" );
	gSkillData.massnGrenadeSpeed = GetSkillCvar( "sk_massn_gspeed" );

	// Robot?
	gSkillData.robotHealth = GetSkillCvar( "sk_robot_health" );
	gSkillData.robotDmgKick = GetSkillCvar( "sk_robot_kick" );
	gSkillData.robotShotgunPellets = GetSkillCvar( "sk_robot_pellets" );
	gSkillData.robotGrenadeSpeed = GetSkillCvar( "sk_robot_gspeed" );

	// Houndeye
	gSkillData.houndeyeHealth = GetSkillCvar( "sk_houndeye_health" );
	gSkillData.houndeyeDmgBlast = GetSkillCvar( "sk_houndeye_dmg_blast" );

	// ISlave
	gSkillData.slaveHealth = GetSkillCvar( "sk_islave_health" );
	gSkillData.slaveDmgClaw = GetSkillCvar( "sk_islave_dmg_claw" );
	gSkillData.slaveDmgClawrake = GetSkillCvar( "sk_islave_dmg_clawrake" );
	gSkillData.slaveDmgZap = GetSkillCvar( "sk_islave_dmg_zap" );

	// ISlave2
	gSkillData.slave2Health = GetSkillCvar( "sk_islave2_health" );
	gSkillData.slave2DmgClaw = GetSkillCvar( "sk_islave2_dmg_claw" );
	gSkillData.slave2DmgClawrake = GetSkillCvar( "sk_islave2_dmg_clawrake" );
	gSkillData.slave2DmgZap = GetSkillCvar( "sk_islave2_dmg_zap" );

	// Icthyosaur
	gSkillData.ichthyosaurHealth = GetSkillCvar( "sk_ichthyosaur_health" );
	gSkillData.ichthyosaurDmgShake = GetSkillCvar( "sk_ichthyosaur_shake" );

	// Leech
	gSkillData.leechHealth = GetSkillCvar( "sk_leech_health" );

	gSkillData.leechDmgBite = GetSkillCvar( "sk_leech_dmg_bite" );

	// Controller
	gSkillData.controllerHealth = GetSkillCvar( "sk_controller_health" );
	gSkillData.controllerDmgZap = GetSkillCvar( "sk_controller_dmgzap" );
	gSkillData.controllerSpeedBall = GetSkillCvar( "sk_controller_speedball" );
	gSkillData.controllerDmgBall = GetSkillCvar( "sk_controller_dmgball" );

	// Nihilanth
	gSkillData.nihilanthHealth = GetSkillCvar( "sk_nihilanth_health" );
	gSkillData.nihilanthZap = GetSkillCvar( "sk_nihilanth_zap" );

	// Scientist
	gSkillData.scientistHealth = GetSkillCvar( "sk_scientist_health" );

	// Snark
	gSkillData.snarkHealth = GetSkillCvar( "sk_snark_health" );
	gSkillData.snarkDmgBite = GetSkillCvar( "sk_snark_dmg_bite" );
	gSkillData.snarkDmgPop = GetSkillCvar( "sk_snark_dmg_pop" );

	// Zombie
	gSkillData.zombieHealth = GetSkillCvar( "sk_zombie_health" );
	gSkillData.zombieDmgOneSlash = GetSkillCvar( "sk_zombie_dmg_one_slash" );
	gSkillData.zombieDmgBothSlash = GetSkillCvar( "sk_zombie_dmg_both_slash" );

	//Turret
	gSkillData.turretHealth = GetSkillCvar( "sk_turret_health" );

	// MiniTurret
	gSkillData.miniturretHealth = GetSkillCvar( "sk_miniturret_health" );

	// Sentry Turret
	gSkillData.sentryHealth = GetSkillCvar( "sk_sentry_health" );

	// PLAYER WEAPONS

	// Crowbar whack
	gSkillData.plrDmgCrowbar = GetSkillCvar( "sk_plr_crowbar" );

	// AK47
	gSkillData.plrDmgAK47 = GetSkillCvar( "sk_plr_ak47" );

	gSkillData.plrDmgP226 = GetSkillCvar( "sk_plr_p226" );

	gSkillData.plrDmgColt45 = GetSkillCvar( "sk_plr_colt45" );

	// AK47
	gSkillData.plrDmgAUTOSNIPER = GetSkillCvar( "sk_plr_autosniper" );

	gSkillData.plrDmgD50 = GetSkillCvar( "sk_plr_d50" );

	gSkillData.plrDmgCG = GetSkillCvar( "sk_plr_chaingun" );

	// AK47
	gSkillData.plrDmgDeagle = GetSkillCvar( "sk_plr_deagle" );

	// AK47
	gSkillData.plrDmgP904 = GetSkillCvar( "sk_plr_p904" );

	// dbarrel
	gSkillData.plrDmgDB = GetSkillCvar( "sk_plr_db" );

	//flamethrower
	gSkillData.plrDmgFlamethrower = GetSkillCvar( "sk_plr_flamethrower" );

	// mp5a3
	gSkillData.plrDmgMP5A3 = GetSkillCvar( "sk_plr_mp5a3" );

	gSkillData.plrDmgM1014 = GetSkillCvar( "sk_plr_m1014" );

	gSkillData.plrDmgKSG12 = GetSkillCvar( "sk_plr_ksg12" );

	gSkillData.plrDmgFNFAL = GetSkillCvar( "sk_plr_fnfal" );
	gSkillData.plrDmgMP7 = GetSkillCvar( "sk_plr_mp7" );

	// Crowbar whack
	gSkillData.plrDmgKnife = GetSkillCvar( "sk_plr_knife" );

	// Crowbar whack
	gSkillData.plrDmgPipe = GetSkillCvar( "sk_plr_pipe" );

	// Crowbar whack
	gSkillData.plrDmgBeretta = GetSkillCvar( "sk_plr_beretta" );

	// Crowbar whack
	gSkillData.plrDmgM41 = GetSkillCvar( "sk_plr_m41" );

	// Glock Round
	gSkillData.plrDmg9MM = GetSkillCvar( "sk_plr_9mm_bullet" );

	// 357 Round
	gSkillData.plrDmg357 = GetSkillCvar( "sk_plr_357_bullet" );

	// MP5 Round
	gSkillData.plrDmgMP5 = GetSkillCvar( "sk_plr_9mmAR_bullet" );

	// M203 grenade
	gSkillData.plrDmgM203Grenade = GetSkillCvar( "sk_plr_9mmAR_grenade" );

	// Shotgun buckshot
	gSkillData.plrDmgBuckshot = GetSkillCvar( "sk_plr_buckshot" );

	// Crossbow
	gSkillData.plrDmgCrossbowClient = GetSkillCvar( "sk_plr_xbow_bolt_client" );
	gSkillData.plrDmgCrossbowMonster = GetSkillCvar( "sk_plr_xbow_bolt_monster" );

	// RPG
	gSkillData.plrDmgRPG = GetSkillCvar( "sk_plr_rpg" );

	// Gauss gun
	gSkillData.plrDmgGauss = GetSkillCvar( "sk_plr_gauss" );

	// Egon Gun
	gSkillData.plrDmgEgonNarrow = GetSkillCvar( "sk_plr_egon_narrow" );
	gSkillData.plrDmgEgonWide = GetSkillCvar( "sk_plr_egon_wide" );

	// Hand Grendade
	gSkillData.plrDmgHandGrenade = GetSkillCvar( "sk_plr_hand_grenade" );

	// Satchel Charge
	gSkillData.plrDmgSatchel = GetSkillCvar( "sk_plr_satchel" );
	gSkillData.plrDmgNuclear = GetSkillCvar( "sk_plr_nuclear" );

	// Tripmine
	gSkillData.plrDmgTripmine = GetSkillCvar( "sk_plr_tripmine" );

	// MONSTER WEAPONS
	gSkillData.monDmg12MM = GetSkillCvar( "sk_12mm_bullet" );
	gSkillData.monDmg23 = GetSkillCvar( "sk_23_bullet" );
	gSkillData.monDmgMP5 = GetSkillCvar( "sk_9mmAR_bullet" );
	gSkillData.monDmgP904 = GetSkillCvar( "sk_p904_bullet" );
	gSkillData.monDmgM41 = GetSkillCvar( "sk_m41_bullet" );
	gSkillData.monDmgAKM = GetSkillCvar( "sk_akm_bullet" );
	gSkillData.monDmgAS = GetSkillCvar( "sk_as_bullet" );
	gSkillData.monDmg9MM = GetSkillCvar( "sk_9mm_bullet" );
	gSkillData.monDmg357 = GetSkillCvar( "sk_357_bullet" );
	gSkillData.monDmgD50 = GetSkillCvar( "sk_d50_bullet" );
	gSkillData.monDmgFN = GetSkillCvar( "sk_fnfal_bullet" );
	gSkillData.monDmgCG = GetSkillCvar( "sk_cg_bullet" );
	gSkillData.otisHealth = GetSkillCvar( "sk_otis_health" );

	// MONSTER HORNET
	gSkillData.monDmgHornet = GetSkillCvar( "sk_hornet_dmg" );

	// PLAYER HORNET
// Up to this point, player hornet damage and monster hornet damage were both using
// monDmgHornet to determine how much damage to do. In tuning the hivehand, we now need
// to separate player damage and monster hivehand damage. Since it's so late in the project, we've
// added plrDmgHornet to the SKILLDATA struct, but not to the engine CVar list, so it's inaccesible
// via SKILLS.CFG. Any player hivehand tuning must take place in the code. (sjb)
	gSkillData.plrDmgHornet = 8;

	// HEALTH/CHARGE
	gSkillData.suitchargerCapacity = GetSkillCvar( "sk_suitcharger" );
	gSkillData.batteryCapacity = GetSkillCvar( "sk_battery" );
	gSkillData.healthchargerCapacity = GetSkillCvar ( "sk_healthcharger" );
	gSkillData.healthkitCapacity = GetSkillCvar ( "sk_healthkit" );
	gSkillData.scientistHeal = GetSkillCvar ( "sk_scientist_heal" );

	// monster damage adj
	gSkillData.monHead = GetSkillCvar( "sk_monster_head" );
	gSkillData.monChest = GetSkillCvar( "sk_monster_chest" );
	gSkillData.monStomach = GetSkillCvar( "sk_monster_stomach" );
	gSkillData.monLeg = GetSkillCvar( "sk_monster_leg" );
	gSkillData.monArm = GetSkillCvar( "sk_monster_arm" );

	// player damage adj
	gSkillData.plrHead = GetSkillCvar( "sk_player_head" );
	gSkillData.plrChest = GetSkillCvar( "sk_player_chest" );
	gSkillData.plrStomach = GetSkillCvar( "sk_player_stomach" );
	gSkillData.plrLeg = GetSkillCvar( "sk_player_leg" );
	gSkillData.plrArm = GetSkillCvar( "sk_player_arm" );
}

void CGameRules::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	pPlayer->SetPrefsFromUserinfo( infobuffer );
}

//=========================================================
// instantiate the proper game rules object
//=========================================================

CGameRules *InstallGameRules( void )
{
	SERVER_COMMAND( "exec game.cfg\n" );
	SERVER_EXECUTE();

	if( !gpGlobals->deathmatch )
	{
		// generic half-life
		g_teamplay = 0;
		return new CHalfLifeRules;
	}
	else
	{
		if( teamplay.value > 0 )
		{
			// teamplay
			g_teamplay = 1;
			return new CHalfLifeTeamplay;
		}
		if( (int)gpGlobals->deathmatch == 1 )
		{
			// vanilla deathmatch
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
		else
		{
			// vanilla deathmatch??
			g_teamplay = 0;
			return new CHalfLifeMultiplay;
		}
	}
}

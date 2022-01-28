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

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "soundent.h"
#include "gamerules.h"

enum chaingun_e
{
	CHAINGUN_LONGIDLE = 0,
	CHAINGUN_IDLE1,
	CHAINGUN_RELOAD,
	CHAINGUN_DEPLOY,
	CHAINGUN_FIRE1,
	CHAINGUN_FIRE2,
	CHAINGUN_FIRE3,
};

LINK_ENTITY_TO_CLASS( weapon_chaingun, CChaingun );

//=========================================================
//=========================================================

void CChaingun::Spawn()
{
	pev->classname = MAKE_STRING( "weapon_chaingun" ); // hack to allow for old names
	Precache();
	SET_MODEL( ENT( pev ), "models/w_chaingun.mdl" );
	m_iId = WEAPON_CHAINGUN;

	m_iDefaultAmmo = CHAINGUN_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}

void CChaingun::Precache( void )
{
	PRECACHE_MODEL( "models/v_chaingun.mdl" );
	PRECACHE_MODEL( "models/w_chaingun.mdl" );
	PRECACHE_MODEL( "models/p_chaingun.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" ); // brass shellTE_MODEL

	PRECACHE_MODEL( "models/w_chaingunclip.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "weapons/chain-1.wav" ); // H to the K

	PRECACHE_SOUND( "weapons/357_cock1.wav" );

	m_usFireChaingun = PRECACHE_EVENT( 1, "events/cg.sc" );
}

int CChaingun::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "20MM";
	p->iMaxAmmo1 = _20MM_MAX_CARRY;
	p->iMaxClip = -1;
	p->iSlot = 3;
	p->iPosition = 5;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_CHAINGUN;
	p->iWeight = MP5_WEIGHT;

	return 1;
}

int CChaingun::AddToPlayer( CBasePlayer *pPlayer )
{
	if ( CBasePlayerWeapon::AddToPlayer( pPlayer ) )
	{
		MESSAGE_BEGIN( MSG_ONE, gmsgWeapPickup, NULL, pPlayer->pev );
		WRITE_BYTE( m_iId );
		MESSAGE_END();
		return TRUE;
	}
	return FALSE;
}

BOOL CChaingun::Deploy()
{
	return DefaultDeploy( "models/v_chaingun.mdl", "models/p_chaingun.mdl", CHAINGUN_DEPLOY, "chaingun" );
}

void CChaingun::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 1 )
	{
		PlayEmptySound();
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 2;

	m_pPlayer->pev->effects = (int)( m_pPlayer->pev->effects ) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	Vector vecDir;

#ifdef CLIENT_DLL
	if ( !bIsMultiplayer() )
#else
	if ( !g_pGameRules->IsMultiplayer() )
#endif
	{
		// optimized multiplayer. Widened to make it easier to hit a moving player
		vecDir = m_pPlayer->FireBulletsPlayer( 2, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_PKM, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
	else
	{
		// single player spread
		vecDir = m_pPlayer->FireBulletsPlayer( 2, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 8192, BULLET_PLAYER_PKM, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usFireChaingun, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.08;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CChaingun::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:
		iAnim = CHAINGUN_LONGIDLE;
		break;

	default:
	case 1:
		iAnim = CHAINGUN_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

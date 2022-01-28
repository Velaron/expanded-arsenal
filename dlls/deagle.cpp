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

enum deagle_e
{
	DEAGLE_LONGIDLE = 0,
	DEAGLE_IDLE1,
	DEAGLE_RELOAD,
	DEAGLE_DEPLOY,
	DEAGLE_FIRE1,
	DEAGLE_FIRE2,
	DEAGLE_FIRE3,
};

LINK_ENTITY_TO_CLASS( weapon_deagle, CDeagle );
LINK_ENTITY_TO_CLASS( weapon_eagle, CDeagle );

//=========================================================
//=========================================================

void CDeagle::Spawn()
{
	pev->classname = MAKE_STRING( "weapon_deagle" ); // hack to allow for old names
	Precache();
	SET_MODEL( ENT( pev ), "models/w_deagle.mdl" );
	m_iId = WEAPON_DEAGLE;

	m_iDefaultAmmo = DEAGLE_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}

void CDeagle::Precache( void )
{
	PRECACHE_MODEL( "models/v_deagle.mdl" );
	PRECACHE_MODEL( "models/w_deagle.mdl" );
	PRECACHE_MODEL( "models/p_deagle.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" ); // brass shellTE_MODEL

	PRECACHE_MODEL( "models/w_deagleclip.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "weapons/deagle_clipout.wav" );
	PRECACHE_SOUND( "weapons/deagle_clipin.wav" );

	PRECACHE_SOUND( "weapons/de_shot3.wav" ); // H to the K
	PRECACHE_SOUND( "weapons/de_shot3.wav" ); // H to the K
	PRECACHE_SOUND( "weapons/de_shot3.wav" ); // H to the K

	PRECACHE_SOUND( "weapons/357_cock1.wav" );

	m_usDEAGLE = PRECACHE_EVENT( 1, "events/deagle.sc" );
}

int CDeagle::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "357";
	p->iMaxAmmo1 = _357_MAX_CARRY;
	p->iMaxClip = DEAGLE_MAX_CLIP;
	p->iSlot = 1;
	p->iPosition = 3;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_DEAGLE;
	p->iWeight = PYTHON_WEIGHT;

	return 1;
}

int CDeagle::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CDeagle::Deploy()
{
	return DefaultDeploy( "models/v_deagle.mdl", "models/p_deagle.mdl", DEAGLE_DEPLOY, "deagle" );
}

void CDeagle::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.04;
		return;
	}

	if ( m_iClip <= 0 )
	{
		DefaultReload( 7, DEAGLE_RELOAD, 2.3 );
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.04;
		return;
	}

	m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip--;

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
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_0DEGREES, 8192, BULLET_PLAYER_DEAGLE, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
	else
	{
		// single player spread
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_0DEGREES, 8192, BULLET_PLAYER_DEAGLE, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usDEAGLE, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if ( !m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.35;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.35;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CDeagle::Reload( void )
{
	int iResult;

	if ( m_iClip < 7 )
	{
		iResult = DefaultReload( 7, DEAGLE_RELOAD, 2.3 );
	}

	if ( iResult )
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );
	}
}

void CDeagle::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:
		iAnim = DEAGLE_LONGIDLE;
		break;

	default:
	case 1:
		iAnim = DEAGLE_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

class CDeClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache();
		SET_MODEL( ENT( pev ), "models/w_de_clip.mdl" );
		CBasePlayerAmmo::Spawn();
	}
	void Precache( void )
	{
		PRECACHE_MODEL( "models/w_de_clip.mdl" );
		PRECACHE_SOUND( "items/9mmclip1.wav" );
	}
	BOOL AddAmmo( CBaseEntity *pOther )
	{
		int bResult = ( pOther->GiveAmmo( AMMO_DEAGLECLIP_GIVE, "357", _357_MAX_CARRY ) != -1 );
		if ( bResult )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_de_clip, CDeClip );

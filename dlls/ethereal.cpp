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

enum ethereal_e
{
	ETHEREAL_LONGIDLE = 0,
	ETHEREAL_IDLE1,
	ETHEREAL_RELOAD,
	ETHEREAL_DEPLOY,
	ETHEREAL_FIRE1,
	ETHEREAL_FIRE2,
	ETHEREAL_FIRE3,
};

LINK_ENTITY_TO_CLASS( weapon_ethereal, CETHEREAL );

//=========================================================
//=========================================================

void CETHEREAL::Spawn()
{
	pev->classname = MAKE_STRING( "weapon_ethereal" ); // hack to allow for old names
	Precache();
	SET_MODEL( ENT( pev ), "models/w_ethereal.mdl" );
	m_iId = WEAPON_ETHEREAL;

	m_iDefaultAmmo = ETHEREAL_DEFAULT_GIVE;

	FallInit(); // get ready to fall down.
}

void CETHEREAL::Precache( void )
{
	PRECACHE_MODEL( "models/v_ethereal.mdl" );
	PRECACHE_MODEL( "models/w_ethereal.mdl" );
	PRECACHE_MODEL( "models/p_ethereal.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shell.mdl" ); // brass shellTE_MODEL

	PRECACHE_MODEL( "models/w_etherealclip.mdl" );
	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "weapons/ethereal_clipout.wav" );
	PRECACHE_SOUND( "weapons/ethereal_clipin.wav" );

	PRECACHE_SOUND( "weapons/ethereal-1.wav" ); // H to the K
	PRECACHE_SOUND( "weapons/ethereal-2.wav" ); // H to the K
	PRECACHE_SOUND( "weapons/ethereal-3.wav" ); // H to the K

	PRECACHE_SOUND( "weapons/357_cock1.wav" );

	m_usETHEREAL = PRECACHE_EVENT( 1, "events/ethereal.sc" );
}

int CETHEREAL::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "762MM";
	p->iMaxAmmo1 = _762MM_MAX_CARRY;
	p->iMaxClip = ETHEREAL_MAX_CLIP;
	p->iSlot = 3;
	p->iPosition = 5;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_ETHEREAL;
	p->iWeight = MP5_WEIGHT;

	return 1;
}

int CETHEREAL::AddToPlayer( CBasePlayer *pPlayer )
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

BOOL CETHEREAL::Deploy()
{
	return DefaultDeploy( "models/v_ethereal.mdl", "models/p_ethereal.mdl", ETHEREAL_DEPLOY, "ethereal" );
}

void CETHEREAL::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
		return;
	}

	if ( m_iClip <= 0 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = 0.15;
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
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_2DEGREES, 8192, BULLET_PLAYER_ETHEREAL, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
	else
	{
		// single player spread
		vecDir = m_pPlayer->FireBulletsPlayer( 1, vecSrc, vecAiming, VECTOR_CONE_1DEGREES, 8192, BULLET_PLAYER_ETHEREAL, 2, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usETHEREAL, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if ( !m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	if ( m_flNextPrimaryAttack < UTIL_WeaponTimeBase() )
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.1;

	m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 );
}

void CETHEREAL::Reload( void )
{
	int iResult;

	if ( m_iClip < 30 )
	{
		iResult = DefaultReload( 30, ETHEREAL_RELOAD, 1.5 );
	}

	if ( iResult )
	{
		m_flTimeWeaponIdle = gpGlobals->time + RANDOM_FLOAT( 10, 15 );
	}
}

void CETHEREAL::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
		return;

	int iAnim;
	switch ( RANDOM_LONG( 0, 1 ) )
	{
	case 0:
		iAnim = ETHEREAL_LONGIDLE;
		break;

	default:
	case 1:
		iAnim = ETHEREAL_IDLE1;
		break;
	}

	SendWeaponAnim( iAnim );

	m_flTimeWeaponIdle = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 10, 15 ); // how long till we do this again.
}

class CETHEREALAmmoClip : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache();
		SET_MODEL( ENT( pev ), "models/w_etherealclip.mdl" );
		CBasePlayerAmmo::Spawn();
	}
	void Precache( void )
	{
		PRECACHE_MODEL( "models/w_etherealclip.mdl" );
		PRECACHE_SOUND( "items/9mmclip1.wav" );
	}
	BOOL AddAmmo( CBaseEntity *pOther )
	{
		int bResult = ( pOther->GiveAmmo( AMMO_ETHEREALCLIP_GIVE, "762MM", _762MM_MAX_CARRY ) != -1 );
		if ( bResult )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
		}
		return bResult;
	}
};
LINK_ENTITY_TO_CLASS( ammo_etherealclip, CETHEREALAmmoClip );

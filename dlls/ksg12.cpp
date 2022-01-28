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
#include "gamerules.h"

// special deathmatch ksg12 spreads
#define VECTOR_CONE_DM_KSG12         Vector( 0.08716, 0.04362, 0.00 ) // 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLEKSG12   Vector( 0.17365, 0.04362, 0.00 ) // 20 degrees by 5 degrees
#define VECTOR_CONE_DM_SHOTGUN       Vector( 0.08716, 0.04362, 0.00 ) // 10 degrees by 5 degrees
#define VECTOR_CONE_DM_DOUBLESHOTGUN Vector( 0.17365, 0.04362, 0.00 ) // 20 degrees by 5 degrees

enum ksg12_e
{
	KSG12_IDLE = 0,
	KSG12_FIRE,
	KSG12_FIRE2,
	KSG12_RELOAD,
	KSG12_PUMP,
	KSG12_START_RELOAD,
	KSG12_DRAW,
	KSG12_HOLSTER,
	KSG12_IDLE4,
	KSG12_IDLE_DEEP
};

LINK_ENTITY_TO_CLASS( weapon_ksg12, CKSG12 );

void CKSG12::Spawn()
{
	Precache();
	m_iId = WEAPON_KSG12;
	SET_MODEL( ENT( pev ), "models/w_ksg12.mdl" );

	m_iDefaultAmmo = KSG12_DEFAULT_GIVE;

	FallInit(); // get ready to fall
}

void CKSG12::Precache( void )
{
	PRECACHE_MODEL( "models/v_ksg12.mdl" );
	PRECACHE_MODEL( "models/w_ksg12.mdl" );
	PRECACHE_MODEL( "models/p_ksg12.mdl" );
	PRECACHE_MODEL( "models/w_2375shell.mdl" );

	m_iShell = PRECACHE_MODEL( "models/shotgunshell.mdl" ); // ksg12 shell

	PRECACHE_SOUND( "items/9mmclip1.wav" );

	PRECACHE_SOUND( "weapons/ksg12-1.wav" ); //ksg12

	PRECACHE_SOUND( "weapons/reload1.wav" );    // ksg12 reload
	PRECACHE_SOUND( "weapons/reload3.wav" );    // ksg12 reload
	PRECACHE_SOUND( "weapons/ksg12_cock.wav" ); // ksg12 reload

	//	PRECACHE_SOUND ("weapons/sshell1.wav");	// ksg12 reload - played on client
	//	PRECACHE_SOUND ("weapons/sshell3.wav");	// ksg12 reload - played on client

	PRECACHE_SOUND( "weapons/357_cock1.wav" ); // gun empty sound

	m_usKSG12 = PRECACHE_EVENT( 1, "events/ksg12.sc" );
}

int CKSG12::AddToPlayer( CBasePlayer *pPlayer )
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

int CKSG12::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = "23.75";
	p->iMaxAmmo1 = _2375_MAX_CARRY;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = KSG12_MAX_CLIP;
	p->iSlot = 5;
	p->iPosition = 5;
	p->iFlags = 0;
	p->iId = m_iId = WEAPON_KSG12;
	p->iWeight = KSG12_WEIGHT;

	return 1;
}

BOOL CKSG12::Deploy()
{
	return DefaultDeploy( "models/v_ksg12.mdl", "models/p_ksg12.mdl", KSG12_DRAW, "ksg12" );
}

void CKSG12::PrimaryAttack()
{
	// don't fire underwater
	if ( m_pPlayer->pev->waterlevel == 3 )
	{
		PlayEmptySound();
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.15;
		return;
	}

	if ( m_iClip <= 0 )
	{
		Reload();
		PlayEmptySound();
		return;
	}

	m_pPlayer->m_iWeaponVolume = LOUD_GUN_VOLUME;
	m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	m_iClip -= 1;

	int flags;
#if defined( CLIENT_WEAPONS )
	flags = FEV_NOTHOST;
#else
	flags = 0;
#endif

	m_pPlayer->pev->effects = (int)( m_pPlayer->pev->effects ) | EF_MUZZLEFLASH;

	// player "shoot" animation
	m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	Vector vecDir;

#ifdef CLIENT_DLL
	if ( bIsMultiplayer() )
#else
	if ( g_pGameRules->IsMultiplayer() )
#endif
	{
		// tuned for deathmatch
		vecDir = m_pPlayer->FireBulletsPlayer( 12, vecSrc, vecAiming, VECTOR_CONE_DM_DOUBLESHOTGUN, 2048, BULLET_PLAYER_KSG12, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}
	else
	{
		// untouched default single player
		vecDir = m_pPlayer->FireBulletsPlayer( 12, vecSrc, vecAiming, VECTOR_CONE_10DEGREES, 2048, BULLET_PLAYER_KSG12, 0, 0, m_pPlayer->pev, m_pPlayer->random_seed );
	}

	PLAYBACK_EVENT_FULL( flags, m_pPlayer->edict(), m_usKSG12, 0.0, (float *)&g_vecZero, (float *)&g_vecZero, vecDir.x, vecDir.y, 0, 0, 0, 0 );

	if ( !m_iClip && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 )
		// HEV suit - indicate out of ammo condition
		m_pPlayer->SetSuitUpdate( "!HEV_AMO0", FALSE, 0 );

	if ( m_iClip != 0 )
		m_flPumpTime = gpGlobals->time + 0.5;

	m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.75;
	m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 0.75;
	if ( m_iClip != 0 )
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 5.0;
	else
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.75;
	m_fInSpecialReload = 0;

	m_fInSpecialReload = 0;
}

void CKSG12::Reload( void )
{
	if ( m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0 || m_iClip == KSG12_MAX_CLIP )
		return;

	// don't reload until recoil is done
	if ( m_flNextPrimaryAttack > UTIL_WeaponTimeBase() )
		return;

	// check to see if we're ready to reload
	if ( m_fInSpecialReload == 0 )
	{
		SendWeaponAnim( KSG12_START_RELOAD );
		m_fInSpecialReload = 1;
		m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.6;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.6;
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 1.0;
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;
		return;
	}
	else if ( m_fInSpecialReload == 1 )
	{
		if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
			return;
		// was waiting for gun to move to side
		m_fInSpecialReload = 2;

		if ( RANDOM_LONG( 0, 1 ) )
			EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/reload1.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ) );
		else
			EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/reload3.wav", 1, ATTN_NORM, 0, 85 + RANDOM_LONG( 0, 0x1f ) );

		SendWeaponAnim( KSG12_RELOAD );

		m_flNextReload = UTIL_WeaponTimeBase() + 0.5;
		m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 0.5;
	}
	else
	{
		// Add them to the clip
		m_iClip += 1;
		m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] -= 1;
		m_fInSpecialReload = 1;
	}
}

void CKSG12::WeaponIdle( void )
{
	ResetEmptySound();

	m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );

	if ( m_flPumpTime && m_flPumpTime < gpGlobals->time )
	{
		// play pumping sound
		EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/ksg12_cock.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG( 0, 0x1f ) );
		m_flPumpTime = 0;
	}

	if ( m_flTimeWeaponIdle < UTIL_WeaponTimeBase() )
	{
		if ( m_iClip == 0 && m_fInSpecialReload == 0 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
		{
			Reload();
		}
		else if ( m_fInSpecialReload != 0 )
		{
			if ( m_iClip != 3 && m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] )
			{
				Reload();
			}
			else
			{
				// reload debounce has timed out
				SendWeaponAnim( KSG12_PUMP );

				// play cocking sound
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/ksg12_cock.wav", 1, ATTN_NORM, 0, 95 + RANDOM_LONG( 0, 0x1f ) );
				m_fInSpecialReload = 0;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + 1.5;
			}
		}
		else
		{
			int iAnim;
			float flRand = UTIL_SharedRandomFloat( m_pPlayer->random_seed, 0, 1 );
			if ( flRand <= 0.8 )
			{
				iAnim = KSG12_IDLE_DEEP;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ( 60.0 / 12.0 ); // * RANDOM_LONG(2, 5);
			}
			else if ( flRand <= 0.95 )
			{
				iAnim = KSG12_IDLE;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ( 20.0 / 9.0 );
			}
			else
			{
				iAnim = KSG12_IDLE4;
				m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + ( 20.0 / 9.0 );
			}
			SendWeaponAnim( iAnim );
		}
	}
}

class CKSG12Ammo : public CBasePlayerAmmo
{
	void Spawn( void )
	{
		Precache();
		SET_MODEL( ENT( pev ), "models/w_2375shell.mdl" );
		CBasePlayerAmmo::Spawn();
	}
	void Precache( void )
	{
		PRECACHE_MODEL( "models/w_2375shell.mdl" );
		PRECACHE_SOUND( "items/9mmclip1.wav" );
	}
	BOOL AddAmmo( CBaseEntity *pOther )
	{
		if ( pOther->GiveAmmo( 10, "23.75", _2375_MAX_CARRY ) != -1 )
		{
			EMIT_SOUND( ENT( pev ), CHAN_ITEM, "items/9mmclip1.wav", 1, ATTN_NORM );
			return TRUE;
		}
		return FALSE;
	}
};
LINK_ENTITY_TO_CLASS( ammo_23x75, CKSG12Ammo );

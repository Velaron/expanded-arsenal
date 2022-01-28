//=========================================================
// Opposing Forces Weapon knife (standart knife)
//
// Made by Demiurge
//
//FGD weapon_knife
//
//���������� DIMaN[BBc]
//=========================================================

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "weapons.h"
#include "nodes.h"
#include "player.h"
#include "gamerules.h"

#define KNIFE_BODYHIT_VOLUME 128
#define KNIFE_WALLHIT_VOLUME 512
#define KNIFE_FLOOR          0
#define KNIFE_INFLOOR        1
#define KNIFE_INWALL         2

LINK_ENTITY_TO_CLASS( weapon_knife, CKnife ); //������� ������

enum gauss_e //��������� ��������
{
	KNIFE_IDLE = 0,
	KNIFE_DRAW,
	KNIFE_HOLSTER,
	KNIFE_ATTACK1HIT,
	KNIFE_ATTACK1MISS,
	KNIFE_ATTACK2MISS,
	KNIFE_ATTACK2HIT,
	KNIFE_ATTACK3MISS,
	KNIFE_ATTACK3HIT,
	KNIFE_IDLE2,
	KNIFE_IDLE3,
	KNIFE_CHARGE,
	KNIFE_STAB
};

void CKnife::Spawn()
{
	Precache();
	m_iId = WEAPON_KNIFE;                          //��� ������, ������� ������ � fgd ����
	SET_MODEL( ENT( pev ), "models/w_knife.mdl" ); //������ ������ ���� �� �����
	m_iClip = -1;

	pev->movetype = MOVETYPE_FLY;
	pev->solid = SOLID_TRIGGER;

	UTIL_SetOrigin( pev, pev->origin );
	UTIL_SetSize( pev, Vector( -16, -16, 0 ), Vector( 16, 16, 32 ) ); //pointsize until it lands on the ground.

	SetTouch( &CKnife::DefaultTouch );
	SetThink( &CKnife::FallThink );

	pev->nextthink = gpGlobals->time + 0.1;

	// pev->body
	FallInit(); // get ready to fall down.
}

void CKnife::Precache( void )
{
	//��������� ������ � �����
	PRECACHE_MODEL( "models/v_knife.mdl" ); //������ � ��� � �����
	PRECACHE_MODEL( "models/w_knife.mdl" ); //������ �� �����
	PRECACHE_MODEL( "models/p_knife.mdl" ); //������ � ����� � ������ � �����
	PRECACHE_SOUND( "weapons/knife_hit_wall1.wav" );
	PRECACHE_SOUND( "weapons/knife_hit_wall2.wav" );
	PRECACHE_SOUND( "weapons/knife_hit_flesh1.wav" );
	PRECACHE_SOUND( "weapons/knife_hit_flesh2.wav" );
	PRECACHE_SOUND( "weapons/knife1.wav" );
	PRECACHE_SOUND( "weapons/knife2.wav" );
	PRECACHE_SOUND( "weapons/knife3.wav" );

	m_usKnife = PRECACHE_EVENT( 1, "events/knife.sc" );
}

int CKnife::GetItemInfo( ItemInfo *p )
{
	p->pszName = STRING( pev->classname );
	p->pszAmmo1 = NULL;
	p->iMaxAmmo1 = -1;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = WEAPON_NOCLIP;
	p->iSlot = 0;     //���� � ����, ������� � 0. � ������ ������ 1
	p->iPosition = 1; //������� � ����� ����, ������� � 0. � ������ ������ 2
	p->iId = WEAPON_KNIFE;
	p->iWeight = CROWBAR_WEIGHT;
	return 1;
}

//��������� ����� ������ ������ �� ������� ����� �++

BOOL CKnife::Deploy()
{
	return DefaultDeploy( "models/v_knife.mdl", "models/p_knife.mdl", KNIFE_DRAW, "knife" );
}

void CKnife::Holster( int skiplocal /* = 0 */ )
{
	m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + 0.5;
	SendWeaponAnim( KNIFE_HOLSTER );
}

void FindHullIntersectionKnife( const Vector &vecSrc, TraceResult &tr, float *mins, float *maxs, edict_t *pEntity )
{
	int i, j, k;
	float distance;
	float *minmaxs[2] = { mins, maxs };
	TraceResult tmpTrace;
	Vector vecHullEnd = tr.vecEndPos;
	Vector vecEnd;

	distance = 1e6f;

	vecHullEnd = vecSrc + ( ( vecHullEnd - vecSrc ) * 2 );
	UTIL_TraceLine( vecSrc, vecHullEnd, dont_ignore_monsters, pEntity, &tmpTrace );
	if ( tmpTrace.flFraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, pEntity, &tmpTrace );
				if ( tmpTrace.flFraction < 1.0 )
				{
					float thisDistance = ( tmpTrace.vecEndPos - vecSrc ).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

void CKnife::PrimaryAttack()
{
	if ( !Swing( 1 ) )
	{
		SetThink( &CKnife::SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CKnife::SecondaryAttack()
{
	if ( !Swing2( 1 ) )
	{
		SetThink( &CKnife::SwingAgain );
		pev->nextthink = gpGlobals->time + 0.1;
	}
}

void CKnife::Smack()
{
	DecalGunshot( &m_trHit, BULLET_PLAYER_CROWBAR );
}

void CKnife::SwingAgain( void )
{
	Swing( 0 );
}

int CKnife::Swing( int fFirst )
{
	int fDidHit = FALSE;

	TraceResult tr;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

#ifndef CLIENT_DLL
	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersectionKnife( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	if ( tr.flFraction >= 1.0 )
	{
		if ( fFirst )
		{
			switch ( ( m_iSwing++ ) % 3 )
			{
			case 0:
				SendWeaponAnim( KNIFE_ATTACK1MISS );
				break;
			case 1:
				SendWeaponAnim( KNIFE_ATTACK2MISS );
				break;
			case 2:
				SendWeaponAnim( KNIFE_ATTACK3MISS );
				break;
			}

			m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.5;

			switch ( RANDOM_LONG( 0, 2 ) )
			{
			case 0:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife1.wav", 1, ATTN_NORM );
				break;
			case 1:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife2.wav", 1, ATTN_NORM );
				break;
			default:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife3.wav", 1, ATTN_NORM );
				break;
			}
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch ( ( ( m_iSwing++ ) % 2 ) + 1 )
		{
		case 0:
			SendWeaponAnim( KNIFE_ATTACK1HIT );
			break;
		case 1:
			SendWeaponAnim( KNIFE_ATTACK2HIT );
			break;
		case 2:
			SendWeaponAnim( KNIFE_ATTACK3HIT );
			break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

#ifndef CLIENT_DLL

		// hit
		fDidHit = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		ClearMultiDamage();

		if ( ( m_flNextPrimaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() )
		{
			// first swing does full damage
			pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgKnife, gpGlobals->v_forward, &tr, DMG_CLUB );
		}
		else
		{
			// subsequent swings do half
			pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgKnife / 2, gpGlobals->v_forward, &tr, DMG_CLUB );
		}

		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if ( pEntity )
		{
			if ( pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE )
			{
				// play thwack or smack sound
				switch ( RANDOM_LONG( 0, 1 ) )
				{
				case 0:
					EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_flesh1.wav", 1, ATTN_NORM );
					break;
				case 1:
					EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_flesh2.wav", 1, ATTN_NORM );
					break;
				}
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
				if ( !pEntity->IsAlive() )
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if ( fHitWorld )
		{
			float fvolbar = TEXTURETYPE_PlaySound( &tr, vecSrc, vecSrc + ( vecEnd - vecSrc ) * 2, BULLET_PLAYER_CROWBAR );

			if ( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play knife strike
			switch ( RANDOM_LONG( 0, 1 ) )
			{
			case 0:
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_wall1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			case 1:
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_wall2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			}

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;
#endif
		m_flNextPrimaryAttack = UTIL_WeaponTimeBase() + 0.25;

		SetThink( &CKnife::Smack );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}
	return fDidHit;
}

int CKnife::Swing2( int fFirst )
{
	int fDidHit2 = FALSE;

	TraceResult tr;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle );
	Vector vecSrc = m_pPlayer->GetGunPosition();
	Vector vecEnd = vecSrc + gpGlobals->v_forward * 32;

	UTIL_TraceLine( vecSrc, vecEnd, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

#ifndef CLIENT_DLL
	if ( tr.flFraction >= 1.0 )
	{
		UTIL_TraceHull( vecSrc, vecEnd, dont_ignore_monsters, head_hull, ENT( m_pPlayer->pev ), &tr );
		if ( tr.flFraction < 1.0 )
		{
			// Calculate the point of intersection of the line (or hull) and the object we hit
			// This is and approximation of the "best" intersection
			CBaseEntity *pHit = CBaseEntity::Instance( tr.pHit );
			if ( !pHit || pHit->IsBSPModel() )
				FindHullIntersectionKnife( vecSrc, tr, VEC_DUCK_HULL_MIN, VEC_DUCK_HULL_MAX, m_pPlayer->edict() );
			vecEnd = tr.vecEndPos; // This is the point on the actual surface (the hull could have hit space)
		}
	}
#endif

	if ( tr.flFraction >= 1.0 )
	{
		if ( fFirst )
		{
			switch ( ( m_iSwing2++ ) % 3 )
			{
			case 0:
				SendWeaponAnim( KNIFE_STAB );
				break;
			case 1:
				SendWeaponAnim( KNIFE_STAB );
				break;
			case 2:
				SendWeaponAnim( KNIFE_STAB );
				break;
			}

			m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.2;

			switch ( RANDOM_LONG( 0, 2 ) )
			{
			case 0:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife1.wav", 1, ATTN_NORM );
				break;
			case 1:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife2.wav", 1, ATTN_NORM );
				break;
			default:
				EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_WEAPON, "weapons/knife3.wav", 1, ATTN_NORM );
				break;
			}
			// player "shoot" animation
			m_pPlayer->SetAnimation( PLAYER_ATTACK1 );
		}
	}
	else
	{
		switch ( ( ( m_iSwing2++ ) % 2 ) + 1 )
		{
		case 0:
			SendWeaponAnim( KNIFE_STAB );
			break;
		case 1:
			SendWeaponAnim( KNIFE_STAB );
			break;
		case 2:
			SendWeaponAnim( KNIFE_STAB );
			break;
		}

		// player "shoot" animation
		m_pPlayer->SetAnimation( PLAYER_ATTACK1 );

#ifndef CLIENT_DLL

		// hit
		fDidHit2 = TRUE;
		CBaseEntity *pEntity = CBaseEntity::Instance( tr.pHit );

		ClearMultiDamage();

		if ( ( m_flNextSecondaryAttack + 1 < UTIL_WeaponTimeBase() ) || g_pGameRules->IsMultiplayer() )
		{
			// first Swing2 does full damage
			pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgKnife * 4, gpGlobals->v_forward, &tr, DMG_CLUB );
		}
		else
		{
			// subsequent Swing2s do half
			pEntity->TraceAttack( m_pPlayer->pev, gSkillData.plrDmgKnife * 4, gpGlobals->v_forward, &tr, DMG_CLUB );
		}
		ApplyMultiDamage( m_pPlayer->pev, m_pPlayer->pev );

		// play thwack, smack, or dong sound
		float flVol = 1.0;
		int fHitWorld = TRUE;

		if ( pEntity )
		{
			if ( pEntity->Classify() != CLASS_NONE && pEntity->Classify() != CLASS_MACHINE )
			{
				// play thwack or smack sound
				switch ( RANDOM_LONG( 0, 1 ) )
				{
				case 0:
					EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_flesh1.wav", 1, ATTN_NORM );
					break;
				case 1:
					EMIT_SOUND( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_flesh2.wav", 1, ATTN_NORM );
					break;
				}
				m_pPlayer->m_iWeaponVolume = KNIFE_BODYHIT_VOLUME;
				if ( !pEntity->IsAlive() )
					return TRUE;
				else
					flVol = 0.1;

				fHitWorld = FALSE;
			}
		}

		// play texture hit sound
		// UNDONE: Calculate the correct point of intersection when we hit with the hull instead of the line

		if ( fHitWorld )
		{
			float fvolbar = TEXTURETYPE_PlaySound( &tr, vecSrc, vecSrc + ( vecEnd - vecSrc ) * 2, BULLET_PLAYER_CROWBAR );

			if ( g_pGameRules->IsMultiplayer() )
			{
				// override the volume here, cause we don't play texture sounds in multiplayer,
				// and fvolbar is going to be 0 from the above call.

				fvolbar = 1;
			}

			// also play knife strike
			switch ( RANDOM_LONG( 0, 1 ) )
			{
			case 0:
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_wall1.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			case 1:
				EMIT_SOUND_DYN( ENT( m_pPlayer->pev ), CHAN_ITEM, "weapons/knife_hit_wall2.wav", fvolbar, ATTN_NORM, 0, 98 + RANDOM_LONG( 0, 3 ) );
				break;
			}

			// delay the decal a bit
			m_trHit = tr;
		}

		m_pPlayer->m_iWeaponVolume = flVol * KNIFE_WALLHIT_VOLUME;
#endif
		m_flNextSecondaryAttack = UTIL_WeaponTimeBase() + 1.0;

		SetThink( &CKnife::Smack );
		pev->nextthink = UTIL_WeaponTimeBase() + 0.2;
	}
	return fDidHit2;
}
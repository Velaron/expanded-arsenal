/***
*
*	Copyright (c) 1996-2001, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   This source code contains proprietary and confidential information of
*   Valve LLC and its suppliers.  Access to this code is restricted to
*   persons who have executed a written SDK license with Valve.  Any access,
*   use or distribution of this code by or to any unlicensed person is illegal.
*
****/
//=========================================================
// monster template
//=========================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "monsters.h"
#include "talkmonster.h"
#include "schedule.h"
#include "defaultai.h"
#include "scripted.h"
#include "weapons.h"
#include "soundent.h"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
#define OTIS_AE_DRAW    ( 2 )
#define OTIS_AE_SHOOT   ( 3 )
#define OTIS_AE_HOLSTER ( 4 )

#define OT_GUN_GROUP   1
#define OT_GUN_HOLSTER 0
#define OT_GUN_DRAWN   1
#define OT_GUN_DOGHNUT 2

#define OT_HEAD_GROUP  2
#define OT_HEAD_NORMAL 0
#define OT_HEAD_BALD   1

class COtis : public CTalkMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	int ISoundMask( void );
	void Eagle( void );
	void AlertSound( void );
	int Classify( void );
	void HandleAnimEvent( MonsterEvent_t *pEvent );

	void RunTask( Task_t *pTask );
	void StartTask( Task_t *pTask );
	virtual int ObjectCaps( void ) { return CTalkMonster ::ObjectCaps() | FCAP_IMPULSE_USE; }
	int TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	BOOL CheckRangeAttack1( float flDot, float flDist );
	void KeyValue( KeyValueData *pkvd );
	void DeclineFollowing( void );

	// Override these to set behavior
	Schedule_t *GetScheduleOfType( int Type );
	Schedule_t *GetSchedule( void );
	MONSTERSTATE GetIdealState( void );

	void DeathSound( void );
	void PainSound( void );

	void TalkInit( void );

	void TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType );
	void Killed( entvars_t *pevAttacker, int iGib );

	virtual int Save( CSave &save );
	virtual int Restore( CRestore &restore );
	static TYPEDESCRIPTION m_SaveData[];

	BOOL m_fHostile;
	BOOL m_fGunDrawn;
	float m_painTime;
	float m_checkAttackTime;
	BOOL m_lastAttackCheck;
	int m_iHead;

	// UNDONE: What is this for?  It isn't used?
	float m_flPlayerDamage; // how much pain has the player inflicted on me?

	CUSTOM_SCHEDULES;
};

LINK_ENTITY_TO_CLASS( monster_otis, COtis );

TYPEDESCRIPTION COtis::m_SaveData[] = {
	DEFINE_FIELD( COtis, m_fGunDrawn, FIELD_BOOLEAN ),
	DEFINE_FIELD( COtis, m_painTime, FIELD_TIME ),
	DEFINE_FIELD( COtis, m_checkAttackTime, FIELD_TIME ),
	DEFINE_FIELD( COtis, m_lastAttackCheck, FIELD_BOOLEAN ),
	DEFINE_FIELD( COtis, m_flPlayerDamage, FIELD_FLOAT ),
	DEFINE_FIELD( COtis, m_fHostile, FIELD_BOOLEAN ),
	DEFINE_FIELD( COtis, m_iHead, FIELD_INTEGER ),
};

IMPLEMENT_SAVERESTORE( COtis, CTalkMonster );

//=========================================================
// KeyValue
//=========================================================
void COtis ::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "hostile" ) )
	{
		m_fHostile = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else if ( FStrEq( pkvd->szKeyName, "head" ) )
	{
		m_iHead = ALLOC_STRING( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================
Task_t tlOtisFollow[] = {
	{ TASK_MOVE_TO_TARGET_RANGE, (float)128 }, // Move within 128 of target ent (client)
	{ TASK_SET_SCHEDULE, (float)SCHED_TARGET_FACE },
};

Schedule_t slOtisFollow[] = {
	{ tlOtisFollow,
	  ARRAYSIZE( tlOtisFollow ),
	  bits_COND_NEW_ENEMY | bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE | bits_COND_HEAR_SOUND | bits_COND_PROVOKED,
	  bits_SOUND_DANGER,
	  "Follow" },
};

//=========================================================
// OtisDraw- much better looking draw schedule for when
// otis knows who he's gonna attack.
//=========================================================
Task_t tlOtisEnemyDraw[] = {
	{ TASK_STOP_MOVING, 0 },
	{ TASK_FACE_ENEMY, 0 },
	{ TASK_PLAY_SEQUENCE_FACE_ENEMY, (float)ACT_ARM },
};

Schedule_t slOtisEnemyDraw[] = {
	{ tlOtisEnemyDraw,
	  ARRAYSIZE( tlOtisEnemyDraw ),
	  0,
	  0,
	  "Otis Enemy Draw" }
};

Task_t tlOtisFaceTarget[] = {
	{ TASK_SET_ACTIVITY, (float)ACT_IDLE },
	{ TASK_FACE_TARGET, (float)0 },
	{ TASK_SET_ACTIVITY, (float)ACT_IDLE },
	{ TASK_SET_SCHEDULE, (float)SCHED_TARGET_CHASE },
};

Schedule_t slOtisFaceTarget[] = {
	{ tlOtisFaceTarget,
	  ARRAYSIZE( tlOtisFaceTarget ),
	  bits_COND_CLIENT_PUSH | bits_COND_NEW_ENEMY | bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE | bits_COND_HEAR_SOUND | bits_COND_PROVOKED,
	  bits_SOUND_DANGER,
	  "FaceTarget" },
};

Task_t tlIdleOtisStand[] = {
	{ TASK_STOP_MOVING, 0 },
	{ TASK_SET_ACTIVITY, (float)ACT_IDLE },
	{ TASK_WAIT, (float)2 },          // repick IDLESTAND every two seconds.
	{ TASK_TLK_HEADRESET, (float)0 }, // reset head position
};

Schedule_t slIdleOtisStand[] = {
	{ tlIdleOtisStand,
	  ARRAYSIZE( tlIdleOtisStand ),
	  bits_COND_NEW_ENEMY | bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE | bits_COND_HEAR_SOUND | bits_COND_SMELL | bits_COND_PROVOKED,
	  bits_SOUND_COMBAT |                       // sound flags - change these, and you'll break the talking code.
	      bits_SOUND_DANGER | bits_SOUND_MEAT | // scents
	      bits_SOUND_CARCASS | bits_SOUND_GARBAGE,
	  "IdleStand" },
};

DEFINE_CUSTOM_SCHEDULES( COtis ) {
	slOtisFollow,
	slOtisEnemyDraw,
	slOtisFaceTarget,
	slIdleOtisStand,
};

IMPLEMENT_CUSTOM_SCHEDULES( COtis, CTalkMonster );

void COtis ::StartTask( Task_t *pTask )
{
	CTalkMonster::StartTask( pTask );
}

void COtis ::RunTask( Task_t *pTask )
{
	switch ( pTask->iTask )
	{
	case TASK_RANGE_ATTACK1:
		if ( m_hEnemy != 0 && ( m_hEnemy->IsPlayer() ) || m_fHostile == TRUE )
			pev->framerate = 1.5;

		CTalkMonster::RunTask( pTask );
		break;
	default:
		CTalkMonster::RunTask( pTask );
		break;
	}
}

//=========================================================
// ISoundMask - returns a bit mask indicating which types
// of sounds this monster regards.
//=========================================================
int COtis ::ISoundMask( void )
{
	return bits_SOUND_WORLD | bits_SOUND_COMBAT | bits_SOUND_CARCASS | bits_SOUND_MEAT | bits_SOUND_GARBAGE | bits_SOUND_DANGER | bits_SOUND_PLAYER;
}

//=========================================================
// Classify - indicates this monster's place in the
// relationship table.
//=========================================================
int COtis ::Classify( void )
{
	if ( m_fHostile )
		return CLASS_HUMAN_MILITARY;
	else
		return CLASS_PLAYER_ALLY;
}

//=========================================================
// ALertSound - otis says "Freeze!"
//=========================================================
void COtis ::AlertSound( void )
{
	if ( m_hEnemy != 0 )
	{
		if ( FOkToSpeak() )
			PlaySentence( "OT_ATTACK", RANDOM_FLOAT( 2.8, 3.2 ), VOL_NORM, ATTN_IDLE );
	}
}

//=========================================================
// SetYawSpeed - allows each sequence to have a different
// turn rate associated with it.
//=========================================================
void COtis ::SetYawSpeed( void )
{
	int ys = 0;
	switch ( m_Activity )
	{
	case ACT_IDLE:
		ys = 70;
		break;
	case ACT_WALK:
		ys = 70;
		break;
	case ACT_RUN:
		ys = 90;
		break;
	default:
		ys = 70;
		break;
	}

	pev->yaw_speed = ys;
}

//=========================================================
// CheckRangeAttack1
//=========================================================
BOOL COtis ::CheckRangeAttack1( float flDot, float flDist )
{
	if ( flDist <= 1024 && flDot >= 0.5 )
	{
		if ( gpGlobals->time > m_checkAttackTime )
		{
			TraceResult tr;
			Vector shootOrigin = pev->origin + Vector( 0, 0, 55 );
			CBaseEntity *pEnemy = m_hEnemy;
			Vector shootTarget = ( ( pEnemy->BodyTarget( shootOrigin ) - pEnemy->pev->origin ) + m_vecEnemyLKP );
			UTIL_TraceLine( shootOrigin, shootTarget, dont_ignore_monsters, ENT( pev ), &tr );
			m_checkAttackTime = gpGlobals->time + 1;

			if ( tr.flFraction == 1.0 || ( tr.pHit != NULL && CBaseEntity::Instance( tr.pHit ) == pEnemy ) )
				m_lastAttackCheck = TRUE;
			else
				m_lastAttackCheck = FALSE;

			m_checkAttackTime = gpGlobals->time + 1.5;
		}

		return m_lastAttackCheck;
	}

	return FALSE;
}

//=========================================================
// Eagle - shoots one round from the pistol at
// the enemy otis is facing.
//=========================================================
void COtis ::Eagle( void )
{
	Vector vecShootOrigin;

	UTIL_MakeVectors( pev->angles );
	vecShootOrigin = pev->origin + Vector( 0, 0, 55 );
	Vector vecShootDir = ShootAtEnemy( vecShootOrigin );

	Vector angDir = UTIL_VecToAngles( vecShootDir );
	SetBlending( 0, angDir.x );
	pev->effects = EF_MUZZLEFLASH;

	FireBullets( 1, vecShootOrigin, vecShootDir, VECTOR_CONE_2DEGREES, 1024, BULLET_MONSTER_357 );

	int pitchShift = RANDOM_LONG( 0, 20 );

	// Only shift about half the time
	if ( pitchShift > 10 )
		pitchShift = 0;
	else
		pitchShift -= 5;

	EMIT_SOUND_DYN( ENT( pev ), CHAN_WEAPON, "weapons/desert_eagle_fire.wav", 1, ATTN_NORM, 0, 100 + pitchShift );

	CSoundEnt::InsertSound( bits_SOUND_COMBAT, pev->origin, 384, 0.3 );

	// UNDONE: Reload?
	m_cAmmoLoaded--; // take away a bullet!
}

//=========================================================
// HandleAnimEvent - catches the monster-specific messages
// that occur when tagged animation frames are played.
//
// Returns number of events handled, 0 if none.
//=========================================================
void COtis ::HandleAnimEvent( MonsterEvent_t *pEvent )
{
	switch ( pEvent->event )
	{
	case OTIS_AE_SHOOT:
		Eagle();
		break;

	case OTIS_AE_DRAW:
		// otis's bodygroup switches here so he can pull gun from holster
		SetBodygroup( OT_GUN_GROUP, OT_GUN_DRAWN );
		m_fGunDrawn = TRUE;
		break;

	case OTIS_AE_HOLSTER:
		// change bodygroup to replace gun in holster
		SetBodygroup( OT_GUN_GROUP, OT_GUN_HOLSTER );
		m_fGunDrawn = FALSE;
		break;

	default:
		CTalkMonster::HandleAnimEvent( pEvent );
	}
}

//=========================================================
// Spawn
//=========================================================
void COtis ::Spawn()
{
	Precache();

	SET_MODEL( ENT( pev ), "models/otis.mdl" );
	UTIL_SetSize( pev, VEC_HUMAN_HULL_MIN, VEC_HUMAN_HULL_MAX );

	pev->solid = SOLID_SLIDEBOX;
	pev->movetype = MOVETYPE_STEP;
	m_bloodColor = BLOOD_COLOR_RED;

	if ( !m_fHostile )
		pev->health = gSkillData.otisHealth;
	else
		pev->health = gSkillData.otisHealth - 10;

	pev->view_ofs = Vector( 0, 0, 50 ); // position of the eyes relative to monster's origin.
	m_flFieldOfView = VIEW_FIELD_WIDE;  // NOTE: we need a wide field of view so npc will notice player and say hello
	m_MonsterState = MONSTERSTATE_NONE;

	if ( !m_fHostile )
	{
		SetBodygroup( OT_GUN_GROUP, OT_GUN_HOLSTER );
		m_fGunDrawn = FALSE;
	}
	else
	{
		SetBodygroup( OT_GUN_GROUP, OT_GUN_DRAWN );
		m_fGunDrawn = TRUE;
	}

	if ( m_iHead == 0 )
		SetBodygroup( OT_HEAD_GROUP, OT_HEAD_NORMAL );

	if ( m_iHead == 1 )
		SetBodygroup( OT_HEAD_GROUP, OT_HEAD_BALD );

	m_afCapability = bits_CAP_HEAR | bits_CAP_TURN_HEAD | bits_CAP_DOORS_GROUP;
	MonsterInit();

	if ( !m_fHostile )
		SetUse( &CTalkMonster::FollowerUse );
}

//=========================================================
// Precache - precaches all resources this monster needs
//=========================================================
void COtis ::Precache()
{
	PRECACHE_MODEL( "models/otis.mdl" );

	PRECACHE_SOUND( "weapons/desert_eagle_fire.wav" );

	PRECACHE_SOUND( "otis/ba_pain1.wav" );
	PRECACHE_SOUND( "otis/ba_pain2.wav" );
	PRECACHE_SOUND( "otis/ba_pain3.wav" );

	PRECACHE_SOUND( "otis/ba_die1.wav" );
	PRECACHE_SOUND( "otis/ba_die2.wav" );
	PRECACHE_SOUND( "otis/ba_die3.wav" );

	// every new otis must call this, otherwise
	// when a level is loaded, nobody will talk (time is reset to 0)
	TalkInit();
	CTalkMonster::Precache();
}

// Init talk data
void COtis ::TalkInit()
{

	CTalkMonster::TalkInit();

	// scientists speach group names (group names are in sentences.txt)

	m_szGrp[TLK_ANSWER] = "OT_ANSWER";
	m_szGrp[TLK_QUESTION] = "OT_QUESTION";
	m_szGrp[TLK_IDLE] = "OT_IDLE";
	m_szGrp[TLK_STARE] = "OT_STARE";
	m_szGrp[TLK_USE] = "OT_OK";
	m_szGrp[TLK_UNUSE] = "OT_WAIT";
	m_szGrp[TLK_STOP] = "OT_STOP";

	m_szGrp[TLK_NOSHOOT] = "OT_SCARED";
	m_szGrp[TLK_HELLO] = "OT_HELLO";

	m_szGrp[TLK_PLHURT1] = "!OT_CUREA";
	m_szGrp[TLK_PLHURT2] = "!OT_CUREB";
	m_szGrp[TLK_PLHURT3] = "!OT_CUREC";

	m_szGrp[TLK_PHELLO] = NULL;           //"BA_PHELLO";		// UNDONE
	m_szGrp[TLK_PIDLE] = NULL;            //"BA_PIDLE";			// UNDONE
	m_szGrp[TLK_PQUESTION] = "OT_PQUEST"; // UNDONE

	m_szGrp[TLK_SMELL] = "OT_SMELL";

	m_szGrp[TLK_WOUND] = "OT_WOUND";
	m_szGrp[TLK_MORTAL] = "OT_MORTAL";

	// get voice for head - just one otis voice for now
	m_voicePitch = 100;
}

static BOOL IsFacing( entvars_t *pevTest, const Vector &reference )
{
	Vector vecDir = ( reference - pevTest->origin );
	vecDir.z = 0;
	vecDir = vecDir.Normalize();
	Vector forward, angle;
	angle = pevTest->v_angle;
	angle.x = 0;
	UTIL_MakeVectorsPrivate( angle, forward, NULL, NULL );
	// He's facing me, he meant it
	if ( DotProduct( forward, vecDir ) > 0.96 ) // +/- 15 degrees or so
		return TRUE;

	return FALSE;
}

int COtis ::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	int ret;
	// make sure friends talk about it if player hurts talkmonsters...
	if ( !m_fHostile )
		ret = CTalkMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );
	else
		ret = CBaseMonster::TakeDamage( pevInflictor, pevAttacker, flDamage, bitsDamageType );

	if ( !IsAlive() || pev->deadflag == DEAD_DYING || m_fHostile )
		return ret;

	if ( !m_fHostile )
	{
		if ( m_MonsterState != MONSTERSTATE_PRONE && ( pevAttacker->flags & FL_CLIENT ) )
		{
			m_flPlayerDamage += flDamage;

			// This is a heurstic to determine if the player intended to harm me
			// If I have an enemy, we can't establish intent (may just be crossfire)
			if ( m_hEnemy == 0 )
			{
				// If the player was facing directly at me, or I'm already suspicious, get mad
				if ( ( m_afMemory & bits_MEMORY_SUSPICIOUS ) || IsFacing( pevAttacker, pev->origin ) )
				{
					// Alright, now I'm pissed!
					PlaySentence( "OT_MAD", 4, VOL_NORM, ATTN_NORM );

					Remember( bits_MEMORY_PROVOKED );
					StopFollowing( TRUE );
				}
				else
				{
					// Hey, be careful with that
					PlaySentence( "OT_SHOT", 4, VOL_NORM, ATTN_NORM );
					Remember( bits_MEMORY_SUSPICIOUS );
				}
			}
			else if ( !( m_hEnemy->IsPlayer() ) && pev->deadflag == DEAD_NO )
				PlaySentence( "OT_SHOT", 4, VOL_NORM, ATTN_NORM );
		}
	}

	return ret;
}

//=========================================================
// PainSound
//=========================================================
void COtis ::PainSound( void )
{
	if ( gpGlobals->time < m_painTime )
		return;

	m_painTime = gpGlobals->time + RANDOM_FLOAT( 0.5, 0.75 );
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_pain1.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	case 1: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_pain2.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	case 2: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_pain3.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	}
}

//=========================================================
// DeathSound
//=========================================================
void COtis ::DeathSound( void )
{
	switch ( RANDOM_LONG( 0, 2 ) )
	{
	case 0: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_die1.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	case 1: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_die2.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	case 2: EMIT_SOUND_DYN( ENT( pev ), CHAN_VOICE, "otis/ba_die3.wav", 1, ATTN_NORM, 0, GetVoicePitch() ); break;
	}
}

void COtis::TraceAttack( entvars_t *pevAttacker, float flDamage, Vector vecDir, TraceResult *ptr, int bitsDamageType )
{
	switch ( ptr->iHitgroup )
	{
	case HITGROUP_CHEST:
	case HITGROUP_STOMACH:
		if ( bitsDamageType & ( DMG_BULLET | DMG_SLASH | DMG_BLAST ) )
			flDamage = flDamage / 2;
		break;
	case 10:
		// always a head shot
		ptr->iHitgroup = HITGROUP_HEAD;
		break;
	}

	CTalkMonster::TraceAttack( pevAttacker, flDamage, vecDir, ptr, bitsDamageType );
}

void COtis::Killed( entvars_t *pevAttacker, int iGib )
{
	if ( GetBodygroup( OT_GUN_GROUP ) != OT_GUN_HOLSTER )
	{ // drop the gun!
		Vector vecGunPos;
		Vector vecGunAngles;

		SetBodygroup( OT_GUN_GROUP, OT_GUN_HOLSTER );
		GetAttachment( 0, vecGunPos, vecGunAngles );
		CBaseEntity *pGun = DropItem( "weapon_deagle", vecGunPos, vecGunAngles );
	}

	SetUse( NULL );

	// make sure friends talk about it if player hurts talkmonsters...
	if ( !m_fHostile )
		CTalkMonster::Killed( pevAttacker, iGib );
	else
		CBaseMonster::Killed( pevAttacker, iGib );
}

//=========================================================
// AI Schedules Specific to this monster
//=========================================================

Schedule_t *COtis ::GetScheduleOfType( int Type )
{
	Schedule_t *psched;

	switch ( Type )
	{
	case SCHED_ARM_WEAPON:
		if ( m_hEnemy != 0 )
			return slOtisEnemyDraw;
		break;

	// Hook these to make a looping schedule
	case SCHED_TARGET_FACE:
		// call base class default so that otis will talk
		// when 'used'
		psched = CTalkMonster::GetScheduleOfType( Type );

		if ( psched == slIdleStand )
			return slOtisFaceTarget; // override this for different target face behavior
		else
			return psched;
		break;

	case SCHED_TARGET_CHASE:
		return slOtisFollow;
		break;

	case SCHED_IDLE_STAND:
		// call base class default so that scientist will talk
		// when standing during idle
		psched = CTalkMonster::GetScheduleOfType( Type );

		if ( psched == slIdleStand )
			return slIdleOtisStand;
		else
			return psched;
		break;
	}

	return CTalkMonster::GetScheduleOfType( Type );
}

//=========================================================
// GetSchedule - Decides which type of schedule best suits
// the monster's current state and conditions. Then calls
// monster's member function to get a pointer to a schedule
// of the proper type.
//=========================================================
Schedule_t *COtis ::GetSchedule( void )
{
	if ( HasConditions( bits_COND_HEAR_SOUND ) )
	{
		CSound *pSound;
		pSound = PBestSound();

		ASSERT( pSound != NULL );
		if ( pSound && ( pSound->m_iType & bits_SOUND_DANGER ) )
			return GetScheduleOfType( SCHED_TAKE_COVER_FROM_BEST_SOUND );
	}

	if ( HasConditions( bits_COND_ENEMY_DEAD ) && FOkToSpeak() )
		PlaySentence( "OT_KILL", 4, VOL_NORM, ATTN_NORM );

	switch ( m_MonsterState )
	{
	case MONSTERSTATE_COMBAT:
	{
		// dead enemy
		if ( HasConditions( bits_COND_ENEMY_DEAD ) )
			return CBaseMonster ::GetSchedule(); // call base class, all code to handle dead enemies is centralized there.

		// always act surprized with a new enemy
		if ( HasConditions( bits_COND_NEW_ENEMY ) && HasConditions( bits_COND_LIGHT_DAMAGE ) )
			return GetScheduleOfType( SCHED_SMALL_FLINCH );

		// wait for one schedule to draw gun
		if ( !m_fGunDrawn )
			return GetScheduleOfType( SCHED_ARM_WEAPON );

		if ( HasConditions( bits_COND_HEAVY_DAMAGE ) )
			return GetScheduleOfType( SCHED_TAKE_COVER_FROM_ENEMY );
	}
	break;

	case MONSTERSTATE_ALERT:
	case MONSTERSTATE_IDLE:
		if ( HasConditions( bits_COND_LIGHT_DAMAGE | bits_COND_HEAVY_DAMAGE ) )
			return GetScheduleOfType( SCHED_SMALL_FLINCH ); // flinch if hurt

		if ( m_hEnemy == 0 && IsFollowing() )
		{
			if ( !m_hTargetEnt->IsAlive() )
			{
				// UNDONE: Comment about the recently dead player here?
				StopFollowing( FALSE );
				break;
			}
			else
			{
				if ( HasConditions( bits_COND_CLIENT_PUSH ) )
					return GetScheduleOfType( SCHED_MOVE_AWAY_FOLLOW );

				return GetScheduleOfType( SCHED_TARGET_FACE );
			}
		}

		if ( HasConditions( bits_COND_CLIENT_PUSH ) )
			return GetScheduleOfType( SCHED_MOVE_AWAY );

		// try to say something about smells
		TrySmellTalk();
		break;
	}

	return CTalkMonster::GetSchedule();
}

MONSTERSTATE COtis ::GetIdealState( void )
{
	return CTalkMonster::GetIdealState();
}

void COtis::DeclineFollowing( void )
{
	PlaySentence( "OT_POK", 2, VOL_NORM, ATTN_NORM );
}

//=========================================================
// DEAD OTIS PROP
//
// Designer selects a pose in worldcraft, 0 through num_poses-1
// this value is added to what is selected as the 'first dead pose'
// among the monster's normal animations. All dead poses must
// appear sequentially in the model file. Be sure and set
// the m_iFirstPose properly!
//
//=========================================================
class CDeadOtis : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	int Classify( void ) { return CLASS_PLAYER_ALLY; }

	void KeyValue( KeyValueData *pkvd );

	int m_iPose; // which sequence to display	-- temporary, don't need to save
	static const char *m_szPoses[5];
};

const char *CDeadOtis::m_szPoses[] = { "lying_on_back", "lying_on_side", "lying_on_stomach", "stuffed_in_vent", "dead_sitting" };

void CDeadOtis::KeyValue( KeyValueData *pkvd )
{
	if ( FStrEq( pkvd->szKeyName, "pose" ) )
	{
		m_iPose = atoi( pkvd->szValue );
		pkvd->fHandled = TRUE;
	}
	else
		CBaseMonster::KeyValue( pkvd );
}

LINK_ENTITY_TO_CLASS( monster_otis_dead, CDeadOtis );

//=========================================================
// ********** DeadOtis SPAWN **********
//=========================================================
void CDeadOtis ::Spawn()
{
	Precache();
	SET_MODEL( ENT( pev ), "models/otis.mdl" );

	pev->effects = 0;
	pev->yaw_speed = 8;
	pev->sequence = 0;
	m_bloodColor = BLOOD_COLOR_RED;

	pev->sequence = LookupSequence( m_szPoses[m_iPose] );
	if ( pev->sequence == -1 )
		ALERT( at_console, "Dead Otis with bad pose\n" );

	// Corpses have less health
	pev->health = 8;
	MonsterInitDead();
}

void CDeadOtis ::Precache( void )
{
	PRECACHE_MODEL( "models/otis.mdl" );
}
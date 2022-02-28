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
// skill.h - skill level concerns
//=========================================================
#pragma once
#if !defined(SKILL_H)
#define SKILL_H

struct skilldata_t
{
	int iSkillLevel; // game skill level

	// Monster Health & Damage
	float agruntHealth;
	float agruntDmgPunch;

	float apacheHealth;

	float barneyHealth;

	float animeHealth;

	float bigmommaHealthFactor;		// Multiply each node's health by this
	float bigmommaDmgSlash;			// melee attack damage
	float bigmommaDmgBlast;			// mortar attack damage
	float bigmommaRadiusBlast;		// mortar attack radius

	float bullsquidHealth;
	float bullsquidDmgBite;
	float bullsquidDmgWhip;
	float bullsquidDmgSpit;

	float gargantuaHealth;
	float gargantuaDmgSlash;
	float gargantuaDmgFire;
	float gargantuaDmgStomp;

	float hassassinHealth;
	float superHealth;

	float headcrabHealth;
	float headcrabDmgBite;

	float headcrab2Health;
	float headcrab2DmgBite;

	float hgruntHealth;
	float hgruntDmgKick;
	float hgruntShotgunPellets;
	float hgruntGrenadeSpeed;

	float spforceHealth;
	float spforceDmgKick;
	float spforceShotgunPellets;
	float spforceGrenadeSpeed;
	
	float bossHealth;
	float bossDmgKick;
	float bossShotgunPellets;
	float bossGrenadeSpeed;
	
	float robotHealth;
	float robotDmgKick;
	float robotShotgunPellets;
	float robotGrenadeSpeed;
	
	float agentHealth;
	float agentDmgKick;
	float agentShotgunPellets;
	float agentGrenadeSpeed;
	
	float massnHealth;
	float massnDmgKick;
	float massnShotgunPellets;
	float massnGrenadeSpeed;
	
	float spaceHealth;
	float spaceDmgKick;
	float spaceShotgunPellets;
	float spaceGrenadeSpeed;
	
	float alphaHealth;
	float alphaDmgKick;
	float alphaShotgunPellets;
	float alphaGrenadeSpeed;

	float houndeyeHealth;
	float houndeyeDmgBlast;

	float slaveHealth;
	float slaveDmgClaw;
	float slaveDmgClawrake;
	float slaveDmgZap;

	float slave2Health;
	float slave2DmgClaw;
	float slave2DmgClawrake;
	float slave2DmgZap;

	float ichthyosaurHealth;
	float ichthyosaurDmgShake;

	float leechHealth;
	float leechDmgBite;

	float controllerHealth;
	float controllerDmgZap;
	float controllerSpeedBall;
	float controllerDmgBall;

	float nihilanthHealth;
	float nihilanthZap;

	float scientistHealth;

	float snarkHealth;
	float snarkDmgBite;
	float snarkDmgPop;

	float zombieHealth;
	float zombieDmgOneSlash;
	float zombieDmgBothSlash;

	float turretHealth;
	float miniturretHealth;
	float sentryHealth;

	// Player Weapons
	float plrDmgCrowbar;
	float plrDmgKnife;
	float plrDmgDeagle;
	float plrDmgFlamethrower;
	float plrDmg9MM;
	float plrDmgAK47;
	float plrDmgCG;
	float plrDmgAUTOSNIPER;
	float plrDmgD50;
	float plrDmgMP5A3;
	float plrDmgM41;
	float plrDmgPipe;
	float plrDmgBeretta;
	float plrDmgColt45;
	float plrDmgNuclear;
	float plrDmgM1014;
	float plrDmgKSG12;
	float plrDmgFNFAL;
	float plrDmgP226;
	float plrDmgMP7;
	float plrDmgDB;
	float plrDmg357;
	float plrDmgP904;
	float plrDmgMP5;
	float plrDmgM203Grenade;
	float plrDmgBuckshot;
	float plrDmgCrossbowClient;
	float plrDmgCrossbowMonster;
	float plrDmgRPG;
	float plrDmgGauss;
	float plrDmgEgonNarrow;
	float plrDmgEgonWide;
	float plrDmgHornet;
	float plrDmgHandGrenade;
	float plrDmgSatchel;
	float plrDmgTripmine;
	
	// weapons shared by monsters
	float monDmg9MM;
	float otisHealth;
	float monDmgMP5;
	float monDmgM41;
	float monDmgAKM;
	float monDmgP904;
	float monDmg23;
	float monDmgAS;
	float monDmg12MM;
	float monDmgHornet;
	float monDmg357;
	float monDmgFN;
	float monDmgCG;
	float monDmgD50;

	// health/suit charge
	float suitchargerCapacity;
	float batteryCapacity;
	float healthchargerCapacity;
	float healthkitCapacity;
	float scientistHeal;

	// monster damage adj
	float monHead;
	float monChest;
	float monStomach;
	float monLeg;
	float monArm;

	// player damage adj
	float plrHead;
	float plrChest;
	float plrStomach;
	float plrLeg;
	float plrArm;
};

extern	DLL_GLOBAL	skilldata_t	gSkillData;
float GetSkillCvar( const char *pName );

extern DLL_GLOBAL int		g_iSkillLevel;

#define SKILL_EASY		1
#define SKILL_MEDIUM	2
#define SKILL_HARD		3
#endif // SKILL_H

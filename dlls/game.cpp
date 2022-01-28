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
#include "eiface.h"
#include "util.h"
#include "game.h"

BOOL		g_fIsXash3D;

cvar_t displaysoundlist = {"displaysoundlist","0"};

// multiplayer server rules
cvar_t fragsleft	= { "mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t timeleft		= { "mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "

// multiplayer server rules
cvar_t teamplay		= { "mp_teamplay","0", FCVAR_SERVER };
cvar_t fraglimit	= { "mp_fraglimit","0", FCVAR_SERVER };
cvar_t timelimit	= { "mp_timelimit","0", FCVAR_SERVER };
cvar_t friendlyfire	= { "mp_friendlyfire","0", FCVAR_SERVER };
cvar_t falldamage	= { "mp_falldamage","0", FCVAR_SERVER };
cvar_t weaponstay	= { "mp_weaponstay","0", FCVAR_SERVER };
cvar_t selfgauss	= { "selfgauss", "1", FCVAR_SERVER };
cvar_t chargerfix	= { "chargerfix", "0", FCVAR_SERVER };
cvar_t satchelfix	= { "satchelfix", "0", FCVAR_SERVER };
cvar_t monsteryawspeedfix	= { "monsteryawspeedfix", "1", FCVAR_SERVER };
cvar_t forcerespawn	= { "mp_forcerespawn","1", FCVAR_SERVER };
cvar_t flashlight	= { "mp_flashlight","0", FCVAR_SERVER };
cvar_t aimcrosshair	= { "mp_autocrosshair","1", FCVAR_SERVER };
cvar_t decalfrequency	= { "decalfrequency","30", FCVAR_SERVER };
cvar_t teamlist		= { "mp_teamlist","hgrunt;scientist", FCVAR_SERVER };
cvar_t teamoverride	= { "mp_teamoverride","1" };
cvar_t defaultteam	= { "mp_defaultteam","0" };
cvar_t allowmonsters	= { "mp_allowmonsters","0", FCVAR_SERVER };
cvar_t bhopcap		= { "mp_bhopcap", "1", FCVAR_SERVER };

cvar_t allow_spectators = { "allow_spectators", "0", FCVAR_SERVER };	// 0 prevents players from being spectators
cvar_t multibyte_only = { "mp_multibyte_only", "0", FCVAR_SERVER };

cvar_t mp_chattime	= { "mp_chattime","10", FCVAR_SERVER };

// Engine Cvars
cvar_t *g_psv_gravity = NULL;
cvar_t *g_psv_aim = NULL;
cvar_t *g_footsteps = NULL;
cvar_t *g_enable_cheats = NULL;

cvar_t *g_psv_developer;

//CVARS FOR SKILL LEVEL SETTINGS
// Agrunt
cvar_t	sk_agrunt_health1 = {"sk_agrunt_health1","0"};
cvar_t	sk_agrunt_health2 = {"sk_agrunt_health2","0"};
cvar_t	sk_agrunt_health3 = {"sk_agrunt_health3","0"};

cvar_t	sk_agrunt_dmg_punch1 = {"sk_agrunt_dmg_punch1","0"};
cvar_t	sk_agrunt_dmg_punch2 = {"sk_agrunt_dmg_punch2","0"};
cvar_t	sk_agrunt_dmg_punch3 = {"sk_agrunt_dmg_punch3","0"};

// Apache
cvar_t	sk_apache_health1 = {"sk_apache_health1","0"};
cvar_t	sk_apache_health2 = {"sk_apache_health2","0"};
cvar_t	sk_apache_health3 = {"sk_apache_health3","0"};

// Barney
cvar_t	sk_barney_health1 = {"sk_barney_health1","0"};
cvar_t	sk_barney_health2 = {"sk_barney_health2","0"};
cvar_t	sk_barney_health3 = {"sk_barney_health3","0"};

// Anime?
cvar_t sk_anime_health1 = { "sk_anime_health1", "0" };
cvar_t sk_anime_health2 = { "sk_anime_health2", "0" };
cvar_t sk_anime_health3 = { "sk_anime_health3", "0" };

// Bullsquid
cvar_t	sk_bullsquid_health1 = {"sk_bullsquid_health1","0"};
cvar_t	sk_bullsquid_health2 = {"sk_bullsquid_health2","0"};
cvar_t	sk_bullsquid_health3 = {"sk_bullsquid_health3","0"};

cvar_t	sk_bullsquid_dmg_bite1 = {"sk_bullsquid_dmg_bite1","0"};
cvar_t	sk_bullsquid_dmg_bite2 = {"sk_bullsquid_dmg_bite2","0"};
cvar_t	sk_bullsquid_dmg_bite3 = {"sk_bullsquid_dmg_bite3","0"};

cvar_t	sk_bullsquid_dmg_whip1 = {"sk_bullsquid_dmg_whip1","0"};
cvar_t	sk_bullsquid_dmg_whip2 = {"sk_bullsquid_dmg_whip2","0"};
cvar_t	sk_bullsquid_dmg_whip3 = {"sk_bullsquid_dmg_whip3","0"};

cvar_t	sk_bullsquid_dmg_spit1 = {"sk_bullsquid_dmg_spit1","0"};
cvar_t	sk_bullsquid_dmg_spit2 = {"sk_bullsquid_dmg_spit2","0"};
cvar_t	sk_bullsquid_dmg_spit3 = {"sk_bullsquid_dmg_spit3","0"};


// Big Momma
cvar_t	sk_bigmomma_health_factor1 = {"sk_bigmomma_health_factor1","1.0"};
cvar_t	sk_bigmomma_health_factor2 = {"sk_bigmomma_health_factor2","1.0"};
cvar_t	sk_bigmomma_health_factor3 = {"sk_bigmomma_health_factor3","1.0"};

cvar_t	sk_bigmomma_dmg_slash1 = {"sk_bigmomma_dmg_slash1","50"};
cvar_t	sk_bigmomma_dmg_slash2 = {"sk_bigmomma_dmg_slash2","50"};
cvar_t	sk_bigmomma_dmg_slash3 = {"sk_bigmomma_dmg_slash3","50"};

cvar_t	sk_bigmomma_dmg_blast1 = {"sk_bigmomma_dmg_blast1","100"};
cvar_t	sk_bigmomma_dmg_blast2 = {"sk_bigmomma_dmg_blast2","100"};
cvar_t	sk_bigmomma_dmg_blast3 = {"sk_bigmomma_dmg_blast3","100"};

cvar_t	sk_bigmomma_radius_blast1 = {"sk_bigmomma_radius_blast1","250"};
cvar_t	sk_bigmomma_radius_blast2 = {"sk_bigmomma_radius_blast2","250"};
cvar_t	sk_bigmomma_radius_blast3 = {"sk_bigmomma_radius_blast3","250"};

// Gargantua
cvar_t	sk_gargantua_health1 = {"sk_gargantua_health1","0"};
cvar_t	sk_gargantua_health2 = {"sk_gargantua_health2","0"};
cvar_t	sk_gargantua_health3 = {"sk_gargantua_health3","0"};

cvar_t	sk_gargantua_dmg_slash1	= {"sk_gargantua_dmg_slash1","0"};
cvar_t	sk_gargantua_dmg_slash2	= {"sk_gargantua_dmg_slash2","0"};
cvar_t	sk_gargantua_dmg_slash3	= {"sk_gargantua_dmg_slash3","0"};

cvar_t	sk_gargantua_dmg_fire1 = {"sk_gargantua_dmg_fire1","0"};
cvar_t	sk_gargantua_dmg_fire2 = {"sk_gargantua_dmg_fire2","0"};
cvar_t	sk_gargantua_dmg_fire3 = {"sk_gargantua_dmg_fire3","0"};

cvar_t	sk_gargantua_dmg_stomp1	= {"sk_gargantua_dmg_stomp1","0"};
cvar_t	sk_gargantua_dmg_stomp2	= {"sk_gargantua_dmg_stomp2","0"};
cvar_t	sk_gargantua_dmg_stomp3	= {"sk_gargantua_dmg_stomp3","0"};


// Hassassin
cvar_t	sk_hassassin_health1 = {"sk_hassassin_health1","0"};
cvar_t	sk_hassassin_health2 = {"sk_hassassin_health2","0"};
cvar_t	sk_hassassin_health3 = {"sk_hassassin_health3","0"};

// Super?
cvar_t sk_super_health1 = { "sk_super_health1", "0" };
cvar_t sk_super_health2 = { "sk_super_health2", "0" };
cvar_t sk_super_health3 = { "sk_super_health3", "0" };

// Headcrab
cvar_t	sk_headcrab_health1 = {"sk_headcrab_health1","0"};
cvar_t	sk_headcrab_health2 = {"sk_headcrab_health2","0"};
cvar_t	sk_headcrab_health3 = {"sk_headcrab_health3","0"};

cvar_t	sk_headcrab_dmg_bite1 = {"sk_headcrab_dmg_bite1","0"};
cvar_t	sk_headcrab_dmg_bite2 = {"sk_headcrab_dmg_bite2","0"};
cvar_t	sk_headcrab_dmg_bite3 = {"sk_headcrab_dmg_bite3","0"};

// Headcrab2
cvar_t sk_headcrab2_health1 = { "sk_headcrab2_health1", "0" };
cvar_t sk_headcrab2_health2 = { "sk_headcrab2_health2", "0" };
cvar_t sk_headcrab2_health3 = { "sk_headcrab2_health3", "0" };

cvar_t sk_headcrab2_dmg_bite1 = { "sk_headcrab2_dmg_bite1", "0" };
cvar_t sk_headcrab2_dmg_bite2 = { "sk_headcrab2_dmg_bite2", "0" };
cvar_t sk_headcrab2_dmg_bite3 = { "sk_headcrab2_dmg_bite3", "0" };

// Hgrunt 
cvar_t	sk_hgrunt_health1 = {"sk_hgrunt_health1","0"};
cvar_t	sk_hgrunt_health2 = {"sk_hgrunt_health2","0"};
cvar_t	sk_hgrunt_health3 = {"sk_hgrunt_health3","0"};

cvar_t	sk_hgrunt_kick1 = {"sk_hgrunt_kick1","0"};
cvar_t	sk_hgrunt_kick2 = {"sk_hgrunt_kick2","0"};
cvar_t	sk_hgrunt_kick3 = {"sk_hgrunt_kick3","0"};

cvar_t	sk_hgrunt_pellets1 = {"sk_hgrunt_pellets1","0"};
cvar_t	sk_hgrunt_pellets2 = {"sk_hgrunt_pellets2","0"};
cvar_t	sk_hgrunt_pellets3 = {"sk_hgrunt_pellets3","0"};

cvar_t	sk_hgrunt_gspeed1 = {"sk_hgrunt_gspeed1","0"};
cvar_t	sk_hgrunt_gspeed2 = {"sk_hgrunt_gspeed2","0"};
cvar_t	sk_hgrunt_gspeed3 = {"sk_hgrunt_gspeed3","0"};

// Space?
cvar_t sk_space_health1 = { "sk_space_health1", "0" };
cvar_t sk_space_health2 = { "sk_space_health2", "0" };
cvar_t sk_space_health3 = { "sk_space_health3", "0" };

cvar_t sk_space_kick1 = { "sk_space_kick1", "0" };
cvar_t sk_space_kick2 = { "sk_space_kick2", "0" };
cvar_t sk_space_kick3 = { "sk_space_kick3", "0" };

cvar_t sk_space_pellets1 = { "sk_space_pellets1", "0" };
cvar_t sk_space_pellets2 = { "sk_space_pellets2", "0" };
cvar_t sk_space_pellets3 = { "sk_space_pellets3", "0" };

cvar_t sk_space_gspeed1 = { "sk_space_gspeed1", "0" };
cvar_t sk_space_gspeed2 = { "sk_space_gspeed2", "0" };
cvar_t sk_space_gspeed3 = { "sk_space_gspeed3", "0" };

// Boss?
cvar_t sk_boss_health1 = { "sk_boss_health1", "0" };
cvar_t sk_boss_health2 = { "sk_boss_health2", "0" };
cvar_t sk_boss_health3 = { "sk_boss_health3", "0" };

cvar_t sk_boss_kick1 = { "sk_boss_kick1", "0" };
cvar_t sk_boss_kick2 = { "sk_boss_kick2", "0" };
cvar_t sk_boss_kick3 = { "sk_boss_kick3", "0" };

cvar_t sk_boss_pellets1 = { "sk_boss_pellets1", "0" };
cvar_t sk_boss_pellets2 = { "sk_boss_pellets2", "0" };
cvar_t sk_boss_pellets3 = { "sk_boss_pellets3", "0" };

cvar_t sk_boss_gspeed1 = { "sk_boss_gspeed1", "0" };
cvar_t sk_boss_gspeed2 = { "sk_boss_gspeed2", "0" };
cvar_t sk_boss_gspeed3 = { "sk_boss_gspeed3", "0" };

// Alpha?
cvar_t sk_alpha_health1 = { "sk_alpha_health1", "0" };
cvar_t sk_alpha_health2 = { "sk_alpha_health2", "0" };
cvar_t sk_alpha_health3 = { "sk_alpha_health3", "0" };

cvar_t sk_alpha_kick1 = { "sk_alpha_kick1", "0" };
cvar_t sk_alpha_kick2 = { "sk_alpha_kick2", "0" };
cvar_t sk_alpha_kick3 = { "sk_alpha_kick3", "0" };

cvar_t sk_alpha_pellets1 = { "sk_alpha_pellets1", "0" };
cvar_t sk_alpha_pellets2 = { "sk_alpha_pellets2", "0" };
cvar_t sk_alpha_pellets3 = { "sk_alpha_pellets3", "0" };

cvar_t sk_alpha_gspeed1 = { "sk_alpha_gspeed1", "0" };
cvar_t sk_alpha_gspeed2 = { "sk_alpha_gspeed2", "0" };
cvar_t sk_alpha_gspeed3 = { "sk_alpha_gspeed3", "0" };

// Spforce
cvar_t sk_spforce_health1 = { "sk_spforce_health1", "0" };
cvar_t sk_spforce_health2 = { "sk_spforce_health2", "0" };
cvar_t sk_spforce_health3 = { "sk_spforce_health3", "0" };

cvar_t sk_spforce_kick1 = { "sk_spforce_kick1", "0" };
cvar_t sk_spforce_kick2 = { "sk_spforce_kick2", "0" };
cvar_t sk_spforce_kick3 = { "sk_spforce_kick3", "0" };

cvar_t sk_spforce_pellets1 = { "sk_spforce_pellets1", "0" };
cvar_t sk_spforce_pellets2 = { "sk_spforce_pellets2", "0" };
cvar_t sk_spforce_pellets3 = { "sk_spforce_pellets3", "0" };

cvar_t sk_spforce_gspeed1 = { "sk_spforce_gspeed1", "0" };
cvar_t sk_spforce_gspeed2 = { "sk_spforce_gspeed2", "0" };
cvar_t sk_spforce_gspeed3 = { "sk_spforce_gspeed3", "0" };

// Agent
cvar_t sk_agent_health1 = { "sk_agent_health1", "0" };
cvar_t sk_agent_health2 = { "sk_agent_health2", "0" };
cvar_t sk_agent_health3 = { "sk_agent_health3", "0" };

cvar_t sk_agent_kick1 = { "sk_agent_kick1", "0" };
cvar_t sk_agent_kick2 = { "sk_agent_kick2", "0" };
cvar_t sk_agent_kick3 = { "sk_agent_kick3", "0" };

cvar_t sk_agent_pellets1 = { "sk_agent_pellets1", "0" };
cvar_t sk_agent_pellets2 = { "sk_agent_pellets2", "0" };
cvar_t sk_agent_pellets3 = { "sk_agent_pellets3", "0" };

cvar_t sk_agent_gspeed1 = { "sk_agent_gspeed1", "0" };
cvar_t sk_agent_gspeed2 = { "sk_agent_gspeed2", "0" };
cvar_t sk_agent_gspeed3 = { "sk_agent_gspeed3", "0" };

// Robot?
cvar_t sk_robot_health1 = { "sk_robot_health1", "0" };
cvar_t sk_robot_health2 = { "sk_robot_health2", "0" };
cvar_t sk_robot_health3 = { "sk_robot_health3", "0" };

cvar_t sk_robot_kick1 = { "sk_robot_kick1", "0" };
cvar_t sk_robot_kick2 = { "sk_robot_kick2", "0" };
cvar_t sk_robot_kick3 = { "sk_robot_kick3", "0" };

cvar_t sk_robot_pellets1 = { "sk_robot_pellets1", "0" };
cvar_t sk_robot_pellets2 = { "sk_robot_pellets2", "0" };
cvar_t sk_robot_pellets3 = { "sk_robot_pellets3", "0" };

cvar_t sk_robot_gspeed1 = { "sk_robot_gspeed1", "0" };
cvar_t sk_robot_gspeed2 = { "sk_robot_gspeed2", "0" };
cvar_t sk_robot_gspeed3 = { "sk_robot_gspeed3", "0" };

// Massn
cvar_t sk_massn_health1 = { "sk_massn_health1", "0" };
cvar_t sk_massn_health2 = { "sk_massn_health2", "0" };
cvar_t sk_massn_health3 = { "sk_massn_health3", "0" };

cvar_t sk_massn_kick1 = { "sk_massn_kick1", "0" };
cvar_t sk_massn_kick2 = { "sk_massn_kick2", "0" };
cvar_t sk_massn_kick3 = { "sk_massn_kick3", "0" };

cvar_t sk_massn_pellets1 = { "sk_massn_pellets1", "0" };
cvar_t sk_massn_pellets2 = { "sk_massn_pellets2", "0" };
cvar_t sk_massn_pellets3 = { "sk_massn_pellets3", "0" };

cvar_t sk_massn_gspeed1 = { "sk_massn_gspeed1", "0" };
cvar_t sk_massn_gspeed2 = { "sk_massn_gspeed2", "0" };
cvar_t sk_massn_gspeed3 = { "sk_massn_gspeed3", "0" };

// Houndeye
cvar_t	sk_houndeye_health1 = {"sk_houndeye_health1","0"};
cvar_t	sk_houndeye_health2 = {"sk_houndeye_health2","0"};
cvar_t	sk_houndeye_health3 = {"sk_houndeye_health3","0"};

cvar_t	sk_houndeye_dmg_blast1 = {"sk_houndeye_dmg_blast1","0"};
cvar_t	sk_houndeye_dmg_blast2 = {"sk_houndeye_dmg_blast2","0"};
cvar_t	sk_houndeye_dmg_blast3 = {"sk_houndeye_dmg_blast3","0"};


// ISlave
cvar_t	sk_islave_health1 = {"sk_islave_health1","0"};
cvar_t	sk_islave_health2 = {"sk_islave_health2","0"};
cvar_t	sk_islave_health3 = {"sk_islave_health3","0"};

cvar_t	sk_islave_dmg_claw1 = {"sk_islave_dmg_claw1","0"};
cvar_t	sk_islave_dmg_claw2 = {"sk_islave_dmg_claw2","0"};
cvar_t	sk_islave_dmg_claw3 = {"sk_islave_dmg_claw3","0"};

cvar_t	sk_islave_dmg_clawrake1	= {"sk_islave_dmg_clawrake1","0"};
cvar_t	sk_islave_dmg_clawrake2	= {"sk_islave_dmg_clawrake2","0"};
cvar_t	sk_islave_dmg_clawrake3	= {"sk_islave_dmg_clawrake3","0"};
	
cvar_t	sk_islave_dmg_zap1 = {"sk_islave_dmg_zap1","0"};
cvar_t	sk_islave_dmg_zap2 = {"sk_islave_dmg_zap2","0"};
cvar_t	sk_islave_dmg_zap3 = {"sk_islave_dmg_zap3","0"};

// ISlave2
cvar_t sk_islave2_health1 = { "sk_islave2_health1", "0" };
cvar_t sk_islave2_health2 = { "sk_islave2_health2", "0" };
cvar_t sk_islave2_health3 = { "sk_islave2_health3", "0" };

cvar_t sk_islave2_dmg_claw1 = { "sk_islave2_dmg_claw1", "0" };
cvar_t sk_islave2_dmg_claw2 = { "sk_islave2_dmg_claw2", "0" };
cvar_t sk_islave2_dmg_claw3 = { "sk_islave2_dmg_claw3", "0" };

cvar_t sk_islave2_dmg_clawrake1 = { "sk_islave2_dmg_clawrake1", "0" };
cvar_t sk_islave2_dmg_clawrake2 = { "sk_islave2_dmg_clawrake2", "0" };
cvar_t sk_islave2_dmg_clawrake3 = { "sk_islave2_dmg_clawrake3", "0" };

cvar_t sk_islave2_dmg_zap1 = { "sk_islave2_dmg_zap1", "0" };
cvar_t sk_islave2_dmg_zap2 = { "sk_islave2_dmg_zap2", "0" };
cvar_t sk_islave2_dmg_zap3 = { "sk_islave2_dmg_zap3", "0" };

// Icthyosaur
cvar_t	sk_ichthyosaur_health1	= {"sk_ichthyosaur_health1","0"};
cvar_t	sk_ichthyosaur_health2	= {"sk_ichthyosaur_health2","0"};
cvar_t	sk_ichthyosaur_health3	= {"sk_ichthyosaur_health3","0"};

cvar_t	sk_ichthyosaur_shake1	= {"sk_ichthyosaur_shake1","0"};
cvar_t	sk_ichthyosaur_shake2	= {"sk_ichthyosaur_shake2","0"};
cvar_t	sk_ichthyosaur_shake3	= {"sk_ichthyosaur_shake3","0"};


// Leech
cvar_t	sk_leech_health1 = {"sk_leech_health1","0"};
cvar_t	sk_leech_health2 = {"sk_leech_health2","0"};
cvar_t	sk_leech_health3 = {"sk_leech_health3","0"};

cvar_t	sk_leech_dmg_bite1 = {"sk_leech_dmg_bite1","0"};
cvar_t	sk_leech_dmg_bite2 = {"sk_leech_dmg_bite2","0"};
cvar_t	sk_leech_dmg_bite3 = {"sk_leech_dmg_bite3","0"};

// Controller
cvar_t	sk_controller_health1 = {"sk_controller_health1","0"};
cvar_t	sk_controller_health2 = {"sk_controller_health2","0"};
cvar_t	sk_controller_health3 = {"sk_controller_health3","0"};

cvar_t	sk_controller_dmgzap1 = {"sk_controller_dmgzap1","0"};
cvar_t	sk_controller_dmgzap2 = {"sk_controller_dmgzap2","0"};
cvar_t	sk_controller_dmgzap3 = {"sk_controller_dmgzap3","0"};

cvar_t	sk_controller_speedball1 = {"sk_controller_speedball1","0"};
cvar_t	sk_controller_speedball2 = {"sk_controller_speedball2","0"};
cvar_t	sk_controller_speedball3 = {"sk_controller_speedball3","0"};

cvar_t	sk_controller_dmgball1 = {"sk_controller_dmgball1","0"};
cvar_t	sk_controller_dmgball2 = {"sk_controller_dmgball2","0"};
cvar_t	sk_controller_dmgball3 = {"sk_controller_dmgball3","0"};

// Nihilanth
cvar_t	sk_nihilanth_health1 = {"sk_nihilanth_health1","0"};
cvar_t	sk_nihilanth_health2 = {"sk_nihilanth_health2","0"};
cvar_t	sk_nihilanth_health3 = {"sk_nihilanth_health3","0"};

cvar_t	sk_nihilanth_zap1 = {"sk_nihilanth_zap1","0"};
cvar_t	sk_nihilanth_zap2 = {"sk_nihilanth_zap2","0"};
cvar_t	sk_nihilanth_zap3 = {"sk_nihilanth_zap3","0"};

// Scientist
cvar_t	sk_scientist_health1 = {"sk_scientist_health1","0"};
cvar_t	sk_scientist_health2 = {"sk_scientist_health2","0"};
cvar_t	sk_scientist_health3 = {"sk_scientist_health3","0"};

// Snark
cvar_t	sk_snark_health1 = {"sk_snark_health1","0"};
cvar_t	sk_snark_health2 = {"sk_snark_health2","0"};
cvar_t	sk_snark_health3 = {"sk_snark_health3","0"};

cvar_t	sk_snark_dmg_bite1 = {"sk_snark_dmg_bite1","0"};
cvar_t	sk_snark_dmg_bite2 = {"sk_snark_dmg_bite2","0"};
cvar_t	sk_snark_dmg_bite3 = {"sk_snark_dmg_bite3","0"};

cvar_t	sk_snark_dmg_pop1 = {"sk_snark_dmg_pop1","0"};
cvar_t	sk_snark_dmg_pop2 = {"sk_snark_dmg_pop2","0"};
cvar_t	sk_snark_dmg_pop3 = {"sk_snark_dmg_pop3","0"};

// Zombie
cvar_t	sk_zombie_health1 = {"sk_zombie_health1","0"};
cvar_t	sk_zombie_health2 = {"sk_zombie_health2","0"};
cvar_t	sk_zombie_health3 = {"sk_zombie_health3","0"};

cvar_t	sk_zombie_dmg_one_slash1 = {"sk_zombie_dmg_one_slash1","0"};
cvar_t	sk_zombie_dmg_one_slash2 = {"sk_zombie_dmg_one_slash2","0"};
cvar_t	sk_zombie_dmg_one_slash3 = {"sk_zombie_dmg_one_slash3","0"};

cvar_t	sk_zombie_dmg_both_slash1 = {"sk_zombie_dmg_both_slash1","0"};
cvar_t	sk_zombie_dmg_both_slash2 = {"sk_zombie_dmg_both_slash2","0"};
cvar_t	sk_zombie_dmg_both_slash3 = {"sk_zombie_dmg_both_slash3","0"};

//Turret
cvar_t	sk_turret_health1 = {"sk_turret_health1","0"};
cvar_t	sk_turret_health2 = {"sk_turret_health2","0"};
cvar_t	sk_turret_health3 = {"sk_turret_health3","0"};

// MiniTurret
cvar_t	sk_miniturret_health1 = {"sk_miniturret_health1","0"};
cvar_t	sk_miniturret_health2 = {"sk_miniturret_health2","0"};
cvar_t	sk_miniturret_health3 = {"sk_miniturret_health3","0"};

// Sentry Turret
cvar_t	sk_sentry_health1 = {"sk_sentry_health1","0"};
cvar_t	sk_sentry_health2 = {"sk_sentry_health2","0"};
cvar_t	sk_sentry_health3 = {"sk_sentry_health3","0"};

// PLAYER WEAPONS

// Crowbar whack
cvar_t	sk_plr_crowbar1 = {"sk_plr_crowbar1","0"};
cvar_t	sk_plr_crowbar2 = {"sk_plr_crowbar2","0"};
cvar_t	sk_plr_crowbar3 = {"sk_plr_crowbar3","0"};

cvar_t sk_plr_knife1 = { "sk_plr_knife1", "0" };
cvar_t sk_plr_knife2 = { "sk_plr_knife2", "0" };
cvar_t sk_plr_knife3 = { "sk_plr_knife3", "0" };

cvar_t sk_plr_pipe1 = { "sk_plr_pipe1", "0" };
cvar_t sk_plr_pipe2 = { "sk_plr_pipe2", "0" };
cvar_t sk_plr_pipe3 = { "sk_plr_pipe3", "0" };

cvar_t sk_plr_nuclear1 = { "sk_plr_nuclear1", "0" };
cvar_t sk_plr_nuclear2 = { "sk_plr_nuclear2", "0" };
cvar_t sk_plr_nuclear3 = { "sk_plr_nuclear3", "0" };

cvar_t sk_otis_health1 = { "sk_otis_health1", "0" };
cvar_t sk_otis_health2 = { "sk_otis_health2", "0" };
cvar_t sk_otis_health3 = { "sk_otis_health3", "0" };

// Glock Round
cvar_t	sk_plr_9mm_bullet1 = {"sk_plr_9mm_bullet1","0"};
cvar_t	sk_plr_9mm_bullet2 = {"sk_plr_9mm_bullet2","0"};
cvar_t	sk_plr_9mm_bullet3 = {"sk_plr_9mm_bullet3","0"};

cvar_t sk_plr_deagle1 = { "sk_plr_deagle1", "0" };
cvar_t sk_plr_deagle2 = { "sk_plr_deagle2", "0" };
cvar_t sk_plr_deagle3 = { "sk_plr_deagle3", "0" };

cvar_t sk_plr_m411 = { "sk_plr_m411", "0" };
cvar_t sk_plr_m412 = { "sk_plr_m412", "0" };
cvar_t sk_plr_m413 = { "sk_plr_m413", "0" };

cvar_t sk_plr_p2261 = { "sk_plr_p2261", "0" };
cvar_t sk_plr_p2262 = { "sk_plr_p2262", "0" };
cvar_t sk_plr_p2263 = { "sk_plr_p2263", "0" };

cvar_t sk_plr_ksg121 = { "sk_plr_ksg121", "0" };
cvar_t sk_plr_ksg122 = { "sk_plr_ksg122", "0" };
cvar_t sk_plr_ksg123 = { "sk_plr_ksg123", "0" };

// Flamethrower
cvar_t sk_plr_flamethrower1 = { "sk_plr_flamethrower1", "0" };
cvar_t sk_plr_flamethrower2 = { "sk_plr_flamethrower2", "0" };
cvar_t sk_plr_flamethrower3 = { "sk_plr_flamethrower3", "0" };

cvar_t sk_plr_beretta1 = { "sk_plr_beretta1", "0" };
cvar_t sk_plr_beretta2 = { "sk_plr_beretta2", "0" };
cvar_t sk_plr_beretta3 = { "sk_plr_beretta3", "0" };

// Db
cvar_t sk_plr_db1 = { "sk_plr_db1", "0" };
cvar_t sk_plr_db2 = { "sk_plr_db2", "0" };
cvar_t sk_plr_db3 = { "sk_plr_db3", "0" };

cvar_t sk_plr_m10141 = { "sk_plr_m10141", "0" };
cvar_t sk_plr_m10142 = { "sk_plr_m10142", "0" };
cvar_t sk_plr_m10143 = { "sk_plr_m10143", "0" };

cvar_t sk_plr_fnfal1 = { "sk_plr_fnfal1", "0" };
cvar_t sk_plr_fnfal2 = { "sk_plr_fnfal2", "0" };
cvar_t sk_plr_fnfal3 = { "sk_plr_fnfal3", "0" };

cvar_t sk_plr_mp71 = { "sk_plr_mp71", "0" };
cvar_t sk_plr_mp72 = { "sk_plr_mp72", "0" };
cvar_t sk_plr_mp73 = { "sk_plr_mp73", "0" };

// 357 Round
cvar_t	sk_plr_357_bullet1 = {"sk_plr_357_bullet1","0"};
cvar_t	sk_plr_357_bullet2 = {"sk_plr_357_bullet2","0"};
cvar_t	sk_plr_357_bullet3 = {"sk_plr_357_bullet3","0"};

// MP5 Round
cvar_t	sk_plr_9mmAR_bullet1 = {"sk_plr_9mmAR_bullet1","0"};
cvar_t	sk_plr_9mmAR_bullet2 = {"sk_plr_9mmAR_bullet2","0"};
cvar_t	sk_plr_9mmAR_bullet3 = {"sk_plr_9mmAR_bullet3","0"};

// AK47
cvar_t sk_plr_ak471 = { "sk_plr_ak471", "0" };
cvar_t sk_plr_ak472 = { "sk_plr_ak472", "0" };
cvar_t sk_plr_ak473 = { "sk_plr_ak473", "0" };

cvar_t sk_plr_p9041 = { "sk_plr_p9041", "0" };
cvar_t sk_plr_p9042 = { "sk_plr_p9042", "0" };
cvar_t sk_plr_p9043 = { "sk_plr_p9043", "0" };

cvar_t sk_plr_colt451 = { "sk_plr_colt451", "0" };
cvar_t sk_plr_colt452 = { "sk_plr_colt452", "0" };
cvar_t sk_plr_colt453 = { "sk_plr_colt453", "0" };

cvar_t sk_plr_autosniper1 = { "sk_plr_autosniper1", "0" };
cvar_t sk_plr_autosniper2 = { "sk_plr_autosniper2", "0" };
cvar_t sk_plr_autosniper3 = { "sk_plr_autosniper3", "0" };

cvar_t sk_plr_d501 = { "sk_plr_d501", "0" };
cvar_t sk_plr_d502 = { "sk_plr_d502", "0" };
cvar_t sk_plr_d503 = { "sk_plr_d503", "0" };

cvar_t sk_plr_chaingun1 = { "sk_plr_chaingun1", "0" };
cvar_t sk_plr_chaingun2 = { "sk_plr_chaingun2", "0" };
cvar_t sk_plr_chaingun3 = { "sk_plr_chaingun3", "0" };

// MP5A3
cvar_t sk_plr_mp5a31 = { "sk_plr_mp5a31", "0" };
cvar_t sk_plr_mp5a32 = { "sk_plr_mp5a32", "0" };
cvar_t sk_plr_mp5a33 = { "sk_plr_mp5a33", "0" };

// MP5A3
cvar_t sk_plr_piece1 = { "sk_plr_piece1", "0" };
cvar_t sk_plr_piece2 = { "sk_plr_piece2", "0" };
cvar_t sk_plr_piece3 = { "sk_plr_piece3", "0" };

// M203 grenade
cvar_t	sk_plr_9mmAR_grenade1 = {"sk_plr_9mmAR_grenade1","0"};
cvar_t	sk_plr_9mmAR_grenade2 = {"sk_plr_9mmAR_grenade2","0"};
cvar_t	sk_plr_9mmAR_grenade3 = {"sk_plr_9mmAR_grenade3","0"};


// Shotgun buckshot
cvar_t	sk_plr_buckshot1 = {"sk_plr_buckshot1","0"};
cvar_t	sk_plr_buckshot2 = {"sk_plr_buckshot2","0"};
cvar_t	sk_plr_buckshot3 = {"sk_plr_buckshot3","0"};


// Crossbow
cvar_t	sk_plr_xbow_bolt_client1 = {"sk_plr_xbow_bolt_client1","0"};
cvar_t	sk_plr_xbow_bolt_client2 = {"sk_plr_xbow_bolt_client2","0"};
cvar_t	sk_plr_xbow_bolt_client3 = {"sk_plr_xbow_bolt_client3","0"};

cvar_t	sk_plr_xbow_bolt_monster1 = {"sk_plr_xbow_bolt_monster1","0"};
cvar_t	sk_plr_xbow_bolt_monster2 = {"sk_plr_xbow_bolt_monster2","0"};
cvar_t	sk_plr_xbow_bolt_monster3 = {"sk_plr_xbow_bolt_monster3","0"};


// RPG
cvar_t	sk_plr_rpg1 = {"sk_plr_rpg1","0"};
cvar_t	sk_plr_rpg2 = {"sk_plr_rpg2","0"};
cvar_t	sk_plr_rpg3 = {"sk_plr_rpg3","0"};


// Zero Point Generator
cvar_t	sk_plr_gauss1 = {"sk_plr_gauss1","0"};
cvar_t	sk_plr_gauss2 = {"sk_plr_gauss2","0"};
cvar_t	sk_plr_gauss3 = {"sk_plr_gauss3","0"};


// Tau Cannon
cvar_t	sk_plr_egon_narrow1 = {"sk_plr_egon_narrow1","0"};
cvar_t	sk_plr_egon_narrow2 = {"sk_plr_egon_narrow2","0"};
cvar_t	sk_plr_egon_narrow3 = {"sk_plr_egon_narrow3","0"};

cvar_t	sk_plr_egon_wide1 = {"sk_plr_egon_wide1","0"};
cvar_t	sk_plr_egon_wide2 = {"sk_plr_egon_wide2","0"};
cvar_t	sk_plr_egon_wide3 = {"sk_plr_egon_wide3","0"};


// Hand Grendade
cvar_t	sk_plr_hand_grenade1 = {"sk_plr_hand_grenade1","0"};
cvar_t	sk_plr_hand_grenade2 = {"sk_plr_hand_grenade2","0"};
cvar_t	sk_plr_hand_grenade3 = {"sk_plr_hand_grenade3","0"};


// Satchel Charge
cvar_t	sk_plr_satchel1	= {"sk_plr_satchel1","0"};
cvar_t	sk_plr_satchel2	= {"sk_plr_satchel2","0"};
cvar_t	sk_plr_satchel3	= {"sk_plr_satchel3","0"};


// Tripmine
cvar_t	sk_plr_tripmine1 = {"sk_plr_tripmine1","0"};
cvar_t	sk_plr_tripmine2 = {"sk_plr_tripmine2","0"};
cvar_t	sk_plr_tripmine3 = {"sk_plr_tripmine3","0"};


// WORLD WEAPONS
cvar_t	sk_12mm_bullet1 = {"sk_12mm_bullet1","0"};
cvar_t	sk_12mm_bullet2 = {"sk_12mm_bullet2","0"};
cvar_t	sk_12mm_bullet3 = {"sk_12mm_bullet3","0"};

cvar_t	sk_9mmAR_bullet1 = {"sk_9mmAR_bullet1","0"};
cvar_t	sk_9mmAR_bullet2 = {"sk_9mmAR_bullet2","0"};
cvar_t	sk_9mmAR_bullet3 = {"sk_9mmAR_bullet3","0"};

cvar_t	sk_9mm_bullet1 = {"sk_9mm_bullet1","0"};
cvar_t	sk_9mm_bullet2 = {"sk_9mm_bullet2","0"};
cvar_t	sk_9mm_bullet3 = {"sk_9mm_bullet3","0"};

cvar_t sk_m41_bullet1 = { "sk_m41_bullet1", "0" };
cvar_t sk_m41_bullet2 = { "sk_m41_bullet2", "0" };
cvar_t sk_m41_bullet3 = { "sk_m41_bullet3", "0" };

cvar_t sk_p904_bullet1 = { "sk_p904_bullet1", "0" };
cvar_t sk_p904_bullet2 = { "sk_p904_bullet2", "0" };
cvar_t sk_p904_bullet3 = { "sk_p904_bullet3", "0" };

cvar_t sk_akm_bullet1 = { "sk_akm_bullet1", "0" };
cvar_t sk_akm_bullet2 = { "sk_akm_bullet2", "0" };
cvar_t sk_akm_bullet3 = { "sk_akm_bullet3", "0" };

cvar_t sk_as_bullet1 = { "sk_as_bullet1", "0" };
cvar_t sk_as_bullet2 = { "sk_as_bullet2", "0" };
cvar_t sk_as_bullet3 = { "sk_as_bullet3", "0" };

cvar_t sk_357_bullet1 = { "sk_357_bullet1", "0" };
cvar_t sk_357_bullet2 = { "sk_357_bullet2", "0" };
cvar_t sk_357_bullet3 = { "sk_357_bullet3", "0" };

cvar_t sk_d50_bullet1 = { "sk_d50_bullet1", "0" };
cvar_t sk_d50_bullet2 = { "sk_d50_bullet2", "0" };
cvar_t sk_d50_bullet3 = { "sk_d50_bullet3", "0" };

cvar_t sk_23_bullet1 = { "sk_23_bullet1", "0" };
cvar_t sk_23_bullet2 = { "sk_23_bullet2", "0" };
cvar_t sk_23_bullet3 = { "sk_23_bullet3", "0" };

cvar_t sk_fnfal_bullet1 = { "sk_fnfal_bullet1", "0" };
cvar_t sk_fnfal_bullet2 = { "sk_fnfal_bullet2", "0" };
cvar_t sk_fnfal_bullet3 = { "sk_fnfal_bullet3", "0" };

cvar_t sk_cg_bullet1 = { "sk_cg_bullet1", "0" };
cvar_t sk_cg_bullet2 = { "sk_cg_bullet2", "0" };
cvar_t sk_cg_bullet3 = { "sk_cg_bullet3", "0" };

// HORNET
cvar_t	sk_hornet_dmg1 = {"sk_hornet_dmg1","0"};
cvar_t	sk_hornet_dmg2 = {"sk_hornet_dmg2","0"};
cvar_t	sk_hornet_dmg3 = {"sk_hornet_dmg3","0"};

// HEALTH/CHARGE
cvar_t	sk_suitcharger1	= { "sk_suitcharger1","0" };
cvar_t	sk_suitcharger2	= { "sk_suitcharger2","0" };		
cvar_t	sk_suitcharger3	= { "sk_suitcharger3","0" };		

cvar_t	sk_battery1	= { "sk_battery1","0" };			
cvar_t	sk_battery2	= { "sk_battery2","0" };			
cvar_t	sk_battery3	= { "sk_battery3","0" };			

cvar_t	sk_healthcharger1	= { "sk_healthcharger1","0" };		
cvar_t	sk_healthcharger2	= { "sk_healthcharger2","0" };		
cvar_t	sk_healthcharger3	= { "sk_healthcharger3","0" };		

cvar_t	sk_healthkit1	= { "sk_healthkit1","0" };		
cvar_t	sk_healthkit2	= { "sk_healthkit2","0" };		
cvar_t	sk_healthkit3	= { "sk_healthkit3","0" };		

cvar_t	sk_scientist_heal1	= { "sk_scientist_heal1","0" };	
cvar_t	sk_scientist_heal2	= { "sk_scientist_heal2","0" };	
cvar_t	sk_scientist_heal3	= { "sk_scientist_heal3","0" };	


// monster damage adjusters
cvar_t	sk_monster_head1	= { "sk_monster_head1","2" };
cvar_t	sk_monster_head2	= { "sk_monster_head2","2" };
cvar_t	sk_monster_head3	= { "sk_monster_head3","2" };

cvar_t	sk_monster_chest1	= { "sk_monster_chest1","1" };
cvar_t	sk_monster_chest2	= { "sk_monster_chest2","1" };
cvar_t	sk_monster_chest3	= { "sk_monster_chest3","1" };

cvar_t	sk_monster_stomach1	= { "sk_monster_stomach1","1" };
cvar_t	sk_monster_stomach2	= { "sk_monster_stomach2","1" };
cvar_t	sk_monster_stomach3	= { "sk_monster_stomach3","1" };

cvar_t	sk_monster_arm1	= { "sk_monster_arm1","1" };
cvar_t	sk_monster_arm2	= { "sk_monster_arm2","1" };
cvar_t	sk_monster_arm3	= { "sk_monster_arm3","1" };

cvar_t	sk_monster_leg1	= { "sk_monster_leg1","1" };
cvar_t	sk_monster_leg2	= { "sk_monster_leg2","1" };
cvar_t	sk_monster_leg3	= { "sk_monster_leg3","1" };

// player damage adjusters
cvar_t	sk_player_head1	= { "sk_player_head1","2" };
cvar_t	sk_player_head2	= { "sk_player_head2","2" };
cvar_t	sk_player_head3	= { "sk_player_head3","2" };

cvar_t	sk_player_chest1 = { "sk_player_chest1","1" };
cvar_t	sk_player_chest2 = { "sk_player_chest2","1" };
cvar_t	sk_player_chest3 = { "sk_player_chest3","1" };

cvar_t	sk_player_stomach1 = { "sk_player_stomach1","1" };
cvar_t	sk_player_stomach2 = { "sk_player_stomach2","1" };
cvar_t	sk_player_stomach3 = { "sk_player_stomach3","1" };

cvar_t	sk_player_arm1	= { "sk_player_arm1","1" };
cvar_t	sk_player_arm2	= { "sk_player_arm2","1" };
cvar_t	sk_player_arm3	= { "sk_player_arm3","1" };

cvar_t	sk_player_leg1	= { "sk_player_leg1","1" };
cvar_t	sk_player_leg2	= { "sk_player_leg2","1" };
cvar_t	sk_player_leg3	= { "sk_player_leg3","1" };

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// Register cvars here:
	if( CVAR_GET_POINTER( "build" ) )
		g_fIsXash3D = TRUE;

	g_psv_gravity = CVAR_GET_POINTER( "sv_gravity" );
	g_psv_aim = CVAR_GET_POINTER( "sv_aim" );
	g_footsteps = CVAR_GET_POINTER( "mp_footsteps" );

	g_psv_developer = CVAR_GET_POINTER( "developer" );

	g_enable_cheats = CVAR_GET_POINTER( "sv_cheats" );

	CVAR_REGISTER( &displaysoundlist );
	CVAR_REGISTER( &allow_spectators );

	CVAR_REGISTER( &teamplay );
	CVAR_REGISTER( &fraglimit );
	CVAR_REGISTER( &timelimit );

	CVAR_REGISTER( &fragsleft );
	CVAR_REGISTER( &timeleft );

	CVAR_REGISTER( &friendlyfire );
	CVAR_REGISTER( &falldamage );
	CVAR_REGISTER( &weaponstay );
	CVAR_REGISTER( &selfgauss );
	CVAR_REGISTER( &chargerfix );
	CVAR_REGISTER( &satchelfix );
	CVAR_REGISTER( &monsteryawspeedfix );
	CVAR_REGISTER( &forcerespawn );
	CVAR_REGISTER( &flashlight );
	CVAR_REGISTER( &aimcrosshair );
	CVAR_REGISTER( &decalfrequency );
	CVAR_REGISTER( &teamlist );
	CVAR_REGISTER( &teamoverride );
	CVAR_REGISTER( &defaultteam );
	CVAR_REGISTER( &allowmonsters );
	CVAR_REGISTER( &bhopcap );
	CVAR_REGISTER( &multibyte_only );

	CVAR_REGISTER( &mp_chattime );



// REGISTER CVARS FOR SKILL LEVEL STUFF
	// Agrunt
	CVAR_REGISTER( &sk_agrunt_health1 );// {"sk_agrunt_health1","0"};
	CVAR_REGISTER( &sk_agrunt_health2 );// {"sk_agrunt_health2","0"};
	CVAR_REGISTER( &sk_agrunt_health3 );// {"sk_agrunt_health3","0"};

	CVAR_REGISTER( &sk_agrunt_dmg_punch1 );// {"sk_agrunt_dmg_punch1","0"};
	CVAR_REGISTER( &sk_agrunt_dmg_punch2 );// {"sk_agrunt_dmg_punch2","0"};
	CVAR_REGISTER( &sk_agrunt_dmg_punch3 );// {"sk_agrunt_dmg_punch3","0"};

	// Apache
	CVAR_REGISTER( &sk_apache_health1 );// {"sk_apache_health1","0"};
	CVAR_REGISTER( &sk_apache_health2 );// {"sk_apache_health2","0"};
	CVAR_REGISTER( &sk_apache_health3 );// {"sk_apache_health3","0"};

	// Barney
	CVAR_REGISTER( &sk_barney_health1 );// {"sk_barney_health1","0"};
	CVAR_REGISTER( &sk_barney_health2 );// {"sk_barney_health2","0"};
	CVAR_REGISTER( &sk_barney_health3 );// {"sk_barney_health3","0"};

	// Anime?
	CVAR_REGISTER( &sk_anime_health1 ); // {"sk_barney_health1","0"};
	CVAR_REGISTER( &sk_anime_health2 ); // {"sk_barney_health2","0"};
	CVAR_REGISTER( &sk_anime_health3 ); // {"sk_barney_health3","0"};

	// Bullsquid
	CVAR_REGISTER( &sk_bullsquid_health1 );// {"sk_bullsquid_health1","0"};
	CVAR_REGISTER( &sk_bullsquid_health2 );// {"sk_bullsquid_health2","0"};
	CVAR_REGISTER( &sk_bullsquid_health3 );// {"sk_bullsquid_health3","0"};

	CVAR_REGISTER( &sk_bullsquid_dmg_bite1 );// {"sk_bullsquid_dmg_bite1","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_bite2 );// {"sk_bullsquid_dmg_bite2","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_bite3 );// {"sk_bullsquid_dmg_bite3","0"};

	CVAR_REGISTER( &sk_bullsquid_dmg_whip1 );// {"sk_bullsquid_dmg_whip1","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_whip2 );// {"sk_bullsquid_dmg_whip2","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_whip3 );// {"sk_bullsquid_dmg_whip3","0"};

	CVAR_REGISTER( &sk_bullsquid_dmg_spit1 );// {"sk_bullsquid_dmg_spit1","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_spit2 );// {"sk_bullsquid_dmg_spit2","0"};
	CVAR_REGISTER( &sk_bullsquid_dmg_spit3 );// {"sk_bullsquid_dmg_spit3","0"};

	CVAR_REGISTER( &sk_bigmomma_health_factor1 );// {"sk_bigmomma_health_factor1","1.0"};
	CVAR_REGISTER( &sk_bigmomma_health_factor2 );// {"sk_bigmomma_health_factor2","1.0"};
	CVAR_REGISTER( &sk_bigmomma_health_factor3 );// {"sk_bigmomma_health_factor3","1.0"};

	CVAR_REGISTER( &sk_bigmomma_dmg_slash1 );// {"sk_bigmomma_dmg_slash1","50"};
	CVAR_REGISTER( &sk_bigmomma_dmg_slash2 );// {"sk_bigmomma_dmg_slash2","50"};
	CVAR_REGISTER( &sk_bigmomma_dmg_slash3 );// {"sk_bigmomma_dmg_slash3","50"};

	CVAR_REGISTER( &sk_bigmomma_dmg_blast1 );// {"sk_bigmomma_dmg_blast1","100"};
	CVAR_REGISTER( &sk_bigmomma_dmg_blast2 );// {"sk_bigmomma_dmg_blast2","100"};
	CVAR_REGISTER( &sk_bigmomma_dmg_blast3 );// {"sk_bigmomma_dmg_blast3","100"};

	CVAR_REGISTER( &sk_bigmomma_radius_blast1 );// {"sk_bigmomma_radius_blast1","250"};
	CVAR_REGISTER( &sk_bigmomma_radius_blast2 );// {"sk_bigmomma_radius_blast2","250"};
	CVAR_REGISTER( &sk_bigmomma_radius_blast3 );// {"sk_bigmomma_radius_blast3","250"};

	// Gargantua
	CVAR_REGISTER( &sk_gargantua_health1 );// {"sk_gargantua_health1","0"};
	CVAR_REGISTER( &sk_gargantua_health2 );// {"sk_gargantua_health2","0"};
	CVAR_REGISTER( &sk_gargantua_health3 );// {"sk_gargantua_health3","0"};

	CVAR_REGISTER( &sk_gargantua_dmg_slash1 );// {"sk_gargantua_dmg_slash1","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_slash2 );// {"sk_gargantua_dmg_slash2","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_slash3 );// {"sk_gargantua_dmg_slash3","0"};

	CVAR_REGISTER( &sk_gargantua_dmg_fire1 );// {"sk_gargantua_dmg_fire1","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_fire2 );// {"sk_gargantua_dmg_fire2","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_fire3 );// {"sk_gargantua_dmg_fire3","0"};

	CVAR_REGISTER( &sk_gargantua_dmg_stomp1 );// {"sk_gargantua_dmg_stomp1","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_stomp2 );// {"sk_gargantua_dmg_stomp2","0"};
	CVAR_REGISTER( &sk_gargantua_dmg_stomp3	);// {"sk_gargantua_dmg_stomp3","0"};

	// Hassassin
	CVAR_REGISTER( &sk_hassassin_health1 );// {"sk_hassassin_health1","0"};
	CVAR_REGISTER( &sk_hassassin_health2 );// {"sk_hassassin_health2","0"};
	CVAR_REGISTER( &sk_hassassin_health3 );// {"sk_hassassin_health3","0"};

	// Anime?
	CVAR_REGISTER( &sk_super_health1 ); // {"sk_super_health1","0"};
	CVAR_REGISTER( &sk_super_health2 ); // {"sk_super_health2","0"};
	CVAR_REGISTER( &sk_super_health3 ); // {"sk_super_health3","0"};

	// Headcrab
	CVAR_REGISTER( &sk_headcrab_health1 );// {"sk_headcrab_health1","0"};
	CVAR_REGISTER( &sk_headcrab_health2 );// {"sk_headcrab_health2","0"};
	CVAR_REGISTER( &sk_headcrab_health3 );// {"sk_headcrab_health3","0"};

	CVAR_REGISTER( &sk_headcrab_dmg_bite1 );// {"sk_headcrab_dmg_bite1","0"};
	CVAR_REGISTER( &sk_headcrab_dmg_bite2 );// {"sk_headcrab_dmg_bite2","0"};
	CVAR_REGISTER( &sk_headcrab_dmg_bite3 );// {"sk_headcrab_dmg_bite3","0"};

	// Headcrab2
	CVAR_REGISTER( &sk_headcrab2_health1 ); // {"sk_headcrab_health1","0"};
	CVAR_REGISTER( &sk_headcrab2_health2 ); // {"sk_headcrab_health2","0"};
	CVAR_REGISTER( &sk_headcrab2_health3 ); // {"sk_headcrab_health3","0"};

	CVAR_REGISTER( &sk_headcrab2_dmg_bite1 ); // {"sk_headcrab_dmg_bite1","0"};
	CVAR_REGISTER( &sk_headcrab2_dmg_bite2 ); // {"sk_headcrab_dmg_bite2","0"};
	CVAR_REGISTER( &sk_headcrab2_dmg_bite3 ); // {"sk_headcrab_dmg_bite3","0"};

	// Hgrunt
	CVAR_REGISTER( &sk_hgrunt_health1 );// {"sk_hgrunt_health1","0"};
	CVAR_REGISTER( &sk_hgrunt_health2 );// {"sk_hgrunt_health2","0"};
	CVAR_REGISTER( &sk_hgrunt_health3 );// {"sk_hgrunt_health3","0"};

	CVAR_REGISTER( &sk_hgrunt_kick1 );// {"sk_hgrunt_kick1","0"};
	CVAR_REGISTER( &sk_hgrunt_kick2 );// {"sk_hgrunt_kick2","0"};
	CVAR_REGISTER( &sk_hgrunt_kick3 );// {"sk_hgrunt_kick3","0"};

	CVAR_REGISTER( &sk_hgrunt_pellets1 );
	CVAR_REGISTER( &sk_hgrunt_pellets2 );
	CVAR_REGISTER( &sk_hgrunt_pellets3 );

	CVAR_REGISTER( &sk_hgrunt_gspeed1 );
	CVAR_REGISTER( &sk_hgrunt_gspeed2 );
	CVAR_REGISTER( &sk_hgrunt_gspeed3 );

	// Space?
	CVAR_REGISTER( &sk_space_health1 ); // {"sk_space_health1","0"};
	CVAR_REGISTER( &sk_space_health2 ); // {"sk_space_health2","0"};
	CVAR_REGISTER( &sk_space_health3 ); // {"sk_space_health3","0"};

	CVAR_REGISTER( &sk_space_kick1 ); // {"sk_space_kick1","0"};
	CVAR_REGISTER( &sk_space_kick2 ); // {"sk_space_kick2","0"};
	CVAR_REGISTER( &sk_space_kick3 ); // {"sk_space_kick3","0"};

	CVAR_REGISTER( &sk_space_pellets1 );
	CVAR_REGISTER( &sk_space_pellets2 );
	CVAR_REGISTER( &sk_space_pellets3 );

	CVAR_REGISTER( &sk_space_gspeed1 );
	CVAR_REGISTER( &sk_space_gspeed2 );
	CVAR_REGISTER( &sk_space_gspeed3 );

	// Boss?
	CVAR_REGISTER( &sk_boss_health1 ); // {"sk_boss_health1","0"};
	CVAR_REGISTER( &sk_boss_health2 ); // {"sk_boss_health2","0"};
	CVAR_REGISTER( &sk_boss_health3 ); // {"sk_boss_health3","0"};

	CVAR_REGISTER( &sk_boss_kick1 ); // {"sk_boss_kick1","0"};
	CVAR_REGISTER( &sk_boss_kick2 ); // {"sk_boss_kick2","0"};
	CVAR_REGISTER( &sk_boss_kick3 ); // {"sk_boss_kick3","0"};

	CVAR_REGISTER( &sk_boss_pellets1 );
	CVAR_REGISTER( &sk_boss_pellets2 );
	CVAR_REGISTER( &sk_boss_pellets3 );

	CVAR_REGISTER( &sk_boss_gspeed1 );
	CVAR_REGISTER( &sk_boss_gspeed2 );
	CVAR_REGISTER( &sk_boss_gspeed3 );

	// Alpha?
	CVAR_REGISTER( &sk_alpha_health1 ); // {"sk_alpha_health1","0"};
	CVAR_REGISTER( &sk_alpha_health2 ); // {"sk_alpha_health2","0"};
	CVAR_REGISTER( &sk_alpha_health3 ); // {"sk_alpha_health3","0"};

	CVAR_REGISTER( &sk_alpha_kick1 ); // {"sk_alpha_kick1","0"};
	CVAR_REGISTER( &sk_alpha_kick2 ); // {"sk_alpha_kick2","0"};
	CVAR_REGISTER( &sk_alpha_kick3 ); // {"sk_alpha_kick3","0"};

	CVAR_REGISTER( &sk_alpha_pellets1 );
	CVAR_REGISTER( &sk_alpha_pellets2 );
	CVAR_REGISTER( &sk_alpha_pellets3 );

	CVAR_REGISTER( &sk_alpha_gspeed1 );
	CVAR_REGISTER( &sk_alpha_gspeed2 );
	CVAR_REGISTER( &sk_alpha_gspeed3 );

	// Spforce
	CVAR_REGISTER( &sk_spforce_health1 ); // {"sk_spforce_health1","0"};
	CVAR_REGISTER( &sk_spforce_health2 ); // {"sk_spforce_health2","0"};
	CVAR_REGISTER( &sk_spforce_health3 ); // {"sk_spforce_health3","0"};

	CVAR_REGISTER( &sk_spforce_kick1 ); // {"sk_spforce_kick1","0"};
	CVAR_REGISTER( &sk_spforce_kick2 ); // {"sk_spforce_kick2","0"};
	CVAR_REGISTER( &sk_spforce_kick3 ); // {"sk_spforce_kick3","0"};

	CVAR_REGISTER( &sk_spforce_pellets1 );
	CVAR_REGISTER( &sk_spforce_pellets2 );
	CVAR_REGISTER( &sk_spforce_pellets3 );

	CVAR_REGISTER( &sk_spforce_gspeed1 );
	CVAR_REGISTER( &sk_spforce_gspeed2 );
	CVAR_REGISTER( &sk_spforce_gspeed3 );

	// Agent
	CVAR_REGISTER( &sk_agent_health1 ); // {"sk_agent_health1","0"};
	CVAR_REGISTER( &sk_agent_health2 ); // {"sk_agent_health2","0"};
	CVAR_REGISTER( &sk_agent_health3 ); // {"sk_agent_health3","0"};

	CVAR_REGISTER( &sk_agent_kick1 ); // {"sk_agent_kick1","0"};
	CVAR_REGISTER( &sk_agent_kick2 ); // {"sk_agent_kick2","0"};
	CVAR_REGISTER( &sk_agent_kick3 ); // {"sk_agent_kick3","0"};

	CVAR_REGISTER( &sk_agent_pellets1 );
	CVAR_REGISTER( &sk_agent_pellets2 );
	CVAR_REGISTER( &sk_agent_pellets3 );

	CVAR_REGISTER( &sk_agent_gspeed1 );
	CVAR_REGISTER( &sk_agent_gspeed2 );
	CVAR_REGISTER( &sk_agent_gspeed3 );

	// Robot?
	CVAR_REGISTER( &sk_robot_health1 ); // {"sk_robot_health1","0"};
	CVAR_REGISTER( &sk_robot_health2 ); // {"sk_robot_health2","0"};
	CVAR_REGISTER( &sk_robot_health3 ); // {"sk_robot_health3","0"};

	CVAR_REGISTER( &sk_robot_kick1 ); // {"sk_robot_kick1","0"};
	CVAR_REGISTER( &sk_robot_kick2 ); // {"sk_robot_kick2","0"};
	CVAR_REGISTER( &sk_robot_kick3 ); // {"sk_robot_kick3","0"};

	CVAR_REGISTER( &sk_robot_pellets1 );
	CVAR_REGISTER( &sk_robot_pellets2 );
	CVAR_REGISTER( &sk_robot_pellets3 );

	CVAR_REGISTER( &sk_robot_gspeed1 );
	CVAR_REGISTER( &sk_robot_gspeed2 );
	CVAR_REGISTER( &sk_robot_gspeed3 );

	// Massn
	CVAR_REGISTER( &sk_massn_health1 ); // {"sk_massn_health1","0"};
	CVAR_REGISTER( &sk_massn_health2 ); // {"sk_massn_health2","0"};
	CVAR_REGISTER( &sk_massn_health3 ); // {"sk_massn_health3","0"};

	CVAR_REGISTER( &sk_massn_kick1 ); // {"sk_massn_kick1","0"};
	CVAR_REGISTER( &sk_massn_kick2 ); // {"sk_massn_kick2","0"};
	CVAR_REGISTER( &sk_massn_kick3 ); // {"sk_massn_kick3","0"};

	CVAR_REGISTER( &sk_massn_pellets1 );
	CVAR_REGISTER( &sk_massn_pellets2 );
	CVAR_REGISTER( &sk_massn_pellets3 );

	CVAR_REGISTER( &sk_massn_gspeed1 );
	CVAR_REGISTER( &sk_massn_gspeed2 );
	CVAR_REGISTER( &sk_massn_gspeed3 );

	// Houndeye
	CVAR_REGISTER( &sk_houndeye_health1 );// {"sk_houndeye_health1","0"};
	CVAR_REGISTER( &sk_houndeye_health2 );// {"sk_houndeye_health2","0"};
	CVAR_REGISTER( &sk_houndeye_health3 );// {"sk_houndeye_health3","0"};

	CVAR_REGISTER( &sk_houndeye_dmg_blast1 );// {"sk_houndeye_dmg_blast1","0"};
	CVAR_REGISTER( &sk_houndeye_dmg_blast2 );// {"sk_houndeye_dmg_blast2","0"};
	CVAR_REGISTER( &sk_houndeye_dmg_blast3 );// {"sk_houndeye_dmg_blast3","0"};

	// ISlave
	CVAR_REGISTER( &sk_islave_health1 );// {"sk_islave_health1","0"};
	CVAR_REGISTER( &sk_islave_health2 );// {"sk_islave_health2","0"};
	CVAR_REGISTER( &sk_islave_health3 );// {"sk_islave_health3","0"};

	CVAR_REGISTER( &sk_islave_dmg_claw1 );// {"sk_islave_dmg_claw1","0"};
	CVAR_REGISTER( &sk_islave_dmg_claw2 );// {"sk_islave_dmg_claw2","0"};
	CVAR_REGISTER( &sk_islave_dmg_claw3 );// {"sk_islave_dmg_claw3","0"};

	CVAR_REGISTER( &sk_islave_dmg_clawrake1 );// {"sk_islave_dmg_clawrake1","0"};
	CVAR_REGISTER( &sk_islave_dmg_clawrake2 );// {"sk_islave_dmg_clawrake2","0"};
	CVAR_REGISTER( &sk_islave_dmg_clawrake3 );// {"sk_islave_dmg_clawrake3","0"};

	CVAR_REGISTER( &sk_islave_dmg_zap1 );// {"sk_islave_dmg_zap1","0"};
	CVAR_REGISTER( &sk_islave_dmg_zap2 );// {"sk_islave_dmg_zap2","0"};
	CVAR_REGISTER( &sk_islave_dmg_zap3 );// {"sk_islave_dmg_zap3","0"};

	// ISlave2
	CVAR_REGISTER( &sk_islave2_health1 ); // {"sk_islave_health1","0"};
	CVAR_REGISTER( &sk_islave2_health2 ); // {"sk_islave_health2","0"};
	CVAR_REGISTER( &sk_islave2_health3 ); // {"sk_islave_health3","0"};

	CVAR_REGISTER( &sk_islave2_dmg_claw1 ); // {"sk_islave_dmg_claw1","0"};
	CVAR_REGISTER( &sk_islave2_dmg_claw2 ); // {"sk_islave_dmg_claw2","0"};
	CVAR_REGISTER( &sk_islave2_dmg_claw3 ); // {"sk_islave_dmg_claw3","0"};

	CVAR_REGISTER( &sk_islave2_dmg_clawrake1 ); // {"sk_islave_dmg_clawrake1","0"};
	CVAR_REGISTER( &sk_islave2_dmg_clawrake2 ); // {"sk_islave_dmg_clawrake2","0"};
	CVAR_REGISTER( &sk_islave2_dmg_clawrake3 ); // {"sk_islave_dmg_clawrake3","0"};

	CVAR_REGISTER( &sk_islave2_dmg_zap1 ); // {"sk_islave_dmg_zap1","0"};
	CVAR_REGISTER( &sk_islave2_dmg_zap2 ); // {"sk_islave_dmg_zap2","0"};
	CVAR_REGISTER( &sk_islave2_dmg_zap3 ); // {"sk_islave_dmg_zap3","0"};

	// Icthyosaur
	CVAR_REGISTER( &sk_ichthyosaur_health1 );// {"sk_ichthyosaur_health1","0"};
	CVAR_REGISTER( &sk_ichthyosaur_health2 );// {"sk_ichthyosaur_health2","0"};
	CVAR_REGISTER( &sk_ichthyosaur_health3 );// {"sk_ichthyosaur_health3","0"};

	CVAR_REGISTER( &sk_ichthyosaur_shake1 );// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER( &sk_ichthyosaur_shake2 );// {"sk_ichthyosaur_health3","0"};
	CVAR_REGISTER( &sk_ichthyosaur_shake3 );// {"sk_ichthyosaur_health3","0"};

	// Leech
	CVAR_REGISTER( &sk_leech_health1 );// {"sk_leech_health1","0"};
	CVAR_REGISTER( &sk_leech_health2 );// {"sk_leech_health2","0"};
	CVAR_REGISTER( &sk_leech_health3 );// {"sk_leech_health3","0"};

	CVAR_REGISTER( &sk_leech_dmg_bite1 );// {"sk_leech_dmg_bite1","0"};
	CVAR_REGISTER( &sk_leech_dmg_bite2 );// {"sk_leech_dmg_bite2","0"};
	CVAR_REGISTER( &sk_leech_dmg_bite3 );// {"sk_leech_dmg_bite3","0"};

	// Controller
	CVAR_REGISTER( &sk_controller_health1 );
	CVAR_REGISTER( &sk_controller_health2 );
	CVAR_REGISTER( &sk_controller_health3 );

	CVAR_REGISTER( &sk_controller_dmgzap1 );
	CVAR_REGISTER( &sk_controller_dmgzap2 );
	CVAR_REGISTER( &sk_controller_dmgzap3 );

	CVAR_REGISTER( &sk_controller_speedball1 );
	CVAR_REGISTER( &sk_controller_speedball2 );
	CVAR_REGISTER( &sk_controller_speedball3 );

	CVAR_REGISTER( &sk_controller_dmgball1 );
	CVAR_REGISTER( &sk_controller_dmgball2 );
	CVAR_REGISTER( &sk_controller_dmgball3 );

	// Nihilanth
	CVAR_REGISTER( &sk_nihilanth_health1 );// {"sk_nihilanth_health1","0"};
	CVAR_REGISTER( &sk_nihilanth_health2 );// {"sk_nihilanth_health2","0"};
	CVAR_REGISTER( &sk_nihilanth_health3 );// {"sk_nihilanth_health3","0"};

	CVAR_REGISTER( &sk_nihilanth_zap1 );
	CVAR_REGISTER( &sk_nihilanth_zap2 );
	CVAR_REGISTER( &sk_nihilanth_zap3 );

	// Scientist
	CVAR_REGISTER( &sk_scientist_health1 );// {"sk_scientist_health1","0"};
	CVAR_REGISTER( &sk_scientist_health2 );// {"sk_scientist_health2","0"};
	CVAR_REGISTER( &sk_scientist_health3 );// {"sk_scientist_health3","0"};

	// Snark
	CVAR_REGISTER( &sk_snark_health1 );// {"sk_snark_health1","0"};
	CVAR_REGISTER( &sk_snark_health2 );// {"sk_snark_health2","0"};
	CVAR_REGISTER( &sk_snark_health3 );// {"sk_snark_health3","0"};

	CVAR_REGISTER( &sk_snark_dmg_bite1 );// {"sk_snark_dmg_bite1","0"};
	CVAR_REGISTER( &sk_snark_dmg_bite2 );// {"sk_snark_dmg_bite2","0"};
	CVAR_REGISTER( &sk_snark_dmg_bite3 );// {"sk_snark_dmg_bite3","0"};

	CVAR_REGISTER( &sk_snark_dmg_pop1 );// {"sk_snark_dmg_pop1","0"};
	CVAR_REGISTER( &sk_snark_dmg_pop2 );// {"sk_snark_dmg_pop2","0"};
	CVAR_REGISTER( &sk_snark_dmg_pop3 );// {"sk_snark_dmg_pop3","0"};

	// Zombie
	CVAR_REGISTER( &sk_zombie_health1 );// {"sk_zombie_health1","0"};
	CVAR_REGISTER( &sk_zombie_health2 );// {"sk_zombie_health3","0"};
	CVAR_REGISTER( &sk_zombie_health3 );// {"sk_zombie_health3","0"};

	CVAR_REGISTER( &sk_zombie_dmg_one_slash1 );// {"sk_zombie_dmg_one_slash1","0"};
	CVAR_REGISTER( &sk_zombie_dmg_one_slash2 );// {"sk_zombie_dmg_one_slash2","0"};
	CVAR_REGISTER( &sk_zombie_dmg_one_slash3 );// {"sk_zombie_dmg_one_slash3","0"};

	CVAR_REGISTER( &sk_zombie_dmg_both_slash1 );// {"sk_zombie_dmg_both_slash1","0"};
	CVAR_REGISTER( &sk_zombie_dmg_both_slash2 );// {"sk_zombie_dmg_both_slash2","0"};
	CVAR_REGISTER( &sk_zombie_dmg_both_slash3 );// {"sk_zombie_dmg_both_slash3","0"};

	//Turret
	CVAR_REGISTER( &sk_turret_health1 );// {"sk_turret_health1","0"};
	CVAR_REGISTER( &sk_turret_health2 );// {"sk_turret_health2","0"};
	CVAR_REGISTER( &sk_turret_health3 );// {"sk_turret_health3","0"};

	// MiniTurret
	CVAR_REGISTER( &sk_miniturret_health1 );// {"sk_miniturret_health1","0"};
	CVAR_REGISTER( &sk_miniturret_health2 );// {"sk_miniturret_health2","0"};
	CVAR_REGISTER( &sk_miniturret_health3 );// {"sk_miniturret_health3","0"};

	// Sentry Turret
	CVAR_REGISTER( &sk_sentry_health1 );// {"sk_sentry_health1","0"};
	CVAR_REGISTER( &sk_sentry_health2 );// {"sk_sentry_health2","0"};
	CVAR_REGISTER( &sk_sentry_health3 );// {"sk_sentry_health3","0"};


	// PLAYER WEAPONS

	// Crowbar whack
	CVAR_REGISTER( &sk_plr_crowbar1 );// {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_crowbar2 );// {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_crowbar3 );// {"sk_plr_crowbar3","0"};

	// Knife
	CVAR_REGISTER( &sk_plr_knife1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_knife2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_knife3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_pipe1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_pipe2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_pipe3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_deagle1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_deagle2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_deagle3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_m411 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_m412 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_m413 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_p2261 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_p2262 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_p2263 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_piece1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_piece2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_piece3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_beretta1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_beretta2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_beretta3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_otis_health1 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_otis_health2 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_otis_health3 ); // {"sk_plr_crowbar3","0"};

	CVAR_REGISTER( &sk_plr_ksg121 ); // {"sk_plr_crowbar1","0"};
	CVAR_REGISTER( &sk_plr_ksg122 ); // {"sk_plr_crowbar2","0"};
	CVAR_REGISTER( &sk_plr_ksg123 ); // {"sk_plr_crowbar3","0"};

	// Glock Round
	CVAR_REGISTER( &sk_plr_9mm_bullet1 );// {"sk_plr_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_plr_9mm_bullet2 );// {"sk_plr_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_plr_9mm_bullet3 );// {"sk_plr_9mm_bullet3","0"};

	// 357 Round
	CVAR_REGISTER( &sk_plr_357_bullet1 );// {"sk_plr_357_bullet1","0"};
	CVAR_REGISTER( &sk_plr_357_bullet2 );// {"sk_plr_357_bullet2","0"};
	CVAR_REGISTER( &sk_plr_357_bullet3 );// {"sk_plr_357_bullet3","0"};

	//Flamethrower
	CVAR_REGISTER( &sk_plr_flamethrower1 );
	CVAR_REGISTER( &sk_plr_flamethrower2 );
	CVAR_REGISTER( &sk_plr_flamethrower3 );

	// MP5 Round
	CVAR_REGISTER( &sk_plr_9mmAR_bullet1 );// {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_bullet2 );// {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_bullet3 );// {"sk_plr_9mmAR_bullet3","0"};

	// AK47
	CVAR_REGISTER( &sk_plr_ak471 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_ak472 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_ak473 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_p9041 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_p9042 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_p9043 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_autosniper1 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_autosniper2 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_autosniper3 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_d501 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_d502 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_d503 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_chaingun1 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_chaingun2 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_chaingun3 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_colt451 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_colt452 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_colt453 ); // {"sk_plr_9mmAR_bullet3","0"};

	// MP5A3
	CVAR_REGISTER( &sk_plr_mp5a31 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_mp5a32 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_mp5a33 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_mp71 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_mp72 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_mp73 ); // {"sk_plr_9mmAR_bullet3","0"};

	// DBARREL
	CVAR_REGISTER( &sk_plr_db1 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_db2 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_db3 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_m10141 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_m10142 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_m10143 ); // {"sk_plr_9mmAR_bullet3","0"};

	CVAR_REGISTER( &sk_plr_fnfal1 ); // {"sk_plr_9mmAR_bullet1","0"};
	CVAR_REGISTER( &sk_plr_fnfal2 ); // {"sk_plr_9mmAR_bullet2","0"};
	CVAR_REGISTER( &sk_plr_fnfal3 ); // {"sk_plr_9mmAR_bullet3","0"};

	// M203 grenade
	CVAR_REGISTER( &sk_plr_9mmAR_grenade1 );// {"sk_plr_9mmAR_grenade1","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_grenade2 );// {"sk_plr_9mmAR_grenade2","0"};
	CVAR_REGISTER( &sk_plr_9mmAR_grenade3 );// {"sk_plr_9mmAR_grenade3","0"};

	// Shotgun buckshot
	CVAR_REGISTER( &sk_plr_buckshot1 );// {"sk_plr_buckshot1","0"};
	CVAR_REGISTER( &sk_plr_buckshot2 );// {"sk_plr_buckshot2","0"};
	CVAR_REGISTER( &sk_plr_buckshot3 );// {"sk_plr_buckshot3","0"};

	// Crossbow
	CVAR_REGISTER( &sk_plr_xbow_bolt_monster1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_monster2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_monster3 );// {"sk_plr_xbow_bolt3","0"};

	CVAR_REGISTER( &sk_plr_xbow_bolt_client1 );// {"sk_plr_xbow_bolt1","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_client2 );// {"sk_plr_xbow_bolt2","0"};
	CVAR_REGISTER( &sk_plr_xbow_bolt_client3 );// {"sk_plr_xbow_bolt3","0"};

	// RPG
	CVAR_REGISTER( &sk_plr_rpg1 );// {"sk_plr_rpg1","0"};
	CVAR_REGISTER( &sk_plr_rpg2 );// {"sk_plr_rpg2","0"};
	CVAR_REGISTER( &sk_plr_rpg3 );// {"sk_plr_rpg3","0"};

	// Gauss Gun
	CVAR_REGISTER( &sk_plr_gauss1 );// {"sk_plr_gauss1","0"};
	CVAR_REGISTER( &sk_plr_gauss2 );// {"sk_plr_gauss2","0"};
	CVAR_REGISTER( &sk_plr_gauss3 );// {"sk_plr_gauss3","0"};

	// Egon Gun
	CVAR_REGISTER( &sk_plr_egon_narrow1 );// {"sk_plr_egon_narrow1","0"};
	CVAR_REGISTER( &sk_plr_egon_narrow2 );// {"sk_plr_egon_narrow2","0"};
	CVAR_REGISTER( &sk_plr_egon_narrow3 );// {"sk_plr_egon_narrow3","0"};

	CVAR_REGISTER( &sk_plr_egon_wide1 );// {"sk_plr_egon_wide1","0"};
	CVAR_REGISTER( &sk_plr_egon_wide2 );// {"sk_plr_egon_wide2","0"};
	CVAR_REGISTER( &sk_plr_egon_wide3 );// {"sk_plr_egon_wide3","0"};

	// Hand Grendade
	CVAR_REGISTER( &sk_plr_hand_grenade1 );// {"sk_plr_hand_grenade1","0"};
	CVAR_REGISTER( &sk_plr_hand_grenade2 );// {"sk_plr_hand_grenade2","0"};
	CVAR_REGISTER( &sk_plr_hand_grenade3 );// {"sk_plr_hand_grenade3","0"};

	// Satchel Charge
	CVAR_REGISTER( &sk_plr_satchel1 );// {"sk_plr_satchel1","0"};
	CVAR_REGISTER( &sk_plr_satchel2 );// {"sk_plr_satchel2","0"};
	CVAR_REGISTER( &sk_plr_satchel3 );// {"sk_plr_satchel3","0"};

	CVAR_REGISTER( &sk_plr_nuclear1 ); // {"sk_plr_satchel1","0"};
	CVAR_REGISTER( &sk_plr_nuclear2 ); // {"sk_plr_satchel2","0"};
	CVAR_REGISTER( &sk_plr_nuclear3 ); // {"sk_plr_satchel3","0"};

	// Tripmine
	CVAR_REGISTER( &sk_plr_tripmine1 );// {"sk_plr_tripmine1","0"};
	CVAR_REGISTER( &sk_plr_tripmine2 );// {"sk_plr_tripmine2","0"};
	CVAR_REGISTER( &sk_plr_tripmine3 );// {"sk_plr_tripmine3","0"};

	// WORLD WEAPONS
	CVAR_REGISTER( &sk_12mm_bullet1 );// {"sk_12mm_bullet1","0"};
	CVAR_REGISTER( &sk_12mm_bullet2 );// {"sk_12mm_bullet2","0"};
	CVAR_REGISTER( &sk_12mm_bullet3 );// {"sk_12mm_bullet3","0"};

	CVAR_REGISTER( &sk_9mmAR_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_9mmAR_bullet2 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_9mmAR_bullet3 );// {"sk_9mm_bullet1","0"};

	CVAR_REGISTER( &sk_9mm_bullet1 );// {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_9mm_bullet2 );// {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_9mm_bullet3 );// {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_m41_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_m41_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_m41_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_p904_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_p904_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_p904_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_akm_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_akm_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_akm_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_as_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_as_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_as_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_357_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_357_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_357_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_d50_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_d50_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_d50_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_23_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_23_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_23_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_fnfal_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_fnfal_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_fnfal_bullet3 ); // {"sk_9mm_bullet3","0"};

	CVAR_REGISTER( &sk_cg_bullet1 ); // {"sk_9mm_bullet1","0"};
	CVAR_REGISTER( &sk_cg_bullet2 ); // {"sk_9mm_bullet2","0"};
	CVAR_REGISTER( &sk_cg_bullet3 ); // {"sk_9mm_bullet3","0"};

	// HORNET
	CVAR_REGISTER( &sk_hornet_dmg1 );// {"sk_hornet_dmg1","0"};
	CVAR_REGISTER( &sk_hornet_dmg2 );// {"sk_hornet_dmg2","0"};
	CVAR_REGISTER( &sk_hornet_dmg3 );// {"sk_hornet_dmg3","0"};

	// HEALTH/SUIT CHARGE DISTRIBUTION
	CVAR_REGISTER( &sk_suitcharger1 );
	CVAR_REGISTER( &sk_suitcharger2 );
	CVAR_REGISTER( &sk_suitcharger3 );

	CVAR_REGISTER( &sk_battery1 );
	CVAR_REGISTER( &sk_battery2 );
	CVAR_REGISTER( &sk_battery3 );

	CVAR_REGISTER( &sk_healthcharger1 );
	CVAR_REGISTER( &sk_healthcharger2 );
	CVAR_REGISTER( &sk_healthcharger3 );

	CVAR_REGISTER( &sk_healthkit1 );
	CVAR_REGISTER( &sk_healthkit2 );
	CVAR_REGISTER( &sk_healthkit3 );

	CVAR_REGISTER( &sk_scientist_heal1 );
	CVAR_REGISTER( &sk_scientist_heal2 );
	CVAR_REGISTER( &sk_scientist_heal3 );

	// monster damage adjusters
	CVAR_REGISTER( &sk_monster_head1 );
	CVAR_REGISTER( &sk_monster_head2 );
	CVAR_REGISTER( &sk_monster_head3 );

	CVAR_REGISTER( &sk_monster_chest1 );
	CVAR_REGISTER( &sk_monster_chest2 );
	CVAR_REGISTER( &sk_monster_chest3 );

	CVAR_REGISTER( &sk_monster_stomach1 );
	CVAR_REGISTER( &sk_monster_stomach2 );
	CVAR_REGISTER( &sk_monster_stomach3 );

	CVAR_REGISTER( &sk_monster_arm1 );
	CVAR_REGISTER( &sk_monster_arm2 );
	CVAR_REGISTER( &sk_monster_arm3 );

	CVAR_REGISTER( &sk_monster_leg1 );
	CVAR_REGISTER( &sk_monster_leg2 );
	CVAR_REGISTER( &sk_monster_leg3 );

	// player damage adjusters
	CVAR_REGISTER( &sk_player_head1 );
	CVAR_REGISTER( &sk_player_head2 );
	CVAR_REGISTER( &sk_player_head3 );

	CVAR_REGISTER( &sk_player_chest1 );
	CVAR_REGISTER( &sk_player_chest2 );
	CVAR_REGISTER( &sk_player_chest3 );

	CVAR_REGISTER( &sk_player_stomach1 );
	CVAR_REGISTER( &sk_player_stomach2 );
	CVAR_REGISTER( &sk_player_stomach3 );

	CVAR_REGISTER( &sk_player_arm1 );
	CVAR_REGISTER( &sk_player_arm2 );
	CVAR_REGISTER( &sk_player_arm3 );

	CVAR_REGISTER( &sk_player_leg1 );
	CVAR_REGISTER( &sk_player_leg2 );
	CVAR_REGISTER( &sk_player_leg3 );
// END REGISTER CVARS FOR SKILL LEVEL STUFF

	SERVER_COMMAND( "exec skill.cfg\n" );
}


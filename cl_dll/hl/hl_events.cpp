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

#include "../hud.h"
#include "../cl_util.h"
#include "event_api.h"

extern "C"
{
// HLDM
void EV_FireGlock1( struct event_args_s *args );
void EV_FireGlock2( struct event_args_s *args );
void EV_FireShotGunSingle( struct event_args_s *args );
void EV_FireShotGunDouble( struct event_args_s *args );
void EV_FireMP5( struct event_args_s *args );
void EV_FireMP52( struct event_args_s *args );
void EV_FireP904( struct event_args_s *args );
void EV_FireP9042( struct event_args_s *args );
void EV_FireP226( struct event_args_s *args );
void EV_FirePython( struct event_args_s *args );
void EV_FireGauss( struct event_args_s *args );
void EV_SpinGauss( struct event_args_s *args );
void EV_Crowbar( struct event_args_s *args );
void EV_FireCrossbow( struct event_args_s *args );
void EV_FireCrossbow2( struct event_args_s *args );
void EV_FireRpg( struct event_args_s *args );
void EV_EgonFire( struct event_args_s *args );
void EV_EgonStop( struct event_args_s *args );
void EV_HornetGunFire( struct event_args_s *args );
void EV_TripmineFire( struct event_args_s *args );
void EV_SnarkFire( struct event_args_s *args );
// void EV_KnifeFire( struct event_args_s *args );
void EV_FireAK47( struct event_args_s *args );
void EV_FireCG( struct event_args_s *args );
void EV_FireMP5A3( struct event_args_s *args );
void EV_FireCDbarrel( struct event_args_s *args );
void EV_FireBeretta( struct event_args_s *args );
void EV_FireDeagle( struct event_args_s *args );
void EV_FireM41( struct event_args_s *args );
void EV_FireM412( struct event_args_s *args );
// void EV_FirePipe( struct event_args_s *args );
void EV_FireAUTOSNIPER( struct event_args_s *args );
void EV_FireD50( struct event_args_s *args );
void EV_FireColt45( struct event_args_s *args );
void EV_FireCM1014( struct event_args_s *args );
void EV_FireCKSG12( struct event_args_s *args );
void EV_FireFNFAL( struct event_args_s *args );
void EV_FireMP7( struct event_args_s *args );

void EV_TrainPitchAdjust( struct event_args_s *args );
}

/*
======================
Game_HookEvents

Associate script file name with callback functions.  Callback's must be extern "C" so
 the engine doesn't get confused about name mangling stuff.  Note that the format is
 always the same.  Of course, a clever mod team could actually embed parameters, behavior
 into the actual .sc files and create a .sc file parser and hook their functionality through
 that.. i.e., a scripting system.

That was what we were going to do, but we ran out of time...oh well.
======================
*/
void Game_HookEvents( void )
{
	gEngfuncs.pfnHookEvent( "events/glock1.sc", EV_FireGlock1 );
	gEngfuncs.pfnHookEvent( "events/glock2.sc", EV_FireGlock2 );
	gEngfuncs.pfnHookEvent( "events/shotgun1.sc", EV_FireShotGunSingle );
	gEngfuncs.pfnHookEvent( "events/shotgun2.sc", EV_FireShotGunDouble );
	gEngfuncs.pfnHookEvent( "events/mp5.sc", EV_FireMP5 );
	gEngfuncs.pfnHookEvent( "events/mp52.sc", EV_FireMP52 );
	gEngfuncs.pfnHookEvent( "events/p904.sc", EV_FireP904 );
	gEngfuncs.pfnHookEvent( "events/p9042.sc", EV_FireP9042 );
	gEngfuncs.pfnHookEvent( "events/p226.sc", EV_FireP226 );
	gEngfuncs.pfnHookEvent( "events/python.sc", EV_FirePython );
	gEngfuncs.pfnHookEvent( "events/gauss.sc", EV_FireGauss );
	gEngfuncs.pfnHookEvent( "events/gaussspin.sc", EV_SpinGauss );
	gEngfuncs.pfnHookEvent( "events/crowbar.sc", EV_Crowbar );
	gEngfuncs.pfnHookEvent( "events/crossbow1.sc", EV_FireCrossbow );
	gEngfuncs.pfnHookEvent( "events/crossbow2.sc", EV_FireCrossbow2 );
	gEngfuncs.pfnHookEvent( "events/rpg.sc", EV_FireRpg );
	gEngfuncs.pfnHookEvent( "events/egon_fire.sc", EV_EgonFire );
	gEngfuncs.pfnHookEvent( "events/egon_stop.sc", EV_EgonStop );
	gEngfuncs.pfnHookEvent( "events/firehornet.sc", EV_HornetGunFire );
	gEngfuncs.pfnHookEvent( "events/tripfire.sc", EV_TripmineFire );
	gEngfuncs.pfnHookEvent( "events/snarkfire.sc", EV_SnarkFire );
	// gEngfuncs.pfnHookEvent( "events/knife.sc", EV_KnifeFire );
	gEngfuncs.pfnHookEvent( "events/ak47.sc", EV_FireAK47 );
	gEngfuncs.pfnHookEvent( "events/cg.sc", EV_FireCG );
	gEngfuncs.pfnHookEvent( "events/mp5a3.sc", EV_FireMP5A3 );
	gEngfuncs.pfnHookEvent( "events/dbarrel.sc", EV_FireCDbarrel );
	gEngfuncs.pfnHookEvent( "events/beretta.sc", EV_FireBeretta );
	gEngfuncs.pfnHookEvent( "events/deagle.sc", EV_FireDeagle );
	gEngfuncs.pfnHookEvent( "events/m41.sc", EV_FireM41 );
	gEngfuncs.pfnHookEvent( "events/m412.sc", EV_FireM412 );
	// gEngfuncs.pfnHookEvent( "events/pipe.sc", EV_FirePipe );
	gEngfuncs.pfnHookEvent( "events/autosniper.sc", EV_FireAUTOSNIPER );
	gEngfuncs.pfnHookEvent( "events/d50.sc", EV_FireD50 );
	gEngfuncs.pfnHookEvent( "events/colt45.sc", EV_FireColt45 );
	gEngfuncs.pfnHookEvent( "events/m1014.sc", EV_FireCM1014 );
	gEngfuncs.pfnHookEvent( "events/ksg12.sc", EV_FireCKSG12 );
	gEngfuncs.pfnHookEvent( "events/fnfal.sc", EV_FireFNFAL );
	gEngfuncs.pfnHookEvent( "events/mp7.sc", EV_FireMP7 );

	gEngfuncs.pfnHookEvent( "events/train.sc", EV_TrainPitchAdjust );
}

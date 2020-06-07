// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credits us

/*
==============================
All offsets for mods/ET

Only offsets in this file
==============================
*/

#ifndef HOOK_H_
#define HOOK_H_

const static ethMod_t ethMods[] = {
	// No stripped binary mods	(only here to put a name/version on a mod)
	// MOD			Version			CRC			Type		CG_Missile
	{ "etmain",		"2.55",			0x1ae3ee02,	MOD_ETMAIN, 0x494d0 },
	{ "etmain",		"2.56",			0x4153a646,	MOD_ETMAIN, 0x495d0 },
	{ "etmain",		"2.60",			0xcb26a1fd,	MOD_ETMAIN, 0x4b9b0 },
	{ "etpub",		"20051128",		0x3f93ed44,	MOD_ETMAIN, 0x49034 },
	{ "etpub",		"20051230",		0x4c8cb6a2,	MOD_ETMAIN, 0x49cd0 },
	{ "etpub",		"20060205",		0x1e82f5d2,	MOD_ETMAIN, 0x4c510 },
	{ "etpub",		"20060304",		0x79c4963a,	MOD_ETMAIN,	0x4cc74 },
	{ "etpub",		"20060310",		0x7077b613,	MOD_ETMAIN,	0x4cd3c },
	{ "etpub",		"20060606",		0x95520381,	MOD_ETMAIN,	0x4e190 },
	{ "etpub",		"20060810",		0x59d32ce5,	MOD_ETMAIN,	0x4e194 },
	{ "etpub",		"20060818",		0x2e7903b6,	MOD_ETMAIN,	0x4e194 },
	{ "jaymod",		"1.4",			0xbd9bb6ad,	MOD_ETMAIN },
	{ "noquarter",	"1.0.1",		0x279e6121,	MOD_ETMAIN },
	{ "tce",		"0.48",			0x866bdb5b,	MOD_TCE },
	{ "tce",		"0.49",			0x965ad597,	MOD_TCE },
	{ "tce",		"0.49b",		0x5cb8eae1, MOD_TCE },
	{ "whale",		"1.3",			0x09285202,	MOD_ETMAIN },
	{ "whale",		"1.4-beta1",	0xe62c638e,	MOD_ETMAIN },

	// Stripped binary mods
	{ "etpro",	"3.2.4", 0x6a96361f, MOD_ETPRO,
		0x459c0,        // CG_Missile()
		0x453b360, 2700,	// cg_entities
		0x2ac20c0,0x9408,0x1c4,    // cgs	
		0x75ef0,	// CG_Trace()
		0xa8940,	// BG_EvaluateTrajectory()
		0x4a0f0,	// CG_EntityEvent()
		0x964b0,	// CG_FinishWeaponChange()
		0x738f0,	// CG_DamageFeedback()
		0x92ab0,	// CG_RailTrail2()
		0x982a0,	// CG_WeaponFireRecoil()
		// etpro
		0xa100,		// guid_function
		0xf368		// cvar proof
	},
	{ "etpro",	"3.2.5", 0xc665c831, MOD_ETPRO,
		0x459c0,	// CG_Missile()
		0x453b360, 2700,	// cg_entities
		0x2ac20c0,0x9408,0x1c4, // cgs
		0x75ef0,	// CG_Trace()
		0xa8940,	// BG_EvaluateTrajectory()
		0x4a0f0,	// CG_EntityEvent()
		0x964b0,	// CG_FinishWeaponChange()
		0x738f0,	// CG_DamageFeedback()
		0x92ab0,	// CG_RailTrail2()
		0x982a0,	// CG_WeaponFireRecoil()
		// etpro
		0xa100,		// guid_function
		0xf368		// cvar proof
	},
	{ "etpro",	"3.2.6", 0xc10e7d8c, MOD_ETPRO,
		0x45d90,	// CG_Missile()
		0x453c6a0, 2700,	// cg_entities
		0x2ac33e0, 0x9408, 0x1c4,   // cgs
		0x76300,	// CG_Trace()
		0xa8d80,	// BG_EvaluateTrajectory()
		0x4a4c0,	// CG_EntityEvent()
		0x968c0,	// CG_FinishWeaponChange()
		0x73cc0,	// CG_DamageFeedback()
		0x92ec0,	// CG_RailTrail2()
		0x986e0,	// CG_WeaponFireRecoil()
		// etpro
		0xa360,		// guid_function
		0xf718		// cvar proof
	},
	{ "jaymod",	"2.0.1", 0xc2051741, MOD_ETMAIN,
		0x34d70,		// CG_Missile()
		0x28d6180, 2704,	// cg_entities
		0xe93f20, 0x940c,0x1bc,   // cgs
		0x692f0,	// CG_Trace()
		0xa2b70,	// BG_EvaluateTrajectory()
		0x3c360,	// CG_EntityEvent()
		0x8c450,	// CG_FinishWeaponChange()
		0x66170,	// CG_DamageFeedback()
		0x87c40,	// CG_RailTrail2()
		0x8f950,	// CG_WeaponFireRecoil()
	},
	{ "jaymod",	"2.0.2", 0x6528a7e0, MOD_ETMAIN,
		0x34d70,		// CG_Missile()
		0x28d6180, 2704,	// cg_entities
		0xe93f20, 0x940c,0x1bc,   // cgs
		0x692f0,	// CG_Trace()
		0xa2b70,	// BG_EvaluateTrajectory()
		0x3c360,	// CG_EntityEvent()
		0x8c450,	// CG_FinishWeaponChange()
		0x66170,	// CG_DamageFeedback()
		0x87c40,	// CG_RailTrail2()
		0x8f950,	// CG_WeaponFireRecoil()
	},
	{ "jaymod",	"2.0.3", 0x15ba6692, MOD_ETMAIN,
		0x34d70,	// CG_Missile()
		0x28d6180, 2704,	// cg_entities
		0xe93f20, 0x940c,0x1bc,   // cgs
		0x692f0,	// CG_Trace()
		0xa2b70,	// BG_EvaluateTrajectory()
		0x3c360,	// CG_EntityEvent()
		0x8c450,	// CG_FinishWeaponChange()
		0x66170,	// CG_DamageFeedback()
		0x87c40,	// CG_RailTrail2()
		0x8f950,	// CG_WeaponFireRecoil()
	},
	{ "jaymod", "2.0.4", 0xc1bad5b3, MOD_ETMAIN,
		0x34d70,	// CG_Missile()
		0x28d6180, 2704,	// cg_entities
		0xe93f20, 0x940c,0x1bc,   // cgs
		0x692f0,    // CG_Trace()
		0xa2b70,    // BG_EvaluateTrajectory()
		0x3c360,    // CG_EntityEvent()
		0x8c450,    // CG_FinishWeaponChange()
		0x66170,    // CG_DamageFeedback()
		0x87c40,    // CG_RailTrail2()
		0x8f950,    // CG_WeaponFireRecoil()
	},
	{ "jaymod", "2.1.0 beta", 0x5f24fa23, MOD_ETMAIN,
		0x769e0,	// CG_Missile()
		0x226860, 2708,          // cg_entities
		0x4cb860, 0x940c,0x1bc,  // cgs
		0xace20,	// CG_Trace()
		0xf1a50,	// BG_EvaluateTrajectory()
		0x7e100,	// CG_EntityEvent()
		0xd3230,	// CG_FinishWeaponChange()
		0xa98b0,	// CG_DamageFeedback()
		0xcdd20,	// CG_RailTrail2()
		0xd4a50,	// CG_WeaponFireRecoil()
	},
	{ "jaymod", "2.1.1 beta", 0x8352a951, MOD_ETMAIN,
		0x769e0,	// CG_Missile()
		0x226860, 2708,			// cg_entities
		0x4cb860, 0x940c,0x1bc,	// cgs
		0xace20,	// CG_Trace()
		0xf1a50,	// BG_EvaluateTrajectory()
		0x7e100,	// CG_EntityEvent()
		0xd3230,	// CG_FinishWeaponChange()
		0xa98b0,	// CG_DamageFeedback()
		0xcdd20,	// CG_RailTrail2()
		0xd4a50,	// CG_WeaponFireRecoil()
	}	
};

// centity_t structs offsets
#define CG_ENTITIES_CURRENTSTATE_OFFSET 0x0
#define CG_ENTITIES_CURRENTVALID_OFFSET 0x244

/*
==============================
et.x86 addresses
==============================
*/

const static ethET_t ethET[] = {
	{	"2.55",		// version
		0x21e60afb,	// crc32
		0x080bae24, // Cvar_Set2
		0x080b561c,	// Cmd_AddCommand
		0x0804cbb4,	// COM_StripExtension
		0x080bee9c, // FS_ReadFile
		0x080c1d68,	// FS_PureServerSetLoadedPaks
		0x080b4e2c,	// Cbuf_ExecuteText
		0x087cd9c8,	// fs_searchpaths
		0x08f451d0,	// serverIP
		0x08ffc068,	// cl_mouseDx
		0x08ffc06c,	// cl_mouseDy
		0x0,		// etpro Linux checksum
		0x0,		// etpro win32 checksum
		#ifdef ETH_PRIVATE
			PRIV_ET255
		#endif
	},
	{	"2.56",		// version
		0x3d59a703,	// crc32
		0x080ba89c, // Cvar_Set2
		0x080b5098,	// Cmd_AddCommand
		0x0804cc34,	// COM_StripExtension
		0x080be914, // FS_ReadFile
		0x080c17e0,	// FS_PureServerSetLoadedPaks
		0x080b48a8,	// Cbuf_ExecuteText
		0x087cdfa8,	// fs_searchpaths
		0x08f457d0,	// serverIP
		0x08ffc668,	// cl_mouseDx
		0x08ffc66c,	// cl_mouseDy
		0x0,		// etpro Linux checksum
		0x0,		// etpro win32 checksum
		#ifdef ETH_PRIVATE
			PRIV_ET256
		#endif
	},
	{	"2.60",		// version
		0x3b18a889,	// crc32
		0x08070010, // Cvar_Set2
		0x08069240,	// Cmd_AddCommand
		0x08097880,	// COM_StripExtension
		0x080744e0, // FS_ReadFile
		0x08076570,	// FS_PureServerSetLoadedPaks
		0x08069650,	// Cbuf_ExecuteText
		0x0888c7c8,	// fs_searchpaths
		0x09036108,	// serverIP
		0x0906c1c8,	// cl_mouseDx
		0x0906c1cc,	// cl_mouseDy
		0xfe96e227,	// etpro Linux checksum
		0x6b04ecf8,	// etpro win32 checksum
		#ifdef ETH_PRIVATE
			PRIV_ET260
		#endif
	},
	{	"2.60b",	// version
		0x6ab49f82,	// crc32
		0x08073160, // Cvar_Set2
		0x0806b640,	// Cmd_AddCommand
		0x0,		// COM_StripExtension
		0x08079370, // FS_ReadFile
		0x0807cef0,	// FS_PureServerSetLoadedPaks
		0x0806bd70,	// Cbuf_ExecuteText
		0x088b1f18,	// fs_searchpaths
		0x090471c8,	// serverIP
		0x0907d288,	// cl_mouseDx
		0x0907d28c,	// cl_mouseDy
		0x395789a0,	// etpro Linux checksum
		0x30c3e725,	// etpro win32 checksum
		#ifdef ETH_PRIVATE
			PRIV_ET260B
		#endif
	}
};

#endif // HOOK_H_

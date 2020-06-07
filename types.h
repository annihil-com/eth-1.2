#ifndef TYPES_H_
#define TYPES_H_

/*
==============================
structs use with et.x86
==============================
*/

typedef struct {
	byte ip[4];
	byte UNUSED[10];
	unsigned short port;
} netadr_t;

typedef struct fileInPack_s {
	char					*name;		// name of the file
	unsigned long			pos;		// file info position in zip
	struct	fileInPack_s*	next;		// next file in the hash
} fileInPack_t;

typedef void *unzFile;

typedef struct {
	char		pakFilename[MAX_OSPATH];	// c:\quake3\baseq3\pak0.pk3
	char		pakBasename[MAX_OSPATH];	// pak0
	char		pakGamename[MAX_OSPATH];	// baseq3
	unzFile		handle;						// handle to zip file
	int			checksum;					// regular checksum
	int			pure_checksum;				// checksum for pure
	int			numfiles;					// number of files in pk3
	int			referenced;					// referenced file flags
	int			hashSize;					// hash table size (power of 2)
	fileInPack_t*	*hashTable;					// hash table
	fileInPack_t*	buildBuffer;				// buffer with the filenames etc.
} pack_t;

typedef struct searchpath_s {
	struct searchpath_s *next;
	pack_t *pack;
} searchpath_t;

/*
==============================
eth structs
==============================
*/

// For all console actions/commands
#define MAX_ACTION_LEN 128
typedef struct ethAction_s {
	int state;
	int defaultState;
	char startAction[MAX_ACTION_LEN];
	char stopAction[MAX_ACTION_LEN];
} ethAction_t;

typedef enum ethActions_s {
	ACTION_ATTACK,
	ACTION_BACKWARD,
	ACTION_BINDMOUSE1,
	ACTION_CROUCH,
	ACTION_JUMP,
	ACTION_PRONE,
	ACTION_RUN,
	ACTION_SCOREBOARD,
	ACTION_RELOAD,
	ACTIONS_TOTAL // Number of actions
} ethActions_t;

// Sounds
typedef enum ethSounds_s {
	// Misc sound
	SOUND_HEADSHOT1,
	SOUND_HEADSHOT2,
	SOUND_HUMILIATION,
	SOUND_PREPARE,
	// Spree sound
	// Level 1
	SOUND_DOUBLEKILL1,
	SOUND_EXCELLENT,
	SOUND_IMPRESSIVE,
	SOUND_KILLINGSPREE1,
	SOUND_KILLINGSPREE2,
	SOUND_PERFECT,
	// Level 2
	SOUND_GODLIKE1,
	SOUND_GODLIKE2,
	SOUND_HOLYSHIT,
	SOUND_MULTIKILL1,
	SOUND_MULTIKILL2,
	SOUND_MULTIKILL3,
	SOUND_TRIPLEKILL,
	// Level 3
	SOUND_DOMINATING1,
	SOUND_DOMINATING2,
	SOUND_ULTRAKILL1,
	SOUND_ULTRAKILL2,
	SOUND_ULTRAKILL3,
	// Level 4
	SOUND_MONSTERKILL1,
	SOUND_MONSTERKILL2,
	SOUND_UNREAL,
	SOUND_UNSTOPPABLE1,
	SOUND_UNSTOPPABLE2,
	SOUND_WICKEDSICK,

	// Custom Weapon sounds (from GCTC and DB soundpack)
	// dont mess up the order! customSounds_t expects them in this order
	SOUND_REPLACE_OFFSET,
	SOUND_BROWNING_FIRE,
	SOUND_COLT_FAR,
	SOUND_COLT_FIRE,
	SOUND_COLT_RELOAD,
	SOUND_COLT_SILENCE,
	SOUND_FG42_FIRE,
	SOUND_M1_FIREGRN,
	SOUND_M1_FIRE,
	SOUND_K43_FIRE,
	SOUND_MINE_STEP,
	SOUND_LUGER_FIRE,
	SOUND_LUGER_FAR,
	SOUND_LUGER_SLND,
	SOUND_MG42_FAR,
	SOUND_MG42_FIRE,
	SOUND_MG42_HEAT,
	SOUND_MG42_RELOAD,
	SOUND_MP40_FIRE,
	SOUND_MP40_FAR,
	SOUND_THOMPSON_FIRE,
	SOUND_THOMPSON_FAR,
	SOUND_THOMPSON_REL,
	SOUND_STEN_FIRE,
	
	SOUNDS_TOTAL
} ethSounds_t;

typedef enum customSounds_s {
		// Custom Weapon sounds (from GCTC and DB soundpack)
	CSND_NONE,
	
	CSND_BROWNING_FIRE,
	CSND_COLT_FAR,
	CSND_COLT_FIRE,
	CSND_COLT_RELOAD,
	CSND_COLT_SILENCE,
	CSND_FG42_FIRE,
	CSND_M1_FIREGRN,
	CSND_M1_FIRE,
	CSND_K43_FIRE,
	CSND_MINE_STEP,
	CSND_LUGER_FIRE,
	CSND_LUGER_FAR,
	CSND_LUGER_SLND,
	CSND_MG42_FAR,
	CSND_MG42_FIRE,
	CSND_MG42_HEAT,
	CSND_MG42_RELOAD,
	CSND_MP40_FIRE,
	CSND_MP40_FAR,
	CSND_THOMPSON_FIRE,
	CSND_THOMPSON_FAR,
	CSND_THOMPSON_REL,
	CSND_STEN_FIRE,
	
	CSND_MAX
} customSounds_t;

typedef struct soundReplace_s {
	char *sndName;
	sfxHandle_t custom;
	sfxHandle_t org;
} soundReplace_t;

typedef struct ETSounds_s {
	char *sndName;			// Name of the sfx
	int custom;				// custom sound index
	sfxHandle_t pak0;		// sfxHandle appears in pak0
	sfxHandle_t server;		// sfxHandle is unique to server
	qboolean isHQ;			// 'HQ' sounds
} ETSounds_t;

/*
==============================
multi-game version support struct
==============================
*/

typedef struct ethET_s {
	// Infos
	char *version;
	u_int crc32;

	// Game functions
	u_int32_t Cvar_Set2;
	u_int32_t Cmd_AddCommand;
	u_int32_t COM_StripExtension;	// Only need before 2.60b - http://www.milw0rm.com/exploits/1750
	u_int32_t FS_ReadFile;
	u_int32_t FS_PureServerSetLoadedPaks;
	u_int32_t Cbuf_ExecuteText;

	// Game vars
	u_int32_t fs_searchpaths;
	u_int32_t serverIP;
	u_int32_t cl_mouseDx;
	u_int32_t cl_mouseDy;

	// etpro ET OS checksum
	u_int32_t etproLinuxChecksum;
	u_int32_t etproWin32Checksum;

	#ifdef ETH_PRIVATE
		PRIV_ET_SUPPORT
	#endif
} ethET_t;

/*
==============================
multi-mod support structs
==============================
*/

// cgame centity struct
typedef struct eth_cg_entities_s {
	entityState_t *currentState;
	qboolean *currentValid;
	refEntity_t pe_bodyRefEnt;
} eth_centity_t;

typedef enum {
	MOD_ETMAIN,
	MOD_ETPRO,
	MOD_TCE
} ethModType_t;

typedef struct ethMod_s {
	char *name;
	char *version;
	u_int crc32;
	ethModType_t type;

	// Offset from cgame lib
	int CG_Missile;
	
	int cg_entities;
	int centity_t_size;

	// the client static game struct, cgs_t
	int cgs;				// offset to cgmain's cgs
	int clientInfoOffset;	// offset to clientInfo_t from cgs_t
	int clientInfoSize;		// size of clientInfo_t

	int CG_Trace;
	int BG_EvaluateTrajectory;
	int CG_EntityEvent;
	int CG_FinishWeaponChange;
	int CG_DamageFeedback;
	int CG_RailTrail2;
	int CG_WeaponFireRecoil;
	
	// for etpro
	int guid_function;
	int cvar_proof;
} ethMod_t;

/*
==============================
players structs
==============================
*/

typedef enum ethGfxColors_s {
	GFX_NONE,
	GFX_RED,
	GFX_BLUE,
	GFX_GREEN,
	GFX_CYAN,
	GFX_TEAL,
	GFX_YELLOW,
	GFXS_TOTAL
} ethGfxColors_t;

typedef enum ethChams_s {
	CHAM_GLOW = 1,
	CHAM_GLOWONLY,
	CHAM_FLAT,
	CHAM_QUAKE,
	CHAM_QUAKEB,
	CHAM_QUAKEC,
	CHAMS_TOTAL
} ethChams_t;

typedef struct ethColorModel_s {
	ethGfxColors_t chams;
	float *esp;	// vec3_t
} ethColorModel_t;

typedef struct playerColor_s {
	ethColorModel_t friendVisible;
	ethColorModel_t friendNotVisible;
	ethColorModel_t enemyVisible;
	ethColorModel_t enemyNotVisible;
} ethPlayerColor_t;

typedef struct ethClientInfo_s {
	qboolean infoValid;
	char name[MAX_QPATH];
	char cleanName[MAX_QPATH];
	team_t team;
	int cls;
//	int rank;
//	int weapon;

	int targetType;	// For Teamkillers / Friends
} ethClientInfo_t;

// All entity infos
typedef struct ethEntity_s {
	qboolean isValid;
	qboolean isInScreen;
	qboolean isVisible;	// true if body or head is visible
	qboolean isHeadVisible;
	
	float *espColor;	// vec4_t
	int screenX, screenY;
	float distance;

	vec3_t origin;		// Body origin
	vec3_t bodyPart;	// vec3_origin if no body parts visible
	vec3_t head;

	vec3_t headAxis[3];

	// model bounds to use for hitbox
	vec3_t rmin;
	vec3_t rmax;
	
} ethEntity_t;

/*
==============================
 Enemy/Friend player list selector
==============================
*/

typedef enum {
	PLIST_UNKNOWN,
	PLIST_TEAMKILLER,
	PLIST_FRIEND,
	PLIST_NAMES,
	PLIST_MAX
} playerList_t;

/*
==============================
 Custom HUD stuff
==============================
*/

typedef enum hudFonts_s {
	FONT_BERSOLID,
	FONT_COMIC,
// 	FONT_CREEPY,
// 	FONT_PALETTEN,
	FONT_SPACETOA,
// 	FONT_TEAMBD
	FONT_MAX
} hudFontList_t;

/*
==============================
eth main structs
==============================
*/
#define MAXSOUNDS	8192
// 'Static' eth struct. never zero'ed.
typedef struct seth_s {
	// all eth vars
	ethVars_t vars[VARS_TOTAL];
	float value[VARS_TOTAL];
	float bvalue[VARS_TOTAL];	//backup

	// Start and end addresses of ".text" elf section
	unsigned int textSectionStart;
	unsigned int textSectionEnd;
	
	// Store players spec'ing you
	char specNames[MAX_CLIENTS][MAX_QPATH];
	int specCount;

	// IRC chan topic
	char ircTopic[IRC_TOPIC_SIZE];

	// IRC server invites
	int lastInvite;
	int lastInviteID;

	// Store referees
	char refNames[MAX_CLIENTS][MAX_QPATH];
	int refCount;
} seth_t;

// Main eth struct - zero'ed every CG_INIT
typedef struct eth_s {
	qboolean hookLoad;

	// cgame lib infos
	char *cgameFilename;
	void *cgameLibHandle;
	void *cgameLibAddress;
	const ethMod_t *mod;
	
	// ui lib infos
	void *uiLibHandle;

	server_t server;
	qboolean sv_punkbusterState;

	// cgame vars/functions
	eth_centity_t cg_entities[MAX_GENTITIES];
	// cg
	refdef_t cg_refdef;
	vec3_t cg_refdefViewAngles;
	int cg_time;
	int bullettime;
	snapshot_t *cg_snap;
	int cg_clientNum;
	int cg_redlimbotime;
	int cg_bluelimbotime;
	int cg_frametime;
	// cgs
	cgs_t *cgs;
	glconfig_t cgs_glconfig;
	gameState_t *cgs_gameState;
	int cgs_levelStartTime;
	int cgs_aReinfOffset[TEAM_NUM_TEAMS];
	float cgs_screenXScale;
	float cgs_screenYScale;
	qhandle_t cgs_media_whiteShader;
	qhandle_t cgs_media_cursorIcon;
	qhandle_t cgs_media_reticleShaderSimple;
	qhandle_t cgs_media_binocShaderSimple;
	qhandle_t cgs_media_smallgunBrassModel;
	qhandle_t cgs_media_railCoreShader;
	qhandle_t cgs_media_sndLimboSelect;
	qhandle_t cgs_media_smokePuffshader;
	qhandle_t bg_alliedpin;
	qhandle_t bg_axispin;
	qhandle_t tcescope1;
	qhandle_t tcescope2;	
	gamestate_t cgs_gamestate;
	// functions
	void (*CG_Missile)( centity_t *cent );
	void (*CG_Trace)(trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipNumber, int mask);	
	void (*BG_EvaluateTrajectory)(const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splinePath);
	void *(*CG_EntityEvent)(centity_t *cent, vec3_t position);
	void (*CG_FinishWeaponChange)(int lastweap, int newweap);
	void (*CG_DamageFeedback)(int, int, int);
	void (*CG_RailTrail2)(clientInfo_t *ci, vec3_t start, vec3_t end);
	void (*CG_WeaponFireRecoil)(int weapon);

	// Fonts
	fontInfo_t fontFixed;
	fontInfo_t fontVariable;

	// Zero'ed every CG_DRAW_ACTIVE_FRAME
	ethEntity_t entities[MAX_GENTITIES];
	refEntity_t refEntities[MAX_GENTITIES];
	int refEntitiesCount;

	// All custom shaders
	qhandle_t friendVisibleShader;
	qhandle_t friendNotVisibleShader;
	qhandle_t enemyVisibleShader;
	qhandle_t enemyNotVisibleShader;
	vec3_t friendVisibleColor;
	vec3_t friendNotVisibleColor;
	vec3_t enemyVisibleColor;
	vec3_t enemyNotVisibleColor;

	// All custom sounds
	sfxHandle_t sounds[SOUNDS_TOTAL];
	sfxHandle_t replaceSounds[SOUNDS_TOTAL]; //to replace game sounds with our custom ones
	ethAction_t actions[ACTIONS_TOTAL];
	ethClientInfo_t clientInfo[MAX_CLIENTS];
	soundReplace_t customSounds[CSND_MAX];	
	ETSounds_t soundIndex[MAXSOUNDS];      // ugly but better for speed
	
	// cgame models
	qhandle_t hAmmoPack;
	qhandle_t hMedicPack;
	qhandle_t hHead;
	qhandle_t hGlasses;	// Some pk3 use that for axis medic head model

	// Esp icons
	qhandle_t weaponIcons[WP_NUM_WEAPONS];
	qhandle_t classIcons[NUM_PLAYER_CLASSES];
	qhandle_t rankIcons[NUM_EXPERIENCE_LEVELS];
	qhandle_t number[10];

	// dlight Shaders
	qhandle_t dlightShader[2];

	// Shaders
	qhandle_t shaders[GFXS_TOTAL][CHAMS_TOTAL];
	qhandle_t whiteFlatShader;
	qhandle_t friendMortarShader;
	qhandle_t enemyMortarShader;

	// Custom model/skin
	qhandle_t pigHeadModel;
	qhandle_t pigHeadSkin;

	// Medicbot
	int lastMedicRequest;
	int lastMedicRequestTime;

	// Stats
	int killCount;
	int killSpreeCount;
	int firstKillSpreeTime;
	int lastKillTime;
	int spreelevel;
	int startFadeTime;
	int s_level;
	int killCountNoDeath; // For killspam
	int autoDemoKillCount; // For auto-recording demo filename
	int revives;
	int reviveSpree;

	// Menu
	int mouseX, mouseY, mouseButton;
	int isMenuOpen;

	// Playerlist
	int isPlistOpen;
	
	#ifdef ETH_DEBUG
		// Debug view stuff
		int offsetMul;
		int offsetSub;
	#endif

	// HUD
	int reloading;
	int reloadScale;
	int currentTxt;
	int currentChat;
	char VictimName[MAX_QPATH];
	char KillerName[MAX_QPATH];

	int nameCounter; // TODO: what that ?

	// Custom HUD

	fontInfo_t hFont[FONT_MAX - 1];
	qhandle_t wIcon;
	qhandle_t iIcon;
	qhandle_t gStar;
	qhandle_t cStar;

	int cg_covertopsChargeTime[2];
	int cg_engineerChargeTime[2];
	int cg_soldierChargeTime[2];
	int cg_medicChargeTime[2];
	int cg_ltChargeTime[2];

	qboolean demoPlayback;

	//windowing support
	int pointer;
	int winInUse;

	// Muzzle
	vec3_t muzzle;

	// Misc
	int lastMusicSpamTime;
	qboolean clAttack[MAX_CLIENTS];
	int pbss;
} eth_t;

/*
==============================
Target filters
==============================
*/
// player
#define TARGET_PLAYER			(1<<0)
#define TARGET_PLAYER_VULN		(1<<1)
#define TARGET_PLAYER_ALIVE		(1<<2)
#define TARGET_PLAYER_DEAD		(1<<3)
// player aim target
#define TARGET_PLAYER_HEAD		(1<<4)
#define TARGET_PLAYER_BODY		(1<<5)
// missile common
#define TARGET_MISSILE			(1<<7)
#define TARGET_MISSILE_ARMED	(1<<8)
#define TARGET_MISSILE_NOTARMED	(1<<9)
// missile
#define TARGET_MISSILE_DYNAMITE	(1<<10)
#define TARGET_MISSILE_LANDMINE	(1<<11)
#define TARGET_MISSILE_ROCKET	(1<<12)
// common
#define TARGET_ENEMY			(1<<13)
#define TARGET_FRIEND			(1<<14)
#define TARGET_VISIBLE			(1<<15)
#define TARGET_NOTVISIBLE		(1<<16)
// helper
#define TARGET_PLAYER_HEAD_BODY TARGET_VISIBLE
#define TARGET_PLAYER_ENEMY		(TARGET_PLAYER | TARGET_PLAYER_VULN | TARGET_ENEMY)
/*
==============================
Some filter macro
==============================
*/
//		&& eth.cg_entities[entityNum].currentValid

#define IS_PLAYER_VALID(entityNum) ((entityNum < MAX_CLIENTS) \
		&& (eth.cg_entities[entityNum].currentState->eType == ET_PLAYER) \
		&& eth.clientInfo[entityNum].infoValid \
		&& eth.entities[entityNum].isValid \
		&& (eth.cg_clientNum != entityNum))

#define IS_PLAYER_VULNERABLE(entityNum) (IS_PLAYER_VALID(entityNum) \
		&& (!(eth.cg_entities[entityNum].currentState->powerups & (1 << PW_INVULNERABLE))))

#define IS_PLAYER_ENEMY(entityNum) (IS_PLAYER_VALID(entityNum) \
		&& (((eth.clientInfo[entityNum].team != eth.clientInfo[eth.cg_clientNum].team) \
				&& (eth.clientInfo[entityNum].targetType != PLIST_FRIEND)) \
			|| (eth.clientInfo[entityNum].targetType == PLIST_TEAMKILLER)))

#define IS_PLAYER_DEAD(entityNum) (IS_PLAYER_VALID(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->eFlags & EF_DEAD))

/*#define IS_PLAYER_PRONE(entityNum) (IS_PLAYER_VALID(entityNum) \
		&& (eth.cg_entities[entityNum].currentState.eFlags & EF_PRONE))

#define IS_PLAYER_CROUCH(entityNum) (IS_PLAYER_VALID(entityNum) \
		&& (eth.cg_entities[entityNum].currentState.eFlags & EF_CROUCHING))*/

// grenade, dynamite, landmine, satchel, ... are missiles
#define IS_MISSILE(entityNum) ((*eth.cg_entities[entityNum].currentValid) \
		&& (eth.cg_entities[entityNum].currentState->eType == ET_MISSILE))

#define IS_MISSILE_ARMED(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->teamNum < 4))

#define IS_MISSILE_NOTARMED(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->teamNum > 4))

#define IS_MISSILE_ENEMY(entityNum) (IS_MISSILE(entityNum) \
		&& (IS_MISSILE_NOTARMED(entityNum) ? \
			((eth.cg_entities[entityNum].currentState->teamNum - 4) != eth.clientInfo[eth.cg_snap->ps.clientNum].team) : \
			(eth.cg_entities[entityNum].currentState->teamNum != eth.clientInfo[eth.cg_snap->ps.clientNum].team)))

#define IS_DYNAMITE(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_DYNAMITE))

#define IS_LANDMINE(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_LANDMINE))

// Grenade: axis = LAUNCHER and allies = PINEAPPLE
#define IS_GRENADE(entityNum) (IS_MISSILE(entityNum) \
		&& ((eth.cg_entities[entityNum].currentState->weapon == WP_GRENADE_LAUNCHER) \
			|| (eth.cg_entities[entityNum].currentState->weapon == WP_GRENADE_PINEAPPLE)))

#define IS_MORTAR(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_MORTAR_SET))

#define IS_PANZER(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_PANZERFAUST))

#define IS_MG42(entityNum) ( (eth.cg_entities[entityNum].currentState->weapon == WP_MOBILE_MG42) )

#define IS_HEAVYWP(entityNum) ( IS_MISSILE(entityNum) \
		|| (eth.cg_entities[entityNum].currentState->weapon == WP_FLAMETHROWER) \
		|| IS_MG42(entityNum) )

#define IS_SATCHEL(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_SATCHEL))

#define IS_SMOKE(entityNum) (IS_MISSILE(entityNum) \
		&& (eth.cg_entities[entityNum].currentState->weapon == WP_SMOKE_BOMB))

//#define IS_AMMO(entityNum) ( eth.cg_entities[entityNum].refEnt.hModel == eth.hAmmoPack )
//#define IS_HEALTH(entityNum) ( eth.cg_entities[entityNum].refEnt.hModel == eth.hMedicPack )

#define QTOGGLE(qbool) qbool = qbool ? qfalse : qtrue

#endif /*TYPES_H_*/

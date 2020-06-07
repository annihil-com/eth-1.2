// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
Main header file
All others header files must include this one.
==============================
*/

#ifndef ETH_H_
#define ETH_H_

#include "X11/Xlib.h"
#include <GL/gl.h>
#include "sdk/src/cgame/cg_local.h"
#include "sdk/src/ui/ui_public.h"
// remove stupid define in sdk
#undef vsnprintf

#define ETH_CVAR_PREFIX "eth_"
#define ETH_CMD_PREFIX "eth_"

#define ETH_MENU_KEY XK_F9	// X11 key
#define ETH_IRC_KEY K_F11	// ET key
#define ETH_PLAYERLIST_KEY XK_F12	// X11 key
#define ETH_POINTER_KEY XK_F8 // X11 key
#define ETH_CONFIG_FILE ".ethconf"


#ifdef ETH_PRIVATE
	#include "private.h"
#endif

#include "net.h"
#include "menu.h"
#include "types.h"
#include "hud.h"
#include "windows.h"

// ET version struct
const ethET_t *sethET;
// Static struct for eth
seth_t seth;
// Struct use each new cgame
eth_t eth;

// Spree formats
char ksFormat[256];
char bnFormat[256];
char dieFormat[256];

/*
==============================
from aimbot.c
==============================
*/
void aimAt(vec3_t target);
qboolean getVisibleModelBodyTag(const int player, vec3_t *origin);
void doAutoShoot(qboolean shoot);
void doSatchelAutoShoot();
int findTarget(int targetFlag);
void doAimbot();
void doVecZ(int player);
void blockMouse(qboolean state);
void doPrediction(int player);
#ifdef ETH_DEBUG
	void doPredictionStats();
#endif

/*
==============================
from commands.c
==============================
*/

void registerGameCommands();

/*
==============================
from drawtools.c
==============================
*/

void adjustFrom640(float *x, float *y, float *w, float *h);
void adjustTo640(float *x, float *y, float *w, float *h);
void drawRect(float x, float y, float width, float height, vec4_t color, int borderSize);
void drawFillRect(float x, float y, float width, float height, const float *color);
void drawPic(float x, float y, float width, float height, qhandle_t hShader);
qboolean worldToScreen(vec3_t worldCoord, int *x, int *y);
void drawText(float x, float y, float scale, char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font);
int sizeTextHeight(float scale, fontInfo_t *font);
int sizeTextWidth(float scale, char *text, fontInfo_t *font);
void drawTextRect(float x, float y, float scale, char *text, vec4_t textColor, qboolean forceColor, qboolean textShadow, qboolean background, qboolean border, vec4_t backgroundColor, vec4_t borderColor, fontInfo_t *font);

/*
==============================
from eth.c
==============================
*/

#define ETH_ANNOUNCE_STR "ETH"
#define ETH_REQUEST_STR ETH_ANNOUNCE_STR "W"

// Killing sprees
#define SPREE_TIME 5

// log
void ethLog(const char *format, ...);
#ifdef ETH_DEBUG
	extern FILE *debugFile;
	void ethDebug(const char *format, ...);
#endif

// Utils
char *getOutputSystemCommand(const char *command);
int getSpawntimer (qboolean enemySpawn);
int findSatchel();
void fatalError(const char *msg);
qboolean isKeyActionDown(char *action);
qboolean isVisible(vec3_t target);
qboolean isPlayerVisible(vec3_t target, int player);
int getIdByName (const char *, int);
void gameMessage(qboolean forceConsole, char *format, ...);
void doAutoVote (char *);
void autoRequestMedic();
// Killing spree
void playSpreeSound();
void killSpam();
void deathSpam();
// Auto demo record
void autoRecord();
// Config
void readConfig();
void writeConfig();
// For game console actions
void initActions();
void setAction(int action, int state);
void forceAction(int action, int state);
void resetAllActions();
void setCurrentMuzzle();
char *Format(char *in);
void doMusicSpam();
qboolean isAimbotWeapon(int weapon);

// ET guid changer
#define ET_GUID_SIZE 40	// is the max guid size
void loadCL_GUID (void);

/*
==============================
from engine.c
==============================
*/

void eth_CG_DrawActiveFrame();
void eth_CG_GetSnapshot(int snapshotNumber, snapshot_t *snapshot);
void eth_CG_Init();
void eth_CG_Shutdown();
void eth_CG_R_AddRefEntityToScene(refEntity_t *re);
void eth_CG_S_UpdateEntityPosition(int entityNum, const vec3_t origin);
qboolean eth_CG_R_RenderScene(refdef_t *refdef);
qboolean eth_CG_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
void eth_CG_FinishWeaponChange(int lastweap, int newweap);
void eth_CG_EntityEvent(centity_t *cent, vec3_t position);
void eth_CG_DamageFeedback(int, int, int);
void eth_CG_WeaponFireRecoil(int weapon);
void eth_CG_Missile( centity_t *cent );
// cvars unlocker
cvar_t *(*orig_Cvar_Set2)(const char *var_name, const char *value, qboolean force);
cvar_t *eth_Cvar_Set2(const char *var_name, const char *value, qboolean force);

/*
==============================
from g_functions.c
==============================
*/

char *Q_CleanStr(char *string);
int Q_PrintStrlen(const char *string);
char *eth_Info_ValueForKey(const char *s, const char *key);
void eth_CG_ParseReinforcementTimes(const char *pszReinfSeedString);
int eth_CG_Text_Width_Ext(const char *text, float scale, int limit, fontInfo_t* font);
int eth_CG_Text_Height_Ext(const char *text, float scale, int limit, fontInfo_t* font);
void eth_CG_Text_Paint_Ext(float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font);
qboolean eth_CG_GetTag(int clientNum, char *tagname, orientation_t *or);
float *eth_CG_FadeColor(int startMsec, int totalMsec);
void eth_BG_EvaluateTrajectoryDelta(const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splineData);

// cgame/cg_syscalls.c
#define PASSFLOAT(x) (*(int*)&x)
void syscall_CG_Cvar_Set(const char *var_name, const char *value);
void syscall_CG_Cvar_Register(vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags);
void syscall_CG_Cvar_Update(vmCvar_t *vmCvar);
void syscall_CG_Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);
void syscall_CG_SendConsoleCommand(const char *text);

void syscall_CG_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
void syscall_CG_R_SetColor(const float *rgba );

sfxHandle_t	syscall_CG_S_RegisterSound(const char *sample, qboolean compressed);
void syscall_CG_S_StartLocalSound( sfxHandle_t sfx, int channelNum);

qboolean syscall_CG_Key_IsDown(int keynum);
void syscall_CG_R_AddRefEntityToScene(const refEntity_t *re);
qhandle_t syscall_CG_R_RegisterShader(const char *name);
qhandle_t syscall_CG_R_RegisterShaderNoMip(const char *name);
int syscall_CG_R_LerpTag(orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex);
void syscall_CG_R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs);
void syscall_R_AddLightToScene( const vec3_t org, float radius,float intensity, float r, float g, float b, qhandle_t hShader, int flags );
void syscall_R_ProjectDecal( qhandle_t hShader,int numPoints, vec3_t *points,vec4_t projection, vec4_t color,int lifeTime, int fadeTime );

const char *syscall_UI_Argv(int);
int syscall_UI_Argc();
const char *syscall_CG_Argv(int);
int syscall_CG_Argc();

void orig_Cmd_RemoveCommand(const char *cmdName);

/*
==============================
from hook.c
==============================
*/

void initETstruct();
void initEthCgameStruct();
// System hooked functions
void *eth_dlsym (void *handle, const char *symbol);
void *eth_dlopen (const char *filename, int flag);
int eth_XNextEvent(Display *display, XEvent *event);

// et.x86 function
void (*orig_Cmd_AddCommand) (const char *cmd_name, void *function);
void (*orig_Cbuf_ExecuteText)(int exec_when, const char *text);
void (*orig_COM_StripExtension) (const char *, char *);
void eth_COM_StripExtension (const char *, char *);
int (*orig_syscall) (int command, ...);

// 'Pure pk3' unlocker
char goodChecksumsPak[BIG_INFO_STRING];
void (*orig_FS_PureServerSetLoadedPaks)(const char *pakSums, const char *pakNames);
void eth_FS_PureServerSetLoadedPaks(const char *pakSums, const char *pakNames);
pack_t *getPack(char *filename);
void unreferenceBadPk3();
// cgame lib hooked functions
int eth_CG_syscall(int command, ...);
void (*orig_CG_dllEntry) (int (*syscallptr)(int arg,... ));
void eth_CG_dllEntry (int (*syscallptr)(int arg,... ));
int (*orig_CG_vmMain) (int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);
int eth_CG_vmMain(int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);
// ui lib hooked functions
int eth_UI_syscall(int command, ...);
void (*orig_UI_dllEntry) (int (*syscallptr)(int arg,... ));
void eth_UI_dllEntry (int (*syscallptr)(int arg,... ));
int (*orig_UI_vmMain) (int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);
int eth_UI_vmMain (int command, int arg0, int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10, int arg11);
// etpro guid changer
#define ETPRO_GUID_SIZE 40	// is the max guid size
char *etproGuid;
int etproOSChecksum;
void (*orig_etproAntiCheat)(void *, void *, void *, int checksum, void *, char *guid);
void etproAntiCheat (void *, void *, void *, int checksum, void *, char *guid);
// backup etproAntiCheat func args
void *acA,*acB,*acC,*acE;
// Protect eth files
int (*orig_FS_ReadFile) (const char *, void **);
int eth_FS_ReadFile (const char *, void **);

/*
==============================
from medias.c
==============================
*/
extern char *pak0Sounds[];
void initCustomSounds();
void registerEthMedias();

/*
==============================
from medicbot.c
==============================
*/

#define MEDICBOT_REQUEST "!mb "
void doMedicBotCommand(const int player, char *msg);
void doMedicBot();

/*
==============================
from spycam.c
==============================
*/

void drawSpyCam(float x, float y, float width, float height, refdef_t *refDef, qhandle_t enemyShader, qhandle_t friendShader, qboolean crosshair);
void drawSatchelCam();
void drawMortarCam();
void drawPanzerCam();
void drawMirrorCam();
void drawRiffleCam();

/*
==============================
from visuals.c
==============================
*/

extern vec4_t gameBackColor;
extern vec4_t gameBorderColor;
extern vec4_t gameFontColor;

void drawAdvert();
void drawEspEntities();
void drawRadar(int scale);
void drawRadarNg(int scale);
void drawSpawnTimerRight();
void drawBanner();
void drawSpectators();
void drawIrcFireteam();
void drawIrcChat();
void drawIrcTopic();
qboolean drawLastInvite();
void drawReferees();
void setColors();
void addEspColors(int entityNum, qboolean visible);
void addChams(refEntity_t* refEnt, qboolean visible);
void drawESPref (vec3_t origin, qhandle_t shader, int entityNum);
void drawSpreeText();
void drawMortarTrace(int entityNum);
void drawCrosshair();
// Friend/enemy player list selector
void plistcheck(int pnum);
void drawPlayerList();
// hud
int isAimableWeapon();
void drawETHhud();
char console_p[5][1024];
char chat_p[3][1024];
void initHUD();
/*
==============================
from windows.c
==============================
*/
void initWindow(const char *wname,int x,int y,int w,int h,float r,float g,float b,float a);
void getWindowStatus(eth_window_t *window);
void createWindow (eth_window_t *win, const char *wname);


#ifdef ETH_DEBUG
	// Debug view
	int getVarOffset(int var);
	void crossRailTrail(vec3_t point);
	void drawMemDump(int offset);
	void drawPredictStats(vec3_t origin, vec3_t real, vec3_t predict);
#endif

/*
===============================
from punkbuster.c
===============================
*/
void (*orig_glReadPixels)( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
void eth_glReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );

#endif /*ETH_H_*/

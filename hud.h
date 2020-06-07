#ifndef HUD_H_
#define HUD_H_ 

#define MAX_OUTPUT_MSGS 10
#define HUDOUTPUT_MAXLEN 90

#define MAX_CONSOLE_MSGS 10
#define MAX_CHAT_MSGS 10

typedef enum { hudinfo, hudwarning } hudicontype;

typedef struct {
	int lastCheck;
	int ramTotal;
	int ramUsed;
	float cpuUsed;
	int fpsMax;
	int fpsCurrent;
	int lastFpsCheck;
	int fpsFrameCount;
} hudsystem_t;

typedef struct {
	int leftNextY;
	int bottomChatX;
	fontInfo_t font1;
	fontInfo_t font2;
	fontInfo_t font3;
	int lastID;
	int lastHP;
} hudglobal_t;

typedef struct {
	char msg[HUDOUTPUT_MAXLEN];
	hudicontype type;
} hudoutput_t;

typedef struct {
	int time;
	int hp;
} hudHealthPopup_t;

typedef struct {
	char msg[1024];
	int time;
} hudChatMsgs_t;

typedef struct {
	char msg[1024];
	int time;
} hudConsoleMsgs_t;

extern hudChatMsgs_t hudChatMsgs[MAX_CHAT_MSGS];
extern hudConsoleMsgs_t hudConsoleMsgs[MAX_CONSOLE_MSGS];

void registerHudStuff();
void drawCustomHud();
void drawHudTimers();
void drawHudSystemStats();
void drawBar(float x, float y, float width, float height, float backValue, float frontValue, vec4_t backColor, vec4_t frontColor);
float scaleFromHeight(int height, fontInfo_t *font);
void drawHudOutput();
void addHudOutputMsg(hudicontype type, const char *format, ...);
void drawHudWeaponInfo();
qboolean showHudWeaponAmmo(int weapon);
void drawHudVictimMsg();
void drawHudPowerStats();
void addHudHealthChange(int hp, int time);
void drawHudMisc();
void addHudConsoleMsg(char *msg);
void addHudConsoleChat(char *msg);
void drawHudConsoleOutput();
void drawHudChat();
float getFadeValue(int startTime, int stayTime);
qboolean isChatMsg(char *msg);

#endif

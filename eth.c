// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

#include <unistd.h>

#include "eth.h"

/*
==============================
Some utils functions
==============================
*/

int getSpawntimer(qboolean enemySpawn) {
	int team = eth.clientInfo[eth.cg_snap->ps.clientNum].team;
	int limbotime = 0;

	// Reverse the value from clientinfo[myself].team to make the function return the spawn time of the other team
    if(enemySpawn == qtrue) {
		if (team == TEAM_AXIS) {
			team = TEAM_ALLIES;
			limbotime = eth.cg_bluelimbotime;
		} else if (team == TEAM_ALLIES) {
			team = TEAM_AXIS;
			limbotime = eth.cg_redlimbotime;
		} else
			return -1;
    } else {
		if (team == TEAM_AXIS) {
			limbotime = eth.cg_redlimbotime;
		} else if (team == TEAM_ALLIES) {
			limbotime = eth.cg_bluelimbotime;
		} else
			return -1;
    }
    
    // Sanity check
    if (limbotime == 0) {
    	ethLog("warning: can't get spawntimer for team %i", team);
    	return -1;
    }

    return (int)(1 + (limbotime - ((eth.cgs_aReinfOffset[team] + eth.cg_time - eth.cgs_levelStartTime) % limbotime)) * 0.001f);
}

int findSatchel() {
	int entityNum = 0;
	for (; entityNum < MAX_GENTITIES; entityNum++) {
		if ((eth.cg_entities[entityNum].currentState->weapon == WP_SATCHEL)
				&& (eth.cg_entities[entityNum].currentState->clientNum == eth.cg_snap->ps.clientNum)
				&& (!VectorCompare(eth.entities[entityNum].origin, vec3_origin)))
			return entityNum;
	}
	#ifdef ETH_DEBUG
		ethDebug("satchel cam: don't find satchel for %i", eth.cg_snap->ps.clientNum);
	#endif
	return -1;
}

qboolean isKeyActionDown(char *action) {
	int key1, key2;
	orig_syscall(CG_KEY_BINDINGTOKEYS, action, &key1, &key2);

	if (syscall_CG_Key_IsDown(key1) || syscall_CG_Key_IsDown(key2))
		return qtrue;
	else
		return qfalse;
}

int getIdByName (const char *name, int len) {
	int i;
	if (!len)
		len = strlen(name);
	
	for (i=0; i < MAX_CLIENTS; i++)
		if (eth.clientInfo[i].infoValid && !strncmp(eth.clientInfo[i].name, name, len))
			return i;
	return -1;
}

qboolean isVisible(vec3_t target) {
	trace_t trace;
	eth.CG_Trace(&trace, eth.cg_refdef.vieworg, NULL, NULL, target, eth.cg_snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_CORPSE);
	return (trace.fraction == 1.0f);
}

qboolean isPlayerVisible(vec3_t target, int player) {
	trace_t traceVisible;
	eth.CG_Trace(&traceVisible, eth.cg_refdef.vieworg, NULL, NULL, target, eth.cg_snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_CORPSE);

	trace_t tracePlayer;
	eth.CG_Trace(&tracePlayer, eth.cg_refdef.vieworg, NULL, NULL, target, eth.cg_snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_BODY);

	//printf("XXX: want %i got %i\n", player, tracePlayer.entityNum);

	return ((traceVisible.fraction == 1.0f) && (tracePlayer.entityNum == player));
}

void ethLog(const char *format, ...) {
	printf("eth: ");

	va_list arglist;
	va_start(arglist, format);
		vprintf(format, arglist);
	va_end(arglist);

	printf("\n");

	#ifdef ETH_DEBUG
		// If log file
		if (debugFile) {
			fprintf(debugFile, "log: ");
			va_list arglist;
			va_start(arglist, format);
				vfprintf(debugFile, format, arglist);
			va_end(arglist);	
			fprintf(debugFile, "\n");
		}
	#endif
}

#ifdef ETH_DEBUG

FILE *debugFile = NULL;
void ethDebug(const char *format, ...) {
	// If log file
	if (debugFile) {
		va_list arglist;
		va_start(arglist, format);
			vfprintf(debugFile, format, arglist);
		va_end(arglist);	
		fprintf(debugFile, "\n");
	}
}

#endif // ETH_DEBUG

void fatalError(const char *msg) {
	char str[256];
	snprintf(str, sizeof(str), "A fatal error has occured. You must restart the game.\n\nError: %s\n\n\n*nixCoders eth-v%s", msg, ETH_VERSION);
	orig_syscall(CG_ERROR, str);
	#ifdef ETH_DEBUG
		ethLog("fatal error: %s", msg);
	#endif
}

// Helper function for get ouput of a system command
// This function return the first line of this command output
char *getOutputSystemCommand(const char *command) {
	static char buf[256];
	memset(buf, 0, sizeof(buf));

	FILE *cmd = popen(command, "r");

	int c; 
	int count = 0;
	while (((c = getc(cmd)) != EOF) && (count < (sizeof(buf) - 1))) {
		if (c == '\n')
			break;
		buf[count++] = c;
	}

	pclose(cmd);
	return buf;
}

// If cgame is load, send a UI_PRINT message else just show a message at console,
// WARNING ! use this function only when call from ui vmMain. ex: irc and game command
#define MESSAGE_COLOR "^n"
void gameMessage(qboolean forceConsole, char *format, ...) {
	char msg[MAX_SAY_TEXT];
	memset(msg, 0, sizeof(msg));
	char buffer[sizeof(msg)];
	memset(buffer, 0, sizeof(buffer));
	va_list arglist;

	va_start(arglist, format);
		vsnprintf(msg, sizeof msg, format, arglist);
	va_end(arglist);

	if (eth.hookLoad && !forceConsole) {
		snprintf(buffer, sizeof(buffer), "echo \"" MESSAGE_COLOR "%s\"\n", msg);
		orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buffer);
	} else {
		snprintf(buffer, sizeof(buffer), MESSAGE_COLOR "%s\n", msg);
		orig_syscall(UI_PRINT, buffer);
	}
}


void doAutoVote(char *str) {
	char *ptr = strstr(str, "^7 called a vote.  Voting for: ");
	int id = getIdByName(str, ptr - str);

	// id not found
	if (id == -1)
		return;

	// who votes
	switch (eth.clientInfo[id].targetType) {
		case PLIST_FRIEND:	
			syscall_CG_SendConsoleCommand("vote yes\n");
			return;
		case PLIST_TEAMKILLER:
			syscall_CG_SendConsoleCommand("vote no\n");
			return;
		default:
			break;
	}
	
	// Voter is not a friend/teamkiller
	ptr += strlen("^7 called a vote.  Voting for: ");

	if (!strncmp(ptr, "KICK ", 5) || !strncmp(ptr, "MUTE ", 5)) {
		ptr += 5;
		id = getIdByName(ptr, strrchr(ptr, '\n') - ptr);

		// id not found
		if (id == -1)
			return;

		// vote against client
		if (id == eth.cg_clientNum) {
			syscall_CG_SendConsoleCommand("vote no\n");
			return;
		}
		
		// who is voted against
		switch (eth.clientInfo[id].targetType) {
			case PLIST_FRIEND:
				syscall_CG_SendConsoleCommand("vote no\n");
				return;
			case PLIST_TEAMKILLER:
				syscall_CG_SendConsoleCommand("vote yes\n");
				return;
			default:
				break;
		}
	}
}

// Auto request medic
#define TIME_BETWEEN_REQUEST 5000
void autoRequestMedic() {
	if (eth.cg_snap->ps.clientNum != eth.cg_clientNum)
		return;

	static int lastRequestTime = 0;
	if (lastRequestTime + TIME_BETWEEN_REQUEST > eth.cg_time)
		return;

	// If need revive
	if (eth.cg_entities[eth.cg_clientNum].currentState->eFlags & EF_DEAD) {
		int vsay = (int)(2.0f * (rand() / (RAND_MAX + 1.0f)));
		if (!vsay)
			syscall_CG_SendConsoleCommand("vsay_team Medic\n");
		else
			syscall_CG_SendConsoleCommand("vsay_team FTReviveMe\n");
		
		lastRequestTime = eth.cg_time;
	// If need health
	} else if (eth.cg_snap->ps.stats[STAT_HEALTH] < 40) {
		int vsay = (int)(2.0f * (rand() / (RAND_MAX + 1.0f)));
		if (!vsay)
			syscall_CG_SendConsoleCommand("vsay_team Medic\n");
		else
			syscall_CG_SendConsoleCommand("vsay_team FTHealMe\n");
		
		lastRequestTime = eth.cg_time;
	}
}

// Calc muzzle
void setCurrentMuzzle() {
	vec3_t forward, right, up;

	VectorCopy(eth.cg_snap->ps.origin, eth.muzzle);
	eth.muzzle[2] += eth.cg_snap->ps.viewheight;
	AngleVectors(eth.cg_snap->ps.viewangles, forward, right, up);

	switch (eth.cg_snap->ps.weapon)	{
		case WP_PANZERFAUST:
			VectorMA(eth.muzzle, 10, right, eth.muzzle);
			break;
		case WP_DYNAMITE:
		case WP_GRENADE_PINEAPPLE:
		case WP_GRENADE_LAUNCHER:
		case WP_SATCHEL:
		case WP_SMOKE_BOMB:
			VectorMA(eth.muzzle, 20, right, eth.muzzle);
			break;
		case WP_AKIMBO_COLT:
		case WP_AKIMBO_SILENCEDCOLT:
		case WP_AKIMBO_LUGER:
		case WP_AKIMBO_SILENCEDLUGER:
			VectorMA(eth.muzzle, -6, right, eth.muzzle);
			VectorMA(eth.muzzle, -4, up, eth.muzzle);
			break;
		default:
			VectorMA(eth.muzzle, 6, right, eth.muzzle);
			VectorMA(eth.muzzle, -4, up, eth.muzzle);
			break;
	}
	SnapVector(eth.muzzle);
}

qboolean isAimbotWeapon(int weapon){
	switch( weapon ){
		case WP_AMMO:
		case WP_ARTY:
		case WP_MEDIC_SYRINGE:
		case WP_DYNAMITE:
		case WP_SMOKETRAIL:
		case WP_MAPMORTAR:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_PLIERS:
		case WP_LANDMINE:
		case WP_SATCHEL:
		case WP_TRIPMINE:
		case WP_SMOKE_BOMB:
		case WP_MORTAR:
		case WP_MEDIC_ADRENALINE:
			return qfalse;
		default:
			return qtrue;
	}
}

/*
==============================
 Config stuff
==============================
*/

char *getConfigFilename() {
	// If config filename is set by enviromment var
	if (getenv("ETH_CONF_FILE"))
		return getenv("ETH_CONF_FILE");
	
	static char filename[PATH_MAX];
	snprintf(filename, sizeof(filename), "%s/%s", getenv("HOME"), ETH_CONFIG_FILE);
	return filename;
}

void readConfig() {
	FILE *file;
	// Init all user vars with the default value
	int count = 0;
	for (; count < VARS_TOTAL; count++)
		seth.value[count] = seth.vars[count].defaultValue;

	if ((file = fopen(getConfigFilename(), "rb")) == NULL)
		return;

	// Get config file line by line
	char line[32];
	while (fgets(line, sizeof(line) - 1, file) != 0) {
		char *sep = strrchr(line, '=');
		*sep = '\0';	// Separate name from value
		// Search this var
		int count = 0;
		for (; count < VARS_TOTAL; count++) {
			if (!seth.vars[count].cvarName) {
				ethLog("readConfig: error: VAR_%i undefine", count);
			} else if (!strcmp(line, seth.vars[count].cvarName)) {
				seth.value[count] = atof(sep + 1);
				break;
			} else if ((count + 1) == VARS_TOTAL) {
				ethLog("readConfig: don't know this var: [%s]", line);
			}
			if (count == VAR_CHUD)
				if (atoi(sep + 1))
					syscall_CG_SendConsoleCommand(va("cg_draw2d %s\n", seth.value[count] ? "0" : "1"));
		}
	}

	fclose(file);

}

void writeConfig() {
	FILE *file;
	
	if ((file = fopen(getConfigFilename(), "w")) == NULL) {
		ethLog("eth: can't write config file.");
		return;
	}
	
	int count = 0;
	for (; count < VARS_TOTAL; count++) {
		if (!seth.vars[count].cvarName)
			ethLog("writeConfig: error: VAR_%i undefine", count);
		else if (seth.value[count] == (float)(int)seth.value[count])
			fprintf(file, "%s=%i\n", seth.vars[count].cvarName, (int)seth.value[count]);
		else
			fprintf(file, "%s=%.2f\n", seth.vars[count].cvarName, seth.value[count]);
	}

	fclose(file);
}

/*
==============================
 Console actions
==============================
*/

void initActions() {
	eth.actions[ACTION_ATTACK]		= (ethAction_t){ 0, 0, "+attack\n",				"-attack\n" };
	eth.actions[ACTION_BACKWARD]	= (ethAction_t){ 0, 0, "+backward\n",			"-backward\n" };
	eth.actions[ACTION_BINDMOUSE1]	= (ethAction_t){ 1, 1, "bind mouse1 +attack\n",	"unbind mouse1\n" };
	eth.actions[ACTION_CROUCH]		= (ethAction_t){ 0, 0, "+movedown\n",			"-movedown\n" };
	eth.actions[ACTION_JUMP]		= (ethAction_t){ 0, 0, "+moveup\n",				"-moveup\n" };
	eth.actions[ACTION_PRONE]		= (ethAction_t){ 0, 0, "+prone; -prone\n",		"+moveup; -moveup\n" };
	eth.actions[ACTION_RUN]			= (ethAction_t){ 0, 0, "+sprint; +forward\n",	"-sprint; -forward\n" };
	eth.actions[ACTION_SCOREBOARD]	= (ethAction_t){ 0, 0, "cg_draw2d 1; +scores\n", "-scores; cg_draw2d 0\n" };
	eth.actions[ACTION_RELOAD]			= (ethAction_t){ 0, 0, "+reload\n",	"-reload\n" };
}

// Set an action if not already set.
void setAction(int action, int state) {
	if (state && !eth.actions[action].state)
		forceAction(action, state);
	else if (!state && eth.actions[action].state)
		forceAction(action, state);
}

// Use this with caution
void forceAction(int action, int state) {
	if (state) {
		eth.actions[action].state = 1;
		syscall_CG_SendConsoleCommand(eth.actions[action].startAction);
		#ifdef ETH_DEBUG
			ethDebug("forceAction: %s", eth.actions[action].startAction);
		#endif
	} else {
		eth.actions[action].state = 0;
		syscall_CG_SendConsoleCommand(eth.actions[action].stopAction);
		#ifdef ETH_DEBUG
			ethDebug("forceAction: %s", eth.actions[action].stopAction);
		#endif
	}
}

void resetAllActions() {
	int action;
	for (action=0; action<ACTIONS_TOTAL; action++)
		if (action != ACTION_SCOREBOARD)
			forceAction(action, eth.actions[action].defaultState);
}

/*
==============================
 Killing spree/stats/sound stuff
==============================
*/

// Spree sound
void playSpreeSound() {
	if( !seth.value[VAR_SPREE_SOUNDS] )
		return;
	
	if ((eth.cg_time - eth.lastKillTime) < (SPREE_TIME * 1000)) {
		// Define spree levels range - TODO: Dirty. find a better way to play with level range
		typedef struct { int start; int end; } spreeLevel_t;
		#define SPREE_LEVEL_SIZE 4
		spreeLevel_t spreeLevels[SPREE_LEVEL_SIZE] = {
			{ SOUND_DOUBLEKILL1,	SOUND_PERFECT }	,	// Spree level 1
			{ SOUND_GODLIKE1,		SOUND_TRIPLEKILL },
			{ SOUND_DOMINATING1,	SOUND_ULTRAKILL3 },
			{ SOUND_MONSTERKILL1,	SOUND_WICKEDSICK }	// Spree level 4
		};
		int spreeLevelMax = SPREE_LEVEL_SIZE - 1;

		// Modify level+sound values to fit to spreeLevels_t and eth.sounds order
		int level = eth.killSpreeCount - 2; // never '< 0' because first time here is with 2 kills
		if (level > spreeLevelMax)
			level = spreeLevelMax;
		int levelSize = spreeLevels[level].end - spreeLevels[level].start;
		int sound = (int)((float)(levelSize + 1) * rand() / (RAND_MAX + 1.0f));
		sound += spreeLevels[level].start;
		eth.spreelevel = sound;
		eth.startFadeTime = eth.cg_time;
		eth.s_level = level;
		orig_syscall(CG_S_STARTLOCALSOUND, eth.sounds[sound], CHAN_LOCAL_SOUND, 230);
	}
}

/* replaces tokens with correct value and returns pointer parsed string
	[c] - current spree count
	[v] - victim name
	[n] - clean victim name
	[t] - total kills
	[m] - double/triple/multikill msg
	[x]	- Player XP
	[p] - Player name
	[M] - Mod name
	[h] - Player health
	[k] - Killer name
	[K] - Clean killer name
	[q] - amount of punkbuster screenshots requested
*/
char *Format(char *in){
	char out[256];
	memset(out, 0, sizeof(out));
	
	int a, b, len = strlen(in);
	for (a = 0, b = 0; a < len; a++) {
		if ((a <= len-2) && (in[a] == '[' && in[a+2] == ']')) {
			switch (in[a+1]) {
				case 'v':
					strncat(out, eth.VictimName, sizeof(out));
					break;
				case 'n':
					strncat(out, Q_CleanStr(eth.VictimName), sizeof(out));
					break;
				case 't':
					strncat(out, va("%i", eth.killCount), sizeof(out));
					break;
				case 'x':
					strncat(out, va("%i", eth.cg_snap->ps.stats[STAT_XP]), sizeof(out));
					break;
				case 'q':
					strncat(out, va("%i", eth.pbss), sizeof(out));
					break;					
				case 'p':
					strncat(out, eth.clientInfo[eth.cg_clientNum].name, sizeof(out));
					break;
				case 'M':
				{
					char gameMod[20];
					syscall_CG_Cvar_VariableStringBuffer("fs_game", gameMod, sizeof(gameMod));
					strncat(out, gameMod, sizeof(out));
					break;
				}
				case 'k':
					strncat(out, eth.KillerName, sizeof(out));
					break;
				case 'K':
				{
					char *c = strdup(eth.KillerName);
					strncat(out, Q_CleanStr(c) , sizeof(out));
					free(c);
					break;
				}
				case 'h':
					strncat(out, va("%i", eth.cg_snap->ps.stats[STAT_HEALTH]), sizeof(out));
					break;								
				case 'c':
					strncat(out, va("%i", eth.killCountNoDeath), sizeof(out));
					break;
					case 'm': {
						char *multikillmsg;
						int randmsg = rand() % 3;
						if (eth.killSpreeCount == 2) {
							if (!randmsg)
								multikillmsg = "DOUBLE KILL";
							else if (randmsg == 1)
								multikillmsg = "PERFECT";
							else
								multikillmsg = "OUTSMARTED";
						}
						else if (eth.killSpreeCount == 3) {
							if (!randmsg)
								multikillmsg = "TRIPLE KILL";
							else if (randmsg == 1)
								multikillmsg = "GODLIKE";
							else
								multikillmsg = "LUDICROUS";
						}
						else if (eth.killSpreeCount == 4) {
							if (!randmsg)
								multikillmsg = "DOMINATING";
							else if (randmsg == 1)
								multikillmsg = "ULTRA KILL";
							else
								multikillmsg = "MEGA KILL";
						}
						else if (eth.killSpreeCount > 4) {
							if (!randmsg)
								multikillmsg = "WICKED SICK";
							else if (randmsg == 1)
								multikillmsg = "MONSTER KILL";
							else
								multikillmsg = "BOOM HEADSHOT";
						}
						else
							break;
						strncat(out, multikillmsg, sizeof(out));
						break;
					}
				default:
					break;
			}
			a += 2;
		}
		else
			out[b] = in[a];
		b = strlen(out);
	}

	strncpy(in,out,sizeof(out));
	return in;
}

// Customizable spam
void killSpam() {
	char msg[256];
	char send[256];
	strncpy(msg,ksFormat,sizeof(msg));
	snprintf(send, sizeof(send), "say \"%s\"\n", Format(msg));
	syscall_CG_SendConsoleCommand(send);
}

void deathSpam() {
	char send[256];
	char msg[256];
	strncpy(msg,dieFormat,sizeof(msg));
	snprintf(send, sizeof(send), "say \"%s\"\n", Format(msg));
	syscall_CG_SendConsoleCommand(send);
}

// Auto demo record
void autoRecord() { 
	#define TMP_FILE_NAME "demo_in_progress"
	static qboolean demoState = qfalse;
	static int stateTime ;
	int ethRate;
	
	// Speed return if we don't want record demo
	if (!demoState && !seth.value[VAR_RECDEMO])
		return;

	if ((eth.cg_snap->ps.eFlags != EF_DEAD)
			&& (eth.cg_snap->ps.clientNum == eth.cg_clientNum)
			&& (eth.clientInfo[eth.cg_clientNum].team != TEAM_SPECTATOR)
			&& (eth.cg_snap->ps.stats[STAT_HEALTH] > 0)
			&& (eth.cg_snap->ps.pm_flags != PMF_FOLLOW)
			&& seth.value[VAR_RECDEMO]
			&& !demoState) {
		
		stateTime=0 ;
		// FIXME: check if a tmp demo file already exist
		syscall_CG_SendConsoleCommand("record " TMP_FILE_NAME "\n");
		stateTime = eth.cg_time;
		
		#ifdef ETH_DEBUG
			ethDebug("demo: start");
		#endif

		demoState = qtrue;
		
	} else if (((eth.cg_snap->ps.eFlags == EF_DEAD)
			|| (eth.cg_snap->ps.clientNum != eth.cg_clientNum)
			|| (eth.clientInfo[eth.cg_clientNum].team == TEAM_SPECTATOR)
			|| (eth.cg_snap->ps.stats[STAT_HEALTH] <= 0)
			|| (eth.cg_snap->ps.pm_flags == PMF_FOLLOW)
			|| (!seth.value[VAR_RECDEMO])) 
				&& demoState) {
				
		char buffFrom[MAX_OSPATH];
		char buffTo[MAX_OSPATH];

		// Get fsgame
		char fsGame[MAX_QPATH];
		syscall_CG_Cvar_VariableStringBuffer("fs_game", fsGame, sizeof(fsGame));

		// Get current time
		time_t ethTime = time(NULL);
		struct tm *localTime = localtime(&ethTime);

		orig_Cbuf_ExecuteText(EXEC_NOW, "stoprecord\n");

		stateTime = ((eth.cg_time - stateTime)/1000) ; 
		ethRate=(int)((((float)eth.autoDemoKillCount/(float)stateTime)*100.0f)/(float)(0.2)) ;
		
		#ifdef ETH_DEBUG
			ethDebug("demo: final stateTime:[%i]", stateTime) ;
			ethDebug("demo: kill acc:[%i]",ethRate);
			ethDebug("demo: stop");
		#endif
		
		// buffForm = /homepath/fsgame/
		syscall_CG_Cvar_VariableStringBuffer("fs_homepath", buffFrom, sizeof(buffFrom));
		sprintf(buffFrom, "%s/%s/demos/", buffFrom, fsGame);

		strncpy(buffTo, buffFrom, sizeof(buffFrom));

		// buffForm = /homepath/fsgame/demos/demoinprogress
		strcat(buffFrom, TMP_FILE_NAME ".dm_");

		// Contruct final file name, buffTo = /homepath/fsgame/demo/aaaa_mm_dd_hhmmss_mod_map_killcount
		sprintf(buffTo,"%s%i-%.2i-%.2i-%.2i%.2i%.2i_%s_%s_%ikills_%isec_%irate.dm_", buffTo, localTime->tm_year + 1900, localTime->tm_mon + 1,
				localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, fsGame,
				eth_Info_ValueForKey(eth.cgs_gameState->stringData + eth.cgs_gameState->stringOffsets[CS_SERVERINFO], "mapname"),
				eth.autoDemoKillCount, stateTime, ethRate);
		
		// Version of the demo
		if (!strcmp(sethET->version, "2.55")){
			strcat(buffFrom, "82");
			strcat(buffTo, "82");
		} else if(!strcmp(sethET->version, "2.56")){
			strcat(buffFrom, "83");
			strcat(buffTo, "83");
		} else {
			strcat(buffFrom, "84");
			strcat(buffTo, "84");
		}

		#ifdef ETH_DEBUG
			ethDebug("demo: got %i kills, need %i kills", eth.autoDemoKillCount, (int)seth.value[VAR_RECDEMO]);
		#endif

		// If we more than kills threshold then rename the demo
		if (eth.autoDemoKillCount >= seth.value[VAR_RECDEMO]) {
			#ifdef ETH_DEBUG
				ethDebug("demo: move [%s] to [%s]", buffFrom, buffTo);
			#endif
			rename(buffFrom, buffTo);
		// Else delete it
		} else {
			#ifdef ETH_DEBUG
				ethDebug("demo: delete [%s]", buffFrom, buffTo);
			#endif
			unlink(buffFrom);
		}

		eth.autoDemoKillCount = 0;
		demoState = qfalse;
	}
}

/*
==============================
et guid
==============================
*/

// Init cl_guid by environnement var
void loadCL_GUID(void) {
	char *cl_guid = getenv("CLGUID");
	if (cl_guid) {
		syscall_CG_Cvar_Set("cl_guid", cl_guid);
		// Delete env var so cl_guid is set once, and not each cg_init
		unsetenv("CLGUID");
	}
}

void doMusicSpam() {
	static char oldTitle[MAX_STRING_CHARS];
	static qboolean firstTime = qtrue;
	char *artist,*currentTime,*totalTime,*title;
	artist=currentTime=totalTime=title=NULL;

	if( firstTime ){
		oldTitle[0] = '\0';
		firstTime = qfalse;
		return;
	}

	if( seth.value[VAR_MUSICSPAM]==1 )
		title = strdup(getOutputSystemCommand("dcop amarok player title"));
	else if (seth.value[VAR_MUSICSPAM]==2)
		title = strdup(getOutputSystemCommand("xmmsctrl print %T"));
	else
		return;

	if( strcmp(title,oldTitle) ){	//new song
		char msg[MAX_STRING_CHARS];
		strncpy(oldTitle,title,MAX_STRING_CHARS);
		if( seth.value[VAR_MUSICSPAM]==1 ){
			artist = strdup(getOutputSystemCommand("dcop amarok player artist"));
			currentTime = strdup(getOutputSystemCommand("dcop amarok player currentTime"));
			totalTime = strdup(getOutputSystemCommand("dcop amarok player totalTime"));
		} else if( seth.value[VAR_MUSICSPAM]==2 ) {
			artist = NULL; // no artist print in xmmsctrl?? wtf?
			currentTime = strdup(getOutputSystemCommand("xmmsctrl print %m"));
			totalTime = strdup(getOutputSystemCommand("xmmsctrl print %M"));
		}

		if( artist ){
			sprintf(msg,"say \"^oPlaying ^0[^w%s - %s^0]^*-^0[^w%s/%s^0]\"\n", artist, title, currentTime, totalTime);
			free(artist);
		} else
			sprintf(msg,"say \"^oPlaying ^0[^w%s^0]^*-^0[^w%s/%s^0]\"\n", title, currentTime, totalTime);

		syscall_CG_SendConsoleCommand(msg);
		
		free(currentTime);
		free(totalTime);
	}
	free(title);
}

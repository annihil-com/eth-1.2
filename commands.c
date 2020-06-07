/*
==============================
 Game commands
==============================
*/

#include "eth.h"

void sysInfoCommand() {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nSpam with system infos\n");
		orig_syscall(UI_PRINT, "^nUsage: eth_sysinfo\n");
		return;
	}
	
	char *kernel = strdup(getOutputSystemCommand("uname -sr"));
	char *proc = strdup(getOutputSystemCommand("grep 'model name' /proc/cpuinfo | cut -d' ' -f3-"));
	
	char buf[512];
	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^nkernel^0[^3%s^0]\"\n", kernel);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^nproc^0[^3%s^0]\"\n", proc);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	memset(buf, 0, sizeof(buf));
	snprintf(buf, sizeof(buf), "say \"^ngfx^0[^3%s^0] [^3%s^0]\"\n", eth.cgs_glconfig.vendor_string, eth.cgs_glconfig.renderer_string);
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);

	free(kernel);
	free(proc);
}

// Stupid and useless spam ;) from http://www.network-science.de/ascii/
void spamCommand() {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nSpam with 'eth powaa'\n");
		orig_syscall(UI_PRINT, "^nUsage: eth_spam\n");
		return;
	}
	
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1      _   _\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1  ___| |_| |__    _ __   _____      ____ _  __ _\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1 / _ \\ __| '_ \\  | '_ \\ / _ \\ \\ /\\ / / _` |/ _` |\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1|  __/ |_| | | | | |_) | (_) \\ V  V / (_| | (_| |\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1 \\___|\\__|_| |_| | .__/ \\___/ \\_/\\_/ \\__,_|\\__,_|\"\n");
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "say \"^1                 |_|\"\n");
}

void fortune() {
	char *msg;
	char buf[256];
	msg = strdup(getOutputSystemCommand("fortune -a -n 140 -s"));
	sprintf(buf,"say \"%s\"",Q_CleanStr(msg));
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);
	free(msg);
}

void execCommand() {
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nExecute a system command\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: eth_exec [COMMAND]\n");
		return;
	}
	
	if (syscall_UI_Argc() == 2) {
		char buf[128];
		snprintf(buf, sizeof(buf), "echo \"%s\"\n", getOutputSystemCommand(syscall_UI_Argv(1)));
		orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, buf);
	}
}

void findETHCommand (void) {
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "vsay " ETH_REQUEST_STR "\n");
}

void cleanPK3Command (void) {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nRemove all small pk3\n");
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "cleanpk3\n");
		return;
	}

	system("find ~/.etwolf/ -name '*.pk3' -size -1200k -delete");
	orig_syscall(UI_PRINT, "^nClean pk3 done.\nYou must restart the game now.\n");
}

// For terrorising :)
void randomKickCommand (void) {
	int kickID[MAX_CLIENTS];
	int kickCount = 0;

	// Get all kickable players
	int entityNum;
	for (entityNum = 0; entityNum < MAX_CLIENTS; entityNum++) {
		if ((eth.clientInfo[entityNum].infoValid) && (eth.clientInfo[eth.cg_clientNum].team != TEAM_SPECTATOR) && (entityNum != eth.cg_clientNum) && (eth.clientInfo[entityNum].targetType != PLIST_FRIEND)) {
			if ((eth.clientInfo[entityNum].team == eth.clientInfo[eth.cg_clientNum].team) || (eth.clientInfo[entityNum].team == TEAM_SPECTATOR)) {
				kickID[kickCount] = entityNum;
				kickCount++;
			}
		}
	}

	#ifdef ETH_DEBUG
		ethDebug("randomkick: Players found for kick");
		int count = 0;
		for (; count < kickCount; count++)
			ethDebug("randomkick: id %i name %s", kickID[count], eth.clientInfo[kickID[count]].name);
	#endif

	if (kickCount) {
		char str[24];
		int randomID = (int)((float)kickCount * rand() / (RAND_MAX + 1.0));
		snprintf(str, sizeof(str), "callvote kick %i\n", kickID[randomID]);
		orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, str);
		#ifdef ETH_DEBUG
			ethDebug("randomkick: vote kicking id %i name %s", kickID[randomID], eth.clientInfo[kickID[randomID]].name);
		#endif
	} else {
		orig_syscall(UI_PRINT, "^nCouldn't find a player to votekick.\n");	
	}
}

void inviteCommand (void) {
	if (eth.clientInfo[eth.cg_clientNum].team != TEAM_SPECTATOR) {
		int i;
		char str[35];
		for (i=0; i < MAX_CLIENTS; i++) {
			if ((eth.clientInfo[i].infoValid) && (eth.clientInfo[i].targetType == PLIST_FRIEND) && (eth.clientInfo[i].team == eth.clientInfo[eth.cg_clientNum].team)) {
				if (eth.mod->type == MOD_ETPRO)
					snprintf(str, sizeof(str), "fireteam invite %d\n", i);
				else
					snprintf(str, sizeof(str), "fireteam invite %d\n", i+1); // other mods use player's id+1 for working with fireteam
				orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, str);
			}
		}
	}
}

void saveCommand (void) {
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nSave config to a file\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: eth_save config_name\n");
		return;
	}
	
	if (syscall_UI_Argc() == 2) {
		char path[128 + 16];
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), syscall_UI_Argv(1));

		FILE *file;
		
		if ((file = fopen(path, "w")) == NULL) {
			orig_syscall(UI_PRINT, "^nCan't write config file.\n");
			ethLog("eth: can't write config file.");
			return;
		}
		
		int count = 0;
		for (; count < VARS_TOTAL; count++) {
			if (seth.value[count] == (float)(int)seth.value[count])
				fprintf(file, "%s=%i\n", seth.vars[count].cvarName, (int)seth.value[count]);
			else
				fprintf(file, "%s=%.2f\n", seth.vars[count].cvarName, seth.value[count]);
		}

		char done[512];
		snprintf(done, sizeof(done), "^nConfig '%s' saved.\n", path);

		orig_syscall(UI_PRINT, done);
	
		fclose(file);
	}
}

void loadCommand (void) {
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nLoad settings from a config file\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: eth_load config_name\n");
		return;
	}
	
	if (syscall_UI_Argc() == 2) {
		FILE *file;
	
		char path[128 + 16];
		memset(path, 0, sizeof(path));
		snprintf(path, sizeof(path), "%s/%s", getenv("HOME"), syscall_UI_Argv(1));
	
		if ((file = fopen(path, "rb")) == NULL) {
			orig_syscall(UI_PRINT, "^nCan't read config file.\n");
			return;
		}
	
		// Get config file line by line
		char line[32];
		while (fgets(line, sizeof(line) - 1, file) != 0) {
			char *sep = strrchr(line, '=');
			*sep = '\0';	// Separate name from value
			// Search this var
			int count = 0;
			for (; count < VARS_TOTAL; count++) {
				if (strcmp(line, seth.vars[count].cvarName) == 0) {
					seth.value[count] = atof(sep + 1);
					break;
				} else if ((count + 1) == VARS_TOTAL) {
					ethLog("readConfig: don't know this var: [%s]", line);
				}
			}
		}

		char done[512];
		snprintf(done, sizeof(done), "^nConfig '%s' loaded.\n", path);

		orig_syscall(UI_PRINT, done);
	
		fclose(file);
	}
}

// etpro guid game command
void etproGuidCommand() {
	if( !eth.hookLoad ){
		orig_syscall(UI_PRINT, "^nNot connected to a server.\n");
		return;
	}

	if(!strstr(eth.mod->name, "etpro")){
		orig_syscall(UI_PRINT, "^nMod is not etpro!\n");
		return;
	}
	
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nChange your etpro GUID\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "etpro_guid [GUID]\n");
		return;
	}
	
	if (syscall_UI_Argc() == 2) {
		etproGuid = realloc(etproGuid, ETPRO_GUID_SIZE);
		memset(etproGuid, 0, ETPRO_GUID_SIZE);
		strncpy(etproGuid, syscall_UI_Argv(1), ETPRO_GUID_SIZE);
	}
	
	if (etproGuid) {
		orig_etproAntiCheat(acA, acB, acC, etproOSChecksum, acE, etproGuid);
		char txt[64];
		snprintf(txt, sizeof(txt), "^nNew etpro GUID: [^*%s^n]\n", (char *)etproGuid);
		orig_syscall(UI_PRINT, txt);
	} else {
		orig_syscall(UI_PRINT, "^nEtpro GUID is not set\n");
	}
}

// etpro OS game command
void etproOSCommand() {
	if( !eth.hookLoad ){
		orig_syscall(UI_PRINT, "^nNot connected to a server.\n");
		return;
	}

	if(!strstr(eth.mod->name, "etpro")){
		orig_syscall(UI_PRINT, "^nMod is not etpro!\n");
		return;
	}
	
	if (syscall_UI_Argc() == 1)
		orig_syscall(UI_PRINT, "^nChange your etpro OS\n");

	if ((syscall_UI_Argc() == 1) || (syscall_UI_Argc() > 2)) {
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "etpro_os [linux/win32]\n");
		return;
	}

	if (!strcasecmp(syscall_UI_Argv(1), "linux")) {
		etproOSChecksum = sethET->etproLinuxChecksum;
		orig_syscall(UI_PRINT, "^nNew etpro OS: [^*linux^n]\n");
	} else if (!strcasecmp(syscall_UI_Argv(1), "win32")) {
		etproOSChecksum = sethET->etproWin32Checksum;
		orig_syscall(UI_PRINT, "^nNew etpro OS: [^*win32^n]\n");
	} else {
		orig_syscall(UI_PRINT, "^nUnknown OS\n");
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "etpro_os [linux/win32]\n");
		return;
	}
		
	orig_etproAntiCheat(acA, acB, acC, etproOSChecksum, acE, etproGuid);
}

void amarokCommand() {
	if (syscall_UI_Argc() != 1) {
		orig_syscall(UI_PRINT, "^nSpam with amaroK infos\n");
		orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "amarok\n");
		return;
	}

	char *artist = strdup(getOutputSystemCommand("dcop amarok player artist"));
	char *title = strdup(getOutputSystemCommand("dcop amarok player title"));
	char *currentTime = strdup(getOutputSystemCommand("dcop amarok player currentTime"));
	char *totalTime = strdup(getOutputSystemCommand("dcop amarok player totalTime"));

	if (!strlen(artist) && !strlen(title)) {
		orig_syscall(UI_PRINT, "^namaroK: nothing to display\n");
		return;
	}

	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND,
    		va("say \"^oMusic: ^0[^w%s - %s^0]^*-^0[^w%s/%s^0] "" \n", artist, title, currentTime, totalTime));

	free(artist);
	free(title);
	free(currentTime);
	free(totalTime);
}

void setValueCommand() {
	char msg[MAX_STRING_CHARS];
	const char *var;
	int p[VARS_TOTAL];
	qboolean found;
	int i,match,k;
	match = found = k = 0;
	if(!eth.hookLoad){
		orig_syscall(UI_PRINT,"^nETH vars not yet initialized, connect to a server first!\n");
		return;
	}
    
	if (syscall_UI_Argc() == 1){
		orig_syscall(UI_PRINT, "^nUsage " ETH_CMD_PREFIX "set [var] [value]\n");
		return;
	}

	if (syscall_UI_Argc() >= 2) {
		var = syscall_UI_Argv(1);
		memset(p,-1,sizeof(p));
		for(i=0;i<VARS_TOTAL;i++){
			if( !seth.vars[i].cvarName )
				continue;
			if( strstr( seth.vars[i].cvarName, var ) ){
				p[k++] = i;
				found = qtrue;
				if( !strcmp( var, seth.vars[i].cvarName ) )
					match = i;
			}
		}
        
		if(!found){
			snprintf(msg,sizeof(msg),"^nNo cvars found matching ^2%s\n",var);
			orig_syscall(UI_PRINT, msg);
			return;
		}
		if( syscall_UI_Argc() == 2 ) {
			for(k=0;k<VARS_TOTAL;k++){
				if( p[k] < 0 )
					continue;
				snprintf(msg,sizeof(msg),"^f%s ^nhas value ^2%f\n", seth.vars[p[k]].cvarName, seth.value[p[k]]);
				orig_syscall(UI_PRINT, msg);
			}
		}
	}
    
	if (syscall_UI_Argc() == 3) {
		if(k > 1 && !match ){
			orig_syscall(UI_PRINT, "^nWarning: more than than 1 cvars match query, aborting\n");
			return;
		}
		if( match )
			p[0]=match;
		
		seth.value[p[0]] = atof(syscall_UI_Argv(2));
		snprintf(msg,sizeof(msg),"^nnew value: ^f%s = ^2%f\n", seth.vars[p[0]].cvarName, seth.value[p[0]]);
		orig_syscall(UI_PRINT, msg);
	}
}

// Get infos through xmmsctrl - http://user.it.uu.se/~adavid/utils/
void xmmsCommand() {
    if (syscall_UI_Argc() != 1) {
        orig_syscall(UI_PRINT, "^nSpam with xmms infos\n");
        orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "xmms\n");
        return;
    }

	char *title = strdup(getOutputSystemCommand("xmmsctrl print %T"));
	char *currentTime = strdup(getOutputSystemCommand("xmmsctrl print %m"));
	char *totalTime = strdup(getOutputSystemCommand("xmmsctrl print %M"));

	if (!strlen(title)) {
		orig_syscall(UI_PRINT, "^nxmms: nothing to display\n");
		return;
	}

	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND,
    		va("say \"^3Playing: ^0[^n%s^0]^*-^0[^n%s/%s^0] "" \n", title, currentTime, totalTime));

	free(title);
	free(currentTime);
	free(totalTime);
}

void targetCommand(){
    if (syscall_UI_Argc() == 1) {
        orig_syscall(UI_PRINT, "^nPlayer targeting control\n");
        orig_syscall(UI_PRINT, "^nUsage: " ETH_CMD_PREFIX "target [list/add/del/clear] [id]\n");
        return;
    }
	
	if (syscall_UI_Argc() > 1){
		const char *cmd = syscall_UI_Argv(1);
		int i;
		char msg[MAX_STRING_CHARS];
				
		if( !strcmp(cmd,"list") ){
			for(i=0 ; i<MAX_CLIENTS;i++ ){
				if( eth.clAttack[i] ){
					sprintf(msg, "^f(^2%i^f) %s\n",i,eth.clientInfo[i].name);
					orig_syscall(UI_PRINT,msg);
				}
			}
		} else if( !strcmp(cmd,"add") ) {
			if( syscall_UI_Argc() < 3 ){
				orig_syscall(UI_PRINT,"^nnot enough parameters given\n");
				return;
			}
			char cln[MAX_STRING_CHARS];
			strncpy(cln,syscall_UI_Argv(2),sizeof(cln));

			for(i=0;i<MAX_CLIENTS;i++){
				if( strstr(eth.clientInfo[i].cleanName,cln) ){
					sprintf(msg,"^fadded ^7%s\n",eth.clientInfo[i].cleanName);
					orig_syscall(UI_PRINT,msg);
					eth.clAttack[i] = qtrue;
				}
			}
		} else if( !strcmp(cmd,"del") ) {
			if( syscall_UI_Argc() < 3 ){
				orig_syscall(UI_PRINT,"^nnot enough parameters given\n");
				return;
			}
			eth.clAttack[atoi(syscall_UI_Argv(2))] = qfalse;
		} else if( !strcmp(cmd,"clear") ){
			memset(eth.clAttack,qfalse,sizeof(eth.clAttack));
		} else {
			orig_syscall(UI_PRINT, "^nunknown option, aborting\n");
			return;
		}
	}
}

void goHome() {
	orig_syscall(UI_CMD_EXECUTETEXT, EXEC_APPEND, "connect et.nixcoders.org:27962\n");
}

void deathSpamCmd() {
	if (syscall_UI_Argc() == 1) {
		orig_syscall(UI_PRINT, dieFormat);
		return;
	}

	strncpy(dieFormat,syscall_UI_Argv(1),sizeof(dieFormat));
}

void killSpamCmd() {
	if (syscall_UI_Argc() == 1) {
		orig_syscall(UI_PRINT, ksFormat);
		return;
	}

	strncpy(ksFormat,syscall_UI_Argv(1),sizeof(ksFormat));
}

void bannerCmd() {
	if (syscall_UI_Argc() == 1) {
		orig_syscall(UI_PRINT, bnFormat);
		return;
	}

	strncpy(bnFormat,syscall_UI_Argv(1),sizeof(bnFormat));
}

void registerGameCommands() {
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "exec", &execCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "sysinfo", &sysInfoCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "spam", &spamCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "find", &findETHCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "cleanpk3", &cleanPK3Command);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "randomkick", &randomKickCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "invite", &inviteCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "save", &saveCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "load", &loadCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "etpro_guid", &etproGuidCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "etpro_os", &etproOSCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "amarok", &amarokCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "xmms", &xmmsCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "set", &setValueCommand);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "fortune", &fortune);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "home", &goHome);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "target", &targetCommand); 
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "deathspam", &deathSpamCmd);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "banner", &bannerCmd);
	orig_Cmd_AddCommand(ETH_CMD_PREFIX "killspam", &killSpamCmd);

	
	registerIrcCommands();
}

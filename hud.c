// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us
// new custom hud, many things left to add/fix/improve

#include "eth.h"

// Health popups

#define MAX_HEALTH_POPUPS 10
#define HEALTH_POPUP_HEIGHT seth.value[VAR_CHUD_HPOPUP_HEIGHT]
#define HEALTH_POPUP_STAYTIME (seth.value[VAR_CHUD_HPOPUP_STAYTIME] * 1000)
#define HEALTH_POPUP_FONTSIZE 0.22f

// General

#define HUD_BOX_BGCOLOR (vec4_t) { 0.5f, 0.5f, 0.5f, 0.6f }

#define HUD_BAR_BGCOLOR (vec4_t) { 0.36f, 0.41f, 0.45f, 1.0f }
#define HUD_BAR_COLOR (vec4_t) { 0.65f, 0.76f, 0.75f, 1.0f }

#define HUD_TEXT_COLOR (vec4_t) { 1.0f, 0.89f, 0.87f, 0.7f }
#define HUD_VALUE_COLOR (vec4_t) { 0.58f, 0.91f, 0.64f, 0.7f }

#define HUD_INSIDEBOX_SPACE 3
#define HUD_BOX_SPACE 3

// Timers

#define TIMERS_FONTSIZE 0.25f
#define TIMERS_INSIDEBOX_SPACE 5

// System

#define SYSTEM_BAR_WIDTH seth.value[VAR_CHUD_SYS_BAR_WIDTH]
#define SYSTEM_BAR_HEIGHT seth.value[VAR_CHUD_SYS_BAR_HEIGHT]

// Log output

#define OUTPUT_FONTSIZE seth.value[VAR_CHUD_LOG_FONTSIZE]
#define OUTPUT_COLOR colorWhite

// Weapon stuff

#define WEAPON_FONTSIZE 0.15f
#define AMMO_FONTSIZE 0.35f

// You killed XXX! msg

#define KILLED_Y 375
#define KILLED_FONTSIZE 0.24f

// HEALTH, STAMINA, POWER - BARS

#define POWER_ICONSIZE 14

// Misc

#define SPECING_FONTSIZE 0.20f

// Chats & other console output

#define HUD_CONSOLE_FONTSIZE 0.16f
#define HUD_CONSOLE_STAYTIME (seth.value[VAR_CHUD_CONSOLE_STAYTIME] * 1000)
#define HUD_CONSOLE_MSGS seth.value[VAR_CHUD_CONSOLE_MSGS]

#define HUD_CHAT_X hudglobal.bottomChatX
#define HUD_CHAT_MSGS seth.value[VAR_CHUD_CHAT_MSGS]
#define HUD_CHAT_FONTSIZE 0.16f
#define HUD_CHAT_STAYTIME (seth.value[VAR_CHUD_CHAT_STAYTIME] * 1000)

hudsystem_t hudsystem;
hudglobal_t hudglobal;
hudoutput_t hudoutput[MAX_OUTPUT_MSGS];
hudHealthPopup_t hudhpp[MAX_HEALTH_POPUPS];
hudChatMsgs_t hudChatMsgs[MAX_CHAT_MSGS];
hudConsoleMsgs_t hudConsoleMsgs[MAX_CONSOLE_MSGS];

void registerHudStuff() {
	// Fonts
	orig_syscall(CG_R_REGISTERFONT, "comic", 18, &eth.hFont[FONT_COMIC]);
// 	orig_syscall(CG_R_REGISTERFONT, "spacetoa", 18, &eth.hFont[FONT_SPACETOA]);
	orig_syscall(CG_R_REGISTERFONT, "bersolid", 18, &eth.hFont[FONT_BERSOLID]);

	// Gfx
	eth.wIcon = syscall_CG_R_RegisterShaderNoMip("gfx/2d/hudWarning");
	eth.iIcon = syscall_CG_R_RegisterShaderNoMip("gfx/2d/hudInfo");
	eth.gStar = syscall_CG_R_RegisterShaderNoMip("gfx/2d/gStar");
	eth.cStar = syscall_CG_R_RegisterShaderNoMip("gfx/2d/cStar");

	// Set fonts
	hudglobal.font1 = eth.hFont[FONT_COMIC];
	hudglobal.font2 = eth.hFont[FONT_SPACETOA]; // REMOVED FOR NOW. DON'T NEED IT ATMf
	hudglobal.font3 = eth.hFont[FONT_BERSOLID];
}

void drawCustomHud() {
	hudglobal.leftNextY = HUD_BOX_SPACE;
	drawHudTimers();
	drawHudWeaponInfo();
	drawHudVictimMsg();
	drawHudPowerStats();
	drawHudChat(); // This one has to be called after drawHudPowerStats() !!!
	drawSpreeText(); // here ?
	// The following info will be drawn in order these 3 functions are called
	drawHudOutput();
	drawHudSystemStats();
	drawHudMisc();

	drawHudConsoleOutput(); // This one has to be called after the above 3 !!!

}

qboolean isChatMsg(char *msg) { // it's not 100% accurate (if someone puts these checks in his nick..), but should work properly for now
	// TODO: maybe improve by checking getIdByName()
	if (msg[0] == '(') { // possible team/fireteam chat
		if (strstr(msg, "^7) (")) {
			if ((strstr(msg, "): ^5")) || (strstr(msg, "): ^3")))
				return qtrue; // team or fireteam msg
		}
	}
	else if ((strstr(msg, "^7: ^2")) && (!strstr(msg, "^1TEAM KILL^7: "))) // global chat
		return qtrue;
	return qfalse;
}

void drawHudConsoleOutput() {
	int i;
	int msgCount = 0;
	int height = 0;
	int textHeight = sizeTextHeight(HUD_CONSOLE_FONTSIZE, &hudglobal.font1);
	int lineHeight = textHeight + HUD_INSIDEBOX_SPACE;
	vec4_t color;
	Vector4Copy(colorWhite, color);

	for (i=0; i<MAX_CONSOLE_MSGS; i++) {
		if (eth.cg_time - hudConsoleMsgs[i].time < HUD_CONSOLE_STAYTIME) {
			height += lineHeight;
			msgCount++;
		}
		if (msgCount >= HUD_CONSOLE_MSGS)
			break;
	}

	for(i=msgCount-1; i>=0; i--) {
		color[3] = (float) 1 - (eth.cg_time - hudConsoleMsgs[i].time) / HUD_CONSOLE_STAYTIME;
		drawText(HUD_BOX_SPACE, hudglobal.leftNextY, HUD_CONSOLE_FONTSIZE, hudConsoleMsgs[i].msg, color, qfalse, qtrue, &hudglobal.font1);
		hudglobal.leftNextY += lineHeight;
	}
};

void drawHudChat() { // TODO: seperate msg into more lines in case it's too long
	int i;
	int msgCount = 0;
	int height = 0;
	int textHeight = sizeTextHeight(HUD_CHAT_FONTSIZE, &hudglobal.font1);
	int lineHeight = textHeight + 2;
	vec4_t color;
	Vector4Copy(colorWhite, color);

	for (i=0; i<MAX_CHAT_MSGS; i++) {
		if (eth.cg_time - hudChatMsgs[i].time < HUD_CHAT_STAYTIME) {
			height += lineHeight;
			msgCount++;
		}
		if (msgCount >= HUD_CHAT_MSGS)
			break;
	}

	int textY = SCREEN_HEIGHT - HUD_BOX_SPACE - height;

	for(i=msgCount-1; i>=0; i--) {
		color[3] = (float) 1 - (eth.cg_time - hudChatMsgs[i].time) / HUD_CHAT_STAYTIME;
		drawText(HUD_CHAT_X, textY, HUD_CHAT_FONTSIZE, hudChatMsgs[i].msg, color, qfalse, qtrue, &hudglobal.font1);
		textY += lineHeight;
	}
}

void addHudConsoleChat(char *msg) {
	int msgCount = MAX_CHAT_MSGS - 2;
	while (msgCount >= 0) {
		hudChatMsgs[msgCount + 1].time = hudChatMsgs[msgCount].time;
		strncpy(hudChatMsgs[msgCount + 1].msg, hudChatMsgs[msgCount].msg, sizeof(hudChatMsgs->msg));
		msgCount--;
	}


	hudChatMsgs[0].time = eth.cg_time;
	strncpy(hudChatMsgs[0].msg, msg, sizeof(hudChatMsgs->msg));
}

void addHudConsoleMsg(char *msg) {

	int msgCount = MAX_CONSOLE_MSGS - 2;
	while (msgCount >= 0) {
		hudConsoleMsgs[msgCount + 1].time = hudConsoleMsgs[msgCount].time;
		strncpy(hudConsoleMsgs[msgCount + 1].msg, hudConsoleMsgs[msgCount].msg, sizeof(hudConsoleMsgs->msg));
		msgCount--;		
	}

	hudConsoleMsgs[0].time = eth.cg_time;
	strncpy(hudConsoleMsgs[0].msg, msg, sizeof(hudConsoleMsgs->msg));
}

void drawHudMisc() {
	char msg[256];
	char *team;
	char *class;
	switch(eth.cg_snap->ps.persistant[PERS_TEAM]) {
		case TEAM_SPECTATOR:
			team = "spectator";
			break;
		case TEAM_AXIS:
			team = "axis";
			break;
		default:
			team = "allies";
			break;
	}
	switch(eth.clientInfo[eth.cg_snap->ps.clientNum].cls) {
		case PC_ENGINEER:
			class = "engineer";
			break;
		case PC_FIELDOPS:
			class = "field ops";
			break;
		case PC_COVERTOPS:
			class = "covert ops";
			break;
		case PC_MEDIC:
			class = "medic";
			break;
		default:
			class = "soldier";
			break;
	}
	char *action;
	if (eth.cg_snap->ps.clientNum != eth.cg_clientNum)
		action = "Following";
	else
		action = "Playing as";
	snprintf(msg, sizeof(msg), "^7%s ^2[ ^7%s ^2] [ ^f%s ^2] [ ^f%s ^2]", action, eth.clientInfo[eth.cg_snap->ps.clientNum].name, team, class);
	drawText(HUD_BOX_SPACE, hudglobal.leftNextY, SPECING_FONTSIZE, msg, colorWhite, qfalse, qtrue, &hudglobal.font1);
	hudglobal.leftNextY += sizeTextHeight(SPECING_FONTSIZE, &hudglobal.font1) + HUD_BOX_SPACE;
}

void addHudHealthChange(int hp, int time) {
	int hpCount = MAX_HEALTH_POPUPS - 2;
	while (hpCount >= 0) {
		if (hudhpp[hpCount].hp) {
			hudhpp[hpCount + 1].time = hudhpp[hpCount].time;
			hudhpp[hpCount + 1].hp = hudhpp[hpCount].hp;
		}
		hpCount--;
	}

	hudhpp[0].time = time;
	hudhpp[0].hp = hp;
}

void drawHudPowerStats() {
	int current = eth.cg_snap->ps.stats[STAT_HEALTH];
	float textSize = scaleFromHeight(POWER_ICONSIZE, &hudglobal.font1);
	int width = 5 * HUD_INSIDEBOX_SPACE + 10 * POWER_ICONSIZE + sizeTextWidth(textSize, "^n156HP ^2(^n100%%^2)", &hudglobal.font1);
	hudglobal.bottomChatX = HUD_BOX_SPACE + width + 2 * HUD_BOX_SPACE;

	if ((current > 0) && (eth.cg_snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR)) {
		int max = eth.cg_snap->ps.stats[STAT_MAX_HEALTH];
		if (eth.clientInfo[eth.cg_snap->ps.clientNum].cls == PC_MEDIC)
			max *= 1.12f;
	
		float percentage = (float) current / max * 100;

		char cHPstr[30];
		snprintf(cHPstr, sizeof(cHPstr), "^n%dHP ^2(^n%d%%^2)", current, (int) percentage);


		int height = 5 * HUD_INSIDEBOX_SPACE + 3 * POWER_ICONSIZE;
		
		int y = SCREEN_HEIGHT - HUD_BOX_SPACE - height;
		float popupStartY = y - POWER_ICONSIZE;
		drawFillRect(HUD_BOX_SPACE, y, width, height, HUD_BOX_BGCOLOR);
		y += HUD_INSIDEBOX_SPACE;

	
		int colorNum = (int) percentage / 10.0f;
		int greyNum = 10 - colorNum;

 	
		int i;
		int drawX = HUD_BOX_SPACE + 2 * HUD_INSIDEBOX_SPACE;

		for (i=0; i<greyNum; i++) {
			drawPic(drawX, y, POWER_ICONSIZE, POWER_ICONSIZE, eth.gStar);
			drawX += POWER_ICONSIZE;
		}
		for (i=0; i<colorNum; i++) {
			drawPic(drawX, y, POWER_ICONSIZE, POWER_ICONSIZE, eth.cStar);
			drawX += POWER_ICONSIZE;
		}

		drawText(drawX, y, textSize, cHPstr, colorWhite, qfalse, qtrue, &hudglobal.font1);

		y += POWER_ICONSIZE + HUD_INSIDEBOX_SPACE;
		drawX = HUD_BOX_SPACE + 2 * HUD_INSIDEBOX_SPACE;

		int chargeTime;
		switch(eth.cg_snap->ps.stats[STAT_PLAYER_CLASS]) {
			case PC_ENGINEER:
				chargeTime = eth.cg_engineerChargeTime[eth.cg_snap->ps.persistant[PERS_TEAM]-1];
				break;
			case PC_FIELDOPS:
				chargeTime = eth.cg_ltChargeTime[eth.cg_snap->ps.persistant[PERS_TEAM]-1];
				break;
			case PC_COVERTOPS:
				chargeTime = eth.cg_covertopsChargeTime[eth.cg_snap->ps.persistant[PERS_TEAM]-1];
				break;
			case PC_MEDIC:
				chargeTime = eth.cg_medicChargeTime[eth.cg_snap->ps.persistant[PERS_TEAM]-1];
				break;
			default:
				chargeTime = eth.cg_soldierChargeTime[eth.cg_snap->ps.persistant[PERS_TEAM]-1];
				break;
		}
	
		float powerBarPercentage = (float) (eth.cg_time - eth.cg_snap->ps.classWeaponTime) / chargeTime;
		if (powerBarPercentage > 1.0f)
			powerBarPercentage = 1.0f;

		int barX = HUD_BOX_SPACE + HUD_INSIDEBOX_SPACE;
		int barWidth = width - 2*HUD_INSIDEBOX_SPACE;
		drawBar(barX, y, barWidth, POWER_ICONSIZE, 1.0f, powerBarPercentage, HUD_BAR_BGCOLOR, HUD_BAR_COLOR);

		y += POWER_ICONSIZE + HUD_INSIDEBOX_SPACE;

		char killCount[64];
		int lastKillTime = (eth.cg_time - eth.lastKillTime) / 1000;
		if (!eth.lastKillTime)
			lastKillTime = 0;
		snprintf(killCount, sizeof(killCount), "^0( ^nKills: ^2%d ^0| ^nLast kill: ^2%d^nsec ago ^0)", eth.killCount, lastKillTime);

		drawText(drawX, y, textSize, killCount, colorWhite, qfalse, qtrue, &hudglobal.font1);

		if (eth.cg_snap->ps.clientNum == hudglobal.lastID) {
			int difference = (hudglobal.lastHP - current) * -1;
			if ((difference > 4) || (difference < -4)) // Avoid auto-regeneration @ medics
					addHudHealthChange(difference, eth.cg_time);
		}

		for (i=MAX_HEALTH_POPUPS-1; i>=0; i--) {
			if ((eth.cg_time - hudhpp[i].time) < HEALTH_POPUP_STAYTIME) {
				float popUpTimePercentage = (float) (eth.cg_time - hudhpp[i].time) / HEALTH_POPUP_STAYTIME;
				float popupY = popupStartY - popUpTimePercentage * HEALTH_POPUP_HEIGHT;
				char *hpType;
				vec4_t popUpColor;
				if (hudhpp[i].hp < 0) {
					hpType = "^1";
					Vector4Copy(colorRed, popUpColor);
				}
				else {
					hpType = "^2+";
					Vector4Copy(colorGreen, popUpColor);
				}
				popUpColor[3] = (float) 1 - popUpTimePercentage;
				char popUpMsg[10];
				snprintf(popUpMsg, sizeof(popUpMsg), "%s%d", hpType, hudhpp[i].hp);

				drawText(drawX, popupY, HEALTH_POPUP_FONTSIZE, popUpMsg, popUpColor, qfalse, qtrue, &eth.fontVariable);
				
			}
		}
	}

	hudglobal.lastID = eth.cg_snap->ps.clientNum;
	hudglobal.lastHP = current;
}

void drawHudVictimMsg() {
	float stayTime = seth.value[VAR_CHUD_VICTIM_STAYTIME] * 1000.0f;
	float fadeTime = seth.value[VAR_CHUD_VICTIM_FADETIME] * 1000.0f;
	if ((eth.lastKillTime) && (eth.VictimName) && (stayTime)) {
		if (eth.lastKillTime + stayTime > eth.cg_time) {
			vec4_t color = { 1.0f, 1.0f, 1.0f, 1.0f }; // White
			char msg[256];
			snprintf(msg, sizeof(msg), "^nYou killed ^7%s^n !", eth.VictimName);
			int msgWidth = sizeTextWidth(KILLED_FONTSIZE, msg, &hudglobal.font1);

			float fadeValue = ((eth.cg_time - stayTime + fadeTime - eth.lastKillTime) * -1.0f + fadeTime) / fadeTime;
			if (fadeValue < 1.0f)
				color[3] = fadeValue;
			drawText((SCREEN_WIDTH - msgWidth) / 2, KILLED_Y , KILLED_FONTSIZE, msg, color, qfalse, qtrue, &hudglobal.font1);
		}
	}
}

void drawHudOutput() {
	int i,w,max;
	int count = -1;
	int msgAmount = seth.value[VAR_CHUD_OUTPUTNUM];
	int iconSize = seth.value[VAR_CHUD_OUTICONSIZE];
	for (i=0, max=0; i < msgAmount; i++) {
			if (strlen(hudoutput[i].msg) > 0) {
				w = sizeTextWidth(OUTPUT_FONTSIZE, hudoutput[i].msg, &eth.fontVariable);
				if (w > max)
					max = w;
				count++;
			}
	}
	if (count >= 0) {
		int width = HUD_INSIDEBOX_SPACE + iconSize + HUD_INSIDEBOX_SPACE + max + HUD_INSIDEBOX_SPACE;
		int height = (count + 1) * iconSize + (count + 2) * HUD_INSIDEBOX_SPACE;
		drawFillRect(HUD_BOX_SPACE, HUD_BOX_SPACE, width, height, HUD_BOX_BGCOLOR);
		hudglobal.leftNextY += height + HUD_BOX_SPACE;
	
		qhandle_t icon;
		int textHeight = sizeTextHeight(OUTPUT_FONTSIZE, &eth.fontVariable);
		int lineHeight = iconSize + HUD_INSIDEBOX_SPACE;
		int y = HUD_BOX_SPACE + HUD_BOX_SPACE;
		int iconX = HUD_BOX_SPACE + HUD_BOX_SPACE;
		int x = HUD_BOX_SPACE + HUD_BOX_SPACE + iconSize + HUD_INSIDEBOX_SPACE;
		int textY = y + (iconSize / 2) - (textHeight / 2); // Draw text in the center of icon
		while (count >= 0) {
			if (hudoutput[count].type == hudinfo)
				icon = eth.iIcon;
			else
				icon = eth.wIcon;
			drawPic(iconX, y, iconSize, iconSize, icon);
			drawText(x, textY, OUTPUT_FONTSIZE, hudoutput[count].msg, OUTPUT_COLOR, qfalse, qfalse, &eth.fontVariable);
			y += lineHeight;
			textY += lineHeight;
			count--;
		}
	}
}

void addHudOutputMsg(hudicontype type, const char *format, ...) {

	char msg[HUDOUTPUT_MAXLEN];
	va_list argv;
	va_start (argv, format);
		vsnprintf(msg, sizeof msg, format, argv); 
	va_end (argv);

	int msgCount = MAX_OUTPUT_MSGS - 2;
	while (msgCount >= 0) {
		if (hudoutput[msgCount].msg) {
			hudoutput[msgCount + 1].type = hudoutput[msgCount].type;
			strncpy(hudoutput[msgCount + 1].msg, hudoutput[msgCount].msg, sizeof(hudoutput->msg));
			msgCount--;
		}
	}

	strncpy(hudoutput[0].msg, msg, sizeof(hudoutput->msg));
	hudoutput[0].type = type;
}

void drawHudSystemStats() {

	hudsystem.fpsFrameCount++;

	if ((hudsystem.lastCheck + (seth.value[VAR_CHUD_SYS_UPDATEDELAY] * 1000)) < eth.cg_time) { // TODO: get this info with C functions so it works on all distros
		hudsystem.ramTotal = atoi(getOutputSystemCommand("free -mto | grep Mem: | awk '{ print $2 }'"));
		hudsystem.ramUsed = atoi(getOutputSystemCommand("free -mto | grep Mem: | awk '{ print $3 }'"));
		hudsystem.cpuUsed = atof(getOutputSystemCommand("uptime |awk -F'average:' '{ print $2}' | sed s/,//g | awk '{ print $2}'"));
		hudsystem.lastCheck = eth.cg_time;
	}

	int boxWidth = 3 * HUD_INSIDEBOX_SPACE + SYSTEM_BAR_WIDTH + sizeTextWidth(TIMERS_FONTSIZE, "RAM", &hudglobal.font1);
	int boxHeight = 4 * HUD_INSIDEBOX_SPACE + 3 * SYSTEM_BAR_HEIGHT;

	qboolean showFps = qfalse;
	if ((hudsystem.fpsMax) && (hudsystem.fpsCurrent)) {
		showFps = qtrue;
		boxHeight += 2 * HUD_INSIDEBOX_SPACE + 2 * SYSTEM_BAR_HEIGHT;
	}

	drawFillRect(HUD_BOX_SPACE, hudglobal.leftNextY, boxWidth, boxHeight, HUD_BOX_BGCOLOR);

	int barX = HUD_BOX_SPACE + HUD_INSIDEBOX_SPACE;
	int textX = barX + SYSTEM_BAR_WIDTH + HUD_INSIDEBOX_SPACE;
	int timeY = hudglobal.leftNextY + HUD_INSIDEBOX_SPACE;
	int ramY = timeY + SYSTEM_BAR_HEIGHT + HUD_INSIDEBOX_SPACE;
	int cpuY = ramY + SYSTEM_BAR_HEIGHT + HUD_INSIDEBOX_SPACE;
	float textSize = scaleFromHeight(SYSTEM_BAR_HEIGHT, &hudglobal.font1);

	struct tm *ptr;
	time_t tm;
	char localTime[24];
	char local[64];

	tm = time(NULL);
	ptr = localtime(&tm);
	strftime(localTime, 100, "^n%X", ptr);
	snprintf(local, sizeof(local), "%s ^2@^n %s %s", localTime, eth.mod->name, eth.mod->version);

	drawText(barX, timeY, textSize, local, colorWhite, qfalse, qfalse, &hudglobal.font1);

 	drawBar(barX, ramY, SYSTEM_BAR_WIDTH, SYSTEM_BAR_HEIGHT, hudsystem.ramTotal, hudsystem.ramUsed, HUD_BAR_BGCOLOR, HUD_BAR_COLOR);
	drawBar(barX, cpuY, SYSTEM_BAR_WIDTH, SYSTEM_BAR_HEIGHT, 1.0f, hudsystem.cpuUsed, HUD_BAR_BGCOLOR, HUD_BAR_COLOR);

	drawText(textX, ramY, textSize, "^nRAM", colorWhite, qfalse, qfalse, &hudglobal.font1);
	drawText(textX, cpuY, textSize, "^nCPU", colorWhite, qfalse, qfalse, &hudglobal.font1);
	if ((hudsystem.lastFpsCheck + seth.value[VAR_CHUD_SYS_FPSUPDATEDELAY]) < eth.cg_time) {
		hudsystem.fpsCurrent = hudsystem.fpsFrameCount / seth.value[VAR_CHUD_SYS_FPSUPDATEDELAY] * 1000;
		if (hudsystem.fpsCurrent > hudsystem.fpsMax)
			hudsystem.fpsMax = hudsystem.fpsCurrent;
		hudsystem.fpsFrameCount = 0;
		hudsystem.lastFpsCheck = eth.cg_time;
	}

	if (showFps) {
		char msgCurrent[20];
		snprintf(msgCurrent, sizeof(msgCurrent), "^nFPS(^2%d^n)", hudsystem.fpsCurrent);
		int fpsCurrentX = HUD_BOX_SPACE + (boxWidth - sizeTextWidth(textSize, msgCurrent, &hudglobal.font1)) / 2;

		int fpsCurrentY = cpuY + SYSTEM_BAR_HEIGHT + HUD_INSIDEBOX_SPACE;
		drawText(fpsCurrentX, fpsCurrentY, textSize, msgCurrent, colorWhite, qfalse, qfalse, &hudglobal.font1);

		char msgMax[20];
		snprintf(msgMax, sizeof(msgMax), "^nMAX(^2%d^n)", hudsystem.fpsMax);
		int fpsMaxTextWidth = sizeTextWidth(textSize, msgMax, &hudglobal.font1);
 
		int fpsBarY = fpsCurrentY + SYSTEM_BAR_HEIGHT + HUD_INSIDEBOX_SPACE;
		int fpsBarWidth = boxWidth - 3 * HUD_INSIDEBOX_SPACE - fpsMaxTextWidth;

		drawBar(barX, fpsBarY, fpsBarWidth, SYSTEM_BAR_HEIGHT, hudsystem.fpsMax, hudsystem.fpsCurrent, HUD_BAR_BGCOLOR, HUD_BAR_COLOR);

		int fpsMaxTextX = barX + fpsBarWidth + HUD_INSIDEBOX_SPACE;
		drawText(fpsMaxTextX, fpsBarY, textSize, msgMax, colorWhite, qfalse, qfalse, &hudglobal.font1);
	}

	hudglobal.leftNextY += boxHeight + HUD_BOX_SPACE;
	
}

void drawHudTimers() { // TODO: round timer (timelimit)

	qboolean isSpectator = (eth.cg_snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR);

	if (!isSpectator) {
		char str[30];
		snprintf(str, sizeof(str), "^0[^1 %d ^2-^1 %d ^0]", getSpawntimer(qfalse), getSpawntimer(qtrue));

		int x = 320 - (sizeTextWidth(TIMERS_FONTSIZE, str, &eth.fontVariable) / 2);
		drawTextRect(x, HUD_BOX_SPACE * 2, TIMERS_FONTSIZE, str, colorWhite, qfalse, qtrue, qtrue, qfalse, HUD_BOX_BGCOLOR, colorBlack, &eth.fontVariable);

	}
}

void drawHudWeaponInfo() { // TODO: better ammo handling (akimbos)
	if (eth.cg_snap->ps.persistant[PERS_TEAM] == TEAM_SPECTATOR)
		return;

	if (eth.cg_snap->ps.stats[STAT_HEALTH] <= 0)
		return;

	int weapon = eth.cg_snap->ps.weapon;
	int t_weapon = weapon;

	char *weaponName;
	char ammoStr[24];
	switch (weapon) {
		case WP_NONE:
			weaponName = "^nNONE";
			break;
		case WP_KNIFE:
			weaponName = "^nKNIFE";
			break;
		case WP_LUGER:
			weaponName = "^nLUGER";
			break;
		case WP_MP40:
			weaponName = "^nMP40";
			break;
		case WP_GRENADE_LAUNCHER:
			weaponName = "^nGRENADE";
			break;
		case WP_PANZERFAUST:
			weaponName = "^nPANZERFAUST";
			break;
		case WP_FLAMETHROWER:
			weaponName = "^nFLAMETHROWER";
			break;
		case WP_COLT:
			weaponName = "^nCOLT";
			break;
		case WP_THOMPSON:
			weaponName = "^nTHOMPSON";
			break;
		case WP_GRENADE_PINEAPPLE:
			weaponName = "^nGRENADE";
			break;
		case WP_STEN:
			weaponName = "^nSTEN";
			break;
		case WP_MEDIC_SYRINGE:
			weaponName = "^nSYRINGE";
			break;
		case WP_AMMO:
			weaponName = "^nAMMO";
			break;
		case WP_ARTY:
			weaponName = "^nARTY";
			break;
		case WP_SILENCER:
			weaponName = "^nLUGER SILENCED";
			t_weapon = WP_LUGER;
			break;
		case WP_DYNAMITE:
			weaponName = "^nDYNAMITE";
			break;
		case WP_SMOKETRAIL:
			weaponName = "^nSMOKETRAIL";
			break;
		case WP_MAPMORTAR:
			weaponName = "^nMAP MORTAR";
			break;
		case WP_MEDKIT:
			weaponName = "^nMEDKIT";
			break;
		case WP_BINOCULARS:
			weaponName = "^nBINOCULARS";
			break;
		case WP_PLIERS:
			weaponName = "^nPLIERS";
			break;
		case WP_SMOKE_MARKER:
			weaponName = "^nSMOKE MARKER";
			break;
		case WP_GARAND:
			weaponName = "^nGARAND";
			break;
		case WP_LANDMINE:
			weaponName = "^nLANDMINE";
			break;
		case WP_SATCHEL:
			weaponName = "^nSATCHEL";
			break;
		case WP_SATCHEL_DET:
			weaponName = "^nSATCHEL (DET)";
			break;
		case WP_TRIPMINE:
			weaponName = "^nTRIP MINE";
			break;
		case WP_SMOKE_BOMB:
			weaponName = "^nSMOKE BOMB";
			break;
		case WP_MOBILE_MG42:
		case WP_MOBILE_MG42_SET:
			weaponName = "^nMOBILE MG42";
			break;
		case WP_K43:
			weaponName = "^nK43";
			break;
		case WP_FG42:
			weaponName = "^nFG42";
			break;
		case WP_DUMMY_MG42:
			weaponName = "^nDUMMY MG42";
			break;
		case WP_MORTAR:
		case WP_MORTAR_SET:
			weaponName = "^nMORTAR";
			break;
		case WP_AKIMBO_COLT:
			weaponName = "^nAKIMBO COLT";
			t_weapon = WP_COLT;
			break;
		case WP_AKIMBO_LUGER:
			weaponName = "^nAKIMBO LUGER";
			t_weapon = WP_LUGER;
			break;
		case WP_SILENCED_COLT:
			weaponName = "^nCOLT SILENCED";
			t_weapon = WP_COLT;
			break;
		case WP_GARAND_SCOPE:
			weaponName = "^nGARAND SCOPED";
			t_weapon = WP_GARAND;
			break;
		case WP_K43_SCOPE:
			weaponName = "^nK43 SCOPED";
			t_weapon = WP_K43;
			break;
		case WP_FG42SCOPE:
			weaponName = "^nFG42 SCOPED";
			t_weapon = WP_FG42;
			break;
		case WP_MEDIC_ADRENALINE:
			weaponName = "^nADRENALINE";
			break;
		case WP_AKIMBO_SILENCEDCOLT:
			weaponName = "^nAKIMBO COLT SILENCED";
			t_weapon = WP_AKIMBO_COLT;
			break;
		case WP_AKIMBO_SILENCEDLUGER:
			weaponName = "^nAKIMBO LUGER SILENCED";
			t_weapon = WP_AKIMBO_LUGER;
			break;
		case WP_KAR98:
			weaponName = "^nK43";
			break;
		case WP_GPG40:
			weaponName = "^nK43 ARMED";
			break;
		case WP_CARBINE:
			weaponName = "^nGARAND";
			break;
		case WP_M7:
			weaponName = "^nGARAND ARMED";
			break;
		default:
			weaponName = "^nUNKNOWN";
			printf("WP NUM: %d\n", weapon);
			break;
	}

	if (showHudWeaponAmmo(weapon)) {
		int ammo = eth.cg_snap->ps.ammo[t_weapon];
		int clips = eth.cg_snap->ps.ammoclip[t_weapon];

		if ((clips > 0) || (ammo > 0))
			snprintf(ammoStr, sizeof(ammoStr), "^n%2d^2/^n%d", clips, ammo);
		else
			snprintf(ammoStr, sizeof(ammoStr), "^n%d", clips);

		int wpnameHeight = sizeTextHeight(WEAPON_FONTSIZE, &hudglobal.font3);
		int ammoWidth = sizeTextWidth(AMMO_FONTSIZE, ammoStr, &hudglobal.font3);

		int boxHeight = 3 * HUD_INSIDEBOX_SPACE + wpnameHeight + sizeTextHeight(AMMO_FONTSIZE, &hudglobal.font3);
		

		int maxWidth = sizeTextWidth(WEAPON_FONTSIZE, weaponName, &hudglobal.font3);

		if (ammoWidth > maxWidth)
			maxWidth = ammoWidth;

		int boxWidth = 2 * HUD_INSIDEBOX_SPACE + maxWidth;

		int boxX = SCREEN_WIDTH - boxWidth - HUD_BOX_SPACE;
		int boxY = SCREEN_HEIGHT - boxHeight - HUD_BOX_SPACE;

		drawFillRect(boxX, boxY, boxWidth, boxHeight, HUD_BOX_BGCOLOR);

		int wpnY = SCREEN_HEIGHT - HUD_BOX_SPACE - boxHeight + HUD_INSIDEBOX_SPACE;

		drawText(boxX + HUD_INSIDEBOX_SPACE, wpnY, WEAPON_FONTSIZE, weaponName, colorWhite, qfalse, qfalse, &hudglobal.font1);

		int ammoX = SCREEN_WIDTH - HUD_BOX_SPACE - boxWidth + (boxWidth - ammoWidth) / 2;
		int ammoY = wpnY + wpnameHeight + HUD_INSIDEBOX_SPACE;

		drawText(ammoX, ammoY, AMMO_FONTSIZE, ammoStr, colorWhite, qfalse, qfalse, &hudglobal.font3);
	}
}

qboolean showHudWeaponAmmo(int weapon) {
	switch (weapon) {
		case WP_KNIFE:
		case WP_ARTY:
		case WP_AMMO:
		case WP_DYNAMITE:
		case WP_MEDKIT:
		case WP_BINOCULARS:
		case WP_SMOKE_BOMB:
		case WP_SATCHEL:
		case WP_SATCHEL_DET:
		case WP_SMOKE_MARKER:
			return qfalse;
		default:
			return qtrue;
	}
	return qtrue;
}

void drawBar(float x, float y, float width, float height, float backValue, float frontValue, vec4_t backColor, vec4_t frontColor) {
	drawFillRect(x, y, width, height, backColor);

	float frontPercentage;

	if ((backValue <= 0.0f) || (frontValue <= 0.0f))
		frontPercentage = 0.0f;

	else frontPercentage = frontValue / backValue;
	if (frontPercentage > 1.0f)
		frontPercentage = 1.0f;

	drawFillRect(x, y, width * frontPercentage, height, frontColor);	
}

float scaleFromHeight(int height, fontInfo_t *font) {
	return height / ((font->glyphs['['].height + 6) * font->glyphScale);
}


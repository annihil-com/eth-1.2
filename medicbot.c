// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
 Medicbot
==============================
*/

#include "eth.h"

#define	MB_IDENTIFY "identify"
#define MB_FOLLOW "follow"
#define MB_FOLLOW_ME "me"
#define MB_FOLLOW_TEAM "team"
#define MB_FOLLOW_STOP "stop"
#define MB_REVIVE "revive"
#define MB_REVIVE_BUDDIES "irc"
#define MB_REVIVE_TEAM "team"
#define MB_REVIVE_ONLYME "me"

#define MB_PASSWORD_CHECK_TIME 300

static char *mbFollowType = MB_FOLLOW_TEAM;
static char *mbReviveType = MB_REVIVE_TEAM;

static int mbOwner = -1;
static char *mbPasswd = "public";

void doMedicBotCommand(const int player, char *msg) {
	char *partsOffset = msg;
	char *command = strsep(&partsOffset, " ");
	char *text = strsep(&partsOffset, " ");

	// Sanity check
	if ((command == NULL) || (text == NULL)) {
		printf("eth: medicbot: error: bad command received from player %i\n", player);
		return;
	}
	
	// Set owner
	if (!strcasecmp(command, MB_IDENTIFY)) {
		// Check password
		static int lastPasswordCheck = 0;
		if (((lastPasswordCheck + MB_PASSWORD_CHECK_TIME) < eth.cg_time) && !strcmp(text, mbPasswd)) {
			printf("eth: medicbot: new owner %i\n", player);
			mbOwner = player;
		} else {
			printf("eth: medicbot: identify failed for player %i\n", player);
		}
		lastPasswordCheck = eth.cg_time;
		return;
	}

	// If not the owner
	if (mbOwner != player)
		return;

	// Get action type
	char *type = strsep(&partsOffset, " ");
		
	// Sanity check
	if (!strcasecmp(command, MB_FOLLOW) || !strcasecmp(command, MB_REVIVE)) {
		if (!type) {
			printf("eth: medicbot: warning: no action type received from player %i for cmd [%s]\n", player, text);
			return;
		}
	} else {
		printf("eth: medicbot: warning: invalid cmd received [%s] from player %i\n", text, player);
		return;
	}

	// Follow type
	if (!strcasecmp(text, MB_FOLLOW)) {
		if (!strcasecmp(type, MB_FOLLOW_ME) || !strcasecmp(type, MB_FOLLOW_TEAM) || !strcasecmp(type, MB_FOLLOW_STOP)) {
			mbFollowType = type;
		} else {
			printf("eth: medicbot: warning: invalid follow type received [%s] from player %i\n", type, player);
			return;
		}
	}

	// Revive type
	if (!strcasecmp(text, MB_REVIVE)) {
		if (!strcasecmp(type, MB_REVIVE_BUDDIES) || !strcasecmp(type, MB_REVIVE_ONLYME) || !strcasecmp(type, MB_REVIVE_TEAM)) {
			mbReviveType = type;
		} else {
			printf("eth: medicbot: warning: invalid revive type received [%s] from player %i\n", type, player);
			return;
		}
	}
	
}

// Follow a player and assist him
void doMedicBot() {
	// Quit limbomenu
	static int lastOpenLimboMenuTime = 0;
	if ((eth.cg_snap->ps.eFlags & EF_TALK) && ((lastOpenLimboMenuTime + 1000) < eth.cg_time)) {
		syscall_CG_SendConsoleCommand("openlimbomenu\n");
		lastOpenLimboMenuTime = eth.cg_time;
	}
	
	// Do nothing if spec'ing a player
	if (eth.cg_snap->ps.clientNum != eth.cg_clientNum) {
		#ifdef ETH_DEBUG
			printf("eth: medicbot: do nothing cause spec'ing\n");
		#endif
		return;
	}
	
	// Can't follow/revive owner if owner don't exist
	if (mbOwner == -1) {
		mbFollowType = MB_FOLLOW_TEAM;
		mbReviveType = MB_REVIVE_TEAM;
	}
		
	// Change team action
	if (!strcmp(mbFollowType, MB_FOLLOW_ME)) {
		if (eth.clientInfo[eth.cg_clientNum].team != eth.clientInfo[mbOwner].team) {
			if (eth.clientInfo[eth.cg_clientNum].team != TEAM_SPECTATOR)
				syscall_CG_SendConsoleCommand("team spectator\n");
			else if (eth.clientInfo[mbOwner].team == TEAM_ALLIES)
				syscall_CG_SendConsoleCommand("team allies 1\n");
			else if (eth.clientInfo[mbOwner].team == TEAM_AXIS)
				syscall_CG_SendConsoleCommand("team axis 1\n");
			return;
		}
	} else if (!strcmp(mbFollowType, MB_FOLLOW_TEAM)) {
		// Get the numbers of players in each team
		int alliesPlayers = 0;
		int axisPlayers = 0;
		
		int count = 0;
		for (; count < MAX_CLIENTS; count++) {
			if (eth.clientInfo[count].infoValid) {
				if (eth.clientInfo[count].team == TEAM_ALLIES)
					alliesPlayers++;
				else if (eth.clientInfo[count].team == TEAM_AXIS)
					axisPlayers++;
			}
		}
		
		// Medicbot go to the smallest team
		if ((eth.clientInfo[eth.cg_clientNum].team == TEAM_SPECTATOR) || (abs(alliesPlayers - axisPlayers) > 1)) {
			if (alliesPlayers > axisPlayers)
				syscall_CG_SendConsoleCommand("team axis 1\n");
			else
				syscall_CG_SendConsoleCommand("team allies 1\n");
		}
	}
	
	// If bot is dead do tapeout
	if (IS_PLAYER_DEAD(eth.cg_clientNum))
		syscall_CG_SendConsoleCommand("forcetapout\n");
	
	// Find revive target
	int reviveTarget = -1;
	if (!strcmp(mbReviveType, MB_REVIVE_ONLYME) && IS_PLAYER_DEAD(mbOwner))
		reviveTarget = mbOwner;
	else if (!strcmp(mbReviveType, MB_REVIVE_TEAM))
		reviveTarget = findTarget(TARGET_FRIEND | TARGET_PLAYER_DEAD | TARGET_VISIBLE);
	// TODO: revive irc buddies

	// Find heal target
	#define HEAL_REQUEST_TIMEOUT 4000
	int healTarget = -1;
	if (!strcmp(mbReviveType, MB_REVIVE_ONLYME)
			&& (eth.lastMedicRequest == mbOwner)
			&& ((eth.lastMedicRequestTime + HEAL_REQUEST_TIMEOUT) > eth.cg_time)) {
		healTarget = mbOwner;
	} else if (!strcmp(mbReviveType, MB_REVIVE_TEAM)
			&& (!IS_PLAYER_ENEMY(eth.lastMedicRequest))
			&& ((eth.lastMedicRequestTime + HEAL_REQUEST_TIMEOUT) > eth.cg_time))
		healTarget = eth.lastMedicRequest;
	// TODO: heal irc buddies

	// Find follow target
	int followTarget = -1;
	#define FOLLOW_TIMEOUT 5000
	static int lastFollowTime = 0;
	static int lastFollowTarget = -1;
	if (!strcmp(mbFollowType, MB_FOLLOW_ME) && !VectorCompare(vec3_origin, eth.entities[mbOwner].origin))
		followTarget = mbOwner;
	else if (!strcmp(mbFollowType, MB_FOLLOW_TEAM)) {
		followTarget = findTarget(TARGET_FRIEND | TARGET_PLAYER_ALIVE | TARGET_VISIBLE);
		if ((followTarget == -1)
				&& ((lastFollowTime + FOLLOW_TIMEOUT) > eth.cg_time)
				&& !VectorCompare(vec3_origin, eth.entities[lastFollowTarget].origin))
			followTarget = lastFollowTarget;
	}

	// Find attack target
	int attackTarget = findTarget(TARGET_ENEMY | TARGET_PLAYER_ALIVE | TARGET_VISIBLE | TARGET_PLAYER_VULN);

	#ifdef ETH_DEBUG
		char reviveName[MAX_QPATH];
		strncpy(reviveName, eth.clientInfo[reviveTarget].name, sizeof(reviveName));
		Q_CleanStr(reviveName);
		char healName[MAX_QPATH];
		strncpy(healName, eth.clientInfo[healTarget].name, sizeof(healName));
		Q_CleanStr(healName);
		char followName[MAX_QPATH];
		strncpy(followName, eth.clientInfo[followTarget].name, sizeof(followName));
		Q_CleanStr(followName);
		char attackName[MAX_QPATH];
		strncpy(attackName, eth.clientInfo[attackTarget].name, sizeof(attackName));
		Q_CleanStr(attackName);
		if (strlen(reviveName) || strlen(healName) || strlen(followName) || strlen(attackName))
			printf("eth: medicbot: found target: revive: [%s] heal: [%s] follow: [%s] attack: [%s]\n", reviveName, healName, followName, attackName);
	#endif

	// Where the medicbot have to aim
	int target = -1;
	vec3_t *aim;
	if (reviveTarget != -1) {
		target = reviveTarget;
		aim = &eth.entities[target].origin;
	} else if (healTarget != -1) {
		target = healTarget;
		aim = &eth.entities[target].head;
	} else if ((followTarget != -1) && (VectorDistance(eth.cg_refdef.vieworg, eth.entities[followTarget].head) > 150.0f)) {
		target = followTarget;
		aim = &eth.entities[target].head;
		lastFollowTime = eth.cg_time;
		lastFollowTarget = target;
	} else if (attackTarget != -1) {
		target = attackTarget;
		aim = &eth.entities[target].head;
	// Nothing todo
	} else {
		setAction(ACTION_RUN, 0);
		return;
	}

	#ifdef ETH_DEBUG
		char targetName[MAX_QPATH];
		strncpy(targetName, eth.clientInfo[target].name, sizeof(targetName));
		Q_CleanStr(targetName);
		printf("eth: medicbot: aim at: [%s]\n", targetName);
	#endif

	// Aiming action
	aimAt(*aim);

	// Init some vars for make code more simple to read
	float distance = VectorDistance(eth.cg_refdef.vieworg, *aim);
	int isTargetFarAway = (distance > 150.0f);
	int isTargetNear = (distance < 50.0f);
	int isBotSyringe = (eth.cg_entities[eth.cg_clientNum].currentState->weapon == WP_MEDIC_SYRINGE);
	int isBotMedicPack = (eth.cg_entities[eth.cg_clientNum].currentState->weapon == WP_MEDKIT);

	#ifdef ETH_DEBUG
		printf("eth: medicbot: target: distance: %.2f\n", distance);
	#endif

	// --- Weapon select ---
	if (target == reviveTarget) {
		if (!isBotSyringe)
			syscall_CG_SendConsoleCommand("weapon 5\n");
	} else if (target == healTarget) {
		if (!isBotMedicPack)
			syscall_CG_SendConsoleCommand("weapon 6\n");
	} else {
		syscall_CG_SendConsoleCommand("weapon 3\n");
	}

	// --- Run action ---
	if (((target == followTarget) && isTargetFarAway)
			|| ((target == reviveTarget) && !isTargetNear)
			|| ((target == healTarget) && !isTargetNear)) {
		setAction(ACTION_RUN, 1);
	} else {
		setAction(ACTION_RUN, 0);
	}
	
	// --- Attack action ---
	if (((target == attackTarget) && !isBotSyringe && !isBotMedicPack)
			|| ((target == reviveTarget) && isBotSyringe)
			|| ((target == healTarget) && isBotMedicPack)) {
		setAction(ACTION_ATTACK, 1);
	} else {
		setAction(ACTION_ATTACK, 0);
	}

	// --- Crouch action ---
	/*if (((target == reviveTarget) || (target == followTarget)) && !isTargetFarAway)
		setAction(ACTION_CROUCH, 1);
	else if (target == attackTarget)
		setAction(ACTION_CROUCH, 1);
	else
		setAction(ACTION_CROUCH, 0);*/

}

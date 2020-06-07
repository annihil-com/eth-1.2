// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
All game functions hooked for get information from it.
==============================
*/

#include <sys/resource.h>

#include "eth.h"

void eth_CG_R_AddRefEntityToScene(refEntity_t *refEnt) {
	ethEntity_t* entity = &eth.entities[refEnt->entityNum];
	int entityNum = refEnt->entityNum;
	trace_t trace;

	// Backup refEntity for spycam
	memcpy(&eth.refEntities[eth.refEntitiesCount++], refEnt, sizeof(refEntity_t));

	if ((entityNum < MAX_CLIENTS)
		&& (eth.cg_entities[entityNum].currentState->eType == ET_PLAYER)
		&& eth.clientInfo[entityNum].infoValid
		&& (eth.cg_clientNum != entityNum)) {
			entity->isValid = qtrue;

		if ((refEnt->hModel == eth.hHead) || (refEnt->hModel == eth.hGlasses)) {
			// Skip already define head entity (hud head)
			if (!VectorCompare(vec3_origin, entity->head))
				return;

			// Backup head origin
			VectorCopy(refEnt->origin, entity->head);

			//orientation_t or;
			//eth_CG_GetTag(entityNum, "tag_head", &or);
			//if (!VectorCompare(or.origin, refEnt->origin))
			//	return;			

			// Add some vector Z for better display of esp
			vec3_t tmpVector;
			VectorCopy(entity->head, tmpVector);
			tmpVector[ROLL] += 20.0f;
			entity->isInScreen = worldToScreen(tmpVector, &entity->screenX, &entity->screenY);

			// Backup head axis for mortar trace
			memcpy(entity->headAxis, refEnt->axis, sizeof(vec3_t) * 3);

			if( seth.value[VAR_AIMFOV] > 0 ){
				// Get hitboxes from models
				orig_syscall( CG_R_MODELBOUNDS, refEnt->hModel, entity->rmin, entity->rmax );
				// Apply vecz
				doVecZ(entityNum);

				// Prediction
				#ifdef ETH_DEBUG
					if (!seth.value[VAR_PREDICT_STATS])
				#endif
				doPrediction(entityNum);
			}
			
			// Draw a rail trail to see where the player look
			if (seth.value[VAR_RAILAXIS]) {
				vec3_t forward;
				VectorMA(entity->head, 64, refEnt->axis[0], forward);
				eth.CG_RailTrail2(NULL, entity->head, forward);
			}

			// If head visible
			if (isVisible(entity->head)) {
				entity->isVisible = qtrue;
				entity->isHeadVisible = qtrue;
			}

			// Pig head for enemy
			if (seth.value[VAR_VISUALS] && seth.value[VAR_PIGHEAD] && IS_PLAYER_ENEMY(entityNum)) {
				refEnt->hModel = eth.pigHeadModel;
				refEnt->customSkin = eth.pigHeadSkin;
			// Chams
			} else {
				if (seth.value[VAR_VISUALS] && seth.value[VAR_CHAMS]
						&& !seth.value[VAR_PIGHEAD]
						&& (seth.value[VAR_CHAMS] != CHAM_GLOWONLY)) {
					addChams(refEnt, entity->isVisible);
				}
			}

		// If body
		} else if (refEnt->torsoFrameModel) {
			// Backup bodyRefEnt for tag grabbing
			memcpy(&eth.cg_entities[entityNum].pe_bodyRefEnt, refEnt, sizeof(refEntity_t));

			// Find a visible body parts
			vec3_t target;
			if (getVisibleModelBodyTag(entityNum, &target)) {
				VectorCopy(target, entity->bodyPart);
				entity->isVisible = qtrue;
			}

			// Chams
			if (seth.value[VAR_VISUALS] && seth.value[VAR_CHAMS])
				addChams(refEnt, entity->isVisible);
		// Not body or head
		} else
			return;
	} else if (IS_MISSILE(entityNum)) {
		entity->isValid = qtrue;

		// If visible
		eth.CG_Trace(&trace, eth.cg_refdef.vieworg, NULL, NULL, refEnt->origin, eth.cg_snap->ps.clientNum, CONTENTS_SOLID | CONTENTS_CORPSE);
		if (trace.fraction == 1.0f)
			entity->isVisible = qtrue;

		// Chams
		if (seth.value[VAR_VISUALS] && seth.value[VAR_CHAMS])
			addChams(refEnt, (trace.fraction == 1.0f));
	// Add rail trail here so it can have wallhack too
	} else if (refEnt->reType == RT_RAIL_CORE) {

	// Special care if AMMO or HEALTH
	} else if( (refEnt->hModel == eth.hMedicPack) || (refEnt->hModel == eth.hAmmoPack) ) {
		if( !seth.value[VAR_ESPITEMS] )
			return;
		if( seth.value[VAR_CHAMS] )
			addChams(refEnt, entity->isVisible);
	// Not player or missile
	} else
		return;

	// Wallhack
	if (seth.value[VAR_VISUALS] && seth.value[VAR_WALLHACK])
		refEnt->renderfx |= RF_DEPTHHACK | RF_NOSHADOW;
}

void eth_CG_S_UpdateEntityPosition(int entityNum, const vec3_t origin) {
	ethEntity_t* entity = &eth.entities[entityNum];
	entity->isValid = qtrue;

	if (IS_PLAYER_VALID(entityNum) || IS_MISSILE(entityNum)) {
		VectorCopy(origin, entity->origin);
		entity->distance = VectorDistance(eth.cg_refdef.vieworg, entity->origin);
		
		// isInScreen is define below for players
		if (entityNum > MAX_CLIENTS)
			entity->isInScreen = worldToScreen(entity->origin, &entity->screenX, &entity->screenY);

		addEspColors(entityNum, entity->isVisible);
	}
}

void eth_CG_DrawActiveFrame(void) {
	// Don't draw stuff in intermission
	if (eth.cgs_gamestate == GS_INTERMISSION)
		return;
	
	setCurrentMuzzle();

	#ifdef ETH_DEBUG
		// Draw debugview
		if (seth.value[VAR_MEMDUMP])
			drawMemDump(getVarOffset(seth.value[VAR_MEMDUMP]) + (eth.offsetMul * 200) + (eth.offsetSub * 20));
		if (seth.value[VAR_PREDICT_STATS])
			doPredictionStats();
	#endif

	// Don't draw eth's visuals while drawing scoreboard (TAB)
	// it's a big mess otherwise
	qboolean drawEthVisuals = qtrue;
	if (isKeyActionDown("+scores")) {
		drawEthVisuals = qfalse;
		setAction(ACTION_SCOREBOARD, 1);
	} else if (seth.value[VAR_CHUD]) {
		setAction(ACTION_SCOREBOARD, 0);
	}

	if (!eth.demoPlayback) {
		if (seth.value[VAR_REQUEST_MEDIC] && (eth.mod->type != MOD_TCE))
			autoRequestMedic();

		// Auto demo record
		autoRecord();

		// Autoshoot (dont steal mouse if wp = ammo/health/etc)
		if ( seth.value[VAR_AUTOSHOOT] != AUTOSHOOT_OFF ){
			 if( isAimbotWeapon(eth.cg_entities[eth.cg_clientNum].currentState->weapon) )
				setAction(ACTION_BINDMOUSE1, qfalse);
			else
				setAction(ACTION_BINDMOUSE1, qtrue);	
		} else
			setAction(ACTION_BINDMOUSE1, qtrue);

		// Medicbot
		if (seth.value[VAR_MEDICBOT])
			doMedicBot();

		// Random name changer
		if (seth.value[VAR_RANDOMNAME]) {
			// If error stop it
			if (!setRandomName())
				seth.value[VAR_RANDOMNAME] = qfalse;
		}
		
		doAimbot();

	}

	if (eth.pointer)
	    orig_syscall(CG_KEY_SETCATCHER, KEYCATCH_UI | orig_syscall(CG_KEY_GETCATCHER));

	if (eth.isMenuOpen) {
		drawMenu();
		// Get mouse/key events
		orig_syscall(CG_KEY_SETCATCHER, KEYCATCH_UI | orig_syscall(CG_KEY_GETCATCHER));
	}

	// Remove spectator locking
	if (seth.value[VAR_SPECLOCK])
		eth.cg_snap->ps.powerups[PW_BLACKOUT] = 0;

	// User setting to not draw anything
	if( seth.value[VAR_VISUALS] ){
		// Set esp colors from vars
		setColors();
			
		if (!eth.demoPlayback && drawEthVisuals) { // These funcs are not needed when watching a demo
				// Draw spectators
			if (seth.value[VAR_SPEC] && (eth.cg_snap->ps.persistant[PERS_TEAM] != TEAM_SPECTATOR)) {
				getSpectators();
				drawSpectators();
			}
				
				// Draw irc stuff
			drawIrcChat();
			if (seth.value[VAR_IRC]) {
				if (seth.value[VAR_IRC_FT])
					drawIrcFireteam();
				if (seth.value[VAR_IRC_TOPIC] || seth.value[VAR_IRC_INVITE])
					drawIrcTopic();
			}
		
				// Teamkillers / Friends add/remove list
			if (eth.isPlistOpen) {
				drawPlayerList();
					// Get mouse/key events
				orig_syscall(CG_KEY_SETCATCHER, KEYCATCH_UI | orig_syscall(CG_KEY_GETCATCHER));
			}

				// Referee counter
			if (seth.value[VAR_REFLIST])
				drawReferees();

		}
			
		if (drawEthVisuals && (eth.cgs_gamestate != GS_INTERMISSION) ) {
				// Custom HUDs
			if (seth.value[VAR_CHUD] == HUD_1)
				drawCustomHud();
			else if (seth.value[VAR_CHUD] == HUD_2)
				drawETHhud();

				// Draw right spawntimer
			if (seth.value[VAR_SPAWNTIMER] && !seth.value[VAR_CHUD]) // Don't need 2 spawntimers at the same time
				drawSpawnTimerRight();
			
				// Draw advert
			if (seth.value[VAR_ADVERT])
				drawAdvert();

			if( seth.value[VAR_BANNER] )
				drawBanner();
			
				// Draw players infos
			drawEspEntities();
			
				// Draw radar
			switch ((int)seth.value[VAR_RADAR]) {
				case 1: drawRadar(200); break;
				case 2: drawRadar(150); break;
				case 3: drawRadar(100); break;
				case 4: drawRadar(50); break;
				case 5: drawRadarNg(150); break;
				case 6: drawRadarNg(100); break;
				case 7: drawRadarNg(50); break;
				default: break;
			}

				// Crosshair
			if (seth.value[VAR_CROSSHAIR])
				drawCrosshair();
		}
	}
}

qboolean eth_CG_R_RenderScene(refdef_t *refdef) {
	char fov[8];
	float x;

	// tce zoom hack
	if (seth.value[VAR_WEAPZOOM] && eth.mod->type == MOD_TCE) {
		static float lastfx = 0.0f;
		static float lastfy = 0.0f;
		static int twidth = 0;
		static int theight = 0;

		// Init max fov
		if (lastfx == 0.0f)
			lastfx = refdef->fov_x;
		if (lastfy == 0.0f)
			lastfy = refdef->fov_y;

		if (refdef->x < 2) {
			//"fixed" fov for aiming mode
			if (refdef->fov_x < lastfx)
				refdef->fov_x = lastfx;
			if (refdef->fov_y < lastfy)
				refdef->fov_y = lastfy;
			// init max w and h
			if (!twidth || twidth < refdef->width)
				twidth = refdef->width;
			if (!theight || theight < refdef->height)
				theight = refdef->height;
		} else {
			// increase zoom win a little 
			if (refdef->x > 21) {
				refdef->x -= 20;
				refdef->width += 40;
			}	    
			if (refdef->y > 51) {
				refdef->y -= 20;
				refdef->height += 40;
		    } 
			// Simple fov correction for scope
			if (twidth && theight) {
				refdef->fov_x = (lastfx * refdef->width) / twidth;
				refdef->fov_y = (lastfy * refdef->height) / theight;
				// Decide which way is better but it's not good enough
				//float tcx = refdef->width / tan(refdef->fov_x / 360 * M_PI);
				//refdef->fov_y = atan2(refdef->height, tcx);
				//refdef->fov_y *= 360 / M_PI;
			}
		}    
	}

	// Check if the scene is the mainview
	if ((refdef->x == 0) || (refdef->x == 1)) {	// Some mods use 0 or 1 for x
		// Check for background scene (only some tce version use that)
		if ((eth.mod->type == MOD_TCE) && (refdef->rdflags & RDF_SKYBOXPORTAL) && (eth.mod->crc32 == 0x965ad597))
			return qtrue;

		// Copy main refdef
		memcpy(&eth.cg_refdef, refdef, sizeof(refdef_t));
		vectoangles(eth.cg_refdef.viewaxis[0], eth.cg_refdefViewAngles);

		if (seth.value[VAR_WEAPZOOM] && (eth.mod->type != MOD_TCE)) {
			switch(eth.cg_snap->ps.weapon) {
				case WP_FG42SCOPE:
				case WP_GARAND_SCOPE:
				case WP_K43_SCOPE:
					// fov_x - Get from cg_fov cvar
					syscall_CG_Cvar_VariableStringBuffer("cg_fov", fov, sizeof(fov));
					refdef->fov_x = atof(fov);

					// fov_y - Algo taken from cg_view.c line 961
					x = refdef->width / tan(refdef->fov_x / 360 * M_PI);
					refdef->fov_y = atan2(refdef->height, x);
					refdef->fov_y *= 360 / M_PI;

	        			break;
				case WP_MOBILE_MG42_SET:
					// fov_x - Get from cg_fov cvar
					syscall_CG_Cvar_VariableStringBuffer("cg_fov", fov, sizeof(fov));
					refdef->fov_x = atof(fov);
					
					// fov_y - Algo taken from cg_view.c line 961
					x = refdef->width / tan(refdef->fov_x / 360 * M_PI);
					refdef->fov_y = atan2(refdef->height, x);
					refdef->fov_y *= 360 / M_PI;
					break;
				default:
					break;
			}
		} else if (eth.mod->type == MOD_TCE) {
			char fov[8];
			syscall_CG_Cvar_VariableStringBuffer("cg_fov", fov, sizeof(fov));
			int fov_val = atoi(fov);
			refdef->fov_x = fov_val;
			refdef->fov_y = fov_val;
		}

		// Call original function
		orig_syscall(CG_R_RENDERSCENE, refdef);
		
		// Weapons are different in tce
		if (eth.mod->type != MOD_TCE) {
			// Weapons spy cam
			switch (eth.cg_entities[eth.cg_snap->ps.clientNum].currentState->weapon) {
				case WP_MORTAR_SET:
					if (seth.value[VAR_MORTARCAM])
						drawMortarCam();
					break;
				case WP_PANZERFAUST:
					if (seth.value[VAR_PANZERCAM])
						drawPanzerCam();
					break;
				case WP_SATCHEL_DET:
					if (seth.value[VAR_SATCHELCAM])
						drawSatchelCam();
					break;
				case WP_GPG40:
				case WP_M7:
					if (seth.value[VAR_RIFLECAM])
						drawRiffleCam();
					break;
				default:
					break;
			}
		}

		if (seth.value[VAR_MIRRORCAM])
			drawMirrorCam();

		return qfalse;
	}
	return qtrue;
}

// Return qfalse if this pic don't have to be draw
qboolean eth_CG_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader) {

 	adjustTo640(&x, &y, &w, &h);

 	// Remove sniper/binocular picture
 	if (eth.mod->type == MOD_TCE) {
		if (hShader == eth.tcescope1 || hShader == eth.tcescope2 || hShader == eth.cgs_media_reticleShaderSimple)
			return qfalse;
	if ((int)w > 20 && (int)h > 40 && hShader == eth.cgs_media_whiteShader )
		return qfalse;
	if ((((int)w < 7 && (int)h > 40) || ((int)h < 7 && (int)w > 40)) && hShader == eth.cgs_media_whiteShader )
		return qfalse;
	if (((int)w < 7 && (int)h < 7) && hShader == eth.cgs_media_whiteShader )
		return qfalse;
 	} else {
		if (hShader == eth.cgs_media_reticleShaderSimple || hShader == eth.cgs_media_binocShaderSimple)
			return qfalse;
 	}

	// Remove sniper/binocular black area
	if (((int)y == 0) && ((int)w == 80) && (hShader == eth.cgs_media_whiteShader))
		return qfalse;

	// Remove zoom black fade in/out and flashbang in tce
	if (((int)x == -10) && ((int)y == -10) && ((int)w == 650) && ((int)h == 490) && (hShader == eth.cgs_media_whiteShader))
		return qfalse;

	return qtrue;
}

void eth_CG_Init(void) {
	loadCL_GUID();
	
	initHUD();
	
	srand(time(NULL));
	initActions();

	// Init stats
	eth.lastKillTime = 0;
	eth.firstKillSpreeTime = 0;

	#ifdef ETH_DEBUG
		// Init debug view vars
		eth.offsetSub = 0;
		eth.offsetMul = 0;
	#endif

	// Init eth medias
	registerEthMedias();

	//syscall_CG_S_StartLocalSound(eth.sounds[SOUND_PREPARE], CHAN_AUTO); // TODO: not work ??? wtf ?

	initActions();
	resetAllActions();

	// If we do \disconnect and then \connect without quitting the game, update our buddy slot
	ircCGinit();
}

void eth_CG_Shutdown(void) {
	setAction(ACTION_BINDMOUSE1, 1);
	blockMouse(qfalse);	// To avoid sensitivity 0
}

// This function was hook only to force sniper view
void eth_CG_FinishWeaponChange(int lastweap, int newweap) {
	qboolean callOriginal = qfalse;
	
	if (!((lastweap == WP_K43_SCOPE && newweap == WP_K43)
			|| (lastweap == WP_GARAND_SCOPE && newweap == WP_GARAND)
			|| (lastweap == WP_FG42SCOPE && newweap == WP_FG42)))
		callOriginal = qtrue;

	int weapon = lastweap;
	switch (weapon) {
		case WP_K43_SCOPE:
			weapon = WP_K43;
			break;
		case WP_GARAND_SCOPE:
			weapon = WP_GARAND;
			break;
		case WP_FG42SCOPE:
			weapon = WP_FG42;
			break;
		default:
			break;
	}

	if (eth.cg_snap->ps.ammoclip[weapon] == 0)
		callOriginal = qtrue;

	if (isKeyActionDown("+reload") || isKeyActionDown("weapalt"))
		callOriginal = qtrue;

	if (callOriginal)
		eth.CG_FinishWeaponChange(lastweap, newweap);
}

// Hooked for catching kills and revive spree
void eth_CG_EntityEvent(centity_t *cent, vec3_t position) {
	entityState_t *es = &cent->currentState;;
	int event = es->event & ~EV_EVENT_BITS;

	switch (event) {
		case EV_OBITUARY:
			// Don't count kills in warmup.
			if (eth.cgs_gamestate != GS_PLAYING)
				break;
			// For now this way, report if it doesen't work on some mods
			int target, attacker;
			if (eth.mod->type == MOD_ETPRO) {
				target = es->time;
				attacker = es->time2;
			} else {
				target = es->otherEntityNum;
				attacker = es->otherEntityNum2;
			}
			// If we do a selfkill or someone kills us, reset killing spree
			if (target == eth.cg_clientNum) {
				if( attacker > MAX_CLIENTS )
					strncpy( eth.KillerName, "world" ,sizeof(eth.KillerName) );
				else
					strncpy( eth.KillerName, eth.clientInfo[attacker].name,sizeof(eth.KillerName) );
				if (seth.value[VAR_KILLSPAM])
					deathSpam();				
				eth.killCountNoDeath = 0;
				eth.killSpreeCount = 0;
				eth.reviveSpree = 0;
				break;
			// Increase killcount only if we're the attacker and if we killed a player from the opposite team
			} else if (attacker == eth.cg_clientNum && eth.clientInfo[target].team != eth.clientInfo[eth.cg_clientNum].team) {
				strncpy(eth.VictimName, eth.clientInfo[target].name, sizeof(eth.VictimName));
				eth.killCount++;
				eth.killCountNoDeath++;
				eth.autoDemoKillCount++;	// Backup killcount for auto-record demo
				// If not a killing spree
				if ((eth.cg_time - eth.lastKillTime) > (SPREE_TIME * 1000)) {
					eth.firstKillSpreeTime = eth.cg_time;
					eth.killSpreeCount = 1;
				} else {
					eth.killSpreeCount++;
				}
				eth.lastKillTime = eth.cg_time;
				if (eth.killSpreeCount > 1)
					playSpreeSound();
				if (seth.value[VAR_KILLSPAM])
					killSpam();
			}
			break;
		case EV_FIRE_WEAPON:
			eth.bullettime = eth.cg_time;
			break;			
		default:
			break;
	}

	eth.CG_EntityEvent(cent, position);
}

// Kick some recoil
void eth_CG_WeaponFireRecoil(int weapon){
	if( seth.value[VAR_RECOIL] )
		eth.CG_WeaponFireRecoil( weapon );
	return;
}
void eth_CG_DamageFeedback(int yawByte, int pitchByte, int damage){
	if( seth.value[VAR_RECOIL] )
		eth.CG_DamageFeedback( yawByte, pitchByte, damage );
	return;
}

// landmine hack
void eth_CG_Missile( centity_t *cent ) {
	if( !cent )
		return;
	
	if( cent->currentState.weapon == WP_LANDMINE && seth.value[VAR_ESPMINES] ){
		team_t *cgsTeam;
		if( eth.mod->cgs ){
			int offset = eth.mod->cgs + eth.mod->clientInfoOffset + 0x88; // 2*MAX_QPATH + 8;
			cgsTeam = (team_t *) ((void *)(eth.cgameLibAddress + offset+eth.mod->clientInfoSize*eth.cg_clientNum));
		} else
			cgsTeam = &eth.cgs->clientinfo[eth.cg_clientNum].team;
			
		team_t oldTeam = *cgsTeam;
		team_t mineTeam = (!cent->currentState.otherEntityNum2 ? TEAM_ALLIES : TEAM_AXIS);

		if( *cgsTeam == TEAM_SPECTATOR ){
			*cgsTeam = mineTeam;
			eth.CG_Missile( cent );
			*cgsTeam = TEAM_SPECTATOR;
			return;
		}

		*cgsTeam = mineTeam;
		eth.CG_Missile( cent );
		*cgsTeam = oldTeam;

		return;
	}

	eth.CG_Missile( cent );
}

/*
==============================
cvars unlocker
==============================
*/

cvar_t *eth_Cvar_Set2(const char *var_name, const char *value, qboolean force) {
	#ifdef ETH_DEBUG
		ethDebug("cvar: [%s] forced to [%s]", var_name, value);
	#endif
	return orig_Cvar_Set2(var_name, value, qtrue);
}

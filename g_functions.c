// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2006 *nixCoders team - don't forget to credit us

/*
==============================
All functions take from sdk or call through game syscall.
==============================
*/

#include "eth.h"

/*
==============================
from game/q_shared.c
==============================
*/

int Q_PrintStrlen(const char *string) {
	int			len;
	const char	*p;

	if( !string ) {
		return 0;
	}

	len = 0;
	p = string;
	while( *p ) {
		if( Q_IsColorString( p ) ) {
			p += 2;
			continue;
		}
		p++;
		len++;
	}

	return len;
}

char *Q_CleanStr(char *string) {
	char*	d;
	char*	s;
	int		c;

	s = string;
	d = string;
	while ((c = *s) != 0 ) {
		if ( Q_IsColorString( s ) ) {
			s++;
		}		
		else if ( c >= 0x20 && c <= 0x7E ) {
			*d++ = c;
		}
		s++;
	}
	*d = '\0';

	return string;
}

int Q_stricmpn (const char *s1, const char *s2, int n) {
	int		c1, c2;
	
	do {
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) {
			return 0;		// strings are equal until end point
		}
		
		if (c1 != c2) {
			if (c1 >= 'a' && c1 <= 'z') {
				c1 -= ('a' - 'A');
			}
			if (c2 >= 'a' && c2 <= 'z') {
				c2 -= ('a' - 'A');
			}
			if (c1 != c2) {
				return c1 < c2 ? -1 : 1;
			}
		}
	} while (c1);
	
	return 0;		// strings are equal
}

int Q_stricmp (const char *s1, const char *s2) {
	return (s1 && s2) ? Q_stricmpn (s1, s2, 99999) : -1;
}

char *eth_Info_ValueForKey(const char *s, const char *key) {
	char	pkey[BIG_INFO_KEY];
	static	char value[2][BIG_INFO_VALUE];	// use two buffers so compares
											// work without stomping on each other
	static	int	valueindex = 0;
	char	*o;
	
	if ( !s || !key ) {
		return "";
	}

	if ( strlen( s ) >= BIG_INFO_STRING ) {
		//Com_Error( ERR_DROP, "Info_ValueForKey: oversize infostring [%s] [%s]", s, key );
		printf("Info_ValueForKey: oversize infostring [%s] [%s]", s, key);
	}

	valueindex ^= 1;
	if (*s == '\\')
		s++;
	while (1)
	{
		o = pkey;
		while (*s != '\\')
		{
			if (!*s)
				return "";
			*o++ = *s++;
		}
		*o = 0;
		s++;

		o = value[valueindex];

		while (*s != '\\' && *s)
		{
			*o++ = *s++;
		}
		*o = 0;

		if (!Q_stricmp (key, pkey) )
			return value[valueindex];

		if (!*s)
			break;
		s++;
	}

	return "";
}

char	* QDECL va( char *format, ... ) {
	va_list		argptr;
	#define	MAX_VA_STRING	32000
	static char		temp_buffer[MAX_VA_STRING];
	static char		string[MAX_VA_STRING];	// in case va is called by nested functions
	static int		index = 0;
	char	*buf;
	int len;


	va_start (argptr, format);
	vsprintf (temp_buffer, format,argptr);
	va_end (argptr);

	if ((len = strlen(temp_buffer)) >= MAX_VA_STRING) {
		//Com_Error( ERR_DROP, "Attempted to overrun string in call to va()\n" );
		printf("Attempted to overrun string in call to va()\n");
	}

	if (len + index >= MAX_VA_STRING-1) {
		index = 0;
	}

	buf = &string[index];
	memcpy( buf, temp_buffer, len+1 );

	index += len + 1;

	return buf;
}

/*
==============================
from cgame/cg_players.c
==============================
*/
qboolean eth_CG_GetTag(int clientNum, char *tagname, orientation_t *or) {
	eth_centity_t 	*cent;
	refEntity_t		*refent;
	vec3_t			tempAxis[3];
	vec3_t			org;
	int			i;

	cent = &eth.cg_entities[clientNum];

	if (!cent->currentValid)
		return qfalse;		// not currently in PVS

	refent = &cent->pe_bodyRefEnt;

	if (syscall_CG_R_LerpTag(or, refent, tagname, 0 ) < 0)
		return qfalse;

	VectorCopy(eth.entities[clientNum].origin, org);

	for (i = 0 ; i < 3 ; i++)
		VectorMA(org, or->origin[i], refent->axis[i], org);

	VectorCopy(org, or->origin);

	// rotate with entity
	MatrixMultiply(refent->axis, or->axis, tempAxis);
	memcpy(or->axis, tempAxis, sizeof(vec3_t) * 3);

	return qtrue;
}

/*
==============================
from game/bg_misc.c
==============================
*/
const unsigned int aReinfSeeds[MAX_REINFSEEDS] = { 11, 3, 13, 7, 2, 5, 1, 17 };

void eth_BG_EvaluateTrajectoryDelta( const trajectory_t *tr, int atTime, vec3_t result, qboolean isAngle, int splineData ) {
	float	deltaTime;
	float	phase;

	switch( tr->trType ) {
	case TR_STATIONARY:
	case TR_INTERPOLATE:
		VectorClear( result );
		break;
	case TR_LINEAR:
		VectorCopy( tr->trDelta, result );
		break;
	case TR_SINE:
		deltaTime = ( atTime - tr->trTime ) / (float) tr->trDuration;
		phase = cos( deltaTime * M_PI * 2 );	// derivative of sin = cos
		phase *= 0.5;
		VectorScale( tr->trDelta, phase, result );
		break;
//----(SA)	removed
	case TR_LINEAR_STOP:
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		VectorCopy( tr->trDelta, result );
		break;
	case TR_GRAVITY:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= DEFAULT_GRAVITY * deltaTime;		// FIXME: local gravity...
		break;
	// Ridah
	case TR_GRAVITY_LOW:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= (DEFAULT_GRAVITY * 0.3) * deltaTime;		// FIXME: local gravity...
		break;
	// done.
//----(SA)	
	case TR_GRAVITY_FLOAT:
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorCopy( tr->trDelta, result );
		result[2] -= (DEFAULT_GRAVITY * 0.2) * deltaTime;
		break;
//----(SA)	end
	// RF, acceleration
	case TR_ACCELERATE:	// trDelta is eventual speed
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		phase = deltaTime / (float)tr->trDuration;
		VectorScale( tr->trDelta, deltaTime * deltaTime, result );
		break;
	case TR_DECCELERATE:	// trDelta is breaking force
		if ( atTime > tr->trTime + tr->trDuration ) {
			VectorClear( result );
			return;
		}
		deltaTime = ( atTime - tr->trTime ) * 0.001;	// milliseconds to seconds
		VectorScale( tr->trDelta, deltaTime, result );
		break;
	case TR_SPLINE:
	case TR_LINEAR_PATH:
		VectorClear( result );
		break;
	default:
		printf("BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trTime);
		break;
	}
}

/*
==============================
from cgame/cg_servercmds.c
==============================
*/
void eth_CG_ParseReinforcementTimes(const char *pszReinfSeedString) {
	const char *tmp = pszReinfSeedString, *tmp2;
	unsigned int i, j, dwDummy, dwOffset[TEAM_NUM_TEAMS];

#define GETVAL(x,y) if((tmp = strchr(tmp, ' ')) == NULL) return; x = atoi(++tmp)/y;

	dwOffset[TEAM_ALLIES] = atoi(pszReinfSeedString) >> REINF_BLUEDELT;
	GETVAL(dwOffset[TEAM_AXIS], (1 << REINF_REDDELT));
	tmp2 = tmp;

	for(i=TEAM_AXIS; i<=TEAM_ALLIES; i++) {
		tmp = tmp2;
		for(j=0; j<MAX_REINFSEEDS; j++) {
			if(j == dwOffset[i]) {
				GETVAL(eth.cgs_aReinfOffset[i], aReinfSeeds[j]);
				eth.cgs_aReinfOffset[i] *= 1000;
				break;
			}
			GETVAL(dwDummy, 1);
		}
	}
}

int eth_CG_Text_Width_Ext(const char *text, float scale, int limit, fontInfo_t* font) {
	int count, len;
	glyphInfo_t *glyph;
	const char *s = text;
	float out, useScale = scale * font->glyphScale;
	
	out = 0;
	if( text ) {
		len = strlen( text );
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			if ( Q_IsColorString(s) ) {
				s += 2;
				continue;
			} else {
				glyph = &font->glyphs[(unsigned char)*s];
				out += glyph->xSkip;
				s++;
				count++;
			}
		}
	}

	return out * useScale;
}

void eth_CG_Text_PaintChar_Ext(float x, float y, float w, float h, float scalex, float scaley, float s, float t, float s2, float t2, qhandle_t hShader) {
	w *= scalex;
	h *= scaley;
	adjustFrom640(&x, &y, &w, &h);
	syscall_CG_R_DrawStretchPic(x, y, w, h, s, t, s2, t2, hShader);
}

void eth_CG_Text_Paint_Ext(float x, float y, float scalex, float scaley, vec4_t color, const char *text, float adjust, int limit, int style, fontInfo_t* font) {
	int len, count;
	vec4_t newColor;
	glyphInfo_t *glyph;

	scalex *= font->glyphScale;
	scaley *= font->glyphScale;

	if (text) {
		const char *s = text;
		syscall_CG_R_SetColor( color );
		memcpy(&newColor[0], &color[0], sizeof(vec4_t));
		len = strlen(text);
		if (limit > 0 && len > limit) {
			len = limit;
		}
		count = 0;
		while (s && *s && count < len) {
			glyph = &font->glyphs[(unsigned char)*s];
			if ( Q_IsColorString( s ) ) {
				if( *(s+1) == COLOR_NULL ) {
					memcpy( newColor, color, sizeof(newColor) );
				} else {
					memcpy( newColor, g_color_table[ColorIndex(*(s+1))], sizeof( newColor ) );
					newColor[3] = color[3];
				}
				syscall_CG_R_SetColor(newColor);
				s += 2;
				continue;
			} else {
				float yadj = scaley * glyph->top;
				yadj -= (font->glyphs['['].top + 3) * scaley;	// eth: Add y correction
				if (style == ITEM_TEXTSTYLE_SHADOWED || style == ITEM_TEXTSTYLE_SHADOWEDMORE) {
					int ofs = style == ITEM_TEXTSTYLE_SHADOWED ? 1 : 2;
					colorBlack[3] = newColor[3];
					syscall_CG_R_SetColor( colorBlack );
					eth_CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex) + ofs, y - yadj + ofs, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
					colorBlack[3] = 1.0;
					syscall_CG_R_SetColor( newColor );
				}
				eth_CG_Text_PaintChar_Ext(x + (glyph->pitch * scalex), y - yadj, glyph->imageWidth, glyph->imageHeight, scalex, scaley, glyph->s, glyph->t, glyph->s2, glyph->t2, glyph->glyph);
				x += (glyph->xSkip * scalex) + adjust;
				s++;
				count++;
			}
		}
		syscall_CG_R_SetColor( NULL );
	}
}

float *eth_CG_FadeColor( int startMsec, int totalMsec ) {
        static vec4_t           color;
        int                     t;

        if ( startMsec == 0 ) {
                return NULL;
        }

        t = eth.cg_time - startMsec;

        if ( t >= totalMsec ) {
                return NULL;
        }

        // fade out
        if ( totalMsec - t < FADE_TIME ) {
                color[3] = ( totalMsec - t ) * 1.0/FADE_TIME;
        } else {
                color[3] = 1.0;
        }
        color[0] = color[1] = color[2] = 1.f;

        return color;
}

/*
==============================
all cgame syscalls
==============================
*/

void syscall_CG_Cvar_Register(vmCvar_t *vmCvar, const char *varName, const char *defaultValue, int flags) {
	orig_syscall(CG_CVAR_REGISTER, vmCvar, varName, defaultValue, flags);
}

void syscall_CG_Cvar_Set(const char *var_name, const char *value) {
	orig_syscall(CG_CVAR_SET, var_name, value);
}

void syscall_CG_Cvar_Update(vmCvar_t *vmCvar) {
	orig_syscall(CG_CVAR_UPDATE, vmCvar);
}

void syscall_CG_Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize) {
	orig_syscall(CG_CVAR_VARIABLESTRINGBUFFER, var_name, buffer, bufsize);
}

qboolean syscall_CG_Key_IsDown(int keynum) {
	return orig_syscall(CG_KEY_ISDOWN, keynum);
}

void syscall_CG_R_AddRefEntityToScene(const refEntity_t *re) {
	orig_syscall( CG_R_ADDREFENTITYTOSCENE, re);
}

void syscall_CG_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader) {
	orig_syscall(CG_R_DRAWSTRETCHPIC, PASSFLOAT(x), PASSFLOAT(y), PASSFLOAT(w), PASSFLOAT(h), PASSFLOAT(s1), PASSFLOAT(t1), PASSFLOAT(s2), PASSFLOAT(t2), hShader);
}

int syscall_CG_R_LerpTag(orientation_t *tag, const refEntity_t *refent, const char *tagName, int startIndex) {
	return orig_syscall(CG_R_LERPTAG, tag, refent, tagName, startIndex);
}

void syscall_CG_R_SetColor(const float *rgba ) {
	orig_syscall( CG_R_SETCOLOR, rgba );
}


qhandle_t syscall_CG_R_RegisterShader(const char *name) {
	return orig_syscall(CG_R_REGISTERSHADER, name);
}

qhandle_t syscall_CG_R_RegisterShaderNoMip(const char *name) {
	return orig_syscall(CG_R_REGISTERSHADERNOMIP, name);
}

sfxHandle_t	syscall_CG_S_RegisterSound(const char *sample, qboolean compressed) {
	return orig_syscall(CG_S_REGISTERSOUND, sample, qfalse);
}

void syscall_CG_S_StartLocalSound( sfxHandle_t sfx, int channelNum) {
	orig_syscall(CG_S_STARTLOCALSOUND, sfx, channelNum, 230);
}

void syscall_CG_SendConsoleCommand(const char *text) {
	orig_syscall(CG_SENDCONSOLECOMMAND, text);
}

void syscall_CG_R_ModelBounds(clipHandle_t model, vec3_t mins, vec3_t maxs) {
	orig_syscall(CG_R_MODELBOUNDS, model, mins, maxs);
}

const char *syscall_UI_Argv(int arg) {
	static char buffer[MAX_STRING_CHARS];
	orig_syscall(UI_ARGV, arg, buffer, sizeof(buffer));
	return buffer;
}

int syscall_UI_Argc (void) {
	return orig_syscall(UI_ARGC);
}

int syscall_Argc (void) {
	return orig_syscall(CG_ARGC);
}

const char *syscall_CG_Argv (int arg) {
	static char buffer[MAX_STRING_CHARS];
	orig_syscall(CG_ARGV, arg, buffer, sizeof buffer);
	return buffer;
}

void syscall_R_AddLightToScene( const vec3_t org, float radius, float intensity, float r, float g, float b, qhandle_t hShader, int flags ) {
	orig_syscall( CG_R_ADDLIGHTTOSCENE, org, PASSFLOAT( radius ), PASSFLOAT( intensity ), PASSFLOAT( r ), PASSFLOAT( g ), PASSFLOAT( b ), hShader, flags );
}


void syscall_R_ProjectDecal( qhandle_t hShader,int numPoints, vec3_t *points, vec4_t projection, vec4_t color,int lifeTime, int fadeTime ) {
	orig_syscall( CG_R_PROJECTDECAL, hShader, numPoints, points, projection, color, lifeTime, fadeTime );
}

void orig_Cmd_RemoveCommand(const char *cmdName) {
	orig_syscall( CG_REMOVECOMMAND, cmdName );
}

// GPL License - see http://opensource.org/licenses/gpl-license.php
// Copyright 2005 *nixCoders team - don't forget to credit us

/*
==============================
All tools use for draw on screen.
==============================
*/

#include "eth.h"

void adjustFrom640(float *x, float *y, float *w, float *h) {
	*x *= eth.cgs_screenXScale;
	*y *= eth.cgs_screenYScale;
	*w *= eth.cgs_screenXScale;
	*h *= eth.cgs_screenYScale;
}

void adjustTo640(float *x, float *y, float *w, float *h) {
	*x /= eth.cgs_screenXScale;
	*y /= eth.cgs_screenYScale;
	*w /= eth.cgs_screenXScale;
	*h /= eth.cgs_screenYScale;
}

qboolean worldToScreen(vec3_t worldCoord, int *x, int *y) {
	vec3_t local, transformed;
	vec3_t vfwd;
	vec3_t vright;
	vec3_t vup;
	float xzi;
	float yzi;
	
	AngleVectors(eth.cg_refdefViewAngles, vfwd, vright, vup);
	VectorSubtract(worldCoord, eth.cg_refdef.vieworg, local);
	
	transformed[0] = DotProduct (local, vright);
	transformed[1] = DotProduct (local, vup);
	transformed[2] = DotProduct (local, vfwd);
	
	// Point is behind
	if (transformed[2] < 0.01)
		return qfalse;

	if (eth.mod->type != MOD_TCE) {
		switch (eth.cg_snap->ps.weapon) {
			case WP_FG42SCOPE:
			case WP_GARAND_SCOPE:
			case WP_K43_SCOPE:
				xzi = 320.0f / transformed[2];
				yzi = 240.0f / transformed[2];
	    		break;
			default:
				xzi = 320.0f / transformed[2] * (90.0 / eth.cg_refdef.fov_x);
				yzi = 240.0f / transformed[2] * (90.0 / eth.cg_refdef.fov_y);
				break;
		}
	} else {
		xzi = 320.0f / transformed[2] * (90.0 / eth.cg_refdef.fov_x);
		yzi = 240.0f / transformed[2] * (90.0 / eth.cg_refdef.fov_y);
	}
	
	*x = 320 + xzi * transformed[0];
	*y = 240 - yzi * transformed[1];

	return qtrue;
}

void drawFillRect(float x, float y, float width, float height, const float *color) {
	syscall_CG_R_SetColor(color);

    adjustFrom640(&x, &y, &width, &height);
    syscall_CG_R_DrawStretchPic(x, y, width, height, 0, 0, 0, 1, eth.cgs_media_whiteShader);

	syscall_CG_R_SetColor(NULL);
}

void drawRect(float x, float y, float width, float height, vec4_t color, int borderSize) {
	syscall_CG_R_SetColor(color);

	adjustFrom640(&x, &y, &width, &height);
	syscall_CG_R_DrawStretchPic(x, y, borderSize, height, 0, 0, 0, 0, eth.cgs_media_whiteShader);
	syscall_CG_R_DrawStretchPic(x + width - borderSize, y, borderSize, height, 0, 0, 0, 0, eth.cgs_media_whiteShader);
	syscall_CG_R_DrawStretchPic(x, y, width, borderSize, 0, 0, 0, 0, eth.cgs_media_whiteShader);
	syscall_CG_R_DrawStretchPic(x, y + height - borderSize, width, borderSize, 0, 0, 0, 0, eth.cgs_media_whiteShader);

	syscall_CG_R_SetColor(NULL);
}

void drawPic(float x, float y, float width, float height, qhandle_t hShader) {
	adjustFrom640(&x, &y, &width, &height);
	syscall_CG_R_DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
}

int sizeTextHeight(float scale, fontInfo_t *font) {
	return (font->glyphs['['].height + 6) * font->glyphScale * scale;
}

int sizeTextWidth(float scale, char *text, fontInfo_t *font) {
	return eth_CG_Text_Width_Ext(text, scale, 0, font) + 1;
}

void drawText(float x, float y, float scale, char *text, vec4_t color, qboolean forceColor, qboolean shadow, fontInfo_t *font) {
	char *finalText = text;

	if (forceColor) {
		finalText = strdup(text);
		Q_CleanStr(finalText);
	}

	eth_CG_Text_Paint_Ext(x, y, scale, scale, color, finalText, 0.0f, 0, shadow ? ITEM_TEXTSTYLE_SHADOWED : 0, font);

	if (forceColor)
		free(finalText);
}

void drawTextRect(float x, float y, float scale, char *text, vec4_t textColor, qboolean forceColor, qboolean textShadow, qboolean background, qboolean border, vec4_t backgroundColor, vec4_t borderColor, fontInfo_t *font) {
	int width = sizeTextWidth(scale, text, font);
	int height = sizeTextHeight(scale, font);
	float borderSize = font->glyphs['['].pitch * scale * 3;
	if (background)
		drawFillRect(x - borderSize, y - borderSize, width + (2 * borderSize), height + (2 * borderSize), backgroundColor);
	if (border)
		drawRect(x - borderSize, y - borderSize, width + (2 * borderSize), height + (2 * borderSize), borderColor, 1);

	drawText(x, y, scale, text, textColor, forceColor, textShadow, font);
}

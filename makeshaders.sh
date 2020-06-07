#!/bin/sh
# This script generate shader script and define files for eth

SHADER_SCRIPT=pk3/scripts/eth.shader
SHADER_DEFINE=shaders.h

rm -rf $SHADER_SCRIPT $SHADER_DEFINE

# Function to create script and define files
makeShader() {
	for SHADER in $1; do
		# Create shader script
		NAME=`echo $SHADER | cut -f1 -d:`
		SIZE=`echo $SHADER | cut -f2 -d:`
		MAP=`echo $SHADER | cut -f3 -d:`
		$2 >> $SHADER_SCRIPT

		# Create shader define
		COLOR=`echo $SHADER | cut -f4 -d:`
		echo "eth.shaders[$COLOR][$3] = syscall_CG_R_RegisterShader(\"$NAME\");" >> $SHADER_DEFINE
	done

	echo >> $SHADER_SCRIPT
	echo >> $SHADER_DEFINE
}


# Quake shader type A
quakeAShaderSkel() {
	echo "$NAME
	{
		q3map_surfacelight 1800
		deformVertexes wave 100 sin $SIZE 0 0 0
		{
			map $MAP.tga
			blendfunc GL_ONE GL_ONE
				tcGen environment
				tcmod rotate 15
				tcmod scroll 1 .1
		}
	}"
}
QUAKE_A_SHADERS="
	quakeABlueShader:2:gfx/effects/quadblue:GFX_BLUE
	quakeACyanShader:2:gfx/effects/quadcyan:GFX_CYAN
	quakeAGreenShader:2:gfx/effects/quadgreen:GFX_GREEN
	quakeARedShader:2:gfx/effects/quadred:GFX_RED
	quakeATealShader:2:gfx/effects/quadteal:GFX_TEAL
	quakeAYellowShader:2:gfx/effects/quadyellow:GFX_YELLOW
"
makeShader "$QUAKE_A_SHADERS" quakeAShaderSkel CHAM_QUAKE


# Quake shader type B
quakeBShaderSkel() {
	echo "$NAME
	{
		q3map_surfacelight 1800
		deformVertexes wave 100 sin $SIZE 0 0 0
		nofog
		{
			map $MAP.tga
			tcGen environment
		}
	}"
}
QUAKE_B_SHADERS="
	quakeBBlueShader:2:gfx/effects/quadblue:GFX_BLUE
	quakeBCyanShader:2:gfx/effects/quadcyan:GFX_CYAN
	quakeBGreenShader:2:gfx/effects/quadgreen:GFX_GREEN
	quakeBRedShader:2:gfx/effects/quadred:GFX_RED
	quakeBTealShader:2:gfx/effects/quadteal:GFX_TEAL
	quakeBYellowShader:2:gfx/effects/quadyellow:GFX_YELLOW
"
makeShader "$QUAKE_B_SHADERS" quakeBShaderSkel CHAM_QUAKEB

# Quake shader type C
quakeCShaderSkel() {
	echo "$NAME
	{
		q3map_surfacelight 1800
		{
			map textures/effects/envmap_slate.tga
			rgbGen lightingDiffuse
			tcGen environment
		}
		cull none
		{
			map $MAP.tga
			tcmod rotate 2
			tcmod scroll 1 .01
			tcGen environment
			rgbGen lightingDiffuse
		}
	}"
}
QUAKE_C_SHADERS="
	quakeCBlueShader:0:gfx/effects/quadblue:GFX_BLUE
	quakeCCyanShader:0:gfx/effects/quadcyan:GFX_CYAN
	quakeCGreenShader:0:gfx/effects/quadgreen:GFX_GREEN
	quakeCRedShader:0:gfx/effects/quadred:GFX_RED
	quakeCTealShader:0:gfx/effects/quadteal:GFX_TEAL
	quakeCYellowShader:0:gfx/effects/quadyellow:GFX_YELLOW
"
makeShader "$QUAKE_C_SHADERS" quakeCShaderSkel CHAM_QUAKEC

# Glow shader
glowShaderSkel() {
	echo "$NAME
	{
		deformVertexes wave 100 sin $SIZE 0 0 0
		{
			map $MAP.tga
			blendfunc GL_ONE GL_ONE
		}
	}"
}
GLOW_SHADERS="
	glowBlueShader:4:menu/art/fx_blue:GFX_BLUE
	glowCyanShader:4:menu/art/fx_teal:GFX_CYAN
	glowGreenShader:4:menu/art/fx_grn:GFX_GREEN
	glowRedShader:4:menu/art/fx_red:GFX_RED
	glowTealShader:4:menu/art/fx_cyan:GFX_TEAL
	glowYellowShader:4:menu/art/fx_yel:GFX_YELLOW
"
makeShader "$GLOW_SHADERS" glowShaderSkel CHAM_GLOW

# Flat shader
flatShaderSkel() {
	echo "$NAME
	{
		cull back
		surfaceparm nodlight
		surfaceparm nolightmap
		sort 3
		{
			map $MAP.tga
		}
	}"
}
FLAT_SHADERS="
	flatBlueShader:0:gfx/colors/blue:GFX_BLUE
	flatCyanShader:0:gfx/colors/cyan:GFX_CYAN
	flatGreenShader:0:gfx/colors/green:GFX_GREEN
	flatRedShader:0:gfx/colors/red:GFX_RED
	flatTealShader:0:gfx/colors/teal:GFX_TEAL
	flatYellowShader:0:gfx/colors/yellow:GFX_YELLOW
"
makeShader "$FLAT_SHADERS" flatShaderSkel CHAM_FLAT

# Flat white shader
echo "flatWhiteShader
{
	cull back
	surfaceparm nodlight
	surfaceparm nolightmap
	sort 3
	{
		map menu/art/fx_white.tga
	}
}" >> $SHADER_SCRIPT
echo "eth.whiteFlatShader = syscall_CG_R_RegisterShader(\"flatWhiteShader\");" >> $SHADER_DEFINE

# Mortar shader
echo "friendMortarShader
{
	q3map_surfacelight 1800
	deformVertexes wave 100 sin 40 0 0 0
	{
		map gfx/effects/quadblue.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 15
		tcmod scroll 1 .1
	}
}" >> $SHADER_SCRIPT
echo "eth.friendMortarShader = syscall_CG_R_RegisterShader(\"friendMortarShader\");" >> $SHADER_DEFINE

echo "enemyMortarShader
{
	q3map_surfacelight 1800
	deformVertexes wave 100 sin 40 0 0 0
	{
		map gfx/effects/quadred.tga
		blendfunc GL_ONE GL_ONE
		tcGen environment
		tcmod rotate 15
		tcmod scroll 1 .1
	}
}" >> $SHADER_SCRIPT
echo "eth.enemyMortarShader = syscall_CG_R_RegisterShader(\"enemyMortarShader\");" >> $SHADER_DEFINE

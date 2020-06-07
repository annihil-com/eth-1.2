#!/bin/sh
#---------------------------
#export ETH_CONF_FILE=
#export ETH_IRC_NICK=
#export ETH_LOG_FILE=
#export ETH_KILLSPAM="^o[n] was eaten by a penguin ^f(^2[c]^o/^2[t]^f)"
#export ETH_BANNER="^2|^7Name: ^x[p]^* ^2| ^3[M] ^2| ^7Kills: ^1[t] ^2| ^7Spree: ^1[c] ^2| ^7Xp: ^1[x]"
#export ETH_DEATHSPAM="^oOHHENOE! My ^1[c] ^ospree was cut short by ^1[K]"
#export ETH_PBSS=

# explanaition of shortcuts
#	[c] - current spree count
#	[v] - victim name
#	[n] - clean victim name
#	[t] - total kills
#	[m] - double/triple/multikill msg
#	[x]	- Player XP
#	[p] - Player name
#	[M] - Mod name
#	[h] - Player health
#	[k] - Killer name
#	[K] - Clean killer name
#	[q] - amount of punkbuster screenshots requested
#---------------------------

export LD_PRELOAD=./libETH.so
./et.x86 $*
unset LD_PRELOAD
exit 0

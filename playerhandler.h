#pragma once

#include <prism/blitz.h>

ActorBlueprint getPlayerHandler();

Position getPlayerPosition();
int isPlayerActive();
void setPlayerInactive();
void setPlayerActive();
void setPlayerHit();

void setGunActive();
void setPlayerInvisible();
#pragma once

#include <prism/actorhandler.h>

ActorBlueprint getHealthBarHandler();

void setHealthbarsActive();
void setHealthbarsInactive();

void decreasePlayerHealth();
void resetPlayerHealth();
int getPlayerHealth();

void decreaseHouseHealth();
void increaseHouseHealth(int tMoney);
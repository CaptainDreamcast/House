#pragma once

#include <prism/actorhandler.h>


ActorBlueprint getEnemyHandler();

void spawnFirstEnemies();
void spawnEnemyInHouse();
void setEnemiesInvisible();
void setEnemiesVisible();
void setEnemiesInactive();

void removeEnemy(void* tData);
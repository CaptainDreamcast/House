#pragma once

#include <vector>

#include <prism/actorhandler.h>

#include <prism/geometry.h>

ActorBlueprint getBackgroundHandler();

int isInside();
void setInside();
void setOutside();

int getTileAmount();
int isTileFree(int x, int y);

void setHouseCollapsed();

Vector3DI getFreeStartTile();
std::vector<Vector3DI> getPath(Vector3DI tStart, Vector3DI tTarget);
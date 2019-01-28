#pragma once

#include <prism/actorhandler.h>

ActorBlueprint getScratchingHandler();

void setScratchingActive(int mScratchTime, int mScratchTimeVariation);
void pauseScratching();
void unpauseScratching();
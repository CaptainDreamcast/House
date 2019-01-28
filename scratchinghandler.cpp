#include "scratchinghandler.h"

#include <prism/blitz.h>

#include "gamescreen.h"

static struct {

	int mIsActive;
	int mNow;
	int mScratchTimeNext;
	int mScratchTime;
	int mScratchVariation;
	int mIsPaused;

} gScratchingData;

static void loadScratchingHandler(void* tData) {
	(void)tData;

	gScratchingData.mIsActive = 0;
	gScratchingData.mIsPaused = 0;
}

static void updateScratchingHandler(void* tData) {
	(void)tData;
	if (!gScratchingData.mIsActive) return;
	if (gScratchingData.mIsPaused) return;

	gScratchingData.mNow++;
	if (gScratchingData.mNow >= gScratchingData.mScratchTimeNext) {
		
		playMugenSound(getGameSounds(), 2, 0);

		gScratchingData.mNow = 0;
		gScratchingData.mScratchTimeNext = gScratchingData.mScratchTime + randfromInteger(-gScratchingData.mScratchVariation, gScratchingData.mScratchVariation);
	}

}

ActorBlueprint getScratchingHandler()
{
	return makeActorBlueprint(loadScratchingHandler, NULL, updateScratchingHandler);
}

void setScratchingActive(int mScratchTime, int mScratchTimeVariation)
{
	gScratchingData.mScratchTime = mScratchTime;
	gScratchingData.mScratchVariation = mScratchTimeVariation;
	gScratchingData.mNow = gScratchingData.mScratchTimeNext = gScratchingData.mScratchTime;

	gScratchingData.mIsPaused = 0;
	gScratchingData.mIsActive = 1;
}

void pauseScratching()
{
	gScratchingData.mIsPaused = 1;
}

void unpauseScratching()
{
	gScratchingData.mIsPaused = 0;

}



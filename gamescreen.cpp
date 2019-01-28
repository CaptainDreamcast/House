#include "gamescreen.h"

#include <prism/blitz.h>

#include "backgroundhandler.h"
#include "piggybankhandler.h"
#include "sleephandler.h"
#include "playerhandler.h"
#include "collision.h"
#include "musichandler.h"
#include "titlehandler.h"
#include "helptexthandler.h"
#include "coinhandler.h"
#include "scratchinghandler.h"
#include "enemyhandler.h"
#include "healthbarhandler.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	MugenSounds mSounds;

	int mStoryBeat;
} gGameData;

static void loadGameScreen() {

	gGameData.mStoryBeat = 0;

	gGameData.mSprites = loadMugenSpriteFileWithoutPalette("data/GAME.sff");
	gGameData.mAnimations = loadMugenAnimationFile("data/GAME.air");
	gGameData.mSounds = loadMugenSoundFile("data/GAME.snd");

	loadGameCollisions();
	//instantiateActor(getMusicHandler());
	instantiateActor(getBackgroundHandler());
	instantiateActor(getPiggybankHandler());
	instantiateActor(getSleepHandler());
	instantiateActor(getPlayerHandler());
	instantiateActor(getCoinHandler());
	instantiateActor(getTitleHandler());
	instantiateActor(getHelpTextHandler());
	instantiateActor(getScratchingHandler());
	instantiateActor(getEnemyHandler());
	instantiateActor(getHealthBarHandler());
	//activateCollisionHandlerDebugMode();
	//setCollisionHandlerDebuggingScreenPositionReference(getBlitzCameraHandlerPositionReference());
	setInside();

}

static Screen gGameScreen;

Screen* getGameScreen()
{
	gGameScreen = makeScreen(loadGameScreen);
	return &gGameScreen;
}

MugenSpriteFile * getGameSprites()
{
	return &gGameData.mSprites;
}

MugenAnimations * getGameAnimations()
{
	return &gGameData.mAnimations;
}

MugenSounds * getGameSounds()
{
	return &gGameData.mSounds;
}

int getStoryBeat()
{
	return gGameData.mStoryBeat;
}

void resetStoryBeat()
{
	gGameData.mStoryBeat = 0;
}

void increaseStoryBeat()
{
	gGameData.mStoryBeat++;
}

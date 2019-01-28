#include "titlehandler.h"

#include <prism/blitz.h>

#include "musichandler.h"
#include "playerhandler.h"
#include "helptexthandler.h"
#include "gamescreen.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;
	TextureData mWhiteTexture;

	int mIsActive;
	int mTitleAnimationID;
	int mPressStartAnimationID;
	int mWhiteTextureID;
	int mIsGameOver;
} gTitleHandler;


static void setTitleActive() {
	if (gTitleHandler.mIsActive) return;

	gTitleHandler.mTitleAnimationID = addMugenAnimation(getMugenAnimation(&gTitleHandler.mAnimations, 1000), &gTitleHandler.mSprites, makePosition(160, 50, 50));
	gTitleHandler.mPressStartAnimationID = addMugenAnimation(getMugenAnimation(&gTitleHandler.mAnimations, 2000), &gTitleHandler.mSprites, makePosition(160, 190, 50));
	gTitleHandler.mWhiteTextureID = playOneFrameAnimationLoop(makePosition(0, 0, 45), &gTitleHandler.mWhiteTexture);
	setAnimationSize(gTitleHandler.mWhiteTextureID, makePosition(320, 240, 1), makePosition(0, 0, 0));
	setAnimationColor(gTitleHandler.mWhiteTextureID, 0, 0, 0);
	setAnimationTransparency(gTitleHandler.mWhiteTextureID, 1);

	setPlayerInactive();

	gTitleHandler.mIsGameOver = 0;
	gTitleHandler.mIsActive = 1;
}

static void setGameOverActive() {
	if (gTitleHandler.mIsActive) return;

	gTitleHandler.mTitleAnimationID = addMugenAnimation(getMugenAnimation(&gTitleHandler.mAnimations, 4000), &gTitleHandler.mSprites, makePosition(160, 80, 50));
	gTitleHandler.mWhiteTextureID = playOneFrameAnimationLoop(makePosition(0, 0, 45), &gTitleHandler.mWhiteTexture);
	setAnimationSize(gTitleHandler.mWhiteTextureID, makePosition(320, 240, 1), makePosition(0, 0, 0));
	setAnimationColor(gTitleHandler.mWhiteTextureID, 1, 0, 0);

	setPlayerInactive();

	gTitleHandler.mIsGameOver = 1;
	gTitleHandler.mIsActive = 1;
}

static void setTitleInactive() {
	if (!gTitleHandler.mIsActive) return;
	
	removeMugenAnimation(gTitleHandler.mTitleAnimationID);
	removeMugenAnimation(gTitleHandler.mPressStartAnimationID);
	removeHandledAnimation(gTitleHandler.mWhiteTextureID);

	setPlayerActive();

	gTitleHandler.mIsActive = 0;
}

static void loadTitle(void* tData) {
	(void)tData;
	gTitleHandler.mSprites = loadMugenSpriteFileWithoutPalette("title/TITLE.sff");
	gTitleHandler.mAnimations = loadMugenAnimationFile("title/TITLE.air");
	gTitleHandler.mWhiteTexture = createWhiteTexture();

	gTitleHandler.mIsActive = 0;
	setTitleActive();
}

static void updateTitle(void* tData) {
	(void)tData;
	if (!gTitleHandler.mIsActive) return;

	if (hasPressedStartFlank()) {
		if (gTitleHandler.mIsGameOver) {
			setNewScreen(getGameScreen());
		}
		else {
			setTitleInactive();
			showFirstHelpText();
			playPeacefulMusic();
		}


	}
}


ActorBlueprint getTitleHandler()
{
	return makeActorBlueprint(loadTitle, NULL, updateTitle);
}

static void fadeOutFinished(void* tData) {
	(void)tData;

	setFadeColorRGB(0, 0, 0);
	enableDrawing();
	setGameOverActive();
}

void startGameOver()
{
	stopGameMusic();
	setFadeColorRGB(1, 0, 0);
	addFadeOut(180, fadeOutFinished, NULL);
}

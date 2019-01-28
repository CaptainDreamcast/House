#include "Sleephandler.h"

#include "playerhandler.h"
#include "gamescreen.h"
#include "coinhandler.h"
#include "backgroundhandler.h"
#include "helptexthandler.h"
#include "musichandler.h"
#include "scratchinghandler.h"
#include "healthbarhandler.h"
#include "enemyhandler.h"

static struct {

	int mBackgroundAnimationID;
	int mTextID;

	int mCanSleep;
	int mIsActive;

	int mDay;

} gSleepData;

static void loadSleepHandler(void* tData) {
	(void)tData;
	gSleepData.mIsActive = 0;
	gSleepData.mCanSleep = 0;
	gSleepData.mDay = 0;
}

static int isInRange() {
	Position p = getPlayerPosition();
	Position bed = makePosition(580, 645, 0);
	double l = getDistance2D(p, bed);
	return l < 32;
}

static int isActiveCondition() {
	return isInRange() && gSleepData.mCanSleep && isInside();
}

static void updateSleepSetInactive() {
	if (isActiveCondition() || !gSleepData.mIsActive) return;

	removeMugenAnimation(gSleepData.mBackgroundAnimationID);
	removeMugenText(gSleepData.mTextID);

	gSleepData.mIsActive = 0;
}

static void updateSleepSetActive() {
	if (!isActiveCondition() || gSleepData.mIsActive) return;

	gSleepData.mBackgroundAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 3000), getGameSprites(), makePosition(180, 110, 30));
	gSleepData.mTextID = addMugenTextMugenStyle("Sleep (A)", makePosition(190, 103, 31), makeVector3DI(1, 0, 1));

	gSleepData.mIsActive = 1;
}

static void updateSleepActive() {
	if (gSleepData.mIsActive) {
		updateSleepSetInactive();
	}
	else {
		updateSleepSetActive();
	}
}

static void fadeInOver(void* tData) {
	(void)tData;

	if (getDay() == 8) {
		showFinalHelpText();
	}
	else {
		setPlayerActive();
	}
}

static void fadeOutOver(void* tData) {
	(void)tData;
	enableDrawing();
	gSleepData.mDay++;
	resetStoryBeat();
	resetPlayerHealth();
	shuffleCoins();

	if (getDay() == 1) {
		setScratchingActive(120, 60);
	}
	else if (getDay() == 2) {
		setScratchingActive(20, 3);
	}
	else if (getDay() == 3) {
		playActionMusic();
	}
	else if (getDay() == 4) {
		playSadMusic();
	}
	else if (getDay() == 5) {
		stopGameMusic();
	}
	else if (getDay() == 6) {
		playDiscordMusic();
	}
	else if (getDay() == 7) {
		playDiscordMusic();
	}
	else if (getDay() == 8) {
		setOutside();
		playPeacefulMusic();
		setEnemiesInactive();
		setEnemiesInvisible();
		setPlayerInactive();
		setPlayerInvisible();
		setHealthbarsInactive();
	}

	addFadeIn(30, fadeInOver, NULL);
}

static void updateAction() {
	if (!gSleepData.mIsActive) return;

	if (hasPressedAFlank()) {
		gSleepData.mCanSleep = 0;
		setPlayerInactive();
		stopGameMusic();
		playMugenSound(getGameSounds(), 1, 0);
		addFadeOut(120, fadeOutOver, NULL);
	}

}

static void updateSleepHandler(void* tData) {
	(void)tData;
	updateSleepActive();
	updateAction();

}

ActorBlueprint getSleepHandler()
{
	return makeActorBlueprint(loadSleepHandler, NULL, updateSleepHandler);
}

void setCanSleep()
{
	gSleepData.mCanSleep = 1;
}

int getDay()
{
	return gSleepData.mDay;
}

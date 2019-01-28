#include "piggybankhandler.h"

#include "playerhandler.h"
#include "gamescreen.h"
#include "coinhandler.h"
#include "backgroundhandler.h"
#include "helptexthandler.h"
#include "sleephandler.h"
#include "healthbarhandler.h"

static struct {
	
	int mBackgroundAnimationID;
	int mTextID;

	int mIsActive;

} gPiggybankData;

static void loadPiggybankHandler(void* tData) {
	(void)tData;
	gPiggybankData.mIsActive = 0;
}

static int isInRange() {
	Position p = getPlayerPosition();
	Position piggy = makePosition(685 - 320, 645 - 320, 0);
	double l = getDistance2D(p, piggy);
	return l < 32;
}

static int isActiveCondition() {
	return isInRange() && getMoneyAmount() && isInside();
}

static void updatePiggybankSetInactive() {
	if (isActiveCondition() || !gPiggybankData.mIsActive) return;

	removeMugenAnimation(gPiggybankData.mBackgroundAnimationID);
	removeMugenText(gPiggybankData.mTextID);

	gPiggybankData.mIsActive = 0;
}

static void updatePiggybankSetActive() {
	if (!isActiveCondition() || gPiggybankData.mIsActive) return;

	gPiggybankData.mBackgroundAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 3000), getGameSprites(), makePosition(180, 110, 30));
	gPiggybankData.mTextID = addMugenTextMugenStyle("Deposit (A)", makePosition(190, 103, 31), makeVector3DI(1, 0, 1));

	gPiggybankData.mIsActive = 1;
}

static void updatePiggybankActive() {
	if (gPiggybankData.mIsActive) {
		updatePiggybankSetInactive();
	}
	else {
		updatePiggybankSetActive();
	}
}

static void updateDeposit() {
	if (!gPiggybankData.mIsActive) return;

	if (hasPressedAFlank()) {
		increaseHouseHealth(getMoneyAmount());
		resetMoney();
		setCanSleep();
		showSleepHelpText();
	}

}

static void updatePiggybankHandler(void* tData) {
	(void)tData;
	updatePiggybankActive();
	updateDeposit();

}

ActorBlueprint getPiggybankHandler()
{
	return makeActorBlueprint(loadPiggybankHandler, NULL, updatePiggybankHandler);
}

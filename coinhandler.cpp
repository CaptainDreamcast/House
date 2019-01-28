#include "coinhandler.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "collision.h"
#include "backgroundhandler.h"
#include "helptexthandler.h"

#define COIN_AMOUNT 15

typedef struct {
	int mEntityID;
	int mWasCollected;
} Coin;

static struct {
	Coin mCoins[20];
	int mMoney;

	int mBackgroundID;
	int mMoneyTextID;

} gCoinHandler;

static void updateMoneyText() {
	char newText[200];

	sprintf(newText, "$%d", gCoinHandler.mMoney);
	changeMugenText(gCoinHandler.mMoneyTextID, newText);
}

static void coinCollision(void* tCaller, void* tCollisionData) {
	(void)tCollisionData;
	Coin* coin = (Coin*)tCaller;
	if (coin->mWasCollected) return;

	int animationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 2001), getGameSprites(), vecAdd(getBlitzEntityPosition(coin->mEntityID), makePosition(0, 0, 50)));
	setMugenAnimationCameraPositionReference(animationID, getBlitzCameraHandlerPositionReference());
	setMugenAnimationNoLoop(animationID);

	setBlitzMugenAnimationVisibility(coin->mEntityID, 0);
	showDepositHelpText();

	gCoinHandler.mMoney += 100;
	updateMoneyText();
	

	coin->mWasCollected = 1;
}

static void loadCoinHandler(void* tData) {
	(void)tData;

	for (int i = 0; i < COIN_AMOUNT; i++) {
		Coin& coin = gCoinHandler.mCoins[i];
		coin.mWasCollected = 0;
		coin.mEntityID = addBlitzEntity(makePosition(0, 0, 5));
		addBlitzMugenAnimationComponent(coin.mEntityID, getGameSprites(), getGameAnimations(), 2000);
		addBlitzCollisionComponent(coin.mEntityID);
		int collisionID = addBlitzCollisionRect(coin.mEntityID, getCoinCollisionList(), makeCollisionRect(makePosition(-16, -16, 0), makePosition(16, 16, 1)));
		addBlitzCollisionCB(coin.mEntityID, collisionID, coinCollision, &gCoinHandler.mCoins[i]);
	}

	gCoinHandler.mMoney = 0;
	gCoinHandler.mBackgroundID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 2002), getGameSprites(), makePosition(300, 220, 34));
	gCoinHandler.mMoneyTextID = addMugenTextMugenStyle("$0", makePosition(290, 213, 35), makeVector3DI(1, 0, -1));

	shuffleCoins();
}

ActorBlueprint getCoinHandler()
{
	return makeActorBlueprint(loadCoinHandler);
}

void shuffleCoins()
{
	for (int i = 0; i < COIN_AMOUNT; i++) {
		Position pos = makePosition(0, 0, 0);
		for (int dummy = 0; dummy < 100; dummy++) {
			Vector3DI tPos = makeVector3DI(randfromInteger(0, getTileAmount() - 1), randfromInteger(0, getTileAmount() - 1), 0);
			if (!isTileFree(tPos.x, tPos.y)) continue;
			pos = makePosition(32 * tPos.x + 16, 32 * tPos.y + 16, 5);
			int tooClose = 0;
			for (int j = 0; j < i; j++) {
				Position o = getBlitzEntityPosition(gCoinHandler.mCoins[j].mEntityID);
				if (getDistance2D(pos, o) < 200) { 
					tooClose = 1;
					break;
				}
			}
			if (!tooClose) break;
		}

		setBlitzEntityPosition(gCoinHandler.mCoins[i].mEntityID, pos);
		setBlitzMugenAnimationVisibility(gCoinHandler.mCoins[i].mEntityID, 0);
		gCoinHandler.mCoins[i].mWasCollected = 0;
	}
}

void setCoinsInvisible()
{
	for (int i = 0; i < COIN_AMOUNT; i++) {
		if (gCoinHandler.mCoins[i].mWasCollected) continue;
		setBlitzMugenAnimationVisibility(gCoinHandler.mCoins[i].mEntityID, 0);

	}
}

void setCoinsVisible()
{
	for (int i = 0; i < COIN_AMOUNT; i++) {
		if (gCoinHandler.mCoins[i].mWasCollected) continue;
		setBlitzMugenAnimationVisibility(gCoinHandler.mCoins[i].mEntityID, 1);

	}
}

int getMoneyAmount()
{
	return gCoinHandler.mMoney;
}

void resetMoney()
{
	gCoinHandler.mMoney = 0;
	updateMoneyText();
}

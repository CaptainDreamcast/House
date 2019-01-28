#include "healthbarhandler.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "playerhandler.h"
#include "titlehandler.h"
#include "backgroundhandler.h"

static struct {
	int mHouseMaxHealth;
	int mHouseCurrentHealth;

	int mPlayerMaxHealth;
	int mPlayerCurrentHealth;

	int mHouseBGAnimationID;
	int mHouseFGAnimationID;
	int mHouseTextID;

	int mPlayerBGAnimationID;
	int mPlayerFGAnimationID;

	int mIsActive;

} gHealthBarHandler;

static void resetHealth() {
	gHealthBarHandler.mHouseMaxHealth = 30000;
	gHealthBarHandler.mHouseCurrentHealth = gHealthBarHandler.mHouseMaxHealth;

	gHealthBarHandler.mPlayerMaxHealth = 4;
	gHealthBarHandler.mPlayerCurrentHealth = gHealthBarHandler.mPlayerMaxHealth;
}

static void loadHealthBarHandler(void* tData) {
	(void)tData;
	resetHealth();

	gHealthBarHandler.mIsActive = 0;
}

ActorBlueprint getHealthBarHandler()
{
	return makeActorBlueprint(loadHealthBarHandler);
}

void setHealthbarsActive()
{
	gHealthBarHandler.mHouseBGAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 6000), getGameSprites(), makePosition(21, 2, 20));
	gHealthBarHandler.mHouseFGAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 6001), getGameSprites(), makePosition(21, 2, 21));
	gHealthBarHandler.mHouseTextID = addMugenTextMugenStyle("House", makePosition(160, 13, 22), makeVector3DI(1, 0, 0));

	gHealthBarHandler.mPlayerBGAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 6002), getGameSprites(), makePosition(160 - 25, 80, 20));
	gHealthBarHandler.mPlayerFGAnimationID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 6003), getGameSprites(), makePosition(160 - 25, 80, 21));

	gHealthBarHandler.mIsActive = 1;
}

void setHealthbarsInactive()
{
	removeMugenAnimation(gHealthBarHandler.mHouseBGAnimationID);
	removeMugenAnimation(gHealthBarHandler.mHouseFGAnimationID);
	removeMugenText(gHealthBarHandler.mHouseTextID);

	removeMugenAnimation(gHealthBarHandler.mPlayerBGAnimationID);
	removeMugenAnimation(gHealthBarHandler.mPlayerFGAnimationID);

	gHealthBarHandler.mIsActive = 0;
}

void decreasePlayerHealth()
{
	gHealthBarHandler.mPlayerCurrentHealth--;
	double percentage = gHealthBarHandler.mPlayerCurrentHealth / (double)gHealthBarHandler.mPlayerMaxHealth;
	setMugenAnimationRectangleWidth(gHealthBarHandler.mPlayerFGAnimationID, percentage * 50);

}

void resetPlayerHealth()
{
	if (!gHealthBarHandler.mIsActive) return;

	gHealthBarHandler.mPlayerCurrentHealth = gHealthBarHandler.mPlayerMaxHealth;
	double percentage = gHealthBarHandler.mPlayerCurrentHealth / (double)gHealthBarHandler.mPlayerMaxHealth;
	setMugenAnimationRectangleWidth(gHealthBarHandler.mPlayerFGAnimationID, percentage * 50);
}

int getPlayerHealth()
{
	return gHealthBarHandler.mPlayerCurrentHealth;
}

void decreaseHouseHealth()
{
	gHealthBarHandler.mHouseCurrentHealth--;
	double percentage = gHealthBarHandler.mHouseCurrentHealth / (double)gHealthBarHandler.mHouseMaxHealth;
	setMugenAnimationRectangleWidth(gHealthBarHandler.mHouseFGAnimationID, percentage * 277);

	if (gHealthBarHandler.mHouseCurrentHealth == 0) {
		setHouseCollapsed();
		if (!isPlayerActive()) return;
		setPlayerInactive();
		startGameOver();
	}
}

void increaseHouseHealth(int tMoney)
{
	if (!gHealthBarHandler.mIsActive) return;
	gHealthBarHandler.mHouseCurrentHealth += tMoney * 50;
	gHealthBarHandler.mHouseCurrentHealth = fmin(gHealthBarHandler.mHouseCurrentHealth, gHealthBarHandler.mHouseMaxHealth);

	double percentage = gHealthBarHandler.mHouseCurrentHealth / (double)gHealthBarHandler.mHouseMaxHealth;
	setMugenAnimationRectangleWidth(gHealthBarHandler.mHouseFGAnimationID, percentage * 277);
}

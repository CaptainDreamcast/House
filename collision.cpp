#include "collision.h"

#include <prism/blitz.h>

static struct {
	int mBGList;
	int mPlayerList;
	int mPlayerCollectList;
	int mCoinList;
	int mEnemyList;
	int mShotList;
} gGameCollision;

void loadGameCollisions()
{
	gGameCollision.mBGList = addCollisionListToHandler();
	gGameCollision.mPlayerList = addCollisionListToHandler();
	gGameCollision.mPlayerCollectList = addCollisionListToHandler();
	gGameCollision.mCoinList = addCollisionListToHandler();
	gGameCollision.mEnemyList = addCollisionListToHandler();
	gGameCollision.mShotList = addCollisionListToHandler();
	addCollisionHandlerCheck(gGameCollision.mBGList, gGameCollision.mPlayerList);
	addCollisionHandlerCheck(gGameCollision.mCoinList, gGameCollision.mPlayerCollectList);
	addCollisionHandlerCheck(gGameCollision.mEnemyList, gGameCollision.mPlayerList);
	addCollisionHandlerCheck(gGameCollision.mEnemyList, gGameCollision.mShotList);

}

int getBGCollisionList()
{
	return gGameCollision.mBGList;
}

int getPlayerCollisionList()
{
	return gGameCollision.mPlayerList;
}

int getPlayerCollectCollisionList()
{
	return gGameCollision.mPlayerCollectList;
}

int getCoinCollisionList()
{
	return gGameCollision.mCoinList;
}

int getEnemyCollisionList()
{
	return gGameCollision.mEnemyList;
}

int getShotCollisionList()
{
	return gGameCollision.mShotList;
}

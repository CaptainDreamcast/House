#include "enemyhandler.h"

#include <queue>

#include <prism/blitz.h>

#include "gamescreen.h"
#include "playerhandler.h"
#include "collision.h"
#include "healthbarhandler.h"
#include "backgroundhandler.h"


typedef struct {

	int mIsActive;
	int mEntityID;

	queue<Vector3DI> mPath;
	Position mCurrentTarget;
	Position mFinalTarget;

	int mIsChasingPlayer;
	int mHasReachedTarget;

} Enemy;

static struct {
	Enemy mStationaryEnemies[17];

	Enemy mHouseEnemy;
} gEnemyHandler;

static void loadEnemyHandler(void* tData) {
	(void)tData;
	for (int i = 0; i < 17; i++) {
		gEnemyHandler.mStationaryEnemies[i].mIsActive = 0;
	}

	gEnemyHandler.mHouseEnemy.mIsActive = 0;
}

static void updateHasReachedTarget(Enemy& tEnemy) {
	Position p = getBlitzEntityPosition(tEnemy.mEntityID);

	tEnemy.mHasReachedTarget = (getDistance2D(p, tEnemy.mFinalTarget) < 1);
}

static void updateHouseDamage(Enemy& tEnemy) {
	if (tEnemy.mIsChasingPlayer) return;
	if (!tEnemy.mHasReachedTarget) return;

	decreaseHouseHealth();
}

static void setNextTarget(Enemy& e) {
	if (!e.mPath.empty()) {
		Vector3DI first = e.mPath.front();
		e.mPath.pop();
		e.mCurrentTarget = makePosition(first.x * 32 + 16, first.y * 32 + 16, 15);
	}
	else {
		e.mCurrentTarget = e.mFinalTarget;
	}
}


static void updateFacing(Enemy& enemy, Vector3DI tFacing) {
	if (tFacing.x == 1 && tFacing.y == 0) changeBlitzMugenAnimationIfDifferent(enemy.mEntityID, 5002);
	if (tFacing.x == -1 && tFacing.y == 0) changeBlitzMugenAnimationIfDifferent(enemy.mEntityID, 5001);
	if (tFacing.x == 0 && tFacing.y == 1) changeBlitzMugenAnimationIfDifferent(enemy.mEntityID, 5000);
	if (tFacing.x == 0 && tFacing.y == -1) changeBlitzMugenAnimationIfDifferent(enemy.mEntityID, 5003);

}

static void updateMovingToTarget(Enemy& tEnemy) {
	if (tEnemy.mHasReachedTarget) return;

	Position p = getBlitzEntityPosition(tEnemy.mEntityID);

	int hasReachedTarget = (getDistance2D(p, tEnemy.mCurrentTarget) < 1);

	if (hasReachedTarget) {
		setNextTarget(tEnemy);
	}
	else {
		double speed = min(4.0, getDistance2D(p, tEnemy.mCurrentTarget));
		Vector3D dir = vecNormalize(vecSub(tEnemy.mCurrentTarget, p));
		dir = vecScale(dir, speed);
		addBlitzEntityPositionX(tEnemy.mEntityID, dir.x);
		addBlitzEntityPositionY(tEnemy.mEntityID, dir.y);

		Vector3DI tFacing;
		if (dir.y < 0 && fabs(dir.y) > fabs(dir.x)) {
			tFacing = makeVector3DI(0, -1, 0);
		}
		else if (dir.y > 0 && fabs(dir.y) > fabs(dir.x)) {
			tFacing = makeVector3DI(0, 1, 0);
		}
		else if (dir.x > 0) {
			tFacing = makeVector3DI(1, 0, 0);
		}
		else {
			tFacing = makeVector3DI(-1, 0, 0);
		}

		updateFacing(tEnemy, tFacing);
	}

}

static void updateSingleEnemy(Enemy& tEnemy) {
	if (!tEnemy.mIsActive) return;

	updateMovingToTarget(tEnemy);
	updateHasReachedTarget(tEnemy);
	updateHouseDamage(tEnemy);
}

static void updateEnemyHandler(void* tData) {
	(void)tData;
	
	for (int i = 0; i < 17; i++) {
		updateSingleEnemy(gEnemyHandler.mStationaryEnemies[i]);
	}

}

ActorBlueprint getEnemyHandler()
{
	return makeActorBlueprint(loadEnemyHandler, NULL, updateEnemyHandler);
}

static void enemyHitCB(void* tCaller, void* tCollisionData) {
	(void)tCaller;
	(void)tCollisionData;

	if (tCollisionData) {
		setPlayerHit();
	}
}

static void addGeneralEnemy(Enemy& tEnemy, int tIsChasingPlayer) {
	tEnemy.mEntityID = addBlitzEntity(makePosition(0, 0, 15));
	addBlitzMugenAnimationComponent(tEnemy.mEntityID, getGameSprites(), getGameAnimations(), 5000);
	addBlitzCollisionComponent(tEnemy.mEntityID);
	int collisionID = addBlitzCollisionRect(tEnemy.mEntityID, getEnemyCollisionList(), makeCollisionRect(makePosition(-10, -10, 0), makePosition(10, 10, 1)));
	addBlitzCollisionCB(tEnemy.mEntityID, collisionID, enemyHitCB, &tEnemy);
	setBlitzCollisionCollisionData(tEnemy.mEntityID, collisionID, &tEnemy);

	tEnemy.mPath = queue<Vector3DI>();
	tEnemy.mCurrentTarget = makePosition(0, 0, 0);
	tEnemy.mIsChasingPlayer = tIsChasingPlayer;
	tEnemy.mHasReachedTarget = 0;
	tEnemy.mIsActive = 1;
}


static void addStationaryEnemy(int i, Position tPosition, Vector3DI tFacing) {
	Enemy& enemy = gEnemyHandler.mStationaryEnemies[i];
	addGeneralEnemy(enemy, 0);

	setBlitzEntityPosition(enemy.mEntityID, tPosition);
	enemy.mFinalTarget = enemy.mCurrentTarget = tPosition;

	updateFacing(enemy, tFacing);
}

void spawnFirstEnemies()
{
	addStationaryEnemy(0, makePosition(540, 660, 4), makeVector3DI(1, 0, 0));
	addStationaryEnemy(1, makePosition(540, 660 - 32 * 1, 4), makeVector3DI(1, 0, 0));
	addStationaryEnemy(2, makePosition(540, 660 - 32 * 2, 4), makeVector3DI(1, 0, 0));
	addStationaryEnemy(3, makePosition(540, 660 - 32 * 3, 4), makeVector3DI(1, 0, 0));

	addStationaryEnemy(4, makePosition(560, 680, 4), makeVector3DI(0, -1, 0));
	addStationaryEnemy(5, makePosition(560 + 32 * 1, 680, 4), makeVector3DI(0, -1, 0));
	addStationaryEnemy(6, makePosition(560 + 32 * 2, 680, 4), makeVector3DI(0, -1, 0));
	addStationaryEnemy(7, makePosition(560 + 32 * 4, 680, 4), makeVector3DI(0, -1, 0));

	addStationaryEnemy(8, makePosition(560, 544, 4), makeVector3DI(0, 1, 0));
	addStationaryEnemy(9, makePosition(560 + 32 * 1, 544, 4), makeVector3DI(0, 1, 0));
	addStationaryEnemy(10, makePosition(560 + 32 * 2, 544, 4), makeVector3DI(0, 1, 0));
	addStationaryEnemy(11, makePosition(560 + 32 * 3, 544, 4), makeVector3DI(0, 1, 0));
	addStationaryEnemy(12, makePosition(560 + 32 * 4, 544, 4), makeVector3DI(0, 1, 0));

	addStationaryEnemy(13, makePosition(709, 660, 4), makeVector3DI(-1, 0, 0));
	addStationaryEnemy(14, makePosition(709, 660 - 32 * 1, 4), makeVector3DI(-1, 0, 0));
	addStationaryEnemy(15, makePosition(709, 660 - 32 * 2, 4), makeVector3DI(-1, 0, 0));
	addStationaryEnemy(16, makePosition(709, 660 - 32 * 3, 4), makeVector3DI(-1, 0, 0));



}

void spawnEnemyInHouse()
{
	Enemy& enemy = gEnemyHandler.mHouseEnemy;
	addGeneralEnemy(enemy, 2);

	Position p = makePosition(640, 629, 4);
	setBlitzEntityPosition(enemy.mEntityID, p);
	enemy.mFinalTarget = enemy.mCurrentTarget = p;

	updateFacing(enemy, makeVector3DI(0, 1, 0));
}

void setEnemiesInvisible()
{
	for (int i = 0; i < 17; i++) {
		if (!gEnemyHandler.mStationaryEnemies[i].mIsActive) continue;

		setBlitzMugenAnimationVisibility(gEnemyHandler.mStationaryEnemies[i].mEntityID, 0);
	}

	if (gEnemyHandler.mHouseEnemy.mIsActive) {
		setBlitzMugenAnimationVisibility(gEnemyHandler.mHouseEnemy.mEntityID, 1);
	}
}

void setEnemiesVisible()
{
	for (int i = 0; i < 17; i++) {
		if (!gEnemyHandler.mStationaryEnemies[i].mIsActive) continue;

		setBlitzMugenAnimationVisibility(gEnemyHandler.mStationaryEnemies[i].mEntityID, 1);
	}

	if (gEnemyHandler.mHouseEnemy.mIsActive) {
		setBlitzMugenAnimationVisibility(gEnemyHandler.mHouseEnemy.mEntityID, 0);
	}
}

void setEnemiesInactive()
{
}


static void resetEnemyPosition(Enemy& e) {
	Vector3DI targetTile = makeVector3DI(e.mFinalTarget.x / 32, e.mFinalTarget.y / 32, 0);
	Vector3DI startTile = getFreeStartTile();
	vector<Vector3DI> path = getPath(startTile, targetTile);
	e.mPath = queue<Vector3DI>();
	for (int i = 0; i < path.size(); i++) e.mPath.push(path[i]);

	setBlitzEntityPosition(e.mEntityID, makePosition(startTile.x * 32 + 16, startTile.y * 32 + 16, 15));
	setNextTarget(e);
	e.mHasReachedTarget = 0;
}

void removeEnemy(void * tData)
{
	Enemy* e = (Enemy*)tData;
	if (e->mIsChasingPlayer == 2) {
		setBlitzEntityPosition(e->mEntityID, makePosition(-10, -10, 15));
	}
	else {
		resetEnemyPosition(*e);
	}
}

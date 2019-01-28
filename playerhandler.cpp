#include "playerhandler.h"

#include <prism/blitz.h>

#include "collision.h"
#include "backgroundhandler.h"
#include "coinhandler.h"
#include "healthbarhandler.h"
#include "titlehandler.h"
#include "enemyhandler.h"

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;

	Vector3DI mFacing;

	int mEntityID;
	int mIsWalking;
	int mIsFacingBack;
	int mHasGun;

	int mHasShot;
	int mShotEntityID;

	int mHitNow;
	int mIsHit;

	int mIsActive;
} gPlayer;

static void loadPlayer(void* tData) {
	(void)tData;

	gPlayer.mSprites = loadMugenSpriteFileWithoutPalette("player/PLAYER.sff");
	gPlayer.mAnimations = loadMugenAnimationFile("player/PLAYER.air");

	gPlayer.mEntityID = addBlitzEntity(makePosition(32 * 20 - 320, 32 * 20 - 320, 10));
	addBlitzMugenAnimationComponent(gPlayer.mEntityID, &gPlayer.mSprites, &gPlayer.mAnimations, 10);
	addBlitzPhysicsComponent(gPlayer.mEntityID);
	addBlitzCollisionComponent(gPlayer.mEntityID);
	
	int collisionID = addBlitzCollisionRect(gPlayer.mEntityID, getPlayerCollisionList(), makeCollisionRect(makePosition(-8, -8, 0), makePosition(8, 0, 1)));
	setBlitzCollisionSolid(gPlayer.mEntityID, collisionID, 1);
	setBlitzCollisionCollisionData(gPlayer.mEntityID, collisionID, &gPlayer);
	addBlitzCollisionRect(gPlayer.mEntityID, getPlayerCollectCollisionList(), makeCollisionRect(makePosition(-16, -32, 0), makePosition(16, 0, 1)));

	gPlayer.mFacing = makeVector3DI(0, 1, 0);

	gPlayer.mHasShot = 0;

	gPlayer.mIsWalking = 0;
	gPlayer.mIsFacingBack = 0;
	gPlayer.mHasGun = 0;
	gPlayer.mIsHit = 0;
	gPlayer.mIsActive = 1;

}


static void shotCB(void* tCaller, void* tData) {
	(void)tCaller;

	removeEnemy(tData);
}

static void shotFinished(void* tCaller) {
	(void)tCaller;
	removeBlitzEntity(gPlayer.mShotEntityID);
	gPlayer.mHasShot = 0;
}

static void addShot() {

	Position offset = makePosition(10 * gPlayer.mFacing.x, 20 * gPlayer.mFacing.y - 10, 0);

	gPlayer.mShotEntityID = addBlitzEntity(vecAdd(getPlayerPosition(), offset));

	int animationNumber, sHorizontal, sVertical;
	if (gPlayer.mFacing.x == 1 && gPlayer.mFacing.y == 0) {
		animationNumber = 2000;
		sVertical = 0;
		sHorizontal = 0;
	} else if (gPlayer.mFacing.x == -1 && gPlayer.mFacing.y == 0) {
		animationNumber = 2000;
		sVertical = 0;
		sHorizontal = 1;
	}
	else if (gPlayer.mFacing.x == 0 && gPlayer.mFacing.y == 1) {
		animationNumber = 2001;
		sVertical = 0;
		sHorizontal = 0;
	}
	else if (gPlayer.mFacing.x == 0 && gPlayer.mFacing.y == -1) {
		animationNumber = 2001;
		sVertical = 1;
		sHorizontal = 0;
	}
	else if (gPlayer.mFacing.x == 1 && gPlayer.mFacing.y == 1) {
		animationNumber = 2002;
		sVertical = 0;
		sHorizontal = 0;
	}
	else if (gPlayer.mFacing.x == -1 && gPlayer.mFacing.y == 1) {
		animationNumber = 2002;
		sVertical = 0;
		sHorizontal = 1;
	}
	else if (gPlayer.mFacing.x == 1 && gPlayer.mFacing.y == -1) {
		animationNumber = 2002;
		sVertical = 1;
		sHorizontal = 0;
	}
	else {
		animationNumber = 2002;
		sVertical = 1;
		sHorizontal = 1;
	}

	addBlitzMugenAnimationComponent(gPlayer.mShotEntityID, &gPlayer.mSprites, &gPlayer.mAnimations, animationNumber);
	setBlitzMugenAnimationFaceDirection(gPlayer.mShotEntityID, !sHorizontal);
	setBlitzMugenAnimationVerticalFaceDirection(gPlayer.mShotEntityID, !sVertical);
	setBlitzMugenAnimationCallback(gPlayer.mShotEntityID, shotFinished, NULL);

	addBlitzCollisionComponent(gPlayer.mShotEntityID);
	int collisionID = addBlitzCollisionAttackMugen(gPlayer.mShotEntityID, getShotCollisionList());
	addBlitzCollisionCB(gPlayer.mShotEntityID, collisionID, shotCB, NULL);

	gPlayer.mHasShot = 1;
}

static void updatePlayerShooting() {
	if (!gPlayer.mHasGun) return;
	if (gPlayer.mHasShot) return;

	if (hasPressedAFlank()) {
		addShot();
	}
}

static void updatePlayerMovement() {

	Position p = getBlitzEntityPosition(gPlayer.mEntityID);
	p = clampPositionToGeoRectangle(p, makeGeoRectangle(10, 10, 1270 - 640, 1270 - 640));
	setBlitzEntityPosition(gPlayer.mEntityID, p);

	double speed = 3;
	Vector3D delta = makePosition(0, 0, 0);
	Vector3DI target = makeVector3DI(0, 0, 0);

	if (hasPressedLeft()) {
		delta.x -= speed;
		target.x = -1;
	}
	else if (hasPressedRight()) {
		delta.x += speed;
		target.x = 1;

	}

	if (hasPressedUp()) {
		delta.y -= speed;
		target.y = -1;
		gPlayer.mIsFacingBack = 1;
	}
	else if (hasPressedDown()) {
		delta.y += speed;
		target.y = 1;
		gPlayer.mIsFacingBack = 0;
	}

	if (target.x || target.y) {
		gPlayer.mFacing = target;
	}

	gPlayer.mIsWalking = vecLength2D(delta) > 1e-9;

	setBlitzPhysicsVelocityX(gPlayer.mEntityID, delta.x);
	setBlitzPhysicsVelocityY(gPlayer.mEntityID, delta.y);

	Position finalPos = vecSub(getBlitzEntityPosition(gPlayer.mEntityID), makePosition(160, 120, 0));
	finalPos = clampPositionToGeoRectangle(finalPos, makeGeoRectangle(0, 0, 1280 - 320 - 640, 1280 - 240 - 640));
	setBlitzCameraHandlerPosition(finalPos);
}

static void updatePlayerAnimation() {
	int animationValue = gPlayer.mIsWalking ? 20 : 10;
	if (gPlayer.mIsFacingBack) animationValue += 100;
	if (gPlayer.mHasGun) animationValue += 1000;

	changeBlitzMugenAnimationIfDifferent(gPlayer.mEntityID, animationValue);
}

static void updateInsideOutsideSwitch() {
	Position p = getBlitzEntityPosition(gPlayer.mEntityID);

	if (isInside() && p.y > 677 - 320) {
		setOutside();
	}
	else if (!isInside() && p.y > 549 - 320 && p.y < 668 - 320 && p.x > 549 - 320 && p.x < 699 - 320) {
		setInside();
	}
}

static void updatePlayerHit() {
	if (!gPlayer.mIsHit) return;

	setBlitzMugenAnimationColor(gPlayer.mEntityID, 1, 1, 1);

	gPlayer.mHitNow++;
	if (gPlayer.mHitNow > 60) {
		setBlitzMugenAnimationTransparency(gPlayer.mEntityID, 1);
		gPlayer.mIsHit = 0;
	}
}

static void updatePlayer(void* tData) {
	(void)tData;
	setBlitzCameraHandlerPosition(vecSub(getBlitzEntityPosition(gPlayer.mEntityID), makePosition(160, 120, 0)));

	updatePlayerHit();

	if (!gPlayer.mIsActive) return;

	updatePlayerShooting();
	updatePlayerMovement();
	updatePlayerAnimation();
	updateInsideOutsideSwitch();
    updatePlayerHealthOnScreen();
}

ActorBlueprint getPlayerHandler()
{
	return makeActorBlueprint(loadPlayer, NULL, updatePlayer);
}

Position getPlayerPosition()
{
	return getBlitzEntityPosition(gPlayer.mEntityID);
}

int isPlayerActive()
{
	return gPlayer.mIsActive;
}

void setPlayerInactive()
{
	gPlayer.mIsWalking = 0;
	updatePlayerAnimation();
	setBlitzPhysicsVelocity(gPlayer.mEntityID, makePosition(0, 0, 0));

	gPlayer.mIsActive = 0;
}

void setPlayerActive()
{
	gPlayer.mIsActive = 1;

}

void setPlayerHit()
{
	if (gPlayer.mIsHit) return;
	if (isInside()) return;
	if (!gPlayer.mIsActive) return;


	decreasePlayerHealth();
	if (getPlayerHealth() == 0) {
		setPlayerInactive();
		changeBlitzMugenAnimation(gPlayer.mEntityID, 9999);
		startGameOver();
		return;
	}

	setBlitzMugenAnimationColor(gPlayer.mEntityID, 1, 0, 0);
	setBlitzMugenAnimationTransparency(gPlayer.mEntityID, 0.5);

	gPlayer.mHitNow = 0;
	gPlayer.mIsHit = 1;
}

void setGunActive()
{
	gPlayer.mHasGun = 1;
}

void setPlayerInvisible()
{
	setBlitzMugenAnimationVisibility(gPlayer.mEntityID, 0);
}

#include "backgroundhandler.h"

#include <fstream>
#include <queue>


#include <prism/blitz.h>

#include "collision.h"
#include "coinhandler.h"
#include "sleephandler.h"
#include "gamescreen.h"
#include "musichandler.h"
#include "scratchinghandler.h"
#include "enemyhandler.h"
#include "healthbarhandler.h"
#include "playerhandler.h"

#define MAP_SIZE 20

static struct {
	MugenSpriteFile mSprites;
	MugenAnimations mAnimations;

	vector<vector<int> > mOutEntityIDs;
	vector<vector<int> > mInEntityIDs;

	vector<vector<int> > mIsBlockedIn;
	vector<vector<int> > mIsBlockedOut;
	vector<vector<int> > mBlockIDs;

	TextureData mWhiteTexture;
	int mTintID;

	int mIsInside;
} gBackgroundHandler;


typedef struct {
	int mAnimation;
	int mIsBlocked;
} Tile;

static void removeBlocks() {
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			if (gBackgroundHandler.mBlockIDs[y][x] == -1) continue;
			removeBlitzEntity(gBackgroundHandler.mBlockIDs[y][x]);
		}
	}

}

static vector<vector<int> >& getBlockList() {
	return isInside() ? gBackgroundHandler.mIsBlockedIn : gBackgroundHandler.mIsBlockedOut;
}

static void addBlocks() {
	vector<vector<int> >& blockList = getBlockList();

	gBackgroundHandler.mBlockIDs = vector<vector<int> >(MAP_SIZE, vector<int>(MAP_SIZE, -1));
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			if (!blockList[y][x]) continue;
			gBackgroundHandler.mBlockIDs[y][x] = addBlitzEntity(makePosition(32 * x, 32 * y, 0));

			addBlitzCollisionComponent(gBackgroundHandler.mBlockIDs[y][x]);
			int collisionID = addBlitzCollisionRect(gBackgroundHandler.mBlockIDs[y][x], getBGCollisionList(), makeCollisionRect(makePosition(0, 0, 0), makePosition(32, 32, 32)));
			setBlitzCollisionSolid(gBackgroundHandler.mBlockIDs[y][x], collisionID, 0);

		}
	}
}

static void updateBlocks() {
	removeBlocks();
	addBlocks();
}

static void loadLayer(char* tPath, vector<vector<int> >& tEntityIDs, double tBaseZ) {
	Buffer b = fileToBuffer(tPath);
    BufferPointer p = getBufferPointer(b);

	tEntityIDs = vector<vector<int> >(MAP_SIZE, vector<int>(MAP_SIZE, -1));
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			int value = readIntegerFromTextStreamBufferPointer(&p);
			if (!value) continue;
			tEntityIDs[y][x] = addBlitzEntity(makePosition(32 * x, 32 * y, tBaseZ + (y / (double)MAP_SIZE)));
			addBlitzMugenAnimationComponent(tEntityIDs[y][x], &gBackgroundHandler.mSprites, &gBackgroundHandler.mAnimations, value);
		}
	}

    freeBuffer(b);
}

static void loadIsBlocked(char* tPath, vector<vector<int> >& tIsBlocked) {
	Buffer b = fileToBuffer(tPath);
    BufferPointer p = getBufferPointer(b);

	tIsBlocked = vector<vector<int> >(MAP_SIZE, vector<int>(MAP_SIZE, -1));
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			tIsBlocked[y][x] = readIntegerFromTextStreamBufferPointer(&p);
		}
	}

    freeBuffer(b);
}

static void setLayerVisibility(vector<vector<int> >& tEntityIDs, int tVisibility) {
	for (int y = 0; y < MAP_SIZE; y++) {
		for (int x = 0; x < MAP_SIZE; x++) {
			if (tEntityIDs[y][x] == -1) continue;
			setBlitzMugenAnimationVisibility(tEntityIDs[y][x], tVisibility);
		}
	}
}

static void loadBackgroundHandler(void* tData) {
	(void)tData;

	gBackgroundHandler.mSprites = loadMugenSpriteFileWithoutPalette("level/BG.sff");
	gBackgroundHandler.mAnimations = loadMugenAnimationFile("level/BG.air");
	gBackgroundHandler.mWhiteTexture = createWhiteTexture();

	instantiateActor(getBlitzCameraHandler());

	loadLayer("level/OUT.txt", gBackgroundHandler.mOutEntityIDs, 2);
	loadLayer("level/IN.txt", gBackgroundHandler.mInEntityIDs, 2);
	loadIsBlocked("level/IN_BLOCKS.txt", gBackgroundHandler.mIsBlockedIn);
	loadIsBlocked("level/OUT_BLOCKS.txt", gBackgroundHandler.mIsBlockedOut);

	addBlocks();
}

ActorBlueprint getBackgroundHandler()
{
	return makeActorBlueprint(loadBackgroundHandler);
}

int isInside()
{
	return gBackgroundHandler.mIsInside;
}

static void destroyTeddy() {
	changeBlitzMugenAnimation(gBackgroundHandler.mInEntityIDs[17 - 10][19 - 10], 75);
	changeBlitzMugenAnimation(gBackgroundHandler.mInEntityIDs[17 - 10][20 - 10], 76);
	changeBlitzMugenAnimation(gBackgroundHandler.mInEntityIDs[18 - 10][19 - 10], 77);
	changeBlitzMugenAnimation(gBackgroundHandler.mInEntityIDs[18 - 10][20 - 10], 78);
}

static void removeTint() {
	removeHandledAnimation(gBackgroundHandler.mTintID);
}

static void addTint(double r, double g, double b) {
	gBackgroundHandler.mTintID = playOneFrameAnimationLoop(makePosition(0, 0, 70), &gBackgroundHandler.mWhiteTexture);
	setAnimationColor(gBackgroundHandler.mTintID, r, g, b);
	setAnimationTransparency(gBackgroundHandler.mTintID, 0.3);
	setAnimationSize(gBackgroundHandler.mTintID, makePosition(320, 240, 1), makePosition(0, 0, 0));
}

void setInside() {
	setLayerVisibility(gBackgroundHandler.mOutEntityIDs, 0);
	setLayerVisibility(gBackgroundHandler.mInEntityIDs, 1);
	setCoinsInvisible();
	setEnemiesInvisible();

	if (getDay() > 1) {
		unpauseScratching();
	}

	if (getDay() == 2 || getDay() == 3) {
		playSadMusic();
	}

	if (getDay() == 4) {
		stopStreamingMusicFile();
	}

	if (getDay() == 4 && getStoryBeat() == 0) {
		increaseStoryBeat();
		spawnEnemyInHouse();
		destroyTeddy();
	}

	if (getDay() == 5 || getDay() == 6 || getDay() == 7) {
		playDiscordMusic();
		removeTint();
	}

	gBackgroundHandler.mIsInside = 1;
	updateBlocks();
}


void setOutside()
{
	if (getDay() == 1 && getStoryBeat() == 0) {
		increaseStoryBeat();
		playPeacefulMusic();
	}

	if (getDay() == 2 && getStoryBeat() == 0) {
		increaseStoryBeat();
		spawnFirstEnemies();
		setHealthbarsActive();
	}

	if (getDay() >= 2) {
		playActionMusic();
	}

	if (getDay() == 3) {
		setGunActive();
	}

	if (getDay() == 5) {
		addTint(249 / 255.0, 200 / 255.0, 255 / 255.0);
	}

	if (getDay() == 6) {
		addTint(230 / 255.0, 0, 0);
	}

	if (getDay() == 7) {
		addTint(0, 0, 230 / 255.0);
	}

	setLayerVisibility(gBackgroundHandler.mOutEntityIDs, 1);
	setLayerVisibility(gBackgroundHandler.mInEntityIDs, 0);
	setCoinsVisible();
	setEnemiesVisible();
	pauseScratching();

	gBackgroundHandler.mIsInside = 0;
	updateBlocks();
}

int getTileAmount()
{
	return MAP_SIZE;
}

int isTileFree(int x, int y)
{
	auto blockList = gBackgroundHandler.mIsBlockedOut;
	return !blockList[y][x];
}

void setHouseCollapsed()
{
	if (isInside()) {
		setOutside();
		stopGameMusic();
	}
}

Vector3DI getFreeStartTile()
{
	Vector3DI ret = makeVector3DI(0, 0, 0);

	for (int i = 0; i < 100; i++) {

		double p = randfrom(0, 1);

		if (p < 0.25) {
			ret = makeVector3DI(0, randfromInteger(0, MAP_SIZE - 1), 0);
		}
		else if (p < 0.5) {
			ret = makeVector3DI(MAP_SIZE - 1, randfromInteger(0, MAP_SIZE - 1), 0);
		}
		else if (p < 0.75) {
			ret = makeVector3DI(randfromInteger(0, MAP_SIZE - 1), 0, 0);
		}
		else {
			ret = makeVector3DI(randfromInteger(0, MAP_SIZE - 1), MAP_SIZE - 1, 0);
		}

		vector<vector<int> >& blocked = gBackgroundHandler.mIsBlockedOut;

		if (!blocked[ret.y][ret.x]) break;
	}

	return ret;
}

static int manDist(Vector3DI a, Vector3DI b) {
	return abs(a.x - b.x) + abs(a.y - b.y);

}

std::vector<Vector3DI> getPath(Vector3DI tStart, Vector3DI tTarget)
{
	priority_queue<pair<int, pair<int, int> > > q;

	vector<vector<int> >& blocked = gBackgroundHandler.mIsBlockedOut;
	vector<Vector3DI> ret;
	vector<vector<pair<int, int> > > parent(MAP_SIZE, vector<pair<int, int> >(MAP_SIZE, make_pair(-1, -1)));
	vector<vector<int> > vis(MAP_SIZE, vector<int>(MAP_SIZE, 0));

	q.push(make_pair(-manDist(tStart, tTarget), make_pair(tStart.x, tStart.y)));

	while (!q.empty()) {
		auto current = q.top();
		q.pop();

		auto location = current.second;
		if (location.first == tTarget.x && location.second == tTarget.y) {
			Vector3DI current = tTarget;
			while (current.x != tStart.x || current.y != tStart.y) {
				ret.push_back(current);
				current = makeVector3DI(parent[current.y][current.x].first, parent[current.y][current.x].second, 0);
			}

			ret = vector<Vector3DI>(ret.rbegin(), ret.rend());
			break;
		}

		int nx = -1, ny = 0;
		if (location.first > 0 && !vis[location.second + ny][location.first + nx] && !blocked[location.second + ny][location.first + nx]) {
			parent[location.second + ny][location.first + nx] = make_pair(location.first, location.second);
			vis[location.second + ny][location.first + nx] = 1;
			q.push(make_pair(-manDist(makeVector3DI(location.first + nx, location.second + ny, 0), tTarget), make_pair(location.first + nx, location.second + ny)));
		}

		nx = 1;
		ny = 0;
		if (location.first < MAP_SIZE - 1 && !vis[location.second + ny][location.first + nx] && !blocked[location.second + ny][location.first + nx]) {
			parent[location.second + ny][location.first + nx] = make_pair(location.first, location.second);
			vis[location.second + ny][location.first + nx] = 1;
			q.push(make_pair(-manDist(makeVector3DI(location.first + nx, location.second + ny, 0), tTarget), make_pair(location.first + nx, location.second + ny)));
		}

		nx = 0;
		ny = 1;
		if (location.second < MAP_SIZE - 1 && !vis[location.second + ny][location.first + nx] && !blocked[location.second + ny][location.first + nx]) {
			parent[location.second + ny][location.first + nx] = make_pair(location.first, location.second);
			vis[location.second + ny][location.first + nx] = 1;
			q.push(make_pair(-manDist(makeVector3DI(location.first + nx, location.second + ny, 0), tTarget), make_pair(location.first + nx, location.second + ny)));
		}

		nx = 0;
		ny = -1;
		if (location.second > 0 && !vis[location.second + ny][location.first + nx] && !blocked[location.second + ny][location.first + nx]) {
			parent[location.second + ny][location.first + nx] = make_pair(location.first, location.second);
			vis[location.second + ny][location.first + nx] = 1;
			q.push(make_pair(-manDist(makeVector3DI(location.first + nx, location.second + ny, 0), tTarget), make_pair(location.first + nx, location.second + ny)));
		}
	}

	return ret;

}

#include "helptexthandler.h"

#include <prism/blitz.h>

#include "gamescreen.h"
#include "playerhandler.h"

static struct {


	int mHasSeenFirstHelpText;
	int mHasSeenDepositHelpText;
	int mHasSeenSleepHelpText;
	int mHasSeenFinalText;
	int mIsActive;

	int mBackgroundID;
	int mTextID;
	vector<string> mTexts;
	int mCurrentText;
} gHelpTextData;

static void loadHelpTextHandler(void* tData) {
	(void)tData;
	gHelpTextData.mHasSeenFinalText = 0;
	gHelpTextData.mIsActive = 0;
}

static void setTextInactive() {
	removeMugenAnimation(gHelpTextData.mBackgroundID);
	removeMugenText(gHelpTextData.mTextID);
	setPlayerActive();

	gHelpTextData.mIsActive = 0;
}

static void setSingleTextActive() {
	string text = gHelpTextData.mTexts[gHelpTextData.mCurrentText];

	gHelpTextData.mBackgroundID = addMugenAnimation(getMugenAnimation(getGameAnimations(), 1000), getGameSprites(), makePosition(160, 20, 45));
	gHelpTextData.mTextID = addMugenTextMugenStyle(text.data(), makePosition(40, 35, 46), makeVector3DI(1, 0, 1));
	setMugenTextTextBoxWidth(gHelpTextData.mTextID, 257);
	setMugenTextBuildup(gHelpTextData.mTextID, 1);
	setPlayerInactive();
	gHelpTextData.mIsActive = 1;
}


static void advanceHelpText() {
	setTextInactive();
	gHelpTextData.mCurrentText++;
	if (gHelpTextData.mCurrentText >= gHelpTextData.mTexts.size()) { 
		if (gHelpTextData.mHasSeenFinalText) {
			setNewScreen(getGameScreen());
		}
		return; 
	}
	setSingleTextActive();
}

static void updateHelpTextHandler(void* tData) {
	(void)tData;
	if (!gHelpTextData.mIsActive) return;

	if (hasPressedAFlank()) {
		if (isMugenTextBuiltUp(gHelpTextData.mTextID)) {
			advanceHelpText();
		}
		else {
			setMugenTextBuiltUp(gHelpTextData.mTextID);
		}
	}
}

ActorBlueprint getHelpTextHandler()
{
	return makeActorBlueprint(loadHelpTextHandler, NULL, updateHelpTextHandler);
}



static void showGeneralHelpText(vector<string> tTexts, int* tSeenBefore) {
	if (*tSeenBefore) return;

	gHelpTextData.mIsActive = 1;
	gHelpTextData.mTexts = tTexts;
	gHelpTextData.mCurrentText = 0;
	setSingleTextActive();

	*tSeenBefore = 1;
}

void showFirstHelpText()
{
	vector<string> texts;
	texts.push_back("It's the first day in your new house! Congrats!");
	texts.push_back("Having a house is a huge responsibility.");
	texts.push_back("Go outside and collect coins around the neighborhood to pay for repairs!");
	showGeneralHelpText(texts, &gHelpTextData.mHasSeenFirstHelpText);

}

void showDepositHelpText()
{
	vector<string> texts;
	texts.push_back("Once you've collected coins, you can deposit them in the piggy bank in your house!");
	texts.push_back("The repair fund!");
	showGeneralHelpText(texts, &gHelpTextData.mHasSeenDepositHelpText);
}

void showSleepHelpText()
{
	vector<string> texts;
	texts.push_back("After collecting enough coins, take a good night's rest in your bed to recover.");
	showGeneralHelpText(texts, &gHelpTextData.mHasSeenSleepHelpText);
}


void showFinalHelpText()
{
	vector<string> texts;
	texts.push_back("However, the young man who might just be you never woke up the day after that.");
	texts.push_back("A tragic end.");
	texts.push_back("He overslept.");
	texts.push_back("After that, he finally took out the trash, so the rats stopped coming.");
	texts.push_back("Also, he no longer dumped food down the drain.");
	texts.push_back("Don't do that.");
	texts.push_back("Never do that.");
	texts.push_back("Proper housekeeping is important.");
	texts.push_back("And that's the moral of the game.");
	texts.push_back("Thanks a lot for playing!");
	showGeneralHelpText(texts, &gHelpTextData.mHasSeenFinalText);
}
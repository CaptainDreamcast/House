#pragma once

#include <prism/wrapper.h>
#include <prism/mugenanimationhandler.h>
#include <prism/mugensoundfilereader.h>

Screen* getGameScreen();

MugenSpriteFile* getGameSprites();
MugenAnimations* getGameAnimations(); 
MugenSounds* getGameSounds();

int getStoryBeat();
void resetStoryBeat();
void increaseStoryBeat();
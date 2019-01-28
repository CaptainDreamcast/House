#include "musichandler.h"

#include <prism/blitz.h>

static void loadMusicHandler(void* tData) {
	(void)tData;


}

ActorBlueprint getMusicHandler()
{
	return makeActorBlueprint(loadMusicHandler);
}

void playPeacefulMusic()
{
	stopStreamingMusicFile();
	streamMusicFile("music/PEACEFUL.ogg");
}

void playActionMusic()
{
	stopStreamingMusicFile();
	streamMusicFile("music/ACTION.ogg");
}

void playSadMusic()
{
	stopStreamingMusicFile();
	streamMusicFile("music/SAD.ogg");
}

void playDiscordMusic()
{
	stopStreamingMusicFile();
	streamMusicFile("music/DISCORD.ogg");
}

void stopGameMusic()
{
	stopStreamingMusicFile();
}

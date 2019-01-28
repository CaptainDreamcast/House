#pragma once

#include <prism/actorhandler.h>

ActorBlueprint getCoinHandler();

void shuffleCoins();
void setCoinsInvisible();
void setCoinsVisible();

int getMoneyAmount();
void resetMoney();
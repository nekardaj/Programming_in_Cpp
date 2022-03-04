#include "GameLogic.h"

bool Player::TakeTurn(Directions dir)
{
	if(freeze > 0)
	{
		freeze--;
		return true;
	}
	return true;
}

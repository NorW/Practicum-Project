#include "StdAfx.h"
#include "Player.h"

Player::Player(bool isHuman): _isHuman(isHuman)
{

}

int Player::requestFunds(int amount)
{
	int returnVal = std::min(amount, _money);
	_money -= returnVal;
	return returnVal;
}

void Player::reset()
{
	_money = 0;
}
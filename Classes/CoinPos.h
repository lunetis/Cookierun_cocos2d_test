#ifndef __COIN_POS_H__
#define __COIN_POS_H__

#include <iostream>
#include "cocos2d.h"

#define V

USING_NS_CC;

class CoinPos
{
	
public:
	static Vec2 vectors[17];
	static int getSize(std::string str);
	static void getPos(std::string str);
};



#endif
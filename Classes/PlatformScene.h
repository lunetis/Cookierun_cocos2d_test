/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 
 http://www.cocos2d-x.org
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#ifndef __PLATFORM_SCENE_H__
#define __PLATFORM_SCENE_H__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "CoinPos.h"
#include "audio/include/SimpleAudioEngine.h"
using namespace CocosDenshion;
USING_NS_CC;

#define CHARACTER_TAG 10
#define WAKER_TAG 11

class Platformer : public cocos2d::Scene
{
	// Some Tags
	enum ITEM_TAG {
		
		COIN = 20,
		GOLDCOIN = 21,
		JELLY = 25,
		BEARJELLY = 26,
		TRAP = 30,
		SNAKE = 31,
		ENHANCE = 40,
		BOOST = 41,
		HEALTH = 42,
		FINISH = 99
	};


	enum STATUS_TAG {
		RUN = 1,
		JUMP1 = 2,
		JUMP2 = 3,
		SLIDE = 4
	};

public:
    static Scene* createScene();
	

    virtual bool init();
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(Platformer);

	void update(float) override;

	// Keyboard
	void onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event);
	void onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event);

	// Physics
	bool onContactBegin(PhysicsContact &contact);

	// Character Status
	void opacityToggle();
	

private:
	// Functions
	int getHeight(const Vec2 &position);
	void setCoins(TMXObjectGroup* coins);
	void setCoin(float x, float y, float size, int tag);

	void setTraps(TMXObjectGroup* traps);
	void setItems(TMXObjectGroup* items);
	void setItem(float x, float y, int tag);

	// Status function
	void resetInvincible(float dt);
	void resetEnhanced(float dt);
	void resetBoost(float dt);

	// UI
	void setHealthBar(float dt);
	void setHealthBarImmediate(float health);

	void initMap();
	void initUI();
	void initCharacter();

	// Game function
	void finish();
	void flagAnimate();
	void gameOver();

	void printResult(float dt);

	void replay(Ref* pSender);
	void exit(Ref* pSender);

	// Other stuffs
	void shakeScreen();
	void showBoostEffect();


	// Map Data
	int tileSize;


	// Character Component
	Sprite* character;
	
	Vec2 velocity;

	Label* scoreLabel;
	Label* coinLabel;

	int characterHeight = 110;
	int characterWidth = 40;
	float currentScale = 1;

	PhysicsBody *normalBody, *slideBody;

	// Character status
	bool isInvincible;
	bool isEnhanced;
	bool isBoost;

	int statusTag;

	// Moving
	bool isGround;
	bool isMovingLeft;
	bool isMovingRight;
	bool canJump1;
	bool canJump2;
	bool isSliding;

	bool tempSlide;

	// Animation
	Animate *runAnimation, *jump1Animation, *jump2Animation, *slideAnimation;
	

	// Game Data
	int score;
	int coin;
	float health;
	float healthBarLength;

	float blink;
	float boost;

	bool isFinished;
	bool isGameOver;


	// UI
	
	Camera* cam;
	Camera* uiCam;
	Sprite* bgFront, *bgRear;
	Sprite* progressIndicator;
	Size bgSize;
	Vec2 bgOffset;

	Sprite* hit;
	Sprite* healthBarBorder;
	Vec2 healthBarOrigin;
	float healthBarWidth;

	ui::LoadingBar* healthBar;

	// Use for Progress
	int startX, endX, startY, endY;
	float barWidth;
	Vec2 progressOrigin;


	// Ground
	TMXLayer* groundLayer;
	TMXLayer* metaLayer;
	Size mapSize;

	int groundHeight;


	// Else

	Node* waker;
	Sprite* checkpoint;
	Animate* snakeAnimation;

	bool isPaused;

	// Audio System

	SimpleAudioEngine* audio;
};

#endif // __PLATFORM_SCENE_H__

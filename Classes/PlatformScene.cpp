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

#include "PlatformScene.h"
#include "SimpleAudioEngine.h"
// Use Console
// #pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
USING_NS_CC;


#define JUMPFORCE 900
#define VELOCITY 350
#define STARTHEALTH 100
#define DAMAGE 15
#define DECREASE_AMOUNT 1.5


Scene* Platformer::createScene()
{
	Scene* scene = Platformer::createWithPhysics();
	auto layer = Platformer::create();
	scene->addChild(layer);
	// scene->getPhysicsWorld()->setDebugDrawMask(PhysicsWorld::DEBUGDRAW_ALL);
	return scene;
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in PlatformerScene.cpp\n");
}



// Set Single Coin at x, y
void Platformer::setCoin(float x, float y, float size, int tag)
{
	Sprite* coin;
	switch (tag)
	{
	case ITEM_TAG::COIN:		coin = Sprite::create("Items/Coin.png");		break;
	case ITEM_TAG::GOLDCOIN:	coin = Sprite::create("Items/Goldcoin.png");	break;
	case ITEM_TAG::JELLY:		coin = Sprite::create("Items/Jelly.png");		break;
	case ITEM_TAG::BEARJELLY:	coin = Sprite::create("Items/BearJelly.png");	break;
	default: coin = Sprite::create("Items/Coin.png"); break;
	}
	coin->setAnchorPoint(Vec2(0.5, 0));
	coin->setPosition(x, y);

	auto coinPhysicsBody = PhysicsBody::createBox(Size(size, size), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
	coinPhysicsBody->setDynamic(false);
	coinPhysicsBody->setContactTestBitmask(0xFFFFFFFF);
	coin->addComponent(coinPhysicsBody);
	coin->setTag(tag);
	addChild(coin, 2);
}


// Set coins with Object group
void Platformer::setCoins(TMXObjectGroup* coins)
{
	auto &objects = coins->getObjects();
	float x, y, offset;
	int coinHeight, coinWidth;
	int tag;

	for (auto &obj : objects)
	{
		ValueMap &dict = obj.asValueMap();
		x = dict["x"].asFloat();
		y = dict["y"].asFloat();
		offset = dict["Offset"].asFloat();

		// Coin type
		tag = dict["Tag"].asInt();
		
		// Rectengular
		if (dict["type"].asString() == "Rect")
		{
			log("Rect");
			coinHeight = dict["Height"].asInt();
			coinWidth = dict["Width"].asInt();
			for (int i = 0; i < coinHeight; i++)
			{
				for (int j = 0; j < coinWidth; j++)
				{
					setCoin(x + j * offset, y + i * offset, 30, tag);
				}
			}
		}

		// Alphabet
		else if (dict["type"].asString() == "Alphabet")
		{
			log("Alphabet");
			std::string c = dict["Char"].asString();

			int size = CoinPos::getSize(c);
			if (size == 0) continue;
			
			CoinPos::getPos(c);
			for (int i = 0; i < size; i++)
			{
				// log("Alphabet: Set Coin : %f, %f", x + pos[i].x, y + pos[i].y);
				setCoin(x + CoinPos::vectors[i].x, y + CoinPos::vectors[i].y, 30, tag);
			}
		}

		// Single Coin
		else
		{
			setCoin(x, y, 30, tag);
		}
	}
}



// Set traps with Object group
void Platformer::setTraps(TMXObjectGroup* traps)
{
	auto &objects = traps->getObjects();
	float x, y;
	Sprite* trap;
	PhysicsBody* body;

	for (auto &obj : objects)
	{
		ValueMap &dict = obj.asValueMap();
		x = dict["x"].asFloat();
		y = dict["y"].asFloat();

		if (dict["type"].asString() == "Chestnut")
		{
			trap = Sprite::create("Traps/Chestnut.png");
			trap->setAnchorPoint(Vec2(0.5f, 0.1f));
			body = PhysicsBody::createBox(Size(60, 60), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
			trap->setTag(ITEM_TAG::TRAP);
		}

		else if (dict["type"].asString() == "Pine")
		{
			trap = Sprite::create("Traps/PineCone.png");
			trap->setAnchorPoint(Vec2(0.5f, 0.1f));
			body = PhysicsBody::createBox(Size(60, 60), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
			trap->setTag(ITEM_TAG::TRAP);
		}

		else if (dict["type"].asString() == "Wire")
		{
			trap = Sprite::create("Traps/Wire.png");
			trap->setAnchorPoint(Vec2(0.5f, 0));
			body = PhysicsBody::createBox(Size(60, 160), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
			trap->setTag(ITEM_TAG::TRAP);
		}

		else if (dict["type"].asString() == "Snake")
		{
			trap = Sprite::create("Traps/Snake_0000.png");
			trap->setAnchorPoint(Vec2(0.5f, 0));
			body = PhysicsBody::createBox(Size(60, 80), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
			trap->setTag(ITEM_TAG::SNAKE);

			if (dict["UpsideDown"].asBool())
			{
				trap->setScaleY(-1);
				trap->setAnchorPoint(Vec2(0.5f, 1));
			}
		}

		trap->setPosition(x, y);
		
		body->setDynamic(false);
		body->setContactTestBitmask(0xFFFFFFFF);
		trap->addComponent(body);
		addChild(trap, 2);
	}
}



// Set Items with Object group
void Platformer::setItems(TMXObjectGroup* items)
{
	auto &objects = items->getObjects();
	float x, y;

	for (auto &obj : objects)
	{
		ValueMap &dict = obj.asValueMap();
		x = dict["x"].asFloat();
		y = dict["y"].asFloat();

		if (dict["type"].asString() == "Enhance")
		{
			log("Set Item(Enhance) Position (%lf, %lf)", x, y);
			setItem(x, y, ITEM_TAG::ENHANCE);
		}
		else if (dict["type"].asString() == "Boost")
		{
			log("Set Item(Enhance) Position (%lf, %lf)", x, y);
			setItem(x, y, ITEM_TAG::BOOST);
		}
		else if (dict["type"].asString() == "Health")
		{
			log("Set Item(Enhance) Position (%lf, %lf)", x, y);
			setItem(x, y, ITEM_TAG::HEALTH);
		}
	}
}



// Set single item
void Platformer::setItem(float x, float y, int tag)
{
	Sprite* item;
	switch (tag)
	{
	case ITEM_TAG::ENHANCE: item = Sprite::create("Enhance.png"); break;
	case ITEM_TAG::BOOST: item = Sprite::create("Boost.png"); break;
	case ITEM_TAG::HEALTH: item = Sprite::create("Health.png"); break;
	}
	item->setAnchorPoint(Vec2(0, 0));
	item->setPosition(x, y);

	auto itemPhysicsBody = PhysicsBody::createBox(Size(80, 80), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
	itemPhysicsBody->setDynamic(false);
	itemPhysicsBody->setContactTestBitmask(0xFFFFFFFF);
	item->addComponent(itemPhysicsBody);
	item->setTag(tag);
	item->setScale(0.75f);
	addChild(item, 2);
}





// getHeight: Return groundHeight of the character's corresponding ground position X
int Platformer::getHeight(const Vec2 &position)
{
	int height = -200;

	int tileXleft = ((int)(position.x)) / tileSize;
	int tileXright = ((int)(position.x) + characterWidth) / tileSize;
	int tileY = mapSize.height - ((int)(position.y) / tileSize);
	int tempTileX = -1;


	Sprite *groundTile1, *groundTile2;

	// log("Position : %f %f // Search tile at %d, %d", position.x, position.y, tileXleft, tileY);
	for (int i = tileY; i < mapSize.height; i++)
	{
		// Left corner
		groundTile1 = groundLayer->getTileAt(Vec2(tileXleft, i));

		// Right corner
		groundTile2 = groundLayer->getTileAt(Vec2(tileXright, i));
		
		// Search Meta layer
		if (groundTile1 || groundTile2)
		{
			groundTile1 = metaLayer->getTileAt(Vec2(tileXleft, i));
			groundTile2 = metaLayer->getTileAt(Vec2(tileXright, i));

			if (groundTile1 || groundTile2) height = (mapSize.height - i) * tileSize;
			break;
		}
	}

	// Move Limit
	if (tileXright == mapSize.width - 1) velocity.x = 0;

	return height;
}


void Platformer::initMap()
{
	// Background
	bgFront = Sprite::create("background.png");
	bgRear = Sprite::create("background.png");
	bgSize = bgFront->getContentSize();


	bgRear->setPosition(bgFront->getPosition() + Vec2(bgSize.width, 0));
	bgOffset = Vec2::ZERO;

	addChild(bgFront, -1);
	addChild(bgRear, -1);



	// Tilemap
	auto tileMap = TMXTiledMap::create("tilemap/Grassmap.tmx");
	tileMap->setAnchorPoint(Vec2(0, 0));

	groundLayer = tileMap->getLayer("Ground Layer");
	metaLayer = tileMap->getLayer("Meta Layer");
	addChild(tileMap, 1);

	// Get Size
	tileSize = (int)(tileMap->getTileSize().width * 1.25);
	mapSize = tileMap->getMapSize();


	// Start Point
	TMXObjectGroup* points = tileMap->getObjectGroup("Point");
	ValueMap startPoint = points->getObject("Start");

	startX = startPoint["x"].asInt();
	startY = startPoint["y"].asInt();



	// End Point (Flag)
	ValueMap endPoint = points->getObject("End");

	endX = endPoint["x"].asInt();
	endY = endPoint["y"].asInt();

	checkpoint = Sprite::create("Checkpoint/FlagHide1.png");


	auto hideAnimation = Animation::create();
	hideAnimation->setDelayPerUnit(0.2f);

	hideAnimation->addSpriteFrameWithFile("Checkpoint/FlagHide1.png");
	hideAnimation->addSpriteFrameWithFile("Checkpoint/FlagHide2.png");
	auto hideAnim = Animate::create(hideAnimation);
	checkpoint->runAction(RepeatForever::create(hideAnim));


	auto checkpointBody = PhysicsBody::createBox(Size(320, 320), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
	checkpointBody->setGravityEnable(false);
	checkpointBody->setContactTestBitmask(0xFFFFFFFF);

	checkpoint->setAnchorPoint(Vec2::ZERO);
	checkpoint->addComponent(checkpointBody);
	checkpoint->setPosition(endX, endY);

	checkpoint->setTag(ITEM_TAG::FINISH);
	addChild(checkpoint);



	// Coin Setting

	TMXObjectGroup* coins = tileMap->getObjectGroup("Coins");
	if (coins) setCoins(coins);

	// Trap Setting

	TMXObjectGroup* traps = tileMap->getObjectGroup("Traps");
	if (traps) setTraps(traps);

	// Item Setting

	TMXObjectGroup* items = tileMap->getObjectGroup("Items");
	if (items) setItems(items);

	// Snake Animation
	auto snakeAnim = Animation::create();
	snakeAnim->setDelayPerUnit(0.08f);
	snakeAnim->addSpriteFrameWithFile("Traps/Snake_0001.png");
	snakeAnim->addSpriteFrameWithFile("Traps/Snake_0002.png");
	snakeAnim->addSpriteFrameWithFile("Traps/Snake_0003.png");
	snakeAnim->addSpriteFrameWithFile("Traps/Snake_0004.png");

	snakeAnimation = Animate::create(snakeAnim);
	snakeAnimation->retain();

}


void Platformer::initUI()
{

	// User Camera
	cam = this->getDefaultCamera();


	// UI Camera
	uiCam = Camera::create();
	uiCam->setCameraFlag(CameraFlag::USER1);
	addChild(uiCam);

	// UI (CameraFlag::USER1)

	// ScoreLabel
	scoreLabel = Label::createWithTTF("0", "fonts/Station.ttf", 50);
	scoreLabel->setTextColor(Color4B::WHITE);
	scoreLabel->enableOutline(Color4B::BLACK, 5);
	scoreLabel->setAnchorPoint(Vec2(1, 0.5f));
	scoreLabel->setPosition(Vec2(_director->getWinSize().width - 20, _director->getWinSize().height - 150));
	scoreLabel->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(scoreLabel);


	// CoinLabel
	auto coin = Sprite::create("Items/coin.png");
	coin->setPosition(Vec2(50, _director->getWinSize().height - 145));
	coin->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(coin);

	coinLabel = Label::createWithTTF("0", "fonts/Station.ttf", 50);
	coinLabel->setTextColor(Color4B::WHITE);
	coinLabel->enableOutline(Color4B::BLACK, 5);
	coinLabel->setAnchorPoint(Vec2(0, 0.5f));

	coinLabel->setPosition(Vec2(85, _director->getWinSize().height - 150));
	coinLabel->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(coinLabel);


	//Healthbar
	healthBar = ui::LoadingBar::create("UI/healthBar.png");
	healthBar->setDirection(ui::LoadingBar::Direction::LEFT);
	healthBar->setPercent(100);

	healthBar->setPosition(Vec2(_director->getWinSize().width / 2, _director->getWinSize().height - 50));
	healthBar->setCameraMask((unsigned short)CameraFlag::USER1);
	healthBar->setPercent(healthBarLength);

	addChild(healthBar);


	// HealthBarBorder
	healthBarOrigin = healthBar->getPosition();
	healthBarWidth = healthBar->getContentSize().width;
	healthBarOrigin.x -= healthBarWidth * 0.5;
	healthBarBorder = Sprite::create("HealthBorder/HealthBorder_0000.png");

	auto borderAnim = Animation::create();
	borderAnim->setDelayPerUnit(0.05f);


	// Border Animation
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0000.png");
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0001.png");
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0002.png");
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0003.png");
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0004.png");
	borderAnim->addSpriteFrameWithFile("HealthBorder/HealthBorder_0005.png");
	auto borderAnimate = Animate::create(borderAnim);
	auto repeat = RepeatForever::create(borderAnimate);
	healthBarBorder->runAction(repeat);

	healthBarBorder->setPosition(healthBarOrigin + Vec2(healthBarWidth, 0));
	healthBarBorder->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(healthBarBorder);

	setHealthBarImmediate(health);

	// ProgressBar
	auto pgBar = Sprite::create("UI/ProgressBar.png");
	pgBar->setPosition(Vec2(_director->getWinSize().width / 2, healthBar->getPosition().y - 50));
	pgBar->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(pgBar);

	// Indicator
	barWidth = pgBar->getContentSize().width;
	progressIndicator = Sprite::create("UI/ProgressIndicator.png");
	progressIndicator->setPosition(pgBar->getPosition() - Vec2(barWidth / 2, 0));
	progressIndicator->setCameraMask((unsigned short)CameraFlag::USER1);
	progressIndicator->setScale(0.5f);

	progressOrigin = progressIndicator->getPosition();
	addChild(progressIndicator);

	// Waker
	waker = Node::create();
	auto wakerPhysicsBody = PhysicsBody::createBox(Size(10, mapSize.height * tileSize), PHYSICSBODY_MATERIAL_DEFAULT, Vec2::ZERO);
	wakerPhysicsBody->setContactTestBitmask(0xFFFFFFFF);
	wakerPhysicsBody->setDynamic(false);

	waker->addComponent(wakerPhysicsBody);
	waker->setPosition(character->getPosition());
	waker->setTag(WAKER_TAG);

	addChild(waker);

	// Hit Effect
	hit = Sprite::create("UI/Hit.png");
	hit->setOpacity(0);
	hit->setAnchorPoint(Vec2::ZERO);
	hit->setPosition(Vec2::ZERO);
	hit->setCameraMask((unsigned short)CameraFlag::USER1);
	addChild(hit);


	// How to play
	auto tutorial = Sprite::create("Tutorial.png");
	tutorial->setAnchorPoint(Vec2::ZERO);
	tutorial->setCameraMask((unsigned short)CameraFlag::USER1);
	tutorial->setOpacity(160);
	tutorial->setScale(0.8f);
	addChild(tutorial);
}

void Platformer::initCharacter()
{
	// Character
	character = Sprite::create("Cookies/Run_0001.png");


	// Animations
	auto runAnim = Animation::create();
	runAnim->setDelayPerUnit(0.05f);
	runAnim->addSpriteFrameWithFile("Cookies/Run_0001.png");
	runAnim->addSpriteFrameWithFile("Cookies/Run_0002.png");
	runAnim->addSpriteFrameWithFile("Cookies/Run_0003.png");
	runAnim->addSpriteFrameWithFile("Cookies/Run_0004.png");
	runAnimation = Animate::create(runAnim);
	runAnimation->retain();


	auto jump1Anim = Animation::create();
	jump1Anim->setDelayPerUnit(1);
	jump1Anim->addSpriteFrameWithFile("Cookies/Jump1.png");
	jump1Animation = Animate::create(jump1Anim);
	jump1Animation->retain();


	auto jump2Anim = Animation::create();
	jump2Anim->setDelayPerUnit(0.07f);
	jump2Anim->addSpriteFrameWithFile("Cookies/Jump2_0001.png");
	jump2Anim->addSpriteFrameWithFile("Cookies/Jump2_0002.png");
	jump2Anim->addSpriteFrameWithFile("Cookies/Jump2_0003.png");
	jump2Anim->addSpriteFrameWithFile("Cookies/Jump2_0004.png");
	jump2Animation = Animate::create(jump2Anim);
	jump2Animation->retain();


	auto slideAnim = Animation::create();
	slideAnim->setDelayPerUnit(1);
	slideAnim->addSpriteFrameWithFile("Cookies/Slide.png");
	slideAnimation = Animate::create(slideAnim);
	slideAnimation->retain();


	character->runAction(RepeatForever::create(runAnimation));
	character->setAnchorPoint(Vec2(0.5, 0));
	character->setPosition(Vec2(startX, startY));


	// Set Velocity and Physics
	velocity = Vec2(VELOCITY, 0);
	character->setTag(CHARACTER_TAG);

	normalBody = PhysicsBody::createBox(Size(characterWidth, characterHeight), PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0, -5));
	normalBody->setGravityEnable(false);
	normalBody->setContactTestBitmask(0xFFFFFFFF);

	character->setPhysicsBody(normalBody);


	addChild(character, 2);
}



// on "init" you need to initialize your instance
bool Platformer::init()
{
    if ( !Scene::init() )
    {
        return false;
    }

	isFinished = false;
	isGameOver = false;
	isPaused = false;
	// Audio Settings
	/*
	audio = SimpleAudioEngine::getInstance();
	audio->preloadEffect("Sound/coin2.wav");
	audio->preloadEffect("Sound/jelly.wav");
	*/


	// Set Game and Character Components
	score = 0;
	coin = 0;
	health = STARTHEALTH;
	
	// Effect
	blink = 0;
	boost = 0;

	// Move Components
	isGround = false;
	isMovingLeft = false;
	isMovingRight = false;
	canJump1 = false;
	canJump2 = false;
	isSliding = false;



	// Init Map and background
	initMap();
	// Init Character
	initCharacter();
	// Init UI
	initUI();


	// Keyboard Listener
	auto keyListener = EventListenerKeyboard::create();
	keyListener->onKeyPressed = CC_CALLBACK_2(Platformer::onKeyPressed, this);
	keyListener->onKeyReleased = CC_CALLBACK_2(Platformer::onKeyReleased, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(keyListener, this);

	
	// Contact (Physics) Listener
	auto contactListener = EventListenerPhysicsContact::create();
	contactListener->onContactBegin = CC_CALLBACK_1(Platformer::onContactBegin, this);
	
	_eventDispatcher->addEventListenerWithSceneGraphPriority(contactListener, this);


	// DO NOT TOUCH HERE
	scheduleUpdate();
	scheduleOnce(schedule_selector(Platformer::setHealthBar), 0.8f);

    return true;
}



// UPDATE

void Platformer::update(float delta)
{
	if (isGameOver) return;
	// Status
	// Scale
	character->setScale(currentScale);

	// Invincible for 2.5 seconds after hit
	// and blink
	if (isInvincible)
	{
		blink += delta;
		if (blink > 0.2f)
		{
			opacityToggle();
			blink = 0;
		}
	}
	
	// Boost effect
	if (isBoost)
	{
		boost += delta;
		if (boost > 0.05f)
		{
			showBoostEffect();
			boost = 0;
		}
	}

	


	// Slide
	if (isGround)
	{
		if (isSliding == true && statusTag != STATUS_TAG::SLIDE)
		{
			character->stopAllActions();
			character->runAction(RepeatForever::create(slideAnimation));

			slideBody = PhysicsBody::createBox(Size(characterWidth, characterHeight * 0.6), PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0, -27));
			slideBody->setGravityEnable(false);
			slideBody->setContactTestBitmask(0xFFFFFFFF);

			character->removeComponent(character->getPhysicsBody());
			character->addComponent(slideBody);

			statusTag = STATUS_TAG::SLIDE;
		}
		else if(isSliding == false && statusTag != STATUS_TAG::RUN)
		{
			character->stopAllActions();
			character->runAction(RepeatForever::create(runAnimation));

			normalBody = PhysicsBody::createBox(Size(characterWidth, characterHeight), PHYSICSBODY_MATERIAL_DEFAULT, Vec2(0, -5));
			normalBody->setGravityEnable(false);
			normalBody->setContactTestBitmask(0xFFFFFFFF);

			character->removeComponent(character->getPhysicsBody());
			character->addComponent(normalBody);

			statusTag = STATUS_TAG::RUN;
		}
	}

	// Enhanced (get bigger)
	if (isEnhanced)
	{
		if (currentScale < 2) currentScale += delta * 2;
		if (currentScale > 2) currentScale = 2;
	}

	else
	{
		if (currentScale > 1) currentScale -= delta * 2;
		if (currentScale < 1) currentScale = 1;
	}


	// Position
	auto position = character->getPosition();

	// When the character falls
	if (position.y <= -150)
	{
		// Hit Effect
		hit->setOpacity(150);
		hit->runAction(FadeOut::create(0.3f));
		gameOver();
	}


	// Tile collision
	// Call getHeight()
	if(!isGameOver) groundHeight = Platformer::getHeight(position);
	

	// Ground Detection
	// Landing
	if (position.y <= groundHeight && velocity.y <= 0 && isGround == false)
	{
		position.y = groundHeight;
		isGround = true;
		canJump1 = canJump2 = true;
	
		if (isEnhanced) Platformer::shakeScreen();
		
		if (statusTag != STATUS_TAG::RUN)
		{
			statusTag = STATUS_TAG::RUN;
			character->stopAllActions();
			character->runAction(RepeatForever::create(runAnimation));
		}
	}
	// Midair
	else if (position.y > groundHeight)
	{
		isGround = false;

		if (statusTag != STATUS_TAG::JUMP1 && statusTag != STATUS_TAG::JUMP2)
		{
			statusTag = STATUS_TAG::JUMP1;
			character->stopAllActions();
			character->runAction(RepeatForever::create(jump1Animation));
		}
	}

	// Change Velocity
	// X
	/*
	if (!isMovingLeft && !isMovingRight) velocity.x = 0;
	else
	{
		if (isMovingLeft && !isMovingRight) velocity.x = -300;
		else if (!isMovingLeft && isMovingRight) velocity.x = 300;
	}
	*/
	// Y
	if (!isGround) velocity.y -= 50;
	else velocity.y = 0;


	// Move
	character->setPosition(position + (velocity * delta));

	// Ground Correction
	if (character->getPositionY() < groundHeight && velocity.y <= 0) character->setPositionY(groundHeight);

	// UI Control
	// Label
	scoreLabel->setString(std::to_string(score));
	coinLabel->setString(std::to_string(coin));
	
	// Healthbar effect
	float len = healthBar->getPercent();
	if (len > healthBarLength) {
		healthBar->setPercent(len -= delta * 2 * DECREASE_AMOUNT);
		healthBarBorder->setPosition(healthBarOrigin + Vec2(len * healthBarWidth * 0.01, 0));
	}
	if (len < healthBarLength)
	{
		healthBar->setPercent(healthBarLength);
		healthBarBorder->setPosition(healthBarOrigin + Vec2(healthBarLength * healthBarWidth * 0.01, 0));
	}




	if (isFinished) return;


	// Don't execute the scripts below when the game has finished

	// Health
	if (health > 0) health -= delta * DECREASE_AMOUNT;
	else {
		health = 0;
		gameOver();
	}
	// Set Camera and background
	cam->setPositionX(position.x + 350);
	character->setRotation(0);


	// Set Background

	bgOffset.x -= delta * velocity.x * 0.2;
	bgFront->setPosition(cam->getPosition() + bgOffset);
	bgRear->setPosition(bgFront->getPosition() + Vec2(bgSize.width, 0));

	if ((bgOffset.x * -1) >= bgSize.width)
	{
		bgOffset.x += bgSize.width;
	}


	// Set Progress
	progressIndicator->setPosition(progressOrigin + Vec2((position.x - startX) / (endX - startX) * barWidth, 0));

	// Set Waker
	waker->setPositionX(position.x + 400);
}




void Platformer::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
{
	if (isGameOver) return;
	switch (keyCode)
	{
	//case EventKeyboard::KeyCode::KEY_LEFT_ARROW:	isMovingLeft = true;  velocity.x = -300; break;
	//case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:	isMovingRight = true; velocity.x = 300; break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:		
		if (canJump1 || canJump2)
		{
			if (canJump1)
			{
				canJump1 = false;

				statusTag = STATUS_TAG::JUMP1;
				character->stopAllActions();

				character->runAction(RepeatForever::create(jump1Animation));
			}

			else if (canJump2)
			{
				canJump2 = false;

				statusTag = STATUS_TAG::JUMP2;
				character->stopAllActions();

				character->runAction(RepeatForever::create(jump2Animation));
			}

			isGround = false;
			velocity.y = JUMPFORCE;
		}
		
		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
		isSliding = true; 
		break;
	}
}

void Platformer::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
{
	switch (keyCode)
	{
	//case EventKeyboard::KeyCode::KEY_LEFT_ARROW:	isMovingLeft = false; break;
	//case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:	isMovingRight = false; break;
	case EventKeyboard::KeyCode::KEY_UP_ARROW:		break;
	case EventKeyboard::KeyCode::KEY_DOWN_ARROW:	
		isSliding = false;  
		break;

	case EventKeyboard::KeyCode::KEY_ESCAPE:
		if (!isPaused)
		{
			isPaused = true;
			_director->getInstance()->pause();
		}
		else
		{
			isPaused = false;
			_director->getInstance()->resume();
		}
		break;
	}
}




bool Platformer::onContactBegin(PhysicsContact &contact)
{
	auto character = contact.getShapeA()->getBody()->getNode();
	auto object = contact.getShapeB()->getBody()->getNode();
	
	if (object->getTag() == CHARACTER_TAG || object->getTag() == WAKER_TAG)
	{
		auto temp = character;
		character = object;
		object = temp;
	}

	// Neither character nor waker
	if (character->getTag() != CHARACTER_TAG && character->getTag() != WAKER_TAG) return false;

	// Waker and wakeable
	if (character->getTag() == WAKER_TAG)
	{
		if (object->getTag() == ITEM_TAG::SNAKE)
		{
			// log("Snake, wake up!");
			object->runAction(Repeat::create(snakeAnimation, 1));
		}
		return false;
	}

	// item or trap?
	switch (object->getTag())
	{
	case ITEM_TAG::COIN:
		score += 499;
		coin += 1;
		object->removeFromParentAndCleanup(true);
		//audio->playEffect("Sound/coin2.wav", false);
		break;

	case ITEM_TAG::GOLDCOIN:
		score += 833;
		coin += 10;
		object->removeFromParentAndCleanup(true);
		//audio->playEffect("Sound/coin2.wav", false);
		break;

	case ITEM_TAG::JELLY:
		score += 1499;
		object->removeFromParentAndCleanup(true);
		//audio->playEffect("Sound/jelly.wav", false);
		break;

	case ITEM_TAG::BEARJELLY:
		score += 3333;
		object->removeFromParentAndCleanup(true);
		//audio->playEffect("Sound/jelly.wav", false);
		break;

	case ITEM_TAG::TRAP:
	case ITEM_TAG::SNAKE:
		if (!isInvincible && !isEnhanced && !isBoost)
		{
			health -= DAMAGE;
			if (health < 0) health = 0;

			isInvincible = true;
			log("Health : %lf", health);

			setHealthBarImmediate(health);
			
			// Hit Effect
			hit->setOpacity(150);
			hit->runAction(FadeOut::create(0.3f));

			scheduleOnce(schedule_selector(Platformer::resetInvincible), 2.5f);
		}
		break;

	case ITEM_TAG::ENHANCE:
		isEnhanced = true;
		resetInvincible(0);
		scheduleOnce(schedule_selector(Platformer::resetEnhanced), 3);
		object->removeFromParentAndCleanup(true);
		break;

	case ITEM_TAG::BOOST:
		isBoost = true;
		velocity.x = 700;
		resetInvincible(0);
		scheduleOnce(schedule_selector(Platformer::resetBoost), 2);
		object->removeFromParentAndCleanup(true);
		break;

	case ITEM_TAG::HEALTH:
		health += 10;
		setHealthBarImmediate(health);
		object->removeFromParentAndCleanup(true);
		break;

	case ITEM_TAG::FINISH:
		isFinished = true;
		finish();
		break;
	}
	
	return false;
}

void Platformer::menuCloseCallback(Ref* pSender)
{
    Director::getInstance()->end();
}


void Platformer::opacityToggle()
{
	if (character->getColor() == Color3B::WHITE)
	{
		character->setColor(Color3B::GRAY);
	}
	else character->setColor(Color3B::WHITE);
}


// Reset Status

void Platformer::resetInvincible(float dt)
{
	isInvincible = false;
	if (character->getColor() != Color3B::WHITE) Platformer::opacityToggle();
}

void Platformer::resetEnhanced(float dt)
{
	isEnhanced = false;
	// Invincible time
	isInvincible = true;
	scheduleOnce(schedule_selector(Platformer::resetInvincible), 2.5f);
}

void Platformer::resetBoost(float dt)
{
	isBoost = false;
	velocity.x = VELOCITY;
	// Invincible time
	isInvincible = true;
	scheduleOnce(schedule_selector(Platformer::resetInvincible), 2.5f);
}


// Healthbar setter
void Platformer::setHealthBar(float dt)
{
	healthBarLength =  (10 + health) / 1.1f;
	scheduleOnce(schedule_selector(Platformer::setHealthBar), 0.8f);
}


// Change healthBar Length and position of the healthBarBorder
void Platformer::setHealthBarImmediate(float health)
{
	healthBar->setPercent((10 + health) / 1.1f);
	healthBarLength = (10 + health) / 1.1f;
	healthBarBorder->setPosition(healthBarOrigin + Vec2(healthBarLength * healthBarWidth * 0.01, 0));
}


// Shake Screen when the enhanced character jumps
void Platformer::shakeScreen()
{
	auto m1 = MoveBy::create(0.05f, Vec2(0, 10));
	auto m2 = MoveBy::create(0.05f, Vec2(0, -10));
	auto m3 = MoveBy::create(0.05f, Vec2(0, 10));
	auto m4 = MoveBy::create(0.05f, Vec2(0, -10));

	auto seq = Sequence::create(m1, m2, m3, m4, nullptr);

	cam->runAction(seq);
}


void Platformer::finish()
{
	// Flag Animation
	auto riseAnimation = Animation::create();
	riseAnimation->setDelayPerUnit(0.1f);
	riseAnimation->addSpriteFrameWithFile("Checkpoint/FlagRise1.png");
	riseAnimation->addSpriteFrameWithFile("Checkpoint/FlagRise2.png");
	riseAnimation->addSpriteFrameWithFile("Checkpoint/FlagRise3.png");
	riseAnimation->addSpriteFrameWithFile("Checkpoint/FlagRise4.png");
	auto riseAnim = Animate::create(riseAnimation);

	auto flagCallback = CallFunc::create(CC_CALLBACK_0(Platformer::flagAnimate, this));
	auto flagAnimSeq = Sequence::create(Repeat::create(riseAnim, 1), flagCallback, nullptr);
	checkpoint->stopAllActions();
	checkpoint->runAction(flagAnimSeq);

	// Result
	scheduleOnce(schedule_selector(Platformer::printResult), 2);
 }


// Flag Animation 2
void Platformer::flagAnimate()
{
	auto flagAnimation = Animation::create();
	flagAnimation->setDelayPerUnit(0.15f);
	flagAnimation->addSpriteFrameWithFile("Checkpoint/Flag1.png");
	flagAnimation->addSpriteFrameWithFile("Checkpoint/Flag2.png");
	auto flagAnim = Animate::create(flagAnimation);
	checkpoint->stopAllActions();
	checkpoint->runAction(RepeatForever::create(flagAnim));
}


// Game Over!
void Platformer::gameOver()
{
	isGameOver = true;
	
	velocity = Vec2::ZERO;

	// Character dead animation
	auto deadAnimation = Animation::create();
	deadAnimation->setDelayPerUnit(0.1f);
	deadAnimation->addSpriteFrameWithFile("Cookies/Die1.png");
	deadAnimation->addSpriteFrameWithFile("Cookies/Die2.png");
	deadAnimation->addSpriteFrameWithFile("Cookies/Die3.png");
	deadAnimation->addSpriteFrameWithFile("Cookies/Die4.png");

	auto deadAnim = Animate::create(deadAnimation);
	character->stopAllActions();
	character->runAction(Repeat::create(deadAnim, 1));


	// Result
	scheduleOnce(schedule_selector(Platformer::printResult), 1);
}


void Platformer::printResult(float dt)
{
	// log("Print result...");

	// Result Panel
	Node* res = Node::create();
	res->setAnchorPoint(Vec2::ZERO);

	// Show Result UI
	auto resultSprite = Sprite::create("UI/Result.png");
	resultSprite->setAnchorPoint(Vec2::ZERO);
	resultSprite->setCameraMask((unsigned short)CameraFlag::USER1);
	res->addChild(resultSprite, 3);

	resultSprite->setScale(0.8f);

	// Score
	auto scoreRes = Label::createWithTTF("0", "fonts/Station.ttf", 50);
	scoreRes->setTextColor(Color4B::WHITE);
	scoreRes->enableOutline(Color4B::BLACK, 5);
	scoreRes->setAnchorPoint(Vec2(0.5f, 0.5f));
	scoreRes->setPosition(Vec2(_director->getWinSize().width / 3 - 10, _director->getWinSize().height - 320));
	scoreRes->setCameraMask((unsigned short)CameraFlag::USER1);

	scoreRes->setString(std::to_string(score));
	res->addChild(scoreRes, 3);

	// Coin
	auto coinRes = Label::createWithTTF("0", "fonts/Station.ttf", 50);
	coinRes->setTextColor(Color4B::WHITE);
	coinRes->enableOutline(Color4B::BLACK, 5);
	coinRes->setAnchorPoint(Vec2(0.5f, 0.5f));
	coinRes->setPosition(Vec2(_director->getWinSize().width * 2 / 3 + 10, _director->getWinSize().height - 320));
	coinRes->setCameraMask((unsigned short)CameraFlag::USER1);

	coinRes->setString(std::to_string(coin));
	res->addChild(coinRes, 3);


	// Buttons
	auto exitButton = ui::Button::create("UI/Button1.png", "UI/Button1Pressed.png", "UI/Button1Highlighted.png");
	exitButton->addClickEventListener(CC_CALLBACK_1(Platformer::exit, this));
	exitButton->setCameraMask((unsigned short)CameraFlag::USER1);
	exitButton->setPosition(coinRes->getPosition() + Vec2(0, -120));
	res->addChild(exitButton, 3);

	auto replayButton = ui::Button::create("UI/Button2.png", "UI/Button2Pressed.png", "UI/Button2Highlighted.png");
	replayButton->addClickEventListener(CC_CALLBACK_1(Platformer::replay, this));
	replayButton->setCameraMask((unsigned short)CameraFlag::USER1);
	replayButton->setPosition(scoreRes->getPosition() + Vec2(0, -120));

	res->addChild(replayButton, 3);

	// Parent Node
	addChild(res);
	res->setCascadeOpacityEnabled(true);
	res->setOpacity(0);
	res->runAction(FadeIn::create(0.4f));

	return;
}

// Replay Scene
void Platformer::replay(Ref* pSender)
{
	_director->getInstance()->replaceScene(Platformer::createScene());
}

// Exit Scene
void Platformer::exit(Ref* pSender)
{
	_director->getInstance()->end();
}

// Show Boost Effect
void Platformer::showBoostEffect()
{
	auto effect = Sprite::create("BoostEffect.png");
	effect->setPosition(character->getPosition() + Vec2(-20, 50));
	effect->setScale(1.5f);
	addChild(effect);

	// Includes FadeOut
	auto seq = Sequence::create(FadeOut::create(1), RemoveSelf::create(), nullptr);
	effect->runAction(seq);
}
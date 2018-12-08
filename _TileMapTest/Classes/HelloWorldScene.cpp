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

#include "HelloWorldScene.h"
#include "SimpleAudioEngine.h"
#include "TiledBodyCreator.h"

USING_NS_CC;

b2World* _world;
TMXTiledMap *tileMap;

Scene* HelloWorld::createScene()
{
    return HelloWorld::create();
}

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf("Depending on how you compiled you might have to add 'Resources/' in front of filenames in HelloWorldScene.cpp\n");
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !Scene::init() )
    {
        return false;
    }

    Director::getInstance()->getScheduler()->scheduleUpdate(HelloWorld::getInstance(), 1, false); //update()
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
  
    //box2d world
    b2Vec2 gravity(0, -9.8);
    _world = new b2World(gravity);
    _world->SetContactListener(this);
    
    
    // Create map
    tileMap = new TMXTiledMap;
    tileMap->initWithTMXFile("TestMap.tmx");
    tileMap->setPosition(Vec2(origin.x, origin.y));
    addChild(tileMap);
 
    TiledBodyCreator::initCollisionMap(tileMap, _world);
    
    
    // Add the car in the TMX position..
    TMXObjectGroup *objectGroup = tileMap->getObjectGroup("Objects");
    auto &objects = objectGroup->getObjects();
    Sprite *car = Sprite::create("car.png");
    
    for (auto& obj : objects)
    {
        ValueMap& dict = obj.asValueMap();
        
        float x = dict["x"].asFloat();
        float y = dict["y"].asFloat();
        
        if(dict["name"].asString() == "myCar")
        {
            car->setScale(0.09, 0.09);
            car->setName("myCar");
            car->setPosition(Vec2(x, y));
            tileMap->addChild(car);
            
            //Box2d car
            b2BodyDef bdCar;
            b2Body *bCar;
            b2PolygonShape psCar;
            
            bdCar.type = b2_staticBody;
            bdCar.position = b2Vec2(x/SCALE_RATIO, y/SCALE_RATIO);
            bdCar.userData = car;
            bCar = _world->CreateBody(&bdCar);
            psCar.SetAsBox((car->getBoundingBox().size.width/2)/SCALE_RATIO, (car->getBoundingBox().size.height/2)/SCALE_RATIO);
            bCar->CreateFixture(&psCar, 0);
            
        }
    }
    
    
    return true;
}


HelloWorld* HelloWorld::getInstance()
{
    static HelloWorld sharedInstance;
    return &sharedInstance;
}

void HelloWorld::update(float dt)
{
    if(_world != NULL)
    {
        Vec2 origin = Director::getInstance()->getVisibleOrigin();
        auto visibleSize = Director::getInstance()->getVisibleSize();
        
        //Estas 2 variables afectan en la colision de los cuerpos
        int velocityIterations = 8;
        int positionIterations = 3;
        
        _world->Step(dt, velocityIterations, positionIterations); //Simulacion de la fisica puesta en world, step avanza un paso la simulacion de todos los objetos
        
       
        for (b2Body* b = _world->GetBodyList(); b != NULL; b = b->GetNext())
        {
            if (b->GetUserData() != NULL) {
                Sprite* s = (Sprite*)b->GetUserData();
                
                if(s->getName().find("myCar") != std::string::npos)
                    b->SetTransform(b2Vec2(s->getPositionX()/SCALE_RATIO, (s->getPositionY() + (visibleSize.height * 0.0035f))/SCALE_RATIO), 0);
                
                s->setPosition(b->GetPosition().x * SCALE_RATIO, b->GetPosition().y * SCALE_RATIO); //Transformo posiciones de metros (box2d) a pixel (coco2d)
            }
        }
    }
}



void HelloWorld::menuCloseCallback(Ref* pSender)
{
    //Close the cocos2d-x game scene and quit the application
    Director::getInstance()->end();

    #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

    //EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);


}

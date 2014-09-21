#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"
#include "../proj.ios_mac/ios/FileOperation.h"
#include "CCUserDefault.h"
USING_NS_CC;
#define wallHight (winSize.y -62)*PTM_RATIO
const int topWallMark = 1 << 0;
const int ballMark = 1<<1;
const int lineMark = 1 << 2;
int space = 15;
const string fruitName[] = {
    "cauliflower","leaf","grapes","cherry","sushi","watermelon","radish", "pumpkin", "banana","corn" };
const string fruitName2[] = {
"peach","pear","orange","kiwi","strawberry","yellowpepper","redpepper","apple","bananas","piceWatermelon"
    
};
//叶子, 苹果, 梨子, 香蕉, 葡萄, 榴莲, 西瓜,菠萝, 草莓, 黄瓜, 莲藕,甘蔗, 樱桃, 果盆 ,水果刀
Scene* HelloWorld::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
//    time_t nowtime;
//    struct tm *timeinfo;
//    time( &nowtime );
//    timeinfo = localtime( &nowtime );
    
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    FileOperation::deletAd();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    winSize = Director::getInstance()->getWinSize();

    monkey = NULL;
    oldMaxPosition.x = winSize.x * 0.7;
    score = 0;
    deleteLinePointTime = 0;
    createLinePointTime = 0;
    oldLine = NULL;
    restart = false;
    maxScore = 0;
    enableHold = true;
    isFallDownDone = true;
    oldHead = NULL;
    batchNode = SpriteBatchNode::create("head3.png");
    this->addChild(batchNode);
        /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create(
                                           "CloseNormal.png",
                                           "CloseSelected.png",
                                           CC_CALLBACK_1(HelloWorld::menuCloseCallback, this));
    
	closeItem->setPosition(Vec2(origin.x + visibleSize.width - closeItem->getContentSize().width/2 ,
                                origin.y + closeItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    /////////////////////////////
    // 3. add your codes below...

    // add a label shows "Hello World"
    // create and initialize a label
    
    title = LabelTTF::create(" Crazy worms", "Arial", 35);
    
    // position the label on the center of the screen
    titleOriglPosition =Vec2(origin.x + visibleSize.width/2,
                             origin.y + visibleSize.height - title->getContentSize().height);
    title->setPosition(titleOriglPosition);

    // add the label as a child to this layer
    this->addChild(title, 1);

    Sprite* _sp = Sprite::create("zhongli.png");
    _sp->setPosition(Vec2(150, winSize.y/2+50));
    _sp->setScale(2);
    _sp->runAction(RepeatForever::create( Sequence::create(EaseSineInOut::create(RotateBy::create(0.4, 30)) ,EaseSineInOut::create(RotateBy::create(0.4, -30)) , NULL)));
    _sp->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2),RemoveSelf::create(), NULL) );

    this->addChild(_sp,-1);
    
    LabelTTF* _label = LabelTTF::create("Tilt the device \n and tap screen \n to throw the fruit ", "Arial", 40);
    this->addChild(_label,-1);
    _label->setPosition(Vec2(200, winSize.y/2-100));
    _label->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2),RemoveSelf::create(), NULL) );

    Sprite * _tap = Sprite::create("tap.png");
    _tap->setPosition(winSize/2- Vec2(-300,300));
    _tap->setScale(0.5);
    this->addChild(_tap,-1);
    _tap->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2),RemoveSelf::create(), NULL) );

    Sprite* _guiji = Sprite::create("fly.png");
    _guiji->setPosition(winSize/2 - Vec2(-100,0));
    _guiji->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2),RemoveSelf::create(), NULL) );
    this->addChild(_guiji, -1);
    //设置地球和绘图
    {
        b2Vec2 gravity;
        gravity.Set(0.0f, -50.0f); //一个向下10单位的向量，作为重力减速度，Box2D中默认的单位是秒和米
        mWorld = new b2World(gravity); // 创建一个有重力加速度的世界
        //m_debugDraw = new GLESDebugDraw(1/PTM_RATIO);   //这里新建一个 debug渲染模块
        //mWorld->SetDebugDraw(m_debugDraw);    //设置
        //uint32 flags = 0;
        //flags += b2Draw::e_shapeBit ;
        //flags += b2Draw::e_centerOfMassBit;   //获取需要显示debugdraw的块
        //flags += b2Draw::e_aabbBit;  //AABB块
        //flags += b2Draw::e_centerOfMassBit; //物体质心
        //flags += b2Draw::e_jointBit;  //关节
        //flags += b2Draw::e_shapeBit;   //形状
        //flags += b2Draw::e_centerOfMassBit;
        //m_debugDraw->SetFlags(flags);   //需要显示那些东西
        //mWorld->SetDebugDraw(m_debugDraw);
        mWorld->SetContactListener(this);
    }
    
    //绘制天花板
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.position.Set(0.0f,(winSize.y -62)*PTM_RATIO);
        topWall = mWorld->CreateBody(&bodyDef);//添加地面
        
        //申请到之后设置物体属性
        {
            b2EdgeShape shape;
            b2FixtureDef fd;
            fd.shape = &shape;
            fd.filter.categoryBits = topWallMark;
            fd.filter.maskBits = ballMark | lineMark;          
            shape.Set(b2Vec2(-winSize.y, 0.0f), b2Vec2(winSize.y*2, 0.0f));
            topWall->CreateFixture(&fd);
 

        }
    }
    
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.position.Set(0.0f,(winSize.y -62)*PTM_RATIO);
        topJointBody = mWorld->CreateBody(&bodyDef);//添加地面
        
        //申请到之后设置物体属性
        {
            b2EdgeShape shape;
            b2FixtureDef fd;
            fd.shape = &shape;
            fd.filter.categoryBits = topWallMark;
            fd.filter.maskBits = ballMark | lineMark;
            shape.Set(b2Vec2(0, 0.0f), b2Vec2(0, 0.0f));
            topJointBody->CreateFixture(&fd);
            
            
        }
    }
    


    const float32 x = topWall->GetPosition().x +10;
    const float32 y = topWall->GetPosition().y-0.5 ;
    int lenth = 22;
    initLine(x, y, lenth);
    oldLine = linePoints[0];

    createMonkey();

    
    {
        Device::setAccelerometerEnabled(true);
        Device::setAccelerometerInterval(1/30.0);
        auto listener = EventListenerAcceleration::create(CC_CALLBACK_2(HelloWorld::onAcceleration,  this));
        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    }
    
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(false);//
        
        listener->onTouchBegan = CC_CALLBACK_2(HelloWorld::onTouchBegan, this);
        listener->onTouchMoved = CC_CALLBACK_2(HelloWorld::onTouchMoved, this);
        listener->onTouchEnded = CC_CALLBACK_2(HelloWorld::onTouchEnded, this);
        
        _eventDispatcher->addEventListenerWithFixedPriority(listener, -10);//先被触摸
        _touchListener = listener;

    }
    
    
    this->scheduleUpdate();
    
    return true;
}
void HelloWorld::initLine(float x, float y, int lenth)
{
    vector<b2Body*> *points = new vector<b2Body*>;
    linePoints.push_back(points);
    
    
    b2CircleShape shape;
    shape.m_radius = 0.5f;
    
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 1.0f;
    fd.friction = 0.3f;
    fd.restitution = 0.5;
    fd.filter.groupIndex = -8;
    //fd.isSensor = true;
    fd.filter.categoryBits = lineMark;
    fd.filter.maskBits = topWallMark | ballMark;
    //b2RevoluteJointDef jd;
    b2DistanceJointDef jd;
    b2Vec2 p1, p2, d;
    //jd.collideConnected = false;
    
    
    b2Body* prevBody = topJointBody;
    
    for (int32 i = 0; i < lenth; ++i)
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set( x ,  y - 0.6 * i-0.1 );//世界坐标
  
        b2Body* body = mWorld->CreateBody(&bd);
        
        UserData* _data = new UserData;
        _data->n = createLinePointTime;
        _data->pointIndex = i;
        Sprite* _sp = NULL;
        _data->sp = _sp;

        if (i == 0) {
            _sp = Sprite::create("tail0.png");
            _sp->setScale(0.7);
            _sp->setAnchorPoint(Vec2(0.5,0.4));
            this->addChild(_sp);
        }
        else if(i == lenth-1)
        {
            _sp = Sprite::create("head0.png");
            _sp->setScale(0.7);
            this->addChild(_sp);
            _sp->setAnchorPoint(Vec2(0.5,0.6));
        }
        else{
            _sp = Sprite::create("head3.png");
            batchNode->addChild(_sp);

        }
        _data->sp = _sp;
        _sp->setPosition(Vec2(body->GetPosition().x / PTM_RATIO, body->GetPosition().y / PTM_RATIO));

        body->SetUserData(_data);
        
        body->CreateFixture(&fd);
        body->SetSleepingAllowed(true);
        //创建有弹力的连接线
        b2DistanceJointDef jd;
        b2Vec2 p1, p2, d;
        
        jd.frequencyHz = -1.0f;
        jd.dampingRatio = 0.0f;
        
        jd.bodyA = prevBody;
        jd.bodyB = body;
        jd.localAnchorA.Set(0, 0);
        if (i ==0) {
            jd.localAnchorA.Set(x, 0);
        }
        jd.localAnchorB.Set(0, 0);
        p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
        p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
        d = p2 - p1;
        jd.length = d.Length();
        mWorld->CreateJoint(&jd);
        
        jd.length = 0;
        jd.localAnchorA.Set(0.0, -0.3);
        if (i ==0) {
            jd.localAnchorA.Set(x, 0);
        }
        jd.localAnchorB.Set(0.0, 0.3);
            
        mWorld->CreateJoint(&jd);

        
        prevBody = body;
        
        points->push_back(body);
        
        
    }

    createTopWall((*points)[0]);
    

    createScore((*points)[0], score);
    
    score++;
    createLinePointTime++;
    
}
void HelloWorld::createScore(b2Body* body, int num)
{
    
    LabelTTF* _label = LabelTTF::create(CCString::createWithFormat("%d" , num )->getCString(),"Arial", 120);
    _label->setScale(0.3);
    _label->setPosition(Vec2(body->GetPosition().x / PTM_RATIO, 100));
    this->addChild(_label);
    scores.push_back(_label);
    

}
bool HelloWorld::randomHalf()
{
    int _r = CCRANDOM_0_1()*5;
    return _r == 0;
}
void HelloWorld::createLine()
{
    b2Body* _temp = getLinePoints(-1, 0);
    int lenth ;
    
    float _y =0;
    const float32 y = wallHight-0.5 - _y ;
    
    if ((*(linePoints.end()-1))->size()<8) {
        if (createLinePointTime<10) {
            space = 9;
            lenth = CCRANDOM_0_1()* 5+ 11;//叶子
        }
        else if(createLinePointTime<20){
            space = 10;
            lenth = CCRANDOM_0_1()* 5+ 12;//苹果
        }
        else if(createLinePointTime<30){
            space = 11;
            lenth = CCRANDOM_0_1()* 5+ 13;//香蕉
        }
        else if(createLinePointTime<40){
            space = 12;
            lenth = CCRANDOM_0_1()* 5+ 14;//
        }
        else if(createLinePointTime<50){
            space = 13;
            lenth = CCRANDOM_0_1()* 5 + 15;
        }
        else if(createLinePointTime<60){
            space = 14;
            lenth = CCRANDOM_0_1()* 5 + 16;
        }
        else if(createLinePointTime<70){
            space = 15;
            lenth = CCRANDOM_0_1()* 5 + 17;
        }
        else if(createLinePointTime<80){
            space = 16;
            lenth = CCRANDOM_0_1()* 5 + 18;
        }
        else if(createLinePointTime<90){
            space = 17;
            lenth = CCRANDOM_0_1()* 5 + 19;
        }
        else if(createLinePointTime<110){
            space = 18;
            lenth = CCRANDOM_0_1()* 5 + 20;
        }
        else if(createLinePointTime<130){
            space = 19;
            lenth = CCRANDOM_0_1()* 5 + 21;
        }
        else if(createLinePointTime<150){
            space = 20;
            lenth = CCRANDOM_0_1()* 5 + 22;
        }
        else if(createLinePointTime<180){
            space = 21;
            lenth = CCRANDOM_0_1()* 5 + 23;
        }
        else
        {
            space = 22;
            lenth = CCRANDOM_0_1()* 5 + 24;
        }

    }
    else
    {
        if (createLinePointTime<10) {
            space = 9;
            lenth = CCRANDOM_0_1()* 11+ 8;//叶子
        }
        else if(createLinePointTime<20){
            space = 10;
            lenth = CCRANDOM_0_1()* 12+ 8;//苹果
        }
        else if(createLinePointTime<30){
            space = 11;
            lenth = CCRANDOM_0_1()* 13+ 8;//香蕉
        }
        else if(createLinePointTime<40){
            space = 12;
            lenth = CCRANDOM_0_1()* 14+ 8;//
        }
        else if(createLinePointTime<50){
            space = 13;
            lenth = CCRANDOM_0_1()* 15 + 8;
        }
        else if(createLinePointTime<60){
            space = 14;
            lenth = CCRANDOM_0_1()* 16 + 8;
        }
        else if(createLinePointTime<70){
            space = 15;
            lenth = CCRANDOM_0_1()* 17 + 8;
        }
        else if(createLinePointTime<80){
            space = 16;
            lenth = CCRANDOM_0_1()* 18 + 8;
        }
        else if(createLinePointTime<90){
            space = 17;
            lenth = CCRANDOM_0_1()* 19 + 8;
        }
        else if(createLinePointTime<110){
            space = 18;
            lenth = CCRANDOM_0_1()* 20 + 8;
        }
        else if(createLinePointTime<130){
            space = 19;
            lenth = CCRANDOM_0_1()* 20 + 8;
        }
        else if(createLinePointTime<150){
            space = 20;
            lenth = CCRANDOM_0_1()* 20 + 8;
        }
        else if(createLinePointTime<180){
            space = 21;
            lenth = CCRANDOM_0_1()* 20 + 8;
        }
        else
        {
            space = 22;
            lenth = CCRANDOM_0_1()* 20 + 8;
        }

    }
    
    initLine(_temp->GetPosition().x + space, y, lenth);
}
void HelloWorld::createMonkey()
{

    rand();
    int _r = (10.0*rand()/(RAND_MAX+1.0));
    string _name ;
    if (1) {
        GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("fruits.plist");
       // GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("fruits2.plist");

         _name = fruitName[_r];
    }
    else
    {
        GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("fruits2.plist");
         _name = fruitName2[_r];
    }
    
    //加载文件中的物体 big_bird
    {
        
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(0,10); //初始位置
        monkey = mWorld->CreateBody(&bodyDef);
        
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        
        
        
        cache->addFixturesToBody(monkey,_name);
        //monkey->GetFixtureList()->SetSensor(true);
        monkey->SetBullet(true);
        
        Sprite* birdSp = Sprite::create(_name + ".png");
        this->addChild(birdSp);
        birdSp->setPosition(Vec2(0,-200));
        
        monkey->SetUserData(birdSp);
        float mass = monkey->GetMass();
        monkey->ApplyLinearImpulse(b2Vec2(300*mass,100 * mass), b2Vec2(0,0), true);
        //bodys.push_back(birdBody);
    }

}
void HelloWorld::createMonkeyJoint(b2Body* body,b2Vec2 point, float length)
{
    //创建连接线
    b2DistanceJointDef jd;
    b2Vec2 p1, p2, d;
    
    jd.frequencyHz = 100.0f;
    jd.dampingRatio = 1.0f;
    
    jd.bodyA = monkey;
    jd.bodyB = body;
    jd.localAnchorA.Set(point.x, point.y);
    jd.localAnchorB.Set(0, 0);
    p1 = jd.bodyA->GetWorldPoint(jd.localAnchorA);
    p2 = jd.bodyB->GetWorldPoint(jd.localAnchorB);
    d = p2 - p1;
    if (length<0) {
        jd.length = d.Length();
    }else{
        jd.length = length;
    }
    
    jd.collideConnected = true;//设置是否碰撞
    mWorld->CreateJoint(&jd);
    oldHead = body;
    enableHold = false;
}
void HelloWorld::createTopWall(b2Body* body)
{
//    //向世界申请一个物体

}
b2Body* HelloWorld::getLinePoints(int indexA, int indexB)
{

    vector<b2Body*> *_temp = indexA == -1? *(linePoints.end()-1):linePoints[indexA];
    return indexB == -1? *(_temp->end()-1) : ((*_temp)[indexB]);
}
void HelloWorld::removeFirstLine()
{
    
    for(auto child: **(linePoints.begin()))
    {
        if (((UserData*) child->GetUserData())->sp) {
            ((UserData*) child->GetUserData())->sp->removeFromParentAndCleanup(true);
        }
        
        delete((UserData*) child->GetUserData());
        mWorld->DestroyBody(child);
    }
    
    
    if (*(linePoints.begin()) == oldLine) {
        oldLine = NULL;
    }
    linePoints.erase(linePoints.begin());
    deleteLinePointTime++;
    
    
}
void HelloWorld::fallDown()
{
    fallDownLineIndex = getCurrentLineIndex();
    fallDownPointIndex = getCurrentPointIndex();
    fallDownLineLenth = (int)linePoints[fallDownLineIndex]->size();
    fallDownLien = linePoints[fallDownLineIndex];
    isFallDownDone = false;
    this->schedule(SEL_SCHEDULE(&HelloWorld::fallDownSchedule), 1/15.0);
}
void HelloWorld::fallDownSchedule(float dt)
{
    
    fallDownPointIndex++;
    if (fallDownPointIndex >= fallDownLineLenth) {
        this->unschedule(SEL_SCHEDULE(&HelloWorld::fallDownSchedule));
        isFallDownDone = true;
        return;
    }
    removeAllMonkeyJoint();
    createMonkeyJoint((*fallDownLien)[fallDownPointIndex], fallDownOffPoint, 0.5f);
}
void HelloWorld::onAcceleration(Acceleration* acc, Event* unused_event)
{
    float lenth = -70;
    float accx = acc->x;
    if (accx > 0.7) {
        accx = 0.7;
    }
    if (accx < -0.7) {
        accx = -0.7;
    }
    float radian = accx * 3.14159265 / 2;
    float x = sin(radian) * lenth;
    float y = cos(radian) * lenth;
    b2Vec2 gravity(-x , y);//重力感应方向的改变，表示向量力的变化
    mWorld->SetGravity(gravity);//这两句表示判断重力的感应方向的转变
    CCLOG("%f, %f, %f", acc->x, acc->y, acc->z);
    
}


void HelloWorld::update(float delta)
{
    int32 velocityIterations = 10;
    int32 positionIterations = 10;
    
    mWorld->Step(delta, velocityIterations, positionIterations);
    //mWorld->ClearForces();
    
    if (!monkey) {
        return;
    }
    b2Vec2 position = monkey->GetPosition();
    float radian = monkey->GetAngle();
    float positionX = position.x/ PTM_RATIO;
    
    if (positionX > oldMaxPosition.x) {
        this->setPositionX(this->getPositionX() -( positionX - oldMaxPosition.x));
        oldMaxPosition.x = positionX;
        title->setPosition(titleOriglPosition-this->getPosition());
        menu->setPosition(-this->getPosition());
        topWall->SetTransform(b2Vec2(position.x,wallHight), 0);
    }
    
    if (getLinePoints(0, 0)->GetPosition().x + 8< - this->getPositionX()*PTM_RATIO) {
        removeFirstLine();
        
    }
//    if (walls[0]->GetPosition().x +5 < - this->getPositionX()*PTM_RATIO) {
//        b2Body* body = walls[0];
//        walls.erase(walls.begin());
//        mWorld->DestroyBody(body);
//    }
   
    for (int i = 0 ;  i < scores.size(); i++) {
        auto child = scores[i];
        if (child->getPositionX() < positionX+50 && child->getPositionX() > positionX-50) {
            child->runAction(Sequence::create(ScaleTo::create(0.2, 1), ScaleTo::create(0.1, 0.3), NULL));
            
            if (maxScore<i+deleteLinePointTime) {
                maxScore = i + deleteLinePointTime;
            }
            break;
        }
    }
   
    if (scores[0]->getPositionX() < - this->getPositionX()) {
        scores[0]->removeFromParentAndCleanup(true);
        scores.erase(scores.begin());
    }
    
    if (getLinePoints(-1, 0)->GetPosition().x < (- this->getPositionX() + winSize.x)*PTM_RATIO ) {
        createLine();
    }
    
    for(auto & parent : linePoints)
    {
        for(auto & child: *parent)
        {
            Sprite* _sp = ((UserData*)(child->GetUserData()))->sp;
            if (_sp == NULL ) {
                break;
            }
            _sp->setPosition(Vec2(child->GetPosition().x / PTM_RATIO, child->GetPosition().y / PTM_RATIO));
            float _radian = child->GetAngle();
             _sp->setRotation(CC_RADIANS_TO_DEGREES(-_radian));
        }
        //b2Body* body =(*(parent->end()-1));
       
    }
    
    Sprite* _sp = (Sprite*)monkey->GetUserData();
    _sp->setPosition(Vec2(position.x / PTM_RATIO, position.y / PTM_RATIO));
    _sp->setRotation(CC_RADIANS_TO_DEGREES(-radian));
    
    if (position.y<-5) {
        if(monkey->GetJointList() == NULL)
        {
            restart = true;
            this->unscheduleUpdate();
            auto _label = LabelTTF::create(CCString::createWithFormat("%d", maxScore)->getCString(), "Arial", 220);
            _label->setPosition(winSize/2 - this->getPosition());
            _label->runAction(RepeatForever::create( Sequence::create(ScaleTo::create(0.1, 0.7),ScaleTo::create(0.8, 1), NULL)));
            this->addChild(_label);
            
            //UserDefault::getInstance()->setStringForKey("string", "value1");
            //std::string ret = UserDefault::getInstance()->getStringForKey("string");
            
            //UserDefault::getInstance()->setIntegerForKey("best111",111);
            int _bestScore = UserDefault::getInstance()->getIntegerForKey("best");
            if (_bestScore < maxScore) {
                _bestScore = maxScore;
                UserDefault::getInstance()->setIntegerForKey("best",_bestScore);
                }
            
            
            //mLevel = CCUserDefault::getInstance()->getIntegerForKey("level",mLevel);
            auto _best = LabelTTF::create(CCString::createWithFormat("Best: %d", _bestScore)->getCString(), "Arial", 50);
            _best->setPosition(winSize/2 - this->getPosition()+ Vec2(300,-50));
            //_best->runAction(RepeatForever::create( Sequence::create(ScaleTo::create(0.1, 0.7),ScaleTo::create(0.8, 1), NULL)));
            this->addChild(_best);
            
            FileOperation::addAd();
        }
        

    }
    
    
    
}
void HelloWorld::removeAllMonkeyJoint()
{
    b2JointEdge* _jointEdge = monkey->GetJointList();
    
    while (_jointEdge) {
        
        mWorld->DestroyJoint(_jointEdge->joint);
        _jointEdge = _jointEdge->next;
    }

}
int HelloWorld::getCurrentLineIndex()
{
    return ((UserData*)(monkey->GetJointList()->other->GetUserData()))->n - deleteLinePointTime;
}
int HelloWorld::getCurrentPointIndex()
{
    return ((UserData*)(monkey->GetJointList()->other->GetUserData()))->pointIndex;
}

void HelloWorld::BeginContact(b2Contact* contact)
{
    CCLOG("reMark");
    //B2_NOT_USED(contact);
//    if(monkey->GetContactList()->contact->GetFixtureA()->GetFilterData().categoryBits == lineMark)
//    {
//        int i = ((UserData*)(monkey->GetJointList()->other->GetUserData()))->n;
//    }
    if (!enableHold) {
        return;
    }

    removeAllMonkeyJoint();
    fallDownOffPoint =contact->GetManifold()->localPoint;
    if((contact->GetFixtureA()->GetFilterData().categoryBits == ballMark)&&(contact->GetFixtureB()->GetFilterData().categoryBits == lineMark))
    {
        b2Body* body = contact->GetFixtureB()->GetBody();
        if (body == oldHead) {
            return;
        }
        createMonkeyJoint(body, fallDownOffPoint);
        
        if (oldLine) {
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        else
        {
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        
    }
    else if ((contact->GetFixtureB()->GetFilterData().categoryBits == ballMark)&&(contact->GetFixtureA()->GetFilterData().categoryBits == lineMark)) {
        b2Body* body = contact->GetFixtureA()->GetBody();
        if (body == oldHead) {
            return;
        }
        createMonkeyJoint(body, fallDownOffPoint);
        
        if (oldLine) {

            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        else
        {
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        
    }
    

    if (enableHold == false && monkey->GetJointList()->other != getLinePoints(getCurrentLineIndex(), -1) ) {//说明有连接并且不是最后一个
        fallDown();
    }
}
void HelloWorld::EndContact(b2Contact* contact)
{
    //B2_NOT_USED(contact);
}
void HelloWorld::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{
    
}
void HelloWorld::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{
    
}
bool HelloWorld::onTouchBegan(Touch* touch, Event* event){
    
    
   // monkey->GetJointList()
    
    this->unschedule(SEL_SCHEDULE(&HelloWorld::fallDownSchedule));
    
    
    
    //
    //    if (!isFallDownDone) {
    //        return false;
    //    }
    
    //    auto touchLocation = touch->getLocation();
    //
    //    auto nodePosition = convertToNodeSpace( touchLocation );//视图层不是当前场景大小, 所以需要转换视图
    //    log("Box2DView::onTouchBegan, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    
    
    
    
    
    
    removeAllMonkeyJoint();
    
    
    
    
    
    enableHold = true;

    
    return true; //MouseDown(b2Vec2(nodePosition.x*PTM_RATIO,nodePosition.y*PTM_RATIO));
    
}
void HelloWorld::onTouchMoved(Touch* touch, Event* event){
    auto touchLocation = touch->getLocation();
    auto nodePosition = convertToNodeSpace( touchLocation );
    
    log("Box2DView::onTouchMoved, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    

}
void HelloWorld::onTouchEnded(Touch* touch, Event* event){
    auto touchLocation = touch->getLocation();
    auto nodePosition = convertToNodeSpace( touchLocation );
    
    log("Box2DView::onTouchEnded, pos: %f,%f -> %f,%f", touchLocation.x, touchLocation.y, nodePosition.x, nodePosition.y);
    
    if (restart == true)
    {
        this->runAction(Sequence::create(CallFunc::create([this](){_eventDispatcher->removeAllEventListeners();
            this->removeFromParentAndCleanup(true);
            auto scene = HelloWorld::createScene();
            Director::getInstance()->replaceScene(scene);}), NULL));
        
    }
   
    
}
void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);
    
    
    ccDrawColor4B(0, 255, 0, 255); //设置颜色绿色
    ccDrawLine(ccp(0,winSize.y -62),ccp(1000000,winSize.y - 62)); //从00点画到100,100
    
    
//    Director* director = Director::getInstance();
//    CCASSERT(nullptr != director, "Director is null when seting matrix stack");
//    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
//    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
//    
//    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
//    //m_test->Step(&settings);
//    mWorld->DrawDebugData();
//    CHECK_GL_ERROR_DEBUG();
//    
//    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
void HelloWorld::menuCloseCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
    return;
#endif
    _eventDispatcher->removeAllEventListeners();
    Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
}

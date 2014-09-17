#include "HelloWorldScene.h"
#include "GB2ShapeCache-x.h"
USING_NS_CC;
const int topWallMark = 1 << 0;
const int ballMark = 1<<1;
const int lineMark = 1 << 2;
int space = 7;
const string fruitName[] = {"banana", "grapes", "leaf"};
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
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init() )
    {
        return false;
    }
    
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
    
    title = LabelTTF::create("TinyUlt", "Arial", 30);
    
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
    this->addChild(_sp);
    
    LabelTTF* _label = LabelTTF::create("Tilt the device \n and tap screen \n to throw the fruit ", "Arial", 40);
    this->addChild(_label);
    _label->setPosition(Vec2(200, winSize.y/2-100));
    _label->runAction(Sequence::create(DelayTime::create(3), FadeOut::create(2),RemoveSelf::create(), NULL) );

    //设置地球和绘图
    {
        b2Vec2 gravity;
        gravity.Set(0.0f, -50.0f); //一个向下10单位的向量，作为重力减速度，Box2D中默认的单位是秒和米
        mWorld = new b2World(gravity); // 创建一个有重力加速度的世界
        m_debugDraw = new GLESDebugDraw(1/PTM_RATIO);   //这里新建一个 debug渲染模块
        //mWorld->SetDebugDraw(m_debugDraw);    //设置
        uint32 flags = 0;
        flags += b2Draw::e_shapeBit ;
        //flags += b2Draw::e_centerOfMassBit;   //获取需要显示debugdraw的块
        //flags += b2Draw::e_aabbBit;  //AABB块
        //flags += b2Draw::e_centerOfMassBit; //物体质心
        flags += b2Draw::e_jointBit;  //关节
        //flags += b2Draw::e_shapeBit;   //形状
        //flags += b2Draw::e_centerOfMassBit;
        m_debugDraw->SetFlags(flags);   //需要显示那些东西
        mWorld->SetDebugDraw(m_debugDraw);
        mWorld->SetContactListener(this);
    }
    
    //绘制天花板
    {
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.position.Set(0.0f,winSize.y * PTM_RATIO -3);
        topWall = mWorld->CreateBody(&bodyDef);//添加地面
        
        //申请到之后设置物体属性
        {
            b2EdgeShape shape;
            b2FixtureDef fd;
            fd.shape = &shape;
            fd.filter.categoryBits = topWallMark;
            fd.filter.maskBits = ballMark | lineMark;
//            shape.Set(b2Vec2(0.0f,0), b2Vec2(0, 0));
//            topWall->CreateFixture(&fd);
//            
            shape.Set(b2Vec2(0.0f,0), b2Vec2(0, 50));
            topWall->CreateFixture(&fd);
//
//            shape.Set(b2Vec2(Director::getInstance()->getWinSize().width * PTM_RATIO,0), b2Vec2(Director::getInstance()->getWinSize().width * PTM_RATIO, 10));
//            topWall->CreateFixture(&fd);
            

        }
    }
    

    //关节旋转
//    for(int j = 0 ; j < 1; j++)
//    {
        const float32 x = topWall->GetPosition().x + 5 + space;
        const float32 y = topWall->GetPosition().y-0.5 ;
        int lenth = 25;
        initLine(x, y, lenth);
        oldLine = linePoints[0];
        //(*(oldLine->end()))->set
 //   }
    //vector<b2Body*> *points = linePoints[0];
    createMonkey();
    //fallDown();
    //createScore(getLinePoints(1, -1)->GetPosition().x, getLinePoints(1, -1)->GetPosition().y, 2);
    
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
    shape.m_radius = 0.25f;
    
    b2FixtureDef fd;
    fd.shape = &shape;
    fd.density = 1.0f;
    fd.friction = 0.2f;
    fd.restitution = 0.5;
    fd.filter.groupIndex = -8;
    //fd.isSensor = true;
    fd.filter.categoryBits = lineMark;
    fd.filter.maskBits = topWallMark | ballMark;
    //b2RevoluteJointDef jd;
    b2DistanceJointDef jd;
    b2Vec2 p1, p2, d;
    //jd.collideConnected = false;
    
    
    b2Body* prevBody = topWall;
    
    for (int32 i = 0; i < lenth; ++i)
    {
        b2BodyDef bd;
        bd.type = b2_dynamicBody;
        bd.position.Set( x ,  y - 0.5 * i );//世界坐标
        
        if(i == lenth-1)
        {
            bd.position.Set( x ,  y - 0.5 * i-0.5 );
        }
        b2Body* body = mWorld->CreateBody(&bd);
        
        UserData* _data = new UserData;
        _data->n = createLinePointTime;
        _data->pointIndex = i;
        Sprite* _sp = NULL;
        if(i != lenth-1)
        {
            _sp = Sprite::create("head.png");
            this->addChild(_sp);
            _data->sp = _sp;
            _sp->setPosition(Vec2(body->GetPosition().x / PTM_RATIO, body->GetPosition().y / PTM_RATIO));
            _sp->setRotation(10);
            _sp->setScale(0.5);
            if (linePoints.size() == 0) {
                fd.filter.maskBits = topWallMark ;
            }
        }
        else
        {
            shape.m_radius = 0.5;
            
            _sp = Sprite::create("head.png");
            this->addChild(_sp);
            _data->sp = _sp;
            _sp->setPosition(Vec2(body->GetPosition().x / PTM_RATIO, body->GetPosition().y / PTM_RATIO));
            _sp->setRotation(10);
            
            if (linePoints.size() == 0) {
                fd.filter.maskBits = topWallMark ;
            }

        }
        body->SetUserData(_data);
        
        body->CreateFixture(&fd);
        
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
        
        prevBody = body;
        
        points->push_back(body);
        
        
    }
//    int _r = CCRANDOM_0_1()*5;
//    if ( _r!= 0) {
        createTopWall((*points)[0]);
//    }
    

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
    
    float _y;
    if (randomHalf()) {
        _y = CCRANDOM_0_1()*5;
    }
    else
    {
        _y = 0;
    }
    const float32 y = topWall->GetPosition().y-0.5 - _y ;
    if (createLinePointTime<10) {
        space = 7;
        lenth = CCRANDOM_0_1()* 10+ 10;//叶子
    }
    else if(createLinePointTime<20){
        space = 8;
        lenth = CCRANDOM_0_1()* 10+ 12;//苹果
    }
    else if(createLinePointTime<30){
        space = 9;
        lenth = CCRANDOM_0_1()* 10+ 15;//香蕉
    }
    else if(createLinePointTime<40){
        space = 10;
        lenth = CCRANDOM_0_1()* 7+ 18;//
    }
    else if(createLinePointTime<50){
        space = 11;
        lenth = CCRANDOM_0_1()* 5 + 20;
    }
    else if(createLinePointTime<60){
        space = 12;
        lenth = CCRANDOM_0_1()* 3 + 23;
    }
    else if(createLinePointTime<70){
        space = 13;
        lenth = CCRANDOM_0_1()* 2 + 24;
    }
    else if(createLinePointTime<80){
        space = 14;
        lenth = CCRANDOM_0_1()* 1 + 25;
    }
    else if(createLinePointTime<90){
        space = 15;
        lenth = CCRANDOM_0_1()* 0 + 26;
    }
    else if(createLinePointTime<110){
        space = 16;
        lenth = CCRANDOM_0_1()* 0 + 26;
    }
    else if(createLinePointTime<130){
        space = 17;
        lenth = CCRANDOM_0_1()* 0 + 26;
    }
    else if(createLinePointTime<150){
        space = 18;
        lenth = CCRANDOM_0_1()* 0 + 26;
    }
    else if(createLinePointTime<180){
        space = 19;
        lenth = CCRANDOM_0_1()* 0 + 26;
    }
    else
    {
        space = 20;
        lenth = CCRANDOM_0_1()* 0 + 27;
    }

    initLine(_temp->GetPosition().x + space, y, lenth);
}
void HelloWorld::createMonkey()
{
    /*
    
    b2BodyDef bodyDef0;
    bodyDef0.type = b2_dynamicBody;
    bodyDef0.position.Set(body->GetPosition().x, body->GetPosition().y  -1.5); //初始位置
    monkey = mWorld->CreateBody(&bodyDef0);
    monkey->SetBullet(true);
    
    //申请到之后设置物体属性
    {
//        b2CircleShape shape;
//        shape.m_radius = 1.0f;

        b2PolygonShape shape;
        shape.SetAsBox(1.0f, 1.0f);
        
        b2FixtureDef fixtureDef;
        fixtureDef.shape = & shape;
        fixtureDef.density = 0.5f;
        fixtureDef.friction = 0.3f;
        fixtureDef.restitution = 0.5f;
        fixtureDef.filter.categoryBits = ballMark;
        fixtureDef.filter.maskBits = topWallMark | lineMark;
        
        monkey->CreateFixture(&fixtureDef);
    }
    
    */
    
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("fruit.plist");
    //加载文件中的物体 big_bird
    {
        
        //向世界申请一个物体
        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(0,0); //初始位置
        monkey = mWorld->CreateBody(&bodyDef);
        
        GB2ShapeCache* cache	=	GB2ShapeCache::sharedGB2ShapeCache();
        
        int _r = CCRANDOM_0_1()*3;
        string _name = fruitName[_r];
        cache->addFixturesToBody(monkey,_name);
        //monkey->GetFixtureList()->SetSensor(true);
        monkey->SetBullet(true);
        
        Sprite* birdSp = Sprite::create(_name + ".png");
        this->addChild(birdSp);
        birdSp->setPosition(Vec2(0,-100));
        
        monkey->SetUserData(birdSp);
        monkey->ApplyLinearImpulse(b2Vec2(30,70), b2Vec2(0,0), true);
        //bodys.push_back(birdBody);
    }

    
    //createMonkeyJoint(body,b2Vec2(0,0));
}
void HelloWorld::createMonkeyJoint(b2Body* body,b2Vec2 point, float length)
{
    //body->SetBullet(true);
   // b2Vec2 anchor(body->GetPosition().x, body->GetPosition().y );//节点位置,世界坐标
    
//    b2RevoluteJointDef jd;
//    jd.Initialize(monkey, body, point);
//    holdJoint = mWorld->CreateJoint(&jd);
    
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
    
    enableHold = false;
}
void HelloWorld::createTopWall(b2Body* body)
{
    //向世界申请一个物体
    b2BodyDef bodyDef;
    bodyDef.position.Set(body->GetPosition().x,winSize.y * PTM_RATIO -3);
    b2Body* wallBody = mWorld->CreateBody(&bodyDef);//添加地面
    
    //申请到之后设置物体属性
    {
        b2EdgeShape shape;
        b2FixtureDef fd;
        fd.shape = &shape;
        fd.filter.categoryBits = topWallMark;
        fd.filter.maskBits = ballMark | lineMark;
        
        shape.Set(b2Vec2(-20,0), b2Vec2(20, 0));
        wallBody->CreateFixture(&fd);
    }
    
    walls.push_back(wallBody);

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
    this->schedule(SEL_SCHEDULE(&HelloWorld::fallDownSchedule), 1/10.0);
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
void HelloWorld::removeFirstScore()
{

}
void HelloWorld::disableLineHold(b2Body* body)
{
//    vector<b2Body*> *_ParentNode = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
//    for(auto child : *_ParentNode)
//    {
//        if(child == *(_ParentNode->end()-1))//&& body != *(_ParentNode->end()-1))
//           {
//               break;
//           }
//        b2Filter _filter;
//        _filter.categoryBits = lineMark;
//        _filter.maskBits = topWallMark;
//        _filter.groupIndex = -8;
//        child->GetFixtureList()->SetFilterData(_filter);
//    }
}
void HelloWorld::enableLineHold()
{
//    for(auto child : *oldLine)
//    {
//        b2Filter _filter;
//        _filter.categoryBits = lineMark;
//        _filter.maskBits = topWallMark | ballMark;
//        _filter.groupIndex = -8;
//        child->GetFixtureList()->SetFilterData(_filter);
//    }
    
}
void HelloWorld::update(float delta)
{
    int32 velocityIterations = 10;
    int32 positionIterations = 10;
    
    mWorld->Step(delta, velocityIterations, positionIterations);
    mWorld->ClearForces();
    
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
    }
    
    if (getLinePoints(0, 0)->GetPosition().x + 5< - this->getPositionX()*PTM_RATIO) {
        removeFirstLine();
        
    }
    if (walls[0]->GetPosition().x +5 < - this->getPositionX()*PTM_RATIO) {
        b2Body* body = walls[0];
        walls.erase(walls.begin());
        mWorld->DestroyBody(body);
    }
   
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
            
        }
        //b2Body* body =(*(parent->end()-1));
       
    }
    
    Sprite* _sp = (Sprite*)monkey->GetUserData();
    _sp->setPosition(Vec2(position.x / PTM_RATIO, position.y / PTM_RATIO));
    _sp->setRotation(CC_RADIANS_TO_DEGREES(-radian));
    
    if (position.y<-10) {
        restart = true;
        this->unscheduleUpdate();
        auto _label = LabelTTF::create(CCString::createWithFormat("%d", maxScore)->getCString(), "Arial", 220);
        _label->setPosition(winSize/2 - this->getPosition());
        _label->runAction(RepeatForever::create( Sequence::create(ScaleTo::create(0.1, 0.7),ScaleTo::create(0.8, 1), NULL)));
        this->addChild(_label);
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
        createMonkeyJoint(body, fallDownOffPoint);
        
        if (oldLine) {
            enableLineHold();
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        else
        {
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        
        disableLineHold(body);
    }
    else if ((contact->GetFixtureB()->GetFilterData().categoryBits == ballMark)&&(contact->GetFixtureA()->GetFilterData().categoryBits == lineMark)) {
        b2Body* body = contact->GetFixtureA()->GetBody();
        
        createMonkeyJoint(body, fallDownOffPoint);
        
        if (oldLine) {
            enableLineHold();
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        else
        {
            oldLine = linePoints[(((UserData*)body->GetUserData())->n)-deleteLinePointTime];
        }
        
        disableLineHold(body);
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
    
    
    this->unschedule(SEL_SCHEDULE(&HelloWorld::fallDownSchedule));
    
    
    if (restart == true)
    {
        this->runAction(Sequence::create(CallFunc::create([this](){_eventDispatcher->removeAllEventListeners();
            this->removeFromParentAndCleanup(true);
            auto scene = HelloWorld::createScene();
            Director::getInstance()->replaceScene(scene);}), NULL));
        
    }
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

    
}
void HelloWorld::draw(Renderer *renderer, const Mat4 &transform, uint32_t flags)
{
    Layer::draw(renderer, transform, flags);
    Director* director = Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when seting matrix stack");
    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, transform);
    
    GL::enableVertexAttribs( cocos2d::GL::VERTEX_ATTRIB_FLAG_POSITION );
    //m_test->Step(&settings);
    mWorld->DrawDebugData();
    CHECK_GL_ERROR_DEBUG();
    
    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
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

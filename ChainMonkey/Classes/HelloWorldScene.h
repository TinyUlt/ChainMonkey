#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include <Box2D/Box2D.h>
#include "GLES-Render.h"
#include <vector>
USING_NS_CC;
using namespace std;
#define PTM_RATIO 0.0315

class UserData
{
public:
    Sprite* sp;
    int n;
};
class HelloWorld : public cocos2d::Layer,public b2ContactListener
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // a selector callback
    void menuCloseCallback(cocos2d::Ref* pSender);
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    void update(float delta);
    void draw(cocos2d::Renderer *renderer, const cocos2d::Mat4 &transform, uint32_t flags);
    // 启动重力感应后，重力方向改变会回调didAccelerate
    void onAcceleration(Acceleration* acc, Event* unused_event);
    // Callbacks for derived classes.
    virtual void BeginContact(b2Contact* contact);
    virtual void EndContact(b2Contact* contact);
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
    bool onTouchBegan(Touch* touch, Event* event);
    void onTouchMoved(Touch* touch, Event* event);
    void onTouchEnded(Touch* touch, Event* event);
    
    //初始化一条线
    void initLine(float x, float y, int lenth);
    //创建一条线 调用initLine
    void createLine();
    //创建水果
    void createMonkey(b2Body* body);
    //创建水果连接线
    void createMonkeyJoint(b2Body* body,b2Vec2 point);
    //创建一个天花板
    void createTopWall(b2Body* body);
    //一半的几率
    bool randomHalf();
    //创建一个分数
    void createScore(b2Body* body, int num);
    //得到线上的点 根据索引, -1 是最后一个
    b2Body* getLinePoints(int indexA, int indexB);
    //删除一天线(只能第一个)
    void removeFirstLine();
    //删除分数
    void removeFirstScore();
    //禁止当前绳子再能被抓住
    void disableLineHold(b2Body* body);
    //允许之前的那条绳子可以被抓住
    void enableLineHold();
    //重新开始游戏
    void restartGameCallback();
    //
    void removeAllMonkeyJoint();
    //
    int getCurrentLineIndex();
    
    void fallDown();
    void fallDownSchedule(float dt);
    b2Vec2 fallDownOffPoint;
    int lineIndex;
    int pointIndex;
    
    
    //重力世界
    b2World* mWorld;
    //物体线
    GLESDebugDraw *m_debugDraw;

    //天花板
    b2Body* topWall;
    //水果
    b2Body* monkey;
    //水果连接线
    //b2Joint* holdJoint;
    //bool enableHold;
    
    //之前的位置(移动用)
    Vec2 oldMaxPosition;
    //屏幕大小
    Vec2 winSize;
    //分数(用来显示底下的分数标签)
    int score;
    //绳子容器
    vector<vector<b2Body*>*> linePoints;
    //绳子贴图容器
    vector<vector<Sprite*>*> linePointSprite;
    //分数标签
    vector<LabelTTF*> scores;
    //墙容器
    vector<b2Body*> walls;
    //删除绳子的次数
    int deleteLinePointTime;
    //添加绳子的次数
    int createLinePointTime;
    //之前抓住的那根绳
    vector<b2Body*> *oldLine;
    //标题
    LabelTTF* title;
    //标题源位置
    Vec2 titleOriglPosition;
    //是否能重来
    bool restart;
    //到过的最大分数
    int maxScore;
    //菜单(退出按钮)
    Menu* menu;
    //Vec2 oldMinPosition;
    bool enableHold;
};

#endif // __HELLOWORLD_SCENE_H__

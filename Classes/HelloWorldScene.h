#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "SelectiveScroll.h"

class HelloWorld : public cocos2d::Layer, public SelectiveScrollDelegate
{
public:
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    CC_SYNTHESIZE(SelectiveScroll *, _scroll, Scroll);
    CC_SYNTHESIZE(LayerColor *, _bgLayer, BGLayer);
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // a selector callback
    
    // implement the "static create()" method manually
    CREATE_FUNC(HelloWorld);
    
    
    // SelectiveScrollDelegate method
    virtual bool isLayerSelected(Node *node, void *sender);
    virtual void selectiveScrollHighlightLayer(bool hi, Node *node, void *sender);
    virtual void selectiveScrollDidSelectLayer(Node *node, void *sender);
};

#endif // __HELLOWORLD_SCENE_H__

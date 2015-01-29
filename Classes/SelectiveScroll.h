//
//  SelectiveScroll.h
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/07.
//
//

#ifndef __SelectiveScroll__SelectiveScroll__
#define __SelectiveScroll__SelectiveScroll__

#include "cocos2d.h"
#include "SelectiveScrollDelegateProtocol.h"

// Top / Bottom bounding effect kind.
typedef enum {
    BoundingEffectNormal,
    BoundingEffectBack,
    BoundingEffectBounce,
    BoundingEffectElastic,
    BoundingEffect_Count
} BoundingEffect;

USING_NS_CC;

class SelectiveScroll : public Layer {
private:
    SelectiveScroll();
    virtual ~SelectiveScroll();
    virtual bool init();
    virtual void adjustScrollSize();
    virtual void clip();
    virtual void onEnterTransitionDidFinish();
    
    bool _clipToBounds;
    bool _clipScrollInteraction;
    bool _enableToScroll;
    
    // color
    Color4B _scrollLayerColor;
    Color4B _bgColor;
    
    // effect
    BoundingEffect _topBoundingEffect;
    BoundingEffect _bottomBoundingEffect;
    
    // scroll calc
    Point _beganTouchPoint;
    Point _lastTouchPoint;
    Point _beganScrollPosition;
    
    // running animation
    Action* _runningAction;
    
    // touch helper
    void* _selectedItem;
    void detectSelectedItem(Vec2 p);
    bool canScrollVertical();
    bool canScrollHorizontal();
    void stopScrollingAnimation();
    Action* fitToAction(Vec2 toPoint);
    Rect absoluteBoundingBox();
    
    void pagingScrollDidEndCallback(Node* node);
    
    // SelectiveScroll (Delegate)
    virtual bool onTouchBegan(Touch *pTouch, Event *pEvent);
    virtual void onTouchMoved(Touch *pTouch, Event *pEvent);
    virtual void onTouchEnded(Touch *pTouch, Event *pEvent);
    
public:
    CREATE_FUNC(SelectiveScroll);
    
    // delegate
    CC_SYNTHESIZE(SelectiveScrollDelegate*, _delegate, Delegate);
    
    // layer
    CC_SYNTHESIZE(Size, _scrollSize, ScrollSize);
    CC_SYNTHESIZE(LayerColor*, _bgLayer, BGLayer);
    CC_SYNTHESIZE(LayerColor*, _scrollLayer, ScrollLayer);
    
    // Effect kind can set individually.
    void setBoundingEffectKind(BoundingEffect effect); // both
    void setBoundingEffectKind(BoundingEffect top, BoundingEffect bottom); // individual
    
    // clipToBounds Setter/Getter
    bool clipToBounds();
    void clipToBounds(bool clip);
    
    // scrollTo
    void scrollToTop();
    void scrollToPoint(Point p);
    void scrollToPointWithAnimation(Point p);
    
    // enable/disable scroll
    bool enableToScroll();
    void enableToScroll(bool enable);
    void setScrollableSize(Size size);
};

#endif /* defined(__SelectiveScroll__SelectiveScroll__) */



//
//  SelectiveScroll.cpp
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/07.
//
//

#include "SelectiveScroll.h"
#include <cmath>

USING_NS_CC;
using namespace std;

#define CANCEL_SELECTION_THRESHOLD  4.0
#define RUNNING_ANIMATION_TAG       1

// MEMO:
// Define Layer order.
typedef enum {
    ZIndex_BGLayer,
    ZIndex_ScrollLayer,
} ZIndex;


SelectiveScroll::SelectiveScroll()
:   _scrollLayerColor(Color4B(255.0, 0.0, 0.0, 0.0))
,   _bgColor(Color4B(0.0, 0.0, 255.0, 255.0 * 0.5))
,   _clipToBounds(true)
,   _clipScrollInteraction(true)
,   _enableToScroll(true)
,   _scrollSize(Vec2(600, 600))
,   _runningAction(NULL)
{}

SelectiveScroll::~SelectiveScroll()
{
    CC_SAFE_RELEASE(_bgLayer);
    CC_SAFE_RELEASE(_scrollLayer);
}


#pragma mark - CCLayer Lifecycle

bool SelectiveScroll::init()
{
    if (!Layer::init()) {
        return false;
    }
    _delegate = NULL;
    
    // this layer
    // (enable single touch)
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = CC_CALLBACK_2(SelectiveScroll::onTouchBegan, this);
    listener->onTouchMoved = CC_CALLBACK_2(SelectiveScroll::onTouchMoved, this);
    listener->onTouchEnded = CC_CALLBACK_2(SelectiveScroll::onTouchEnded, this);
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);

    // (anchor point)
    this->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    this->ignoreAnchorPointForPosition(false);
    
    // bgLayer
    auto size = this->getContentSize();
    _bgLayer = LayerColor::create(_bgColor, size.width, size.height);
    this->addChild(_bgLayer, ZIndex_BGLayer);
    _bgLayer->setAnchorPoint(Vec2::ZERO);
    _bgLayer->retain();
    
    // scrollLayer
    _scrollLayer = LayerColor::create(_scrollLayerColor, _scrollSize.width, _scrollSize.height);
    _scrollLayer->retain();
    this->addChild(_scrollLayer, ZIndex_ScrollLayer);
    _scrollLayer->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    
    return true;
}

void SelectiveScroll::adjustScrollSize()
{
    // resize (if size was changed.)
    auto size = this->getContentSize();
    
    if (_bgLayer && !_bgLayer->getContentSize().equals(size)) {
        _bgLayer->setContentSize(size);
    }
    if (_scrollLayer && !_scrollLayer->getContentSize().equals(_scrollSize)) {
        _scrollLayer->setContentSize(_scrollSize);
    }
}

// MEMO:
// Clip the Layer by using OpenGL functions.
void SelectiveScroll::clip()
{
    if (_clipToBounds) {
        // not visible, then no need to clip
        if (!this->isVisible()) return;
        
        glEnable(GL_SCISSOR_TEST);
        auto bound = this->absoluteBoundingBox();
        auto glview = Director::getInstance()->getOpenGLView();
        glview->setScissorInPoints(bound.origin.x, bound.origin.y, bound.size.width, bound.size.height);
        Layer::visit();
        glDisable(GL_SCISSOR_TEST);
    }
    else {
        Layer::visit();
    }
}

void SelectiveScroll::onEnterTransitionDidFinish()
{
    //    this->scrollToTop();
}


#pragma mark - UI
#pragma mark Scroll

void SelectiveScroll::scrollToTop()
{
    this->scrollToPoint(Vec2(0.0, this->getContentSize().height - _scrollSize.height));
}

void SelectiveScroll::scrollToPoint(Vec2 p)
{
    _scrollLayer->setPosition(p);
}

void SelectiveScroll::scrollToPointWithAnimation(Vec2 p)
{
    this->stopAction((Action*)_runningAction);
    auto moveTo = MoveTo::create(1.0, p);
    _scrollLayer->runAction(EaseInOut::create(moveTo, 2.0));
}


#pragma mark Touch

// returns Eased Action
void* easeAction(MoveTo* moveTo, BoundingEffect effect)
{
    void* action = EaseExponentialOut::create(moveTo);
    switch ((int)effect) {
        case BoundingEffectBack:    action = EaseBackOut::create(moveTo);         break;
        case BoundingEffectBounce:  action = EaseBounceOut::create(moveTo);       break;
        case BoundingEffectElastic: action = EaseElasticOut::create(moveTo);      break;
    }
    return action;
}

// returns bounds on Scene.
Rect SelectiveScroll::absoluteBoundingBox()
{
    Node* parentLayer = this->getParent();
    if (!parentLayer) return this->boundingBox();
    
    Rect bound = this->boundingBox();
    Node* runningScene = Director::getInstance()->getRunningScene();
    int layerDepth = 0;
    const int LayerHierarchyMax = 100.0;
    
    // search for running scene. (to avoid infinity-loop, max 100 hierarchies are supported)
    while (parentLayer != runningScene) {
        if (LayerHierarchyMax <= layerDepth) break;
        
        bound.origin.x += parentLayer->boundingBox().origin.x;
        bound.origin.y += parentLayer->boundingBox().origin.y;
        
        parentLayer = parentLayer->getParent();
        if (!parentLayer) break;
        layerDepth++;
    }
    return bound;
}

bool SelectiveScroll::canScrollVertical()
{
    return this->getContentSize().width < _scrollSize.width;
}

bool SelectiveScroll::canScrollHorizontal() 
{
    return this->getContentSize().height < _scrollSize.height;
}

void SelectiveScroll::pagingScrollDidEndCallback(Node* node)
{
    if (_delegate) {
        _delegate->pagingScrollDidEnd(node, this);
    }
}

void SelectiveScroll::setScrollableSize(Size size)
{
    this->setContentSize(size);
    this->getBGLayer()->setContentSize(size);
    this->getScrollLayer()->setContentSize(size);
}

Action* SelectiveScroll::fitToAction(Vec2 delta)
{
    Vec2 toPoint = Vec2(_scrollLayer->getPositionX() + delta.x, _scrollLayer->getPositionY() + delta.y);
    Vec2 fitPoint = Vec2::ZERO;
    float top = this->getContentSize().height - _scrollSize.height;
    float right = this->getContentSize().width - _scrollSize.width;
    float bottom = 0.0;
    float left = 0.0;
    
    if (this->canScrollHorizontal() && toPoint.y < top) {
        // top left
        if (left < toPoint.x) {
            fitPoint = Vec2(left, top);
        }
        // top right
        else if (toPoint.x < right) {
            fitPoint = Vec2(right, top);
        }
        // top
        else {
            fitPoint = Vec2(toPoint.x, top);
        }
    }
    else if (this->canScrollHorizontal() && bottom < toPoint.y) {
        // bottom right
        if (toPoint.x < right) {
            fitPoint = Vec2(right, bottom);
        }
        // bottom left
        else if (left < toPoint.x) {
            fitPoint = Vec2(left, bottom);
        }
        // bottom
        else {
            fitPoint = Vec2(toPoint.x, bottom);
        }
    }
    // right
    else if (canScrollVertical() && toPoint.x < right) {
        fitPoint = Vec2(right, top);
    }
    // left
    else if (canScrollVertical() && left < toPoint.x) {
        fitPoint = Vec2(left, top);
    }
    else {
        auto byPoint = Vec2((canScrollVertical() ? delta.x : 0.0), (canScrollHorizontal() ? delta.y : 0.0));
        auto toPoint = _scrollLayer->getPosition() + byPoint;
        
        bool isPagingEnabled = false;
        
        // MEMO:
        // invert position.x while searching paging point.
        // (scrollLayer position goes negative when offset goes positive)
        toPoint.x *= -1.0; 
        
        // search paging point
        vector<Vec2> pagingPoints;
        int nearestIndex = 0;
        Vector<Node *> children = _scrollLayer->getChildren();
        for (int i = 0; i < children.size(); i++) {
            Node* node = dynamic_cast<Node*>(children.at(i));
            if (!node) continue;
            
            if (_delegate && _delegate->isPagingPointNode(node, this)) {
                auto p = node->getPosition();
                p.x -= this->getContentSize().width * 0.5;
                pagingPoints.push_back(p);
                
                isPagingEnabled = true;
            }
        }
        if (pagingPoints.size() != 0) {
            // calc distances between "scroll offset" & "paging point".
            vector<float> distances;
            vector<Vec2>::iterator iP = pagingPoints.begin();
            for (; iP != pagingPoints.end(); iP++) {
                Vec2 p = *iP;
                float distance = p.getDistance(toPoint);
                distances.push_back(distance);
            }
            // find the neareset "paging point".
            vector<float>::iterator iD = distances.begin();
            float nearestDistance;
            for (int i = 0; iD != distances.end(); i++, iD++) {
                if (i == 0 || *iD < nearestDistance) {
                    nearestDistance = *iD;
                    nearestIndex = i;
                }
            }
            toPoint = pagingPoints[nearestIndex];
            toPoint = Vec2((canScrollVertical() ? toPoint.x : 0.0), (canScrollHorizontal() ? toPoint.y : 0.0));
        }
        // back to positive.
        toPoint.x *= -1.0;
        
        // will end callback
        if (isPagingEnabled) {
            auto targetNode = dynamic_cast<Node*>(children.at(nearestIndex));
            _delegate->pagingScrollWillEnd(targetNode, this);
        }
        auto moveTo = MoveTo::create(0.87, toPoint);
        Vector<FiniteTimeAction *> actions;
        actions.pushBack(EaseSineOut::create(moveTo));
        
        // did end callback
        if (isPagingEnabled) {
            void* node = children.at(nearestIndex);
            auto didEndCallback = CallFuncN::create([this, node](Node *node){
                this->pagingScrollDidEndCallback(node);
            });
            actions.pushBack(didEndCallback);
        }
        return (Action*)Sequence::create(actions);
    }
    auto moveTo = MoveTo::create(1.0, fitPoint);
    return (Action*)easeAction(moveTo, _topBoundingEffect);
}

void SelectiveScroll::detectSelectedItem(Vec2 p)
{
    bool cancelSelection;
    
    if (this->canScrollHorizontal()) {
        cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.y - _lastTouchPoint.y));
    }
    else {
        cancelSelection = (CANCEL_SELECTION_THRESHOLD < abs(_beganTouchPoint.x - _lastTouchPoint.x));
    }
    
    // convert "view touch point" to "layer touch point"
    auto rect = this->absoluteBoundingBox();
    float winHeight = Director::getInstance()->getWinSize().height;
    auto pointOnThisLayer = Vec2((p.x - rect.origin.x), (p.y - (winHeight - rect.size.height) + rect.origin.y));
    auto pointOnScrollLayer = Vec2::ZERO;
    pointOnScrollLayer.x = -_scrollLayer->getPositionX() + pointOnThisLayer.x;
    pointOnScrollLayer.y = rect.size.height - _scrollLayer->getPositionY() - pointOnThisLayer.y;
    
    // reset first.
    _selectedItem = NULL;
    
    // search selected item
    Vector<Node *> sprites = _scrollLayer->getChildren();
    if (sprites.size() == 0) return;
    
    for (int i = 0; i < sprites.size(); i++) {
        auto node = dynamic_cast<Node*>(sprites.at(i));
        if (!node) return;
        
        bool isSelected = !cancelSelection && (Sprite*)node->boundingBox().containsPoint(pointOnScrollLayer);
        if (isSelected) {
            _selectedItem = (Node*)node;
        }
        // delegate callback 
        if (_delegate) {
            // if selection state hasn't changed, change it.
            if (_delegate->isLayerSelected(node, this) != isSelected) {
                _delegate->selectiveScrollHighlightLayer(isSelected, node, this);
            }
        }
    }
}


// began
bool SelectiveScroll::onTouchBegan(Touch *pTouch, Event *pEvent)
{
    auto p = pTouch->getLocationInView();
    
    // IMPORTANT:
    // convert GL-coordinate to View-coordinate.
    auto winSize =  Director::getInstance()->getWinSize();
    auto viewRect = this->absoluteBoundingBox();
    viewRect.origin.y = winSize.height - viewRect.size.height - viewRect.origin.y;
    
    // is outside of this layer, ignore touch event.
    if (_clipScrollInteraction && !viewRect.containsPoint(p)) {
        return false;
    }
    // stop scrolling animation.
    this->stopAction((Action*)_runningAction);
    
    // save points for calc.
    _lastTouchPoint = p;
    _beganTouchPoint = p;
    _beganScrollPosition = _scrollLayer->getPosition();
    
    this->detectSelectedItem(p);
    
    return true;
}

// moved
void SelectiveScroll::onTouchMoved(Touch *pTouch, Event *pEvent)
{
    // scroll
    auto p = pTouch->getLocationInView();
    
    if (_enableToScroll) {
        if (canScrollVertical()) {
            _scrollLayer->setPositionX(_beganScrollPosition.x - _beganTouchPoint.x + p.x);
        }
        else if (canScrollHorizontal()) {
            _scrollLayer->setPositionY(_beganScrollPosition.y + _beganTouchPoint.y - p.y);
        }
    }
    
    // save last (technically it is previous location)
    _lastTouchPoint = pTouch->getPreviousLocationInView();
    
    this->detectSelectedItem(p);
}

// ended
void SelectiveScroll::onTouchEnded(Touch *pTouch, Event *pEvent)
{
    auto p = pTouch->getLocationInView();
    
    if (_enableToScroll) {
        // speed scroll correction
        auto delta = Vec2(_lastTouchPoint.x - p.x, _lastTouchPoint.y - p.y);
        delta.x *= -(14.0 <= abs(delta.x) ? 12.0 : 0.0);
        delta.y *= (14.0 <= abs(delta.y) ? 12.0 : 0.0);
        
        // fit
        auto fitAction = this->fitToAction(delta);
        _scrollLayer->runAction(fitAction);
        _runningAction = fitAction;
        
        // has selected item.
        if (_selectedItem != NULL) {
            SelectiveScrollDelegate* delegate = this->getDelegate();
            if (delegate) {
                delegate->selectiveScrollHighlightLayer(false, (Layer*)_selectedItem, this);
                delegate->selectiveScrollDidSelectLayer((Layer*)_selectedItem, this);
            }
        }
    }
}


#pragma mark - Setter/Getter

// clipToBounds
bool SelectiveScroll::clipToBounds() { return _clipToBounds; }
void SelectiveScroll::clipToBounds(bool clip) { _clipToBounds = clip; _clipScrollInteraction = clip; }

// bounce effect kind
void SelectiveScroll::setBoundingEffectKind(BoundingEffect effect)
{
    _topBoundingEffect = effect;
    _bottomBoundingEffect = effect;
}
void SelectiveScroll::setBoundingEffectKind(BoundingEffect top, BoundingEffect bottom)
{
    _topBoundingEffect = top;
    _bottomBoundingEffect = bottom;
}

// enable/disable scroll
bool SelectiveScroll::enableToScroll()
{
    return _enableToScroll;
}
void SelectiveScroll::enableToScroll(bool enable)
{
    _enableToScroll = enable;
}


//
//  HorizonScene.cpp
//  SelectiveScroll
//
//  Created by nakagawa.toshitaka on 2013/08/11.
//
//

#include "HorizonScene.h"
#include "HelloWorldScene.h"

#include "HelloWorldScene.h"

//using namespace cocos2d;
USING_NS_CC;

Scene* HorizonScene::createScene()
{
    Scene* scene = Scene::create();
    auto layer = HorizonScene::create();
    scene->addChild(layer);
    
    return scene;
}

bool HorizonScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin = Director::getInstance()->getVisibleOrigin();
    
    // setup BG
    auto color = Color4B(255.0 * 0.9, 255.0 * 0.9, 255.0 * 0.9, 255.0 * 1.0);
    _bgLayer = LayerColor::create(color, visibleSize.width, visibleSize.height);
    _bgLayer->setPosition(Vec2::ZERO);
    this->addChild(_bgLayer);
    
    // setup scroll
    auto size = visibleSize;
    auto scroll = SelectiveScroll::create();
    scroll->setPosition(Vec2(visibleSize.width / 2.0, visibleSize.height / 2.0));
    scroll->setBoundingEffectKind(BoundingEffectBack);
    scroll->setScrollableSize(size);
    scroll->setDelegate(this);
    
    // add labels
    int rowCount = 50;
    float lastX = 0.0;
    for (int i = 0; i < rowCount; i++) {
        auto label = Label::createWithSystemFont("", "Helvetica", 44);
        
        // position
        label->setPosition(Vec2(size.width * (i + 1) * 0.333, size.height * 0.5));
        
        // add
        scroll->getScrollLayer()->addChild(label);
        
        auto p = label->getPosition();
        string title = to_string(rowCount - i) + " (" + to_string((int)p.y) + ")";
        label->setString(title.c_str());
        
        auto rect = label->boundingBox();
        lastX = rect.origin.x + rect.size.width * 2.0;
    }
    scroll->setScrollSize(Size(lastX, size.height));
    scroll->scrollToTop();
    
    this->addChild(scroll);
    
    return true;
}


#pragma mark - DELEGATE
#pragma mark SelectiveScroll (Delegate)

bool HorizonScene::isLayerSelected(Node* node, void* sender)
{
    return ((Label*)node)->getColor() == Color3B::BLACK;
}

void HorizonScene::selectiveScrollHighlightLayer(bool hi, Node* node, void* sender)
{
    auto label = (Label*)node;
    label->setColor(hi ? Color3B::BLACK : Color3B::WHITE);
}

void HorizonScene::selectiveScrollDidSelectLayer(Node* node, void* sender)
{
    Label* label = (Label *)node;
    log("Selected label is %s", label->getString().c_str());
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    auto transition = TransitionPageTurn::create(0.5, HelloWorld::createScene(), true);
    Director::getInstance()->replaceScene(transition);
}


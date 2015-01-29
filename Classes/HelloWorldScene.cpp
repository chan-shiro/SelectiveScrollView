#include "HelloWorldScene.h"
#include "HorizonScene.h"

USING_NS_CC;

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
    if ( !Layer::init() )
    {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getWinSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    
    // Setup BackGround
    auto color = Color4B(0.0, 255.0 * 0.0, 255.0 * 0.0, 255.0 * 0.4);
    _bgLayer = LayerColor::create(color, visibleSize.width, visibleSize.height);
    _bgLayer->setPosition(Vec2::ZERO);
    this->addChild(_bgLayer);
    
    // setup scroll
    int count = BoundingEffect_Count;
    
    for (int i = 0; i < count; i++) {
        
        auto leftMargin = 20.0;
        auto size = Size((visibleSize.width - count*leftMargin) / count, visibleSize.height);
        
        SelectiveScroll* scroll = SelectiveScroll::create();
        scroll->setPosition(Vec2(size.width / 2.0 + (size.width + leftMargin) * i, visibleSize.height / 2.0));
        scroll->setBoundingEffectKind((BoundingEffect)i);
        scroll->setScrollableSize(size);
        scroll->setDelegate(this);
        scroll->clipToBounds(true);
        
        float lastY = 0.0;
        int rowCount = 50;
        for (int ii = 0; ii < rowCount; ii++) {
            auto label = Label::createWithSystemFont("", "Helvetica", 30);
            
            // position
            float margin = 100.0;
            float y = margin + (label->getContentSize().height + margin) * ii;
            label->setPosition(Vec2(size.width / 2.0 , y));
            scroll->getScrollLayer()->addChild(label);
            
            auto p = label->getPosition();
            std::string title = Value(i).asString() + ":" + Value(rowCount - ii).asString() + " (" + Value((int)p.y).asString() + ")";
            label->setString(title);
            
            lastY = p.y + margin;
        }
        scroll->setScrollSize(Size(size.width, lastY));
        scroll->scrollToTop();
        this->addChild(scroll);
    }
    
    return true;
}

bool HelloWorld::isLayerSelected(Node* node, void* sender)
{
    return ((Label*)node)->getColor() == Color3B::BLACK;
}

void HelloWorld::selectiveScrollHighlightLayer(bool hi, Node* node, void* sender)
{
    Label* label = (Label*)node;
    label->setColor(hi ? Color3B::BLACK : Color3B::WHITE);
}

void HelloWorld::selectiveScrollDidSelectLayer(Node* node, void* sender)
{
    Label* label = (Label *)node;
    log("Selected label is %s", label->getString().c_str());
    Director::getInstance()->getEventDispatcher()->removeAllEventListeners();
    TransitionFade* fade = TransitionFade::create(0.5, HorizonScene::createScene(), Color3B::WHITE);
    Director::getInstance()->replaceScene(fade);
}


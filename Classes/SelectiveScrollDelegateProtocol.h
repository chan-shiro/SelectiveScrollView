//
//  SelectiveScrollDelegateProtocol.h
//  SelectiveScroll
//
//  Created by TOSHITAKA NAKAGAWA on 2013/08/08.
//
//

#ifndef __SelectiveScroll__SelectiveScrollDelegateProtocol__
#define __SelectiveScroll__SelectiveScrollDelegateProtocol__

#include "cocos2d.h"

USING_NS_CC;

class SelectiveScrollDelegate {
    
public:
    virtual bool isLayerSelected(Node* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender); return false;}
    virtual void selectiveScrollHighlightLayer(bool hi, Node* node, void* sender) {CC_UNUSED_PARAM(hi), CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    virtual void selectiveScrollDidSelectLayer(Node* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    
    // paging
    virtual bool isPagingPointNode(Node* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender); return false;}
    virtual void pagingScrollWillEnd(Node* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
    virtual void pagingScrollDidEnd(Node* node, void* sender) {CC_UNUSED_PARAM(node), CC_UNUSED_PARAM(sender);}
};


#endif /* defined(__SelectiveScroll__SelectiveScrollDelegateProtocol__) */

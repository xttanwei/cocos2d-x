/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 
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

#include "UIDragPanel.h"
#include "../../System/UILayer.h"

NS_CC_EXT_BEGIN

UIDragPanel::UIDragPanel()
: m_pInnerContainer(NULL)
, m_bTouchPressed(false)
, m_bTouchMoved(false)
, m_bTouchReleased(false)
, m_bTouchCanceld(false)
, m_touchStartNodeSpace(Point::ZERO)
, m_touchStartWorldSpace(Point::ZERO)
, m_touchEndWorldSpace(Point::ZERO)
, m_fSlidTime(0.0f)
, m_eMoveType(DRAGPANEL_MOVE_TYPE_AUTOMOVE)
, m_fAutoMoveDuration(0.5f)
, m_fAutoMoveEaseRate(2.0f)
, m_eBerthDirection(DRAGPANEL_BERTH_DIR_NONE)
, m_pBerthToLeftListener(NULL)
, m_pfnBerthToLeftSelector(NULL)
, m_pBerthToRightListener(NULL)
, m_pfnBerthToRightSelector(NULL)
, m_pBerthToTopListener(NULL)
, m_pfnBerthToTopSelector(NULL)
, m_pBerthToBottomListener(NULL)
, m_pfnBerthToBottomSelector(NULL)
, m_pBerthToLeftBottomListener(NULL)
, m_pfnBerthToLeftBottomSelector(NULL)
, m_pBerthToLeftTopListener(NULL)
, m_pfnBerthToLeftTopSelector(NULL)
, m_pBerthToRightBottomListener(NULL)
, m_pfnBerthToRightBottomSelector(NULL)
, m_pBerthToRightTopListener(NULL)
, m_pfnBerthToRightTopSelector(NULL)
, m_bBounceEnable(false)
, m_eBounceDirection(DRAGPANEL_BOUNCE_DIR_NONE)
, m_fBounceDuration(0.5f)
, m_fBounceEaseRate(2.0f)
, m_pBounceOverListener(NULL)
, m_pfnBounceOverSelector(NULL)
, m_pBounceToLeftBottomListener(NULL)
, m_pfnBounceToLeftBottomSelector(NULL)
, m_pBounceToLeftTopListener(NULL)
, m_pfnBounceToLeftTopSelector(NULL)
, m_pBounceToRightBottomListener(NULL)
, m_pfnBounceToRightBottomSelector(NULL)
, m_pBounceToRightTopListener(NULL)
, m_pfnBounceToRightTopSelector(NULL)
, m_pBounceToLeftListener(NULL)
, m_pfnBounceToLeftSelector(NULL)
, m_pBounceToTopListener(NULL)
, m_pfnBounceToTopSelector(NULL)
, m_pBounceToRightListener(NULL)
, m_pfnBounceToRightSelector(NULL)
, m_pBounceToBottomListener(NULL)
, m_pfnBounceToBottomSelector(NULL)
, m_bRunningAction(false)
, m_nActionType(0)
, m_pActionWidget(NULL)
, m_fDuration(0.0f)
, m_elapsed(0.0f)
, m_bFirstTick(false)
, m_positionDelta(Point::ZERO)
, m_startPosition(Point::ZERO)
, m_previousPosition(Point::ZERO)
, m_endPosition(Point::ZERO)
{
    
}

UIDragPanel::~UIDragPanel()
{
    
}

UIDragPanel* UIDragPanel::create()
{
    UIDragPanel* widget = new UIDragPanel();
    if (widget && widget->init())
    {
        widget->autorelease();
        return widget;
    }
    CC_SAFE_DELETE(widget);
    return NULL;
}

bool UIDragPanel::init()
{
    if (Layout::init())
    {
        setUpdateEnabled(true);
        setTouchEnabled(true);
        setClippingEnabled(true);
        return true;
    }
    return false;
}

void UIDragPanel::initRenderer()
{
    Layout::initRenderer();
    
    m_pInnerContainer = Layout::create();
    Layout::addChild(m_pInnerContainer);

}

void UIDragPanel::releaseResoures()
{
    setUpdateEnabled(false);
    removeAllChildren();
    _renderer->removeAllChildrenWithCleanup(true);
    _renderer->removeFromParentAndCleanup(true);
    _renderer->release();
    
    Layout::removeChild(m_pInnerContainer);
    
    _children->release();
}

bool UIDragPanel::onTouchBegan(const Point &touchPoint)
{
    bool pass = Layout::onTouchBegan(touchPoint);
    handlePressLogic(touchPoint);
    return pass;
}

void UIDragPanel::onTouchMoved(const Point &touchPoint)
{
    Layout::onTouchMoved(touchPoint);
    handleMoveLogic(touchPoint);
}

void UIDragPanel::onTouchEnded(const Point &touchPoint)
{
    Layout::onTouchEnded(touchPoint);
    handleReleaseLogic(touchPoint);
}

void UIDragPanel::onTouchCancelled(const Point &touchPoint)
{
    Layout::onTouchCancelled(touchPoint);
}

void UIDragPanel::onTouchLongClicked(const Point &touchPoint)
{
    
}

void UIDragPanel::update(float dt)
{
    // widget action
    if (m_bRunningAction)
    {
        if (actionIsDone())
        {
            actionDone();
            actionStop();
        }
        else
        {
            actionStep(dt);
        }
    }
    
    recordSlidTime(dt);
}

bool UIDragPanel::addChild(UIWidget *widget)
{
    m_pInnerContainer->addChild(widget);
    return true;
}

bool UIDragPanel::removeChild(UIWidget *child)
{
    bool value = false;
    if (m_pInnerContainer->removeChild(child))
    {
        value = true;
    }
    
    return value;
}

void UIDragPanel::removeAllChildren()
{
    m_pInnerContainer->removeAllChildren();
}

Array* UIDragPanel::getChildren()
{
    return m_pInnerContainer->getChildren();
}

void UIDragPanel::onSizeChanged()
{
    Layout::onSizeChanged();
    Size innerSize = m_pInnerContainer->getSize();
    float orginInnerSizeWidth = innerSize.width;
    float orginInnerSizeHeight = innerSize.height;
    float innerSizeWidth = MAX(orginInnerSizeWidth, _size.width);
    float innerSizeHeight = MAX(orginInnerSizeHeight, _size.height);
    m_pInnerContainer->setSize(Size(innerSizeWidth, innerSizeHeight));
}

const Size& UIDragPanel::getInnerContainerSize() const
{
	return m_pInnerContainer->getContentSize();
}

void UIDragPanel::setInnerContainerSize(const cocos2d::Size &size)
{
    float innerSizeWidth = _size.width;
    float innerSizeHeight = _size.height;
    if (size.width < _size.width)
    {
        CCLOG("Inner width <= scrollview width, it will be force sized!");
    }
    else
    {
        innerSizeWidth = size.width;
    }
    if (size.height < _size.height)
    {
        CCLOG("Inner height <= scrollview height, it will be force sized!");
    }
    else
    {
        innerSizeHeight = size.height;
    }
    m_pInnerContainer->setSize(Size(innerSizeWidth, innerSizeHeight));
    m_pInnerContainer->setPosition(Point(0, _size.height - m_pInnerContainer->getSize().height));
}

const Point& UIDragPanel::getInnerContainerPosition() const
{
    return m_pInnerContainer->getPosition();
}

void UIDragPanel::setInnerContainerPosition(const Point &point, bool animated)
{
    Point delta = point - m_pInnerContainer->getPosition();

//    Point delta = ccpSub(point, m_pInnerContainer->getPosition());
    setInnerContainerOffset(delta, animated);
}

void UIDragPanel::setInnerContainerOffset(const Point &offset, bool animated)
{
    if (animated)
    {
        Point delta = offset;
        
        if (checkToBoundaryWithDeltaPosition(delta))
        {
            delta = calculateToBoundaryDeltaPosition(delta);
        }
        actionStartWithWidget(m_pInnerContainer);
        moveByWithDuration(m_fAutoMoveDuration, delta);
    }
    else
    {
        setInnerContainerOffset(offset);
    }
}

void UIDragPanel::setInnerContainerOffset(const Point &offset)
{
    Point delta = offset;
    
    if (checkToBoundaryWithDeltaPosition(delta))
    {
        delta = calculateToBoundaryDeltaPosition(delta);
    }
    moveWithDelta(delta);
    if (checkBerth())
    {
        berthEvent();
    }
}


void UIDragPanel::handlePressLogic(const Point &touchPoint)
{
    // check inner rect < drag panel rect
    if (checkContainInnerRect())
    {
        m_bTouchPressed = false;
        return;
    }        
    
    m_bTouchPressed = true;
    m_bTouchMoved = false;
    m_bTouchReleased = false;
    m_bTouchCanceld = false;
    
    if (m_bRunningAction)
    {
        switch (m_eMoveType)
        {
            case DRAGPANEL_MOVE_TYPE_AUTOMOVE:
                stopAutoMove();
                actionStop();
                break;
                
            case DRAGPANEL_MOVE_TYPE_BOUNCE:
                m_bTouchPressed = false;
                break;
                
            default:
                break;
        }
    }
    
    Point nsp = _renderer->convertToNodeSpace(touchPoint);
    m_touchStartNodeSpace = nsp;
    
    m_touchStartWorldSpace = touchPoint;    
}

void UIDragPanel::handleMoveLogic(const Point &touchPoint)
{
    if (!m_bTouchPressed)
    {
        return;
    }
    
    // check touch out of drag panel boundary
    if (m_bTouchCanceld)
    {
        return;
    }
        
    m_bTouchMoved = true;
    
    Point nsp = _renderer->convertToNodeSpace(touchPoint);
    Point delta = nsp - m_touchStartNodeSpace;
//    Point delta = ccpSub(nsp, m_touchStartNodeSpace);
    m_touchStartNodeSpace = nsp;
    
    // reset berth dir to none
    if (!m_bBounceEnable)
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_NONE;
    }
    
    // check will berth (bounce disable)
    if (!m_bBounceEnable)
    {
        if (checkToBoundaryWithDeltaPosition(delta))
        {
            delta = calculateToBoundaryDeltaPosition(delta);
        }                        
    }
    // move
    moveWithDelta(delta);
    // check bounce or berth
    if (m_bBounceEnable)
    {
        // bounce
        if (!hitTest(touchPoint))
        {
            m_bTouchMoved = false;
            
            if (checkNeedBounce())
            {
                m_bTouchCanceld = true;
                startBounce();
            }
        }
    }
    else
    {
        // berth
        if (checkBerth())
        {
            berthEvent();
        }
    }
}

void UIDragPanel::handleReleaseLogic(const Point &touchPoint)
{
    if (!m_bTouchPressed)
    {
        return;
    }
    
    m_bTouchPressed = false;
    m_bTouchMoved = false;
    m_bTouchReleased = true;
    m_bTouchCanceld = false;
    
    // check touch out of drag panel boundary
    if (m_bTouchCanceld)
    {
        return;
    }
    
    if (hitTest(touchPoint))
    {
        m_touchEndWorldSpace = touchPoint;
        startAutoMove();
    }
}

void UIDragPanel::checkChildInfo(int handleState, UIWidget *sender, const Point &touchPoint)
{
    interceptTouchEvent(handleState, sender, touchPoint);
}

void UIDragPanel::interceptTouchEvent(int handleState, UIWidget *sender, const Point &touchPoint)
{
    switch (handleState)
    {
        case 0:
            handlePressLogic(touchPoint);
            break;
            
        case 1:
        {
//            float offset = ccpDistance(sender->getTouchStartPos(), touchPoint);
            float offset = sender->getTouchStartPos().getDistance(touchPoint);
            if (offset > 5.0)
            {
                sender->setFocused(false);
                handleMoveLogic(touchPoint);
            }
        }
            break;
            
        case 2:
            handleReleaseLogic(touchPoint);
            break;
            
        case 3:
            break;
    }
}

void UIDragPanel::recordSlidTime(float dt)
{
    if (m_bTouchPressed)
    {
        m_fSlidTime += dt;
    }
}

// check if dragpanel rect contain inner rect
bool UIDragPanel::checkContainInnerRect()
{
    float width = _size.width;
    float height = _size.height;
    float innerWidth = m_pInnerContainer->getSize().width;
    float innerHeight = m_pInnerContainer->getSize().height;
    
    if (innerWidth <= width && innerHeight <= height)
    {
        return true;
    }
    
    return false;
}

// move
void UIDragPanel::moveWithDelta(const Point &delta)
{
    Point newPos = m_pInnerContainer->getPosition() + delta;
//    Point newPos = ccpAdd(m_pInnerContainer->getPosition(), delta);
    m_pInnerContainer->setPosition(newPos);
}

// auto move
void UIDragPanel::autoMove()
{
    if (m_bBounceEnable)
    {
        if (checkNeedBounce())
        {
            stopAutoMove();
            startBounce();
        }
    }
}

void UIDragPanel::autoMoveOver()
{
    stopAutoMove();
    
    if (checkBerth())
    {        
        berthEvent();
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_NONE;
    }
}

void UIDragPanel::startAutoMove()
{    
    m_eMoveType = DRAGPANEL_MOVE_TYPE_AUTOMOVE;
    
    actionStop();

    Point delta = m_touchEndWorldSpace - m_touchStartWorldSpace;
//    Point delta = ccpSub(m_touchEndWorldSpace, m_touchStartWorldSpace);
    delta.x /= m_fSlidTime * 60;
    delta.y /= m_fSlidTime * 60;
    m_fSlidTime = 0.0;
    
    // bounceEnable is disable
    if (!m_bBounceEnable)
    {
        if (checkToBoundaryWithDeltaPosition(delta))
        {
            delta = calculateToBoundaryDeltaPosition(delta);
        }
    }
    actionStartWithWidget(m_pInnerContainer);
    moveByWithDuration(m_fAutoMoveDuration, delta);
}

void UIDragPanel::stopAutoMove()
{
    m_eMoveType = DRAGPANEL_MOVE_TYPE_NONE;
}

void UIDragPanel::setAutoMoveDuration(float duration)
{
    m_fAutoMoveDuration = duration;
}

void UIDragPanel::setAutoMoveEaseRate(float rate)
{
    m_fAutoMoveEaseRate = rate;
}

// berth

// check if move to boundary

bool UIDragPanel::checkToBoundaryWithDeltaPosition(const Point&  delta)
{
    float innerLeft = m_pInnerContainer->getLeftInParent();
    float innerTop = m_pInnerContainer->getTopInParent();
    float innerRight = m_pInnerContainer->getRightInParent();
    float innerBottom = m_pInnerContainer->getBottomInParent();
    
    float left = 0;
    float top = _size.height;
    float right = _size.width;
    float bottom = 0;
    
    bool toLeftBottom = false;
    bool toLeftTop = false;
    bool toRightBottom = false;
    bool toRightTop = false;
    bool toLeft = false;
    bool toRight = false;
    bool toTop = false;
    bool toBottom = false;
    
    if (innerLeft + delta.x > left && innerBottom + delta.y > bottom) // left bottom
    {
        toLeftBottom = true;
    }
    else if (innerLeft + delta.x > left && innerTop + delta.y < top) // left top
    {
        toLeftTop = true;
    }
    else if (innerRight + delta.x < right && innerBottom + delta.y > bottom) // right bottom
    {
        toRightBottom = true;
    }
    else if (innerRight + delta.x < right && innerTop + delta.y < top) // right top
    {
        toRightTop = true;
    }
    else if (innerLeft + delta.x > left) // left
    {
        toLeft = true;
    }
    else if (innerRight + delta.x < right) // right
    {
        toRight = true;
    }
    else if (innerTop + delta.y < top) // top
    {
        toTop = true;
    }
    else if (innerBottom + delta.y > bottom) // bottom
    {
        toBottom = true;
    }
    
    if (toLeft || toTop || toRight || toBottom
        || toLeftBottom || toLeftTop || toRightBottom || toRightTop)
    {
        return true;
    }
    
    return false;
}

Point UIDragPanel::calculateToBoundaryDeltaPosition(const Point& paramDelta)
{
    float innerLeft = m_pInnerContainer->getLeftInParent();
    float innerTop = m_pInnerContainer->getTopInParent();
    float innerRight = m_pInnerContainer->getRightInParent();
    float innerBottom = m_pInnerContainer->getBottomInParent();
    
    float left = 0;
    float top = _size.height;
    float right = _size.width;
    float bottom = 0;
    
    Point delta = paramDelta;
    
    if (innerLeft + delta.x > left && innerBottom + delta.y > bottom) // left bottom
    {
        delta.x = left - innerLeft;
        delta.y = bottom - innerBottom;
    }
    else if (innerLeft + delta.x > left && innerTop + delta.y < top) // left top
    {
        delta.x = left - innerLeft;
        delta.y = top - innerTop;
    }
    else if (innerRight + delta.x < right && innerBottom + delta.y > bottom) // right bottom
    {
        delta.x = right - innerRight;
        delta.y = bottom - innerBottom;
    }
    else if (innerRight + delta.x < right && innerTop + delta.y < top) // right bottom
    {
        delta.x = right - innerRight;
        delta.y = top - innerTop;
    }
    else if (innerLeft + delta.x > left) // left
    {
        delta.x = left - innerLeft;
    }
    else if (innerRight + delta.x < right) // right
    {
        delta.x = right - innerRight;
    }
    else if (innerTop + delta.y < top) // top
    {
        delta.y = top - innerTop;
    }
    else if (innerBottom + delta.y > bottom) // bottom
    {
        delta.y = bottom - innerBottom;
    }
    
    return delta;
}

bool UIDragPanel::isBerth()
{
    return m_eBerthDirection != DRAGPANEL_BERTH_DIR_NONE;
}

// check berth
bool UIDragPanel::checkBerth()
{
    float innerLeft = m_pInnerContainer->getLeftInParent();
    float innerTop = m_pInnerContainer->getTopInParent();
    float innerRight = m_pInnerContainer->getRightInParent();
    float innerBottom = m_pInnerContainer->getBottomInParent();
    
    float left = 0;
    float top = _size.height;
    float right = _size.width;
    float bottom = 0;
    
    if (innerLeft == left && innerBottom == bottom) // left bottom
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_LEFTBOTTOM;
    }
    else if (innerLeft == left && innerTop == top) // left top
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_LFETTOP;
    }
    else if (innerRight == right && innerBottom == bottom) // right bottom
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_RIGHTBOTTOM;
    }
    else if (innerRight == right && innerTop == top) // right top
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_RIGHTTOP;
    }
    else if (innerLeft == left) // left
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_LEFT;
    }
    else if (innerRight == right) // right
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_RIGHT;
    }
    else if (innerTop == top) // top
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_TOP;
    }
    else if (innerBottom == bottom) // bottom
    {
        m_eBerthDirection = DRAGPANEL_BERTH_DIR_BOTTOM;
    }
    
    if (m_eBerthDirection != DRAGPANEL_BERTH_DIR_NONE)
    {
        return true;
    }    
    
    return false;
}

void UIDragPanel::berthEvent()
{
    switch (m_eBerthDirection)
    {
        case DRAGPANEL_BERTH_DIR_LEFTBOTTOM:
            berthToLeftBottomEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_LFETTOP:
            berthToLeftTopEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_RIGHTBOTTOM:
            berthToRightBottomEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_RIGHTTOP:
            berthToRightTopEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_LEFT:
            berthToLeftEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_TOP:
            berthToTopEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_RIGHT:
            berthToRightEvent();
            break;
            
        case DRAGPANEL_BERTH_DIR_BOTTOM:
            berthToBottomEvent();
            break;
            
        default:
            break;
    }
}

void UIDragPanel::berthToLeftBottomEvent()
{
    if (m_pBerthToLeftBottomListener && m_pfnBerthToLeftBottomSelector)
    {
        (m_pBerthToLeftBottomListener->*m_pfnBerthToLeftBottomSelector)(this);
    }
}

void UIDragPanel::berthToLeftTopEvent()
{
    if (m_pBerthToLeftTopListener && m_pfnBerthToLeftTopSelector)
    {
        (m_pBerthToLeftTopListener->*m_pfnBerthToLeftTopSelector)(this);
    }
}

void UIDragPanel::berthToRightBottomEvent()
{
    if (m_pBerthToRightBottomListener && m_pfnBerthToRightBottomSelector)
    {
        (m_pBerthToRightBottomListener->*m_pfnBerthToRightBottomSelector)(this);
    }
}

void UIDragPanel::berthToRightTopEvent()
{
    if (m_pBerthToRightTopListener && m_pfnBerthToRightTopSelector)
    {
        (m_pBerthToRightTopListener->*m_pfnBerthToRightTopSelector)(this);
    }
}

void UIDragPanel::berthToLeftEvent()
{
    if (m_pBerthToLeftListener && m_pfnBerthToLeftSelector)
    {
        (m_pBerthToLeftListener->*m_pfnBerthToLeftSelector)(this);
    }
}

void UIDragPanel::berthToTopEvent()
{
    if (m_pBerthToTopListener && m_pfnBerthToTopSelector)
    {
        (m_pBerthToTopListener->*m_pfnBerthToTopSelector)(this);
    }
}

void UIDragPanel::berthToRightEvent()
{
    if (m_pBerthToRightListener && m_pfnBerthToRightSelector)
    {
        (m_pBerthToRightListener->*m_pfnBerthToRightSelector)(this);
    }
}

void UIDragPanel::berthToBottomEvent()
{
    if (m_pBerthToBottomListener && m_pfnBerthToBottomSelector)
    {
        (m_pBerthToBottomListener->*m_pfnBerthToBottomSelector)(this);
    }
}

void UIDragPanel::addBerthToLeftBottomEvent(Object *target, SEL_DragPanelBerthToLeftBottomEvent selector)
{
    m_pBerthToLeftBottomListener = target;
    m_pfnBerthToLeftBottomSelector = selector;
}

void UIDragPanel::addBerthToLeftTopEvent(Object *target, SEL_DragPanelBerthToLeftTopEvent selector)
{
    m_pBerthToLeftTopListener = target;
    m_pfnBerthToLeftTopSelector = selector;
}

void UIDragPanel::addBerthToRightBottomEvent(Object *target, SEL_DragPanelBerthToRightBottomEvent selector)
{
    m_pBerthToRightBottomListener = target;
    m_pfnBerthToRightBottomSelector = selector;
}

void UIDragPanel::addBerthToRightTopEvent(Object *target, SEL_DragPanelBerthToRightTopEvent selector)
{
    m_pBerthToRightTopListener = target;
    m_pfnBerthToRightTopSelector = selector;
}

void UIDragPanel::addBerthToLeftEvent(Object *target, SEL_DragPanelBerthToLeftEvent selector)
{
    m_pBerthToLeftListener = target;
    m_pfnBerthToLeftSelector = selector;
}

void UIDragPanel::addBerthToTopEvent(Object *target, SEL_DragPanelBerthToTopEvent selector)
{
    m_pBerthToTopListener = target;
    m_pfnBerthToTopSelector = selector;
}

void UIDragPanel::addBerthToRightEvent(Object *target, SEL_DragPanelBerthToRightEvent selector)
{
    m_pBerthToRightListener = target;
    m_pfnBerthToRightSelector = selector;
}

void UIDragPanel::addBerthToBottomEvent(Object *target, SEL_DragPanelBerthToBottomEvent selector)
{
    m_pBerthToBottomListener = target;
    m_pfnBerthToBottomSelector = selector;
}


// bounce
bool UIDragPanel::isBounceEnable()
{
    return m_bBounceEnable;
}

void UIDragPanel::setBounceEnable(bool bounce)
{
    m_bBounceEnable = bounce;
}

bool UIDragPanel::checkNeedBounce()
{
    float innerLeft = m_pInnerContainer->getLeftInParent();
    float innerTop = m_pInnerContainer->getTopInParent();
    float innerRight = m_pInnerContainer->getRightInParent();
    float innerBottom = m_pInnerContainer->getBottomInParent();
    
    float left = 0;
    float top = _size.height;
    float right = _size.width;
    float bottom = 0;        
    
    bool need = ((innerLeft > left && innerBottom > bottom)
                 || (innerLeft > left && innerTop < top)
                 || (innerRight < right && innerBottom > bottom)
                 || (innerRight < right && innerTop < top)
                 || (innerLeft > left)
                 || (innerTop < top)
                 || (innerRight < right)
                 || (innerBottom > bottom));
    return need;
}

void UIDragPanel::startBounce()
{
    if (m_eMoveType == DRAGPANEL_MOVE_TYPE_BOUNCE)
    {
        return;
    }
    
    actionStop();
    m_eMoveType = DRAGPANEL_MOVE_TYPE_BOUNCE;
    bounceToCorner();
}

void UIDragPanel::stopBounce()
{
    m_eMoveType = DRAGPANEL_MOVE_TYPE_NONE;
}

void UIDragPanel::bounceToCorner()
{
    float innerLeft = m_pInnerContainer->getLeftInParent();
    float innerTop = m_pInnerContainer->getTopInParent();
    float innerRight = m_pInnerContainer->getRightInParent();
    float innerBottom = m_pInnerContainer->getBottomInParent();
    
    float width = _size.width;
    float height = _size.height;
    float left = 0;
    float top = height;
    float right = width;
    float bottom = 0;
    
    float from_x = 0;
    float from_y = 0;
    float to_x = 0;
    float to_y = 0;
    Point delta = Point::ZERO;
    
    if (innerLeft > left && innerBottom > bottom) // left bottom
    {
        from_x = innerLeft;
        from_y = innerBottom;
        to_x = left;
        to_y = bottom;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_LEFTBOTTOM;
    }
    else if (innerLeft > left && innerTop < top) // left top
    {
        from_x = innerLeft;
        from_y = innerTop;
        to_x = left;
        to_y = top;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_LEFTTOP;
    }
    else if (innerRight < right && innerBottom > bottom) // right bottom
    {
        from_x = innerRight;
        from_y = innerBottom;
        to_x = right;
        to_y = bottom;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_RIGHTBOTTOM;
    }
    else if (innerRight < right && innerTop < top) // right top
    {
        from_x = innerRight;
        from_y = innerTop;
        to_x = right;
        to_y = top;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_RIGHTTOP;
    }
    else if (innerLeft > left) // left
    {
        from_x = innerLeft;
        from_y = innerBottom;
        to_x = left;
        to_y = from_y;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_LEFT;
    }
    else if (innerTop < top) // top
    {
        from_x = innerLeft;
        from_y = innerTop;
        to_x = from_x;
        to_y = top;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_TOP;
    }
    else if (innerRight < right) // right
    {
        from_x = innerRight;
        from_y = innerBottom;
        to_x = right;
        to_y = from_y;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_RIGHT;
    }
    else if (innerBottom > bottom) // bottom
    {
        from_x = innerLeft;
        from_y = innerBottom;
        to_x = from_x;
        to_y = bottom;
        
        m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_BOTTOM;
    }
    delta = Point(to_x, to_y) - Point(from_x, from_y);
//    delta = ccpSub(ccp(to_x, to_y), ccp(from_x, from_y));
    
    actionStartWithWidget(m_pInnerContainer);
    moveByWithDuration(m_fBounceDuration, delta);
}

void UIDragPanel::bounceOver()
{
    stopBounce();
    
    bounceOverEvent();
    
    switch (m_eBounceDirection)
    {
        case DRAGPANEL_BOUNCE_DIR_LEFTBOTTOM:
            bounceToLeftBottomEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_LEFTTOP:
            bounceToLeftTopEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_RIGHTBOTTOM:
            bounceToRightBottomEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_RIGHTTOP:
            bounceToRightTopEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_LEFT:
            bounceToLeftEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_TOP:
            bounceToTopEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_RIGHT:
            bounceToRightEvent();
            break;
            
        case DRAGPANEL_BOUNCE_DIR_BOTTOM:
            bounceToBottomEvent();
            break;
            
        default:
            break;
    }
    
    m_eBounceDirection = DRAGPANEL_BOUNCE_DIR_NONE;
}

void UIDragPanel::bounceOverEvent()
{
    if (m_pBounceOverListener && m_pfnBounceOverSelector)
    {
        (m_pBounceOverListener->*m_pfnBounceOverSelector)(this);
    }
}

void UIDragPanel::bounceToLeftBottomEvent()
{
    if (m_pBounceToLeftBottomListener && m_pfnBounceToLeftBottomSelector)
    {
        (m_pBounceToLeftBottomListener->*m_pfnBounceToLeftBottomSelector)(this);
    }
}

void UIDragPanel::bounceToLeftTopEvent()
{
    if (m_pBounceToLeftTopListener && m_pfnBounceToLeftTopSelector)
    {
        (m_pBounceToLeftTopListener->*m_pfnBounceToLeftTopSelector)(this);
    }
}

void UIDragPanel::bounceToRightBottomEvent()
{
    if (m_pBounceToRightBottomListener && m_pfnBounceToRightBottomSelector)
    {
        (m_pBounceToRightBottomListener->*m_pfnBounceToRightBottomSelector)(this);
    }
}

void UIDragPanel::bounceToRightTopEvent()
{
    if (m_pBounceToRightTopListener && m_pfnBounceToRightTopSelector)
    {
        (m_pBounceToRightTopListener->*m_pfnBounceToRightTopSelector)(this);
    }
}

void UIDragPanel::bounceToLeftEvent()
{
    if (m_pBounceToLeftListener && m_pfnBounceToLeftSelector)
    {
        (m_pBounceToLeftListener->*m_pfnBounceToLeftSelector)(this);
    }
}

void UIDragPanel::bounceToTopEvent()
{
    if (m_pBounceToTopListener && m_pfnBounceToTopSelector)
    {
        (m_pBounceToTopListener->*m_pfnBounceToTopSelector)(this);
    }
}

void UIDragPanel::bounceToRightEvent()
{
    if (m_pBounceToRightListener && m_pfnBounceToRightSelector)
    {
        (m_pBounceToRightListener->*m_pfnBounceToRightSelector)(this);
    }
}

void UIDragPanel::bounceToBottomEvent()
{
    if (m_pBounceToBottomListener && m_pfnBounceToBottomSelector)
    {
        (m_pBounceToBottomListener->*m_pfnBounceToBottomSelector)(this);
    }
}

void UIDragPanel::addBounceOverEvent(Object *target, SEL_DragPanelBounceOverEvent selector)
{
    m_pBounceOverListener = target;
    m_pfnBounceOverSelector = selector;
}

void UIDragPanel::addBounceToLeftBottomEvent(Object *target, SEL_DragPanelBounceToLeftBottomEvent selector)
{
    m_pBounceToLeftBottomListener = target;
    m_pfnBounceToLeftBottomSelector = selector;
}

void UIDragPanel::addBounceToLeftTopEvent(Object *target, SEL_DragPanelBounceToLeftTopEvent selector)
{
    m_pBounceToLeftTopListener = target;
    m_pfnBounceToLeftTopSelector = selector;
}

void UIDragPanel::addBounceToRightBottomEvent(Object *target, SEL_DragPanelBounceToRightBottomEvent selector)
{
    m_pBounceToRightBottomListener = target;
    m_pfnBounceToRightBottomSelector = selector;
}

void UIDragPanel::addBounceToRightTopEvent(Object *target, SEL_DragPanelBounceToRightTopEvent selector)
{
    m_pBounceToRightTopListener = target;
    m_pfnBounceToRightTopSelector = selector;
}

void UIDragPanel::addBounceToLeftEvent(Object *target, SEL_DragPanelBounceToLeftEvent selector)
{
    m_pBounceToLeftListener = target;
    m_pfnBounceToLeftSelector = selector;
}

void UIDragPanel::addBounceToTopEvent(Object *target, SEL_DragPanelBounceToTopEvent selector)
{
    m_pBounceToTopListener = target;
    m_pfnBounceToTopSelector = selector;
}

void UIDragPanel::addBounceToRightEvent(Object *target, SEL_DragPanelBounceToRightEvent selector)
{
    m_pBounceToRightListener = target;
    m_pfnBounceToRightSelector = selector;
}

void UIDragPanel::addBounceToBottomEvent(Object *target, SEL_DragPanelBounceToBottomEvent selector)
{
    m_pBounceToBottomListener = target;
    m_pfnBounceToBottomSelector = selector;
}

// widget action
void UIDragPanel::actionWithDuration(float duration)
{
    m_fDuration = duration;
        
    if (m_fDuration == 0)
    {
        m_fDuration = FLT_EPSILON;
    }
    
    m_elapsed = 0;
    m_bFirstTick = true;
}

bool UIDragPanel::actionIsDone()
{
    bool value = (m_elapsed >= m_fDuration);
    return value;
}

void UIDragPanel::actionStartWithWidget(UIWidget *widget)
{
    m_bRunningAction = true;
    m_pActionWidget = widget;
}

void UIDragPanel::actionStep(float dt)
{
    if (m_bFirstTick)
    {
        m_bFirstTick = false;
        m_elapsed = 0;
    }
    else
    {
        m_elapsed += dt;
    }
    
    actionUpdate(MAX (0,
                      MIN(1, m_elapsed /
                          MAX(m_fDuration, FLT_EPSILON)
                          )
                      )
                 );        
}

void UIDragPanel::actionUpdate(float dt)
{
    switch (m_nActionType)
    {
        case 1: // move by
            moveByUpdate(dt);
            break;
            
        case 2: // move to
            moveToUpdate(dt);
            break;
            
        default:
            break;
    }
}

void UIDragPanel::actionStop()
{
    m_bRunningAction = false;
}

void UIDragPanel::actionDone()
{
    switch (m_eMoveType)
    {
        case DRAGPANEL_MOVE_TYPE_AUTOMOVE:
            autoMoveOver();
            break;
            
        case DRAGPANEL_MOVE_TYPE_BOUNCE:
            bounceOver();
            break;
            
        default:
            break;
    }
}

// move by
void UIDragPanel::moveByWithDuration(float duration, const Point& deltaPosition)
{
    actionWithDuration(duration);
    m_positionDelta = deltaPosition;
    moveByInit();
    m_nActionType = 1;
}

void UIDragPanel::moveByInit()
{
    m_previousPosition = m_startPosition = m_pActionWidget->getPosition();
}

void UIDragPanel::moveByUpdate(float t)
{
    float easeRate = 0.0f;
    switch (m_eMoveType)
    {
        case DRAGPANEL_MOVE_TYPE_AUTOMOVE:
            easeRate = m_fAutoMoveEaseRate;
            break;
            
        case DRAGPANEL_MOVE_TYPE_BOUNCE:
            easeRate = m_fBounceEaseRate;
            break;
            
        default:
            break;
    }
    t = powf(t, 1 / easeRate);
    
    Point currentPos = m_pActionWidget->getPosition();
    Point diff = currentPos - m_previousPosition;
    m_startPosition = m_startPosition + diff;
//    Point diff = ccpSub(currentPos, m_previousPosition);
//    m_startPosition = ccpAdd( m_startPosition, diff);
    
//    Point newPos = ccpAdd( m_startPosition, ccpMult(m_positionDelta, t) );
    Point newPos = m_startPosition + (m_positionDelta * t);
    
    m_pActionWidget->setPosition(newPos);
    m_previousPosition = newPos;
    
    switch (m_eMoveType)
    {
        case DRAGPANEL_MOVE_TYPE_AUTOMOVE:
            autoMove();
            break;
            
        default:
            break;
    }
}

// move to
void UIDragPanel::moveToWithDuration(float duration, const Point& position)
{
    actionWithDuration(duration);
    m_endPosition = position;    
    moveToInit();
    m_nActionType = 2;
}

void UIDragPanel::moveToInit()
{
    moveByInit();
    m_positionDelta = m_endPosition - m_pActionWidget->getPosition();
//    m_positionDelta = ccpSub( m_endPosition, m_pActionWidget->getPosition() );
}

void UIDragPanel::moveToUpdate(float t)
{
    moveByUpdate(t);
}

Layout* UIDragPanel::getInnerContainer()
{
    return m_pInnerContainer;
}

NS_CC_EXT_END
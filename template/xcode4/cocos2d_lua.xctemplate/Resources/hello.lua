g_Scene = cocos2d.CCScene:node()
pSprite = cocos2d.CCSprite:spriteWithFile("90001.jpg")
pSprite:setPosition(cocos2d.CCPoint(300, 400))

pLayer = cocos2d.CCLayer:node()
pLayer:setIsTouchEnabled(true)
pLayer:setAnchorPoint(cocos2d.CCPoint(0,0))
pLayer:setPosition( cocos2d.CCPoint(0, -300) )
pLayer:addChild(pSprite)
g_Scene:addChild(pLayer)

function btnTouchMove(e)
    cocos2d.CCLuaLog("mousemove")
end

function btnTouchBegin(e)
    cocos2d.CCLuaLog("btnTouchBegin")
end

function btnTouchEnd(e)
    cocos2d.CCLuaLog("btnTouchEnd")
end

-- regiester touch handler
pLayer.__CCTouchDelegate__:registerScriptTouchHandler(cocos2d.CCTOUCHBEGAN, "btnTouchBegin")
pLayer.__CCTouchDelegate__:registerScriptTouchHandler(cocos2d.CCTOUCHMOVED, "btnTouchMove")
pLayer.__CCTouchDelegate__:registerScriptTouchHandler(cocos2d.CCTOUCHENDED, "btnTouchEnd")

-- add a menu
menuItem = cocos2d.CCMenuItemImage:itemFromNormalImage("menu2.png", "menu2.png")
menuItem:setAnchorPoint(cocos2d.CCPoint(0,0))
menuItem:setPosition( cocos2d.CCPoint(100, 200) )	
menuItem:registerScriptHandler("CloseMenu")
pMenu = cocos2d.CCMenu:menuWithItem(menuItem)
pMenu:setPosition( cocos2d.CCPoint(1000, 200) )
g_Scene:addChild(pMenu)

function CloseMenu()
    pMenu:setPosition(cocos2d.CCPoint(1000, 200) )
end

function PopMenu()
    pMenu:setPosition( cocos2d.CCPoint(0, -100) )
end

pCloseItem = cocos2d.CCMenuItemImage:itemFromNormalImage("menu1.png","menu1.png")
pCloseItem:setPosition( cocos2d.CCPoint(30, 40) )	
pCloseItem:registerScriptHandler("PopMenu")
pcloseMenu = cocos2d.CCMenu:menuWithItem(pCloseItem)
pcloseMenu:setPosition( cocos2d.CCPoint(30, 40) )
g_Scene:addChild(pcloseMenu)

for i=0,3,1 do
    for j=0,1,1 do

        landSprite = cocos2d.CCSprite:spriteWithFile("land1.png")
        pLayer:addChild(landSprite)

        landSprite:setAnchorPoint(cocos2d.CCPoint(0,0))
        landSprite:setPosition(cocos2d.CCPoint(90+j*180 - i%2*90, 200+i*95/2))

    end
end



--crop

for i=0,3,1 do
    for j=0,1,1 do

        texturecrop = cocos2d.CCTextureCache:sharedTextureCache():addImage("crop1.png")
        framecrop = cocos2d.CCSpriteFrame:frameWithTexture(texturecrop, cocos2d.CCRectMake(0, 0, 105, 95))
        spritecrop = cocos2d.CCSprite:spriteWithSpriteFrame(framecrop);

        pLayer:addChild(spritecrop)

        spritecrop:setAnchorPoint(cocos2d.CCPoint(0,0))
        spritecrop:setPosition(cocos2d.CCPoint(45+90+j*180 - i%2*90, 25+200+i*95/2))

    end
end




nFrameWidth = 105
nFrameHeight = 95

texture = cocos2d.CCTextureCache:sharedTextureCache():addImage("dog1.png")
frame0 = cocos2d.CCSpriteFrame:frameWithTexture(texture, cocos2d.CCRectMake(0, 0, nFrameWidth, nFrameHeight))
frame1 = cocos2d.CCSpriteFrame:frameWithTexture(texture, cocos2d.CCRectMake(nFrameWidth*1, 0, nFrameWidth, nFrameHeight))

spritedog = cocos2d.CCSprite:spriteWithSpriteFrame(frame0)
spritedog:setPosition(cocos2d.CCPoint(300, 500))
pLayer:addChild(spritedog)


animFrames = cocos2d.CCMutableArray_CCSpriteFrame__:new(2)

animFrames:addObject(frame0)
animFrames:addObject(frame1)

animation = cocos2d.CCAnimation:animationWithName("wait", 0.5, animFrames)

animate = cocos2d.CCAnimate:actionWithAnimation(animation, false);
spritedog:runAction(cocos2d.CCRepeatForever:actionWithAction(animate))

cocos2d.CCDirector:sharedDirector():runWithScene(g_Scene)

function tick()

    point = cocos2d.CCPoint(300, 500);
    point = spritedog:getPosition();

    if point.x > 600 then
        point.x = 0
        spritedog:setPosition(point)
    else
        point.x = point.x + 1 
        spritedog:setPosition(point) 
    end

end

cocos2d.CCScheduler:sharedScheduler():scheduleScriptFunc("tick", 0.01, false)

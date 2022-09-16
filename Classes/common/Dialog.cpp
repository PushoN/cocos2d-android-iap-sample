#include "Dialog.h"

USING_NS_CC;
using namespace common;

Dialog::Dialog() {
}

Dialog::~Dialog() {
}

Dialog* Dialog::create() {
    auto dialog = new Dialog();
    if(dialog->init()) {
        dialog->autorelease();
        return dialog;
    }

    CC_SAFE_DELETE(dialog);
    return nullptr;
}

bool Dialog::init() {
    if(!Node::init()) {
        return false;
    }

    setContentSize(Director::getInstance()->getWinSize());
    setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    setPosition(Director::getInstance()->getWinSize() / 2);

    hookTouchEvent();
    setLocalZOrder(10000); // Specifies the number that comes above the other nodes

    return true;
}

void Dialog::open(Node* parent) {
    parent->addChild(this);
}

void Dialog::close() {
    runAction(RemoveSelf::create());
}

void Dialog::hookTouchEvent() {
    // Do not notify nodes below self of touch event
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) -> bool {
        return true;
    };
    listener->onTouchEnded = [](Touch* touch, Event* event) {};
}
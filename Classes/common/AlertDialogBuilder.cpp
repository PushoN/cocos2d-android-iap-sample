#include "AlertDialogBuilder.h"
#include "ButtonHelper.h"
#include "LabelHelper.h"

USING_NS_CC;
using namespace common;

AlertDialogBuilder::AlertDialogBuilder() :
    mTitle(""),
    mMessage(""),
    mPositiveButtonConfig(nullptr),
    mNegativeButtonConfig(nullptr)
{
}

AlertDialogBuilder::~AlertDialogBuilder() {
    delete mPositiveButtonConfig;
    delete mNegativeButtonConfig;
}

std::shared_ptr<AlertDialogBuilder> AlertDialogBuilder::create() {
    return std::shared_ptr<AlertDialogBuilder>(new AlertDialogBuilder());
}

void AlertDialogBuilder::setTitle(const std::string& title) {
    mTitle = title;
}

void AlertDialogBuilder::setMessage(const std::string& message) {
    mMessage = message;
}

void AlertDialogBuilder::setPositiveButton(const std::string& text, const ButtonClickCallback& callback) {
    mPositiveButtonConfig = new ButtonConfig();
    mPositiveButtonConfig->buttonText = text;
    mPositiveButtonConfig->clickCallback = callback;
}

void AlertDialogBuilder::setNegativeButton(const std::string& text, const ButtonClickCallback& callback) {
    mNegativeButtonConfig = new ButtonConfig();
    mNegativeButtonConfig->buttonText = text;
    mNegativeButtonConfig->clickCallback = callback;
}

Dialog* AlertDialogBuilder::build() {
    auto winSize = Director::getInstance()->getWinSize();
    const Size buttonSize(140, 60);

    // title label is optional
    Label* titleLabel = nullptr;
    if(mTitle.length() > 0) {
        titleLabel = LabelHelper::createLabel(
            mTitle, 24.0f, Color4B::YELLOW,
            TextHAlignment::LEFT, TextVAlignment::TOP);
    }

    auto messageLabel = LabelHelper::createLabel(
        mMessage, 22.0f, Color4B::WHITE,
        TextHAlignment::LEFT, TextVAlignment::TOP);

    ui::Button* positiveButton = nullptr;
    if(mPositiveButtonConfig != nullptr) {
        positiveButton = ButtonHelper::createBlackButton(buttonSize, mPositiveButtonConfig->buttonText, 20.0f);
    }

    ui::Button* negativeButton = nullptr;
    if(mNegativeButtonConfig != nullptr) {
        negativeButton = ButtonHelper::createBlackButton(buttonSize, mNegativeButtonConfig->buttonText, 20.0f);
    }

    const float paddingLeft = 20;
    const float paddingRight = paddingLeft;
    const float paddingTop = 30;
    const float paddingBottom = 25;

    // my designResolutionSize is 540x960

    const float minContentWidth = 350;
    const float maxContentWidth = 450;

    float maxLabelWidth = messageLabel->getContentSize().width;
    if(titleLabel != nullptr) {
        maxLabelWidth = std::max(titleLabel->getContentSize().width, messageLabel->getContentSize().width);
    }

    float contentWidth = clampf(maxLabelWidth, minContentWidth, maxContentWidth);

    float messageLabelMarginTop = 0;
    float titleLabelHeight = 0;
    if(titleLabel != nullptr) {
        messageLabelMarginTop = 25;
        if(titleLabel->getContentSize().width > contentWidth) {
            titleLabel->setDimensions(contentWidth, 0);
        }

        titleLabelHeight = titleLabel->getContentSize().height;
    }

    if(messageLabel->getContentSize().width > contentWidth) {
        messageLabel->setDimensions(contentWidth, 0);
    }
    float messageLabelHeight = messageLabel->getContentSize().height;

    float frameWidth = contentWidth + paddingLeft + paddingRight;

    auto buttonContainer = ui::Layout::create();
    buttonContainer->setContentSize(Size(frameWidth, 0));
    float buttonMargin = 65;

    if(positiveButton != nullptr && negativeButton != nullptr) {
        buttonContainer->setContentSize(Size(frameWidth, buttonSize.height));
        buttonContainer->addChild(negativeButton);
        buttonContainer->addChild(positiveButton);

        negativeButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
        negativeButton->setPosition(Vec2(buttonContainer->getContentSize().width / 2 - buttonMargin / 2, buttonContainer->getContentSize().height / 2));

        positiveButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
        positiveButton->setPosition(Vec2(buttonContainer->getContentSize().width / 2 + buttonMargin / 2, buttonContainer->getContentSize().height / 2));
    }else if(positiveButton != nullptr) {
        buttonContainer->setContentSize(Size(frameWidth, buttonSize.height));

        positiveButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        positiveButton->setPosition(buttonContainer->getContentSize() / 2);
        buttonContainer->addChild(positiveButton);
    }else if(negativeButton != nullptr) {
        buttonContainer->setContentSize(Size(frameWidth, buttonSize.height));

        negativeButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        negativeButton->setPosition(buttonContainer->getContentSize() / 2);
        buttonContainer->addChild(negativeButton);
    }

    float buttonContainerMarginTop = 0;
    if(buttonContainer->getContentSize().height > 0) {
        buttonContainerMarginTop = 15;
    }

    float frameHeight = paddingTop + paddingBottom + titleLabelHeight +
        messageLabelHeight + messageLabelMarginTop +
        buttonContainerMarginTop + buttonContainer->getContentSize().height;
    if(titleLabel != nullptr) {
        frameHeight += messageLabelMarginTop;
    }

    auto frame = ui::ImageView::create("images/btn_basic.png");
    frame->setScale9Enabled(true);
    frame->setCapInsets(Rect(4, 4, 8, 8));
    frame->setContentSize(Size(frameWidth, frameHeight));

    if(titleLabel != nullptr) {
        frame->addChild(titleLabel);

        titleLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
        titleLabel->setPosition(Vec2(frameWidth / 2, frameHeight - paddingTop));
    }

    messageLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    messageLabel->setPosition(Vec2(frameWidth / 2, frameHeight - paddingTop - titleLabelHeight - messageLabelMarginTop));
    frame->addChild(messageLabel);

    if(buttonContainer->getContentSize().height > 0) {
        buttonContainer->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
        buttonContainer->setPosition(Vec2(frameWidth / 2, paddingBottom));
        frame->addChild(buttonContainer);
    }

    frame->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    frame->setPosition(winSize / 2);

    auto dialog = Dialog::create();
    dialog->addChild(frame);

    if(positiveButton != nullptr) {
        ButtonClickCallback callback = mPositiveButtonConfig->clickCallback;
        positiveButton->addClickEventListener([dialog, callback](Ref*) {
            if(callback != nullptr) {
                callback(dialog);
            }else {
                dialog->close();
            }
        });
    }

    if(negativeButton != nullptr) {
        ButtonClickCallback callback = mNegativeButtonConfig->clickCallback;
        negativeButton->addClickEventListener([dialog, callback](Ref*) {
            if(callback != nullptr) {
                callback(dialog);
            }else {
                dialog->close();
            }
        });
    }

    return dialog;
}
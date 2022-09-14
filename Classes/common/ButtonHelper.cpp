#include "ButtonHelper.h"

USING_NS_CC;
using namespace common;

static const std::string FontPath = "fonts/mplus-1p-bold.ttf";

static const std::string BasicNormalImagePath = "images/btn_basic.png";
static const std::string BasicPressedImagePath = "images/btn_basic_pressed.png";
static const std::string BlueNormalImagePath = "images/btn_basic_blue.png";
static const std::string BluePressedImagePath = "images/btn_basic_blue_pressed.png";

ui::Button* ButtonHelper::createBlackButton(const Size& size, const std::string& text, float fontSize) {
    auto button = ui::Button::create(BasicNormalImagePath, BasicPressedImagePath, BasicPressedImagePath);
    button->setCapInsets(Rect(4, 4, 8, 8));
    button->setScale9Enabled(true);
    button->setTitleFontName(FontPath);
    button->setTitleText(text);
    button->setTitleFontSize(fontSize);
    button->setTitleColor(Color3B::WHITE);
    button->setContentSize(size);

    return button;
}

ui::Button* ButtonHelper::createBlueButton(const Size& size, const std::string& text, float fontSize) {
    auto button = ui::Button::create(BlueNormalImagePath, BluePressedImagePath, BasicPressedImagePath);
    button->setCapInsets(Rect(4, 4, 8, 8));
    button->setScale9Enabled(true);
    button->setTitleFontName(FontPath);
    button->setTitleText(text);
    button->setTitleFontSize(fontSize);
    button->setTitleColor(Color3B::WHITE);
    button->setContentSize(size);

    return button;
}
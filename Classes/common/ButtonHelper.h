#ifndef __common_ButtonHelper__
#define __common_ButtonHelper__

#include "ui/CocosGUI.h"

namespace common {

    class ButtonHelper {
    public:
        static cocos2d::ui::Button* createBlackButton(const cocos2d::Size& size, const std::string& text, float fontSize = 20.0f);
        static cocos2d::ui::Button* createBlueButton(const cocos2d::Size& size, const std::string& text, float fontSize = 20.0f);
    };

}

#endif

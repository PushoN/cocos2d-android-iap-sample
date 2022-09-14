#ifndef __common_LabelHelper__
#define __common_LabelHelper__

#include "ui/CocosGUI.h"

namespace common {

    class LabelHelper {
    public:
        static cocos2d::Label *createLabel(
                const std::string &textContent, float textSize = 18.0f,
                const cocos2d::Color4B &textColor = cocos2d::Color4B::WHITE,
                const cocos2d::TextHAlignment &hAlignment = cocos2d::TextHAlignment::LEFT,
                const cocos2d::TextVAlignment &vAlignment = cocos2d::TextVAlignment::TOP);

        static cocos2d::ui::Text* createText(
                const std::string& textContent, float textSize = 18.0f,
                const cocos2d::Color4B& textColor = cocos2d::Color4B::WHITE,
                const cocos2d::TextHAlignment& hAlignment = cocos2d::TextHAlignment::LEFT,
                const cocos2d::TextVAlignment& vAlignment = cocos2d::TextVAlignment::TOP);
    };

}

#endif

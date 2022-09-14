#include "LabelHelper.h"

USING_NS_CC;
using namespace common;

static const std::string FontPath = "fonts/mplus-1p-bold.ttf";

Label* LabelHelper::createLabel(const std::string& textContent, float textSize,
                                const Color4B& textColor, const TextHAlignment& hAlignment,
                                const TextVAlignment& vAlignment) {
    auto label = Label::createWithTTF(textContent, FontPath, textSize);
    label->setTextColor(textColor);
    label->setHorizontalAlignment(hAlignment);
    label->setVerticalAlignment(vAlignment);

    return label;
}

ui::Text* LabelHelper::createText(const std::string& textContent, float textSize,
                                  const Color4B& textColor, const TextHAlignment& hAlignment,
                                  const TextVAlignment& vAlignment) {
    auto text = ui::Text::create(textContent, FontPath, textSize);
    text->setTextColor(textColor);
    text->setTextHorizontalAlignment(hAlignment);
    text->setTextVerticalAlignment(vAlignment);

    return text;
}

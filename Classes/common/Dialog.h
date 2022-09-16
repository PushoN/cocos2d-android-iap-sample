#ifndef __common_Dialog__
#define __common_Dialog__

#include "cocos2d.h"
#include "ui/CocosGUI.h"

namespace common {

    class Dialog : public cocos2d::Node {
    public:
        static Dialog* create();
        Dialog();
        virtual ~Dialog();
        virtual void open(cocos2d::Node* parent);
        virtual void close();

    protected:
        bool init() override;
        virtual void hookTouchEvent();
    };

}

#endif

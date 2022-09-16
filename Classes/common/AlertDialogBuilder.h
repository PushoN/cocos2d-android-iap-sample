#ifndef __common_AlertDialogBuilder__
#define __common_AlertDialogBuilder__

#include "Dialog.h"

namespace common {

    /**
     * // single button
     * auto builder = AlertDialogBuilder::create();
     * builder->setTitle("title") // optional
     * builder->setMessage("message");
     * builder->setPositiveButton("OK", [](Dialog* dialog) {
     *   dialog->close();
     * });
     * builder->build()->open(this); // this is running scene
     *
     * // two buttons
     * auto builder = AlertDialogBuilder::create();
     * builder->setTitle("title") // optional
     * builder->setMessage("message");
     * builder->setNegativeButton("Cancel", nullptr);
     * builder->setPositiveButton("OK", [](Dialog* dialog) {
     *   dialog->close();
     * });
     * builder->build()->open(this);
     */
    class AlertDialogBuilder {
    public:
        typedef std::function<void(Dialog* dialog)> ButtonClickCallback;

        struct ButtonConfig {
            std::string buttonText;
            ButtonClickCallback clickCallback;
        };

        static std::shared_ptr<AlertDialogBuilder> create();

        AlertDialogBuilder();
        virtual ~AlertDialogBuilder();
        void setTitle(const std::string& title);
        void setMessage(const std::string& message);
        void setPositiveButton(const std::string& text, const ButtonClickCallback& callback);
        void setNegativeButton(const std::string& text, const ButtonClickCallback& callback);
        Dialog* build();

    private:
        std::string mTitle;
        std::string mMessage;
        ButtonConfig* mPositiveButtonConfig;
        ButtonConfig* mNegativeButtonConfig;
    };

}

#endif

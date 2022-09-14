#ifndef __scene_SampleScene__
#define __scene_SampleScene__

#include "cocos2d.h"
#include "ui/CocosGUI.h"
#include "dropbox/json11/json11.hpp"
#include "iap/MingosIAP.h"

class SampleScene : public cocos2d::Scene, public mingos::IAPEventListener {
public:
    static SampleScene* create();

private:
    SampleScene();
    ~SampleScene();
    bool init() override;
    void onEnter() override;
    void createItemList(const std::vector<mingos::Product>& products);

    void onInitialized(bool success) override;
    void onProductRequestSuccess(const std::vector<mingos::Product>& products) override;
    void onProductRequestFailure(const std::string& msg) override;
    void onPurchaseHistory(const std::string& purchaseListJsonString) override;
    void onPurchaseSuccess(const mingos::Product& product) override;
    void onPurchaseFailure(const mingos::Product& product, const mingos::BillingResponseCode& responseCode, const std::string& msg) override;
    void onPurchaseCanceled(const mingos::Product& product) override;
    void onConsumeSuccess(const mingos::Product& product) override;
    void onConsumeFailure(const mingos::Product& product, const mingos::BillingResponseCode& responseCode, const std::string& msg) override;
    void onRestoreSuccess(const std::vector<mingos::Product>& products) override;
    void onRestoreFailure(const mingos::BillingResponseCode& responseCode, const std::string& msg) override;
};

#endif

#include "SampleScene.h"
#include "common/ButtonHelper.h"
#include "common/LabelHelper.h"
#include "common/JsonParser.h"
#include "common/AlertDialogBuilder.h"

USING_NS_CC;
using namespace common;

SampleScene::SampleScene() {
}

SampleScene::~SampleScene() {
    mingos::IAP::removeListener();
}

SampleScene* SampleScene::create() {
    auto scene = new SampleScene();
    if(scene->init()) {
        scene->autorelease();
        return scene;
    }

    CC_SAFE_DELETE(scene);
    return nullptr;
}

bool SampleScene::init() {
    if(!Scene::init()) {
        return false;
    }

    auto winSize = Director::getInstance()->getWinSize();

    auto closeButton = ButtonHelper::createBlueButton(
        Size(240, 60), "Finish Application");
    closeButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    closeButton->setPosition(Vec2(winSize.width / 2, 50));
    closeButton->addClickEventListener([](Ref* sender) {
        Director::getInstance()->end();
    });
    addChild(closeButton);

    return true;
}

void SampleScene::onEnter() {
    Scene::onEnter();

    CCLOG("[SampleScene::onEnter] mingos::IAP::isInitialized=%s",
          mingos::IAP::isInitialized() ? "true" : "false");
    mingos::IAP::setListener(this);
    mingos::IAP::init();
}

void SampleScene::createItemList(const std::vector<mingos::Product>& products) {
    auto winSize = Director::getInstance()->getWinSize();

    auto listView = ui::ListView::create();
    listView->setClippingEnabled(true);
    listView->setBounceEnabled(true);
    listView->setItemsMargin(20);
    listView->setContentSize(Size(520, 700));
    listView->setAnchorPoint(Vec2::ANCHOR_MIDDLE_TOP);
    listView->setPosition(Vec2(winSize.width / 2, winSize.height - 100));
    addChild(listView);

    const float itemWidth = listView->getContentSize().width;
    const float padding = 10;

    for(const auto& product : products) {
        auto layout = ui::Layout::create();
        layout->setBackGroundImage("images/btn_basic.png");
        layout->setBackGroundImageScale9Enabled(true);
        layout->setBackGroundImageCapInsets(Rect(4, 4, 8, 8));

        auto purchaseButton = ButtonHelper::createBlueButton(Size(150, 60), product.price, 20.0f);
        purchaseButton->addClickEventListener([this, product](Ref* sender) {
            mingos::IAP::purchase(product.id);
        });

        float labelMarginRight = 10;
        float labelMaxWidth = itemWidth - (padding*2 + labelMarginRight + purchaseButton->getContentSize().width);

        auto titleLabel = LabelHelper::createLabel(
            product.title, 18.0f, Color4B::WHITE, TextHAlignment::LEFT, TextVAlignment::TOP);
        titleLabel->setDimensions(labelMaxWidth, 0);

        float contentHeight = std::max(purchaseButton->getContentSize().height, titleLabel->getContentSize().height);
        layout->setContentSize(Size(itemWidth, padding*2 + contentHeight));
        layout->addChild(titleLabel);
        layout->addChild(purchaseButton);

        purchaseButton->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
        purchaseButton->setPosition(Vec2(layout->getContentSize().width - padding, layout->getContentSize().height / 2));

        titleLabel->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
        titleLabel->setPosition(Vec2(padding, layout->getContentSize().height - padding));

        listView->addChild(layout);
    }

    listView->forceDoLayout();
}

void SampleScene::onInitialized(bool success) {
    CCLOG("[SampleScene::onInitialized] success=%s", success ? "true" : "false");
    mingos::IAP::refresh();
}

void SampleScene::onProductRequestSuccess(const std::vector<mingos::Product>& products) {
    CCLOG("[SampleScene::onProductRequestSuccess] products.size=%d", (int)products.size());

    for(const auto& product : products) {
        CCLOG("%s", product.toJson().dump().c_str());
    }

    createItemList(products);
}

void SampleScene::onProductRequestFailure(const std::string& msg) {
    CCLOG("[SampleScene::onProductRequestFailure] msg=%s", msg.c_str());
}

void SampleScene::onPurchaseHistory(const std::string& purchaseListJsonString) {
    CCLOG("[SampleScene::onPurchaseHistory] purchaseListJsonString=%s", purchaseListJsonString.c_str());

    auto purchasesListJson = JsonParser::parseFromString(purchaseListJsonString);
    if(purchasesListJson == nullptr) {
        return;
    }

    auto purchases = purchasesListJson.array_items();

    CCLOG("purchases.size=%d", (int)purchases.size());
    for(const auto& purchase : purchases) {
        CCLOG("%s", purchase.dump().c_str());
    }
}

void SampleScene::onPurchaseSuccess(const mingos::Product& product) {
    CCLOG("SampleScene::onPurchaseSuccess: product=%s", product.toJson().dump().c_str());

    if(product.type == mingos::IAP_Type::CONSUMABLE) {
        mingos::IAP::consume(product.id);
    } else if (product.type == mingos::IAP_Type::NON_CONSUMABLE) {
        mingos::IAP::acknowledge(product.id);
    }

    std::string alertMessage = StringUtils::format(
        "productId=%s\norderId=%s",
        product.id.c_str(),
        product.transactionID.c_str());

    auto builder = AlertDialogBuilder::create();
    builder->setTitle("purchase success");
    builder->setMessage(alertMessage);
    builder->setPositiveButton("OK", nullptr);
    builder->build()->open(this);
}

void SampleScene::onPurchaseFailure(const mingos::Product& product, const mingos::BillingResponseCode& responseCode, const std::string& msg) {
    CCLOG("SampleScene::onPurchaseFailure: product=%s, responseCode=%d, msg=%s",
          product.toJson().dump().c_str(), static_cast<int>(responseCode), msg.c_str());

    if(responseCode == mingos::BillingResponseCode::ITEM_ALREADY_OWNED) {
        if(product.type == mingos::IAP_Type::CONSUMABLE) {
            mingos::IAP::consume(product.id);
        } else if (product.type == mingos::IAP_Type::NON_CONSUMABLE) {
            mingos::IAP::acknowledge(product.id);
        }
    }

    std::string alertMessage = StringUtils::format(
        "productId=%s\nresponseCode=%d\nmessage=%s",
        product.id.c_str(),
        responseCode,
        msg.c_str());

    auto builder = AlertDialogBuilder::create();
    builder->setTitle("purchase failure");
    builder->setMessage(alertMessage);
    builder->setPositiveButton("OK", nullptr);
    builder->build()->open(this);
}

void SampleScene::onPurchaseCanceled(const mingos::Product& product) {
    CCLOG("SampleScene::onPurchaseCanceled: product=%s", product.toJson().dump().c_str());
}

void SampleScene::onConsumeSuccess(const mingos::Product& product) {
    CCLOG("SampleScene::onConsumeSuccess: product.id=%s", product.id.c_str());
}

void SampleScene::onConsumeFailure(const mingos::Product& product, const mingos::BillingResponseCode& responseCode, const std::string& msg) {
    CCLOG("SampleScene::onConsumeFailure: product=%s, responseCode=%d, msg=%s",
      product.toJson().dump().c_str(), static_cast<int>(responseCode), msg.c_str());
}

void SampleScene::onRestoreSuccess(const std::vector<mingos::Product>& products) {
    CCLOG("SampleScene::onRestoreSuccess: products.size=%d", (int)products.size());

    for(const auto& product : products) {
        auto receipt = JsonParser::parseFromString(product.receipt);
        std::string purchaseToken = receipt["purchaseToken"].string_value();
        CCLOG("productId=%s, orderId=%s, purchaseToken=%s",
              product.id.c_str(), product.transactionID.c_str(), purchaseToken.c_str());

        // if consumable item, call onPurchaseSuccess(product);
   }
}

void SampleScene::onRestoreFailure(const mingos::BillingResponseCode& responseCode, const std::string& msg) {
    CCLOG("SampleScene::onRestoreFailure:responseCode=%d, msg=%s", static_cast<int>(responseCode), msg.c_str());
}

void SampleScene::onAcknowledgeSuccess(const mingos::Product& product) {
    CCLOG("onAcknowledgeSuccess: product.id=%s", product.id.c_str());
}

void SampleScene::onAcknowledgeFailure(const mingos::Product& product, const mingos::BillingResponseCode& responseCode, const std::string& msg) {
    CCLOG("onAcknowledgeFailure: product=%s, responseCode=%d, msg=%s",
      product.toJson().dump().c_str(), static_cast<int>(responseCode), msg.c_str());
}

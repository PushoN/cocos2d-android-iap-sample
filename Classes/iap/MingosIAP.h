#ifndef __MingosIAP__
#define __MingosIAP__

#include "cocos2d.h"
#include "dropbox/json11/json11.hpp"

namespace mingos {

    enum IAP_Type
    {
        CONSUMABLE = 0,
        NON_CONSUMABLE = 1
    };

    struct Product
    {
        Product() : id(""), title(""), description(""), type(IAP_Type::CONSUMABLE),
                    priceValue(0), price(""), currencyCode(""), receiptCipheredPayload(""),
                    receipt(""), transactionID("") {}

        /**
         * productId must match for both GooglePlay and AppStore
         */
        std::string id;
        IAP_Type type;
        std::string title;
        std::string description;
        float priceValue;
        std::string price;
        std::string currencyCode;
        std::string receiptCipheredPayload;
        std::string receipt;
        std::string transactionID;
        int quantity;

        json11::Json toJson() const {
            auto json = json11::Json::object{
                {"id", id},
                {"type", type == IAP_Type::CONSUMABLE ? "consumable" : "non_consumable"},
                {"title", title},
                {"description", description},
                {"currencyCode", currencyCode},
                {"price", price},
                {"priceValue", priceValue},
                {"receipt", receipt},
                {"receiptCipheredPayload", receiptCipheredPayload},
                {"transactionID", transactionID},
                {"quantity", quantity},
            };

            return json;
        }
    };

    enum class BillingResponseCode {
        SERVICE_TIMEOUT = -3,
        FEATURE_NOT_SUPPORTED = -2,
        SERVICE_DISCONNECTED = -1,
        OK = 0,
        USER_CANCELED = 1,
        SERVICE_UNAVAILABLE = 2,
        BILLING_UNAVAILABLE = 3,
        ITEM_UNAVAILABLE = 4,
        DEVELOPER_ERROR = 5,
        ERROR = 6,
        ITEM_ALREADY_OWNED = 7,
        ITEM_NOT_OWNED = 8
    };

    class IAPEventListener {
    public:
        virtual void onInitialized(bool success) = 0;
        virtual void onProductRequestSuccess(const std::vector<Product>& products) = 0;
        virtual void onProductRequestFailure(const std::string& msg) = 0;
        virtual void onPurchaseHistory(const std::string& purchaseListJsonString) {}
        virtual void onPurchaseSuccess(const Product& product) = 0;
        virtual void onPurchaseFailure(const Product& product, const BillingResponseCode& responseCode, const std::string& msg) = 0;
        virtual void onPurchaseCanceled(const Product& product) = 0;
        virtual void onConsumeSuccess(const Product& product) = 0;
        virtual void onConsumeFailure(const Product& product, const BillingResponseCode& responseCode, const std::string& msg) = 0;
        virtual void onRestoreSuccess(const std::vector<Product>& products) = 0;
        virtual void onRestoreFailure(const BillingResponseCode& responseCode, const std::string& msg) = 0;
        virtual void onAcknowledgeSuccess(const Product& product) = 0;
        virtual void onAcknowledgeFailure(const Product& product, const BillingResponseCode& responseCode, const std::string& msg) = 0;
    };

    class IAP {
    public:
        static void init();
        static bool isInitialized();
        static void setListener(IAPEventListener* listener);
        static void removeListener();
        static void refresh();
        static void purchase(const std::string& productId);

        /**
         * for android
         * @param productId
         */
        static void consume(const std::string& productId);

        /**
         * for android
         * @param productId
         */
        static void acknowledge(const std::string& productId);

        /**
        * get all purchase history, include cancelled, expired
        *
        * purchase history is combine results of `queryPurchases` and `queryPurchaseHistoryAsync`
        * result of `queryPurchaseHistoryAsync` doesn't include orderId
        */
        static void getPurchaseHistory();

        /**
         * call queryPurchasesAsync()
         */
        static void restore();
    };
}

#endif

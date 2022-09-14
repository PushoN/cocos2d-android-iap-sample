#include "cocos2d.h"
#include "platform/android/jni/JniHelper.h"
#include "iap/MingosIAP.h"
#include "dropbox/json11/json11.hpp"

USING_NS_CC;
using namespace mingos;

json11::Json parseJson(const jstring& jsonString) {
    std::string error;
    json11::Json json = json11::Json::parse(JniHelper::jstring2string(jsonString), error);

    if(!error.empty()) {
        return false;
    }

    return json;
}

json11::Json parseJson(const std::string& jsonString) {
    std::string error;
    json11::Json json = json11::Json::parse(jsonString, error);

    if(!error.empty()) {
        return false;
    }

    return json;
}

void callFuncInUIThread(const std::function<void()>& callback) {
    auto scheduler = Director::getInstance()->getScheduler();
    scheduler->performFunctionInCocosThread([callback] {
        callback();
    });
}

Product getProductFromJson(const std::string& jsonString) {
    auto json = parseJson(jsonString);

    Product product;
    product.id = json["id"].string_value();
    product.type = json["type"].string_value() == "consumable" ? IAP_Type::CONSUMABLE : IAP_Type::NON_CONSUMABLE;
    product.title = json["title"].string_value();
    product.description = json["description"].string_value();
    product.price = json["price"].string_value();
    product.priceValue = (float)json["priceValue"].number_value();
    product.currencyCode = json["currencyCode"].string_value();
    product.receipt = json["receipt"].string_value();
    product.receiptCipheredPayload= json["receiptCipheredPayload"].string_value();
    product.transactionID = json["transactionID"].string_value();

    return product;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onInitialized(JNIEnv* env, jobject thiz, jlong delegate, jboolean success);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onProductRequestSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productListJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onProductRequestFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring message);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseHistoryRequestSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring purchaseListJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString, jint responseCode, jstring message);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseCanceled(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onConsumeSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onConsumeFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString, jint responseCode, jstring message);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onRestoreSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productListJsonString);
    JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onRestoreFailure(JNIEnv* env, jobject thiz, jlong delegate, jint responseCode, jstring message);
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onInitialized(JNIEnv* env, jobject thiz, jlong delegate, jboolean success) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    callFuncInUIThread([listener, success] {
        listener->onInitialized(success);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onProductRequestSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productListJsonString) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    json11::Json json = parseJson(productListJsonString);
    if(json == nullptr || !json.is_array()) {
        callFuncInUIThread([listener] {
            listener->onProductRequestFailure("product request success, but parse json failed!!");
        });
        return;
    }

    std::vector<Product> products;
    for(const auto& row : json.array_items()) {
        Product product = getProductFromJson(row.dump());
        products.push_back(product);
    }

    callFuncInUIThread([listener, products] {
       listener->onProductRequestSuccess(products);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onProductRequestFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring message) {
    std::string msg = JniHelper::jstring2string(message);
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    callFuncInUIThread([listener, msg] {
        listener->onProductRequestFailure(msg);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseHistoryRequestSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring purchaseListJsonString) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    json11::Json json = parseJson(purchaseListJsonString);
    if(json == nullptr) {
        // parse failed, set empty array json
        std::string err;
        json = json11::Json::parse("[]", err);
    }

    callFuncInUIThread([listener, json] {
        listener->onPurchaseHistory(json.dump());
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString) {
    Product product = getProductFromJson(JniHelper::jstring2string(productJsonString));
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    callFuncInUIThread([listener, product] {
        listener->onPurchaseSuccess(product);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString, jint responseCode, jstring message) {
    Product product = getProductFromJson(JniHelper::jstring2string(productJsonString));
    auto resCode = static_cast<BillingResponseCode>(responseCode);
    std::string msg = JniHelper::jstring2string(message);
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    callFuncInUIThread([listener, product, resCode, msg] {
        listener->onPurchaseFailure(product, resCode, msg);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onPurchaseCanceled(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString) {
    Product product = getProductFromJson(JniHelper::jstring2string(productJsonString));
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);

    callFuncInUIThread([listener, product] {
        listener->onPurchaseCanceled(product);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onConsumeSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);
    Product product = getProductFromJson(JniHelper::jstring2string(productJsonString));

    callFuncInUIThread([listener, product] {
        listener->onConsumeSuccess(product);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onConsumeFailure(JNIEnv* env, jobject thiz, jlong delegate, jstring productJsonString, jint responseCode, jstring message) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);
    Product product = getProductFromJson(JniHelper::jstring2string(productJsonString));
    auto resCode = static_cast<BillingResponseCode>(responseCode);
    std::string msg = JniHelper::jstring2string(message);

    callFuncInUIThread([listener, product, resCode, msg] {
        listener->onConsumeFailure(product, resCode, msg);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onRestoreSuccess(JNIEnv* env, jobject thiz, jlong delegate, jstring productListJsonString) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);
    auto productListJson = parseJson(productListJsonString);
    if(productListJson == nullptr || !productListJson.is_array()) {
        callFuncInUIThread([listener] {
            listener->onRestoreFailure(BillingResponseCode::ERROR, "restore success but parse json failed");
        });
        return;
    }

    std::vector<Product> products;
    for(const auto& row : productListJson.array_items()) {
        auto product = getProductFromJson(row.dump());
        products.push_back(product);
    }

    callFuncInUIThread([listener, products] {
        listener->onRestoreSuccess(products);
    });
}

JNIEXPORT void JNICALL Java_iap_PurchaseEventListener_onRestoreFailure(JNIEnv* env, jobject thiz, jlong delegate, jint responseCode, jstring message) {
    auto listener = reinterpret_cast<IAPEventListener*>(delegate);
    auto resCode = static_cast<BillingResponseCode>(responseCode);
    std::string msg = JniHelper::jstring2string(message);

    callFuncInUIThread([listener, resCode, msg] {
        listener->onRestoreFailure(resCode, msg);
    });
}
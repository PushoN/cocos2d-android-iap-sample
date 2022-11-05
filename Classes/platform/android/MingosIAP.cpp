#include "iap/MingosIAP.h"
#include "platform/android/jni/JniHelper.h"
#include "common/JsonParser.h"

USING_NS_CC;
using namespace mingos;
using namespace common;

static const std::string ConfigJsonPath = "sdkbox_config.json";

/**
 * get Instance of PurchaseManager (Java Object)
 * @return
 */
jobject getJavaPurchaseManager() {
    JniMethodInfo methodInfo;
    if (!JniHelper::getStaticMethodInfo(methodInfo, "iap/PurchaseManager", "getInstance", "()Liap/PurchaseManager;")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#getInstance()");
    }

    // run java code:
    // return PurchaseManager.getInstance();
    return methodInfo.env->CallStaticObjectMethod(methodInfo.classID, methodInfo.methodID);
}

/**
 * create Instance of PurchaseEventListener (Java Object)
 * @param listener
 * @return
 */
jobject createJavaPurchaseEventListener(IAPEventListener* listener) {
    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseEventListener", "<init>", "(J)V")) {
        CC_ASSERT("can not get method info: iap.PurchaseEventListener<init>");
    }

    // run java code:
    // long delegate = (long)listener;
    // return new iap.PurchaseEventListener(delegate);
    return methodInfo.env->NewObject(methodInfo.classID, methodInfo.methodID, (jlong)listener);
}

/**
 * Each method calls a Java PurchaseManager instance method
 */

void IAP::init() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "init", "(Ljava/lang/String;)V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#init()");
    }

    std::string configJsonString = FileUtils::getInstance()->getStringFromFile(ConfigJsonPath);
    jstring jstrConfigJsonString = methodInfo.env->NewStringUTF(configJsonString.c_str());

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID, jstrConfigJsonString);
}

bool IAP::isInitialized() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "isInitialized", "()Z")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#isInitialized()");
    }

    return (bool)methodInfo.env->CallBooleanMethod(javaPurchaseManager, methodInfo.methodID);
}

void IAP::setListener(IAPEventListener* listener) {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    if(listener == nullptr) {
        removeListener();
        return;
    }

    jobject JavaListener = createJavaPurchaseEventListener(listener);
    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "setPurchaseEventListener", "(Liap/PurchaseEventListener;)V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#setPurchaseEventListener()");
    }

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID, JavaListener);
}

void IAP::removeListener() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "removePurchaseEventListener", "()V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#removePurchaseEventListener()");
    }

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID);
}

void IAP::refresh() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "queryProductListAsync", "()V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#queryProductListAsync()");
    }

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID);
}

void IAP::purchase(const std::string& productId) {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "purchase", "(Ljava/lang/String;)V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#purchase()");
    }

    jstring jstrProductId = methodInfo.env->NewStringUTF(productId.c_str());
    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID, jstrProductId);
}

void IAP::consume(const std::string& productId) {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "consume", "(Ljava/lang/String;)V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#consume()");
    }

    jstring jstrProductId = methodInfo.env->NewStringUTF(productId.c_str());
    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID, jstrProductId);
}

void IAP::getPurchaseHistory() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "getPurchaseHistory", "()V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#getPurchaseHistory()");
    }

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID);
}

void IAP::restore() {
    jobject javaPurchaseManager = getJavaPurchaseManager();

    JniMethodInfo methodInfo;
    if (!JniHelper::getMethodInfo(methodInfo, "iap/PurchaseManager", "queryPurchases", "()V")) {
        CC_ASSERT("can not get method info: iap.PurchaseManager#queryPurchases()");
    }

    methodInfo.env->CallVoidMethod(javaPurchaseManager, methodInfo.methodID);
}
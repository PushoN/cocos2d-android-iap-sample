#include "iap/MingosIAP.h"
#import "PurchaseManager.h"

USING_NS_CC;
using namespace mingos;

void IAP::init() {
    PurchaseManager* purchaseManager = [PurchaseManager getInstance];
    [purchaseManager addTransactionObserver];
    
    IAPEventListener* listener = purchaseManager.iapEventListener;
    if(listener == nullptr) {
        return;
    }

    bool canMakePayments = [purchaseManager canMakePayments];
    bool success = false;
    if(canMakePayments && [purchaseManager isInitialized]) {
        success = true;
    }
    
    listener->onInitialized(success);
}

bool IAP::isInitialized() {
    auto purchaseManger = [PurchaseManager getInstance];
    return [purchaseManger isInitialized];
}

void IAP::setListener(IAPEventListener* listener) {
    auto purchaseManger = [PurchaseManager getInstance];
    purchaseManger.iapEventListener = listener;
}

void IAP::removeListener() {
    auto purchaseManger = [PurchaseManager getInstance];
    purchaseManger.iapEventListener = nullptr;
}

void IAP::refresh() {
}

void IAP::purchase(const std::string& productId) {
}

void IAP::consume(const std::string& productId) {
}

void IAP::getPurchaseHistory() {
}

void IAP::restore() {
}

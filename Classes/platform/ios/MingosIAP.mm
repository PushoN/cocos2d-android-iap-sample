#include "iap/MingosIAP.h"

USING_NS_CC;
using namespace mingos;

void IAP::init() {
    CCLOG("IAP::init");
}

bool IAP::isInitialized() {
    return false;
}

void IAP::setListener(IAPEventListener* listener) {
}

void IAP::removeListener() {
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

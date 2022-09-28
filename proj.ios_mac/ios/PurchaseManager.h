#ifndef __ios_PurchaseManager__
#define __ios_PurchaseManager__

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#include "iap/MingosIAP.h"

@interface PurchaseManager : NSObject
    <SKProductsRequestDelegate, SKPaymentTransactionObserver>

@property bool initialized;
@property (nonatomic, assign) mingos::IAPEventListener* iapEventListener;

+ (PurchaseManager*) getInstance;

- (void)addTransactionObserver;
- (void)removeTransactionObserver;
- (bool)canMakePayments;
- (bool)isInitialized;

@end

#endif

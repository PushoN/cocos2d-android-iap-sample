#import "PurchaseManager.h"

@implementation PurchaseManager

static PurchaseManager *instance_ = nil;

+ (PurchaseManager*)getInstance {
    @synchronized(self) {
        if(!instance_) {
            instance_ = [PurchaseManager new];
        }
    }

    return instance_;
}

- (id)init {
    self = [super init];
    if(self) {
        // initialize
        self.initialized = false;
    }

    return self;
}

- (void)addTransactionObserver {
    [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
    self.initialized = true;
}

- (void)removeTransactionObserver {
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    self.initialized = false;
}

- (bool)canMakePayments {
    return [SKPaymentQueue canMakePayments];
}

- (bool)isInitialized {
    return self.initialized;
}

@end


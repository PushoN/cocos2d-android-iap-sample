package iap;

import android.app.Activity;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClient.BillingResponseCode;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchaseHistoryRecord;
import com.android.billingclient.api.PurchaseHistoryResponseListener;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxHandler;
import org.cocos2dx.lib.Cocos2dxHelper;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Locale;
import java.util.Objects;

public class PurchaseManager {
    private static final String TAG = "PurchaseManager";

    private static PurchaseManager sInstance;
    private PurchaseEventListener mPurchaseEventListener;
    private BillingClient mBillingClient;

    // productId => type(consumable or non_cosumable)
    private HashMap<String, String> mProductTypeMap = new HashMap<>();
    // productId => SkuDetails
    private HashMap<String, SkuDetails> mSkuDetailsMap = new HashMap<>();

    /**
     * combined result queryPurchasesAsync() and queryPurchaseHistoryAsync()
     */
    private List<JSONObject> mPurchaseHistoryList = new ArrayList<JSONObject>();
    private SkuDetails mCurrentRequestPurchaseSkuDetails;
    private String mLicenseKey;

    public static PurchaseManager getInstance() {
        if(sInstance == null) {
            sInstance = new PurchaseManager();
        }

        return sInstance;
    }

    private PurchasesUpdatedListener mPurchasesUpdatedListener = new PurchasesUpdatedListener() {
        @Override
        public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
            if (billingResult.getResponseCode() == BillingResponseCode.OK && purchases != null) {
                for (Purchase purchase : purchases) {
                    handlePurchase(purchase);
                }
            } else if (billingResult.getResponseCode() == BillingResponseCode.USER_CANCELED) {
                Product product = getProductFromSkuDetails(mCurrentRequestPurchaseSkuDetails);
                mPurchaseEventListener.onPurchaseCanceled(product);
            } else {
                Product product = getProductFromSkuDetails(mCurrentRequestPurchaseSkuDetails);
                mPurchaseEventListener.onPurchaseFailure(product, billingResult.getResponseCode(), billingResult.getDebugMessage());
            }
        }
    };

    private Product getProductFromSkuDetails(@NonNull SkuDetails skuDetails) {
        String type = getProductType(skuDetails.getSku());
        return Product.createFromSkuDetails(skuDetails, type);
    }

    private void handlePurchase(Purchase purchase) {
        if(purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        Product product = getProductFromSkuDetails(mCurrentRequestPurchaseSkuDetails);

        product.transactionID = purchase.getOrderId();
        product.receipt = purchase.getOriginalJson();
        product.receiptCipheredPayload = purchase.getSignature();

        mPurchaseEventListener.onPurchaseSuccess(product);
    }

    private PurchaseManager() {
    }

    public Activity getActivity() {
        return Cocos2dxHelper.getActivity();
    }

    public void setPurchaseEventListener(PurchaseEventListener listener) {
        mPurchaseEventListener = listener;
    }

    public void removePurchaseEventListener() {
        mPurchaseEventListener = null;
    }

    public void init(String sdkboxConfigJsonString) {
        if(mBillingClient != null) {
            mBillingClient.endConnection();
            mBillingClient = null;
        }

        try {
            JSONObject configJson = new JSONObject(sdkboxConfigJsonString);
            JSONObject iapJson = configJson.getJSONObject("android").getJSONObject("iap");

            mLicenseKey = iapJson.getString("key");
            JSONObject items = iapJson.getJSONObject("items");

            mProductTypeMap.clear();
            for (Iterator<String> it = items.keys(); it.hasNext(); ) {
                String name = it.next();
                JSONObject item = items.getJSONObject(name);

                String productId = item.getString("id");
                String type = item.getString("type");
                if(!type.equals("consumable")) {
                    type = "non_consumable";
                }

                mProductTypeMap.put(productId, type);
            }
        }catch(JSONException e) {
            if(mPurchaseEventListener != null) {
                mPurchaseEventListener.onInitialized(false);
                return;
            }
        }

        mBillingClient = BillingClient.newBuilder(getActivity())
                .setListener(mPurchasesUpdatedListener)
                .enablePendingPurchases()
                .build();

        mBillingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                boolean isSuccess = false;

                if(billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                    Log.d(TAG, "BillingClient initialize success");
                    isSuccess = true;
                }else {
                    String message = String.format(Locale.ENGLISH,
                            "BillingClient initialize failed: responseCode=%d(%s), debugMessage=%s",
                            billingResult.getResponseCode(),
                            billingResponseCodeToString(billingResult.getResponseCode()),
                            billingResult.getDebugMessage());
                    Log.e(TAG, message);
                }

                if(mPurchaseEventListener != null) {
                    mPurchaseEventListener.onInitialized(isSuccess);
                }
            }

            @Override
            public void onBillingServiceDisconnected() {
                if(mBillingClient != null) {
                    mBillingClient = null;
                }
            }
        });
    }

    public boolean isInitialized() {
        return mBillingClient != null && mBillingClient.isReady();
    }

    public void queryProductListAsync() {
        if(!isInitialized()) {
            if(mPurchaseEventListener != null) {
                mPurchaseEventListener.onQueryProductListFailure("BillingClient is not initialized");
            }
            return;
        }

        SkuDetailsParams skuDetailsParams = SkuDetailsParams.newBuilder()
                .setSkusList(getSkuList())
                .setType(BillingClient.SkuType.INAPP)
                .build();

        mBillingClient.querySkuDetailsAsync(skuDetailsParams, new SkuDetailsResponseListener() {
            @Override
            public void onSkuDetailsResponse(@NonNull BillingResult billingResult, @Nullable List<SkuDetails> skuDetailsList) {
                mSkuDetailsMap.clear();

                if(mPurchaseEventListener == null) {
                    return;
                }

                if(skuDetailsList == null) {
                    mPurchaseEventListener.onQueryProductListFailure("skuDetailsList is empty");
                    return;
                }

                if(billingResult.getResponseCode() == BillingResponseCode.OK) {
                    for (SkuDetails skuDetails : skuDetailsList) {
                        mSkuDetailsMap.put(skuDetails.getSku(), skuDetails);
                    }

                    try {
                        JSONArray products = new JSONArray();
                        for(SkuDetails skuDetails : skuDetailsList) {
                            String type = getProductType(skuDetails.getSku());
                            Product product = Product.createFromSkuDetails(skuDetails, type);
                            products.put(product.toJson());
                        }

                        mPurchaseEventListener.onQueryProductListSuccess(products);
                    }catch(JSONException e) {
                        mPurchaseEventListener.onQueryProductListFailure(e.getMessage());
                    }
                }else {
                    String message = String.format(Locale.ENGLISH,
                            "queryProductListAsync failed: responseCode=%d(%s), debugMessage=%s",
                            billingResult.getResponseCode(),
                            billingResponseCodeToString(billingResult.getResponseCode()),
                            billingResult.getDebugMessage());
                    Log.e(TAG, message);
                    mPurchaseEventListener.onQueryProductListFailure(message);
                }
            }
        });
    }

    private @NonNull List<String> getSkuList() {
        return new ArrayList<>(mProductTypeMap.keySet());
    }

    /**
     * combined result queryPurchasesAsync() and queryPurchaseHistoryAsync()
     */
    public void getPurchaseHistory() {
        mPurchaseHistoryList.clear();

        if(mPurchaseEventListener == null) {
            return;
        }

        if(!isInitialized()) {
            return;
        }

        mBillingClient.queryPurchasesAsync(BillingClient.SkuType.INAPP, new PurchasesResponseListener() {
            @Override
            public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @NonNull List<Purchase> purchases) {
                if(billingResult.getResponseCode() == BillingResponseCode.OK) {
                    for(Purchase purchase : purchases) {
                        try {
                            JSONObject record = new JSONObject();
                            record.put("productId", purchase.getSkus().get(0));
                            record.put("orderId", purchase.getOrderId());
                            record.put("purchaseTime", purchase.getPurchaseTime());
                            record.put("purchaseToken", purchase.getPurchaseToken());
                            record.put("originalJson", purchase.getOriginalJson());
                            record.put("signature", purchase.getSignature());
                            record.put("quantity", purchase.getQuantity());

                            String purchaseState = "UNKNOWN";
                            if(purchase.getPurchaseState() == Purchase.PurchaseState.PURCHASED) {
                                purchaseState = "PURCHASED";
                            }else if(purchase.getPurchaseState() == Purchase.PurchaseState.PENDING) {
                                purchaseState = "PENDING";
                            }else if(purchase.getPurchaseState() == Purchase.PurchaseState.UNSPECIFIED_STATE) {
                                purchaseState = "UNSPECIFIED_STATE";
                            }
                            record.put("purchaseState", purchaseState);

                            mPurchaseHistoryList.add(record);
                        }catch(Exception e) {
                        }
                    }
                }

                mBillingClient.queryPurchaseHistoryAsync(BillingClient.SkuType.INAPP, new PurchaseHistoryResponseListener() {
                    @Override
                    public void onPurchaseHistoryResponse(@NonNull BillingResult billingResult, @Nullable List<PurchaseHistoryRecord> purchaseHistoryRecords) {
                        if(billingResult.getResponseCode() == BillingResponseCode.OK && purchaseHistoryRecords != null) {
                            for(PurchaseHistoryRecord purchaseHistoryRecord : purchaseHistoryRecords) {
                                try {
                                    JSONObject record = new JSONObject();
                                    record.put("productId", purchaseHistoryRecord.getSkus().get(0));
                                    record.put("purchaseTime", purchaseHistoryRecord.getPurchaseTime());
                                    record.put("purchaseToken", purchaseHistoryRecord.getPurchaseToken());
                                    record.put("originalJson", purchaseHistoryRecord.getOriginalJson());
                                    record.put("signature", purchaseHistoryRecord.getSignature());
                                    record.put("quantity", purchaseHistoryRecord.getQuantity());

                                    mPurchaseHistoryList.add(record);
                                }catch(Exception e) {}
                            }
                        }

                        if(mPurchaseEventListener != null) {
                            JSONArray historyList = new JSONArray(mPurchaseHistoryList);
                            mPurchaseEventListener.onPurchaseHistoryRequestSuccess(historyList);
                        }
                    }
                });
            }
        });
    }

    public void queryPurchases() {
        mBillingClient.queryPurchasesAsync(BillingClient.SkuType.INAPP, new PurchasesResponseListener() {
            @Override
            public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @NonNull List<Purchase> purchases) {
                if(billingResult.getResponseCode() == BillingResponseCode.OK) {
                  try {
                      JSONArray productList = new JSONArray();
                      for(Purchase purchase : purchases) {
                          String productId = purchase.getSkus().get(0);
                          SkuDetails skuDetails = getSkuDetailsByProductId(productId);
                          if(skuDetails == null) {
                              continue;
                          }

                          Product product = getProductFromSkuDetails(skuDetails);
                          product.transactionID = purchase.getOrderId();
                          product.receipt = purchase.getOriginalJson();
                          product.receiptCipheredPayload = purchase.getSignature();

                          productList.put(product.toJson());
                      }

                      mPurchaseEventListener.onQueryPurchasesSuccess(productList);
                  }catch(JSONException e) {
                      mPurchaseEventListener.onQueryPurchasesFailure(BillingResponseCode.ERROR, e.getMessage());
                  }
                }else {
                    mPurchaseEventListener.onQueryPurchasesFailure(billingResult.getResponseCode(), billingResult.getDebugMessage());
                }
            }
        });
    }

    public void purchase(String productId) {
        mCurrentRequestPurchaseSkuDetails = null;

        if(mPurchaseEventListener == null) {
            return;
        }

        if(!isInitialized()) {
            return;
        }

        SkuDetails skuDetails = getSkuDetailsByProductId(productId);
        if(skuDetails == null) {
            return;
        }

        mCurrentRequestPurchaseSkuDetails = skuDetails;

        BillingFlowParams params = BillingFlowParams.newBuilder()
                .setSkuDetails(skuDetails)
                .build();
        mBillingClient.launchBillingFlow(getActivity(), params);
    }

    public void consume(final String productId) {
        if(mPurchaseEventListener == null) {
            return;
        }

        if(!isInitialized()) {
            return;
        }

        // Unconsumed purchases should be able to be retrieved with queryPurchasesAsync()
        mBillingClient.queryPurchasesAsync(BillingClient.SkuType.INAPP, new PurchasesResponseListener() {
            @Override
            public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @NonNull List<Purchase> purchases) {
                SkuDetails skuDetails = getSkuDetailsByProductId(productId);
                if(skuDetails == null) {
                    return;
                }
                final Product product = getProductFromSkuDetails(skuDetails);

                if(billingResult.getResponseCode() != BillingResponseCode.OK) {
                    mPurchaseEventListener.onConsumeFailure(product, billingResult.getResponseCode(), billingResult.getDebugMessage());
                    return;
                }

                Purchase targetPurchase = null;
                for(Purchase purchase : purchases) {
                    if(purchase.getSkus().get(0).equals(productId)) {
                        targetPurchase = purchase;
                        break;
                    }
                }

                if(targetPurchase == null) {
                    mPurchaseEventListener.onConsumeFailure(product, BillingResponseCode.ERROR, "no target purchase");
                    return;
                }

                product.transactionID = targetPurchase.getOrderId();
                product.receipt = targetPurchase.getOriginalJson();
                product.receiptCipheredPayload = targetPurchase.getSignature();

                ConsumeParams params = ConsumeParams.newBuilder()
                        .setPurchaseToken(targetPurchase.getPurchaseToken())
                        .build();

                mBillingClient.consumeAsync(params, new ConsumeResponseListener() {
                    @Override
                    public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
                        if(billingResult.getResponseCode() == BillingResponseCode.OK) {
                            mPurchaseEventListener.onConsumeSuccess(product);
                        }else {
                            mPurchaseEventListener.onConsumeFailure(product, billingResult.getResponseCode(), billingResult.getDebugMessage());
                        }
                    }
                });
            }
        });
    }

    private @Nullable SkuDetails getSkuDetailsByProductId(String productId) {
        return mSkuDetailsMap.get(productId);
    }

    private @NonNull String getProductType(String productId) {
        return Objects.requireNonNull(mProductTypeMap.get(productId));
    }

    private static String billingResponseCodeToString(int responseCode) {
        switch(responseCode) {
            case BillingResponseCode.OK:
                return "OK";
            case BillingResponseCode.USER_CANCELED:
                return "USER_CANCELED";
            case BillingResponseCode.SERVICE_UNAVAILABLE:
                return "SERVICE_UNAVAILABLE";
            case BillingResponseCode.SERVICE_TIMEOUT:
                return "SERVICE_TIMEOUT";
            case BillingResponseCode.SERVICE_DISCONNECTED:
                return "SERVICE_DISCONNECTED";
            case BillingResponseCode.ITEM_UNAVAILABLE:
                return "ITEM_UNAVAILABLE";
            case BillingResponseCode.ITEM_NOT_OWNED:
                return "ITEM_NOT_OWNED";
            case BillingResponseCode.ITEM_ALREADY_OWNED:
                return "ITEM_ALREADY_OWNED";
            case BillingResponseCode.FEATURE_NOT_SUPPORTED:
                return "FEATURE_NOT_SUPPORTED";
            case BillingResponseCode.ERROR:
                return "ERROR";
            case BillingResponseCode.DEVELOPER_ERROR:
                return "DEVELOPER_ERROR";
            case BillingResponseCode.BILLING_UNAVAILABLE:
                return "BILLING_UNAVAILABLE";
            default:
                return "UNKNOWN";
        }
    }


}

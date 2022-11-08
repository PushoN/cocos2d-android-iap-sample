package iap;

import com.android.billingclient.api.SkuDetails;

import org.json.JSONException;
import org.json.JSONObject;

public class Product {
    String id;
    String type;
    String title;
    String description;
    float priceValue;
    String price;
    String currencyCode;
    String receiptCipheredPayload;
    String receipt;
    String transactionID;
    int quantity;

    public static Product createFromSkuDetails(SkuDetails skuDetails, String type) {
        Product product = new Product();
        product.id = skuDetails.getSku();
        product.type = type;
        product.price = skuDetails.getPrice();
        product.title = skuDetails.getTitle();
        product.description = skuDetails.getDescription();
        product.currencyCode = skuDetails.getPriceCurrencyCode();
        product.receipt = "";
        product.receiptCipheredPayload = "";
        product.transactionID = "";
        product.quantity = 1;

        double priceValue = (double)skuDetails.getPriceAmountMicros() / 1000000.0;
        product.priceValue = (float)priceValue;

        return product;
    }

    public JSONObject toJson() throws JSONException {
        JSONObject json = new JSONObject();

        json.put("id", id);
        json.put("type", type);
        json.put("title", title);
        json.put("description", description);
        json.put("price", price);
        json.put("priceValue", priceValue);
        json.put("currencyCode", currencyCode);
        json.put("receipt", receipt);
        json.put("receiptCipheredPayload", receiptCipheredPayload);
        json.put("transactionID", transactionID);
        json.put("quantity", quantity);

        return json;
    }

}

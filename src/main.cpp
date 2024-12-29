#include "deribit_api.h"
#include "json.hpp"
#include <iostream>
using namespace std;
using json = nlohmann::json;

string getEnvVar(const string& varName) {
    const char* value = getenv(varName.c_str());
    if (!value) {
        throw runtime_error(varName + " environment variable not set");
    }
    return string(value);
}

int main() {
    string clientId = getEnvVar("CLIENT_ID");
    string clientSecret = getEnvVar("CLIENT_SECRET");
    cout<< "Client ID: " << clientId << endl;
    cout<< "Client Secret: " << clientSecret << endl;
    DeribitAPI api(clientId, clientSecret);

    if (!api.authenticate()) {
        cerr << "Failed to authenticate" << endl;
        return 1;
    }

    string orderBook = api.getOrderBook("BTC-PERPETUAL");
    cout << "Order Book: " << orderBook << endl;


    // string instruments = api.getInstruments("BTC", "option");
    // if (instruments.empty()) {
    //     cerr << "Failed to fetch instruments." << endl;
    //     return 1;
    // }
    // auto jsonResponse = json::parse(instruments);
    // for (const auto &instrument : jsonResponse["result"]) {
    //     cout << "Instrument Name: " << instrument["instrument_name"] << endl;
    //     cout << "Kind: " << instrument["kind"] << endl;
    // }
    

    string accountSummary = api.getAccountSummary("BTC");
    if (accountSummary.empty()) {
        cerr << "Failed to fetch account summary." << endl;
        return 1;
    }
    auto jsonResponseAccountSummary = json::parse(accountSummary);
    if (jsonResponseAccountSummary.contains("result")) {
        auto result = jsonResponseAccountSummary["result"];
        cout << "Balance: " << result["balance"] << endl;
        cout << "Equity: " << result["equity"] << endl;
        cout << "Available Funds: " << result["available_funds"] << endl;
    }

    // string ticker = api.getTicker("BTC-PERPETUAL");
    // if (ticker.empty()) {
    //     cerr << "Failed to fetch ticker." << endl;
    //     return 1;
    // }
    // auto jsonResponse = json::parse(ticker);
    // if(jsonResponse.contains("result")){
    //     auto result = jsonResponse["result"];
    //     cout<< "Timestamp: " << result["timestamp"] << endl;
    //     cout<< "Instrument Name: " << result["instrument_name"] << endl;
    //     cout<< "Current Price: " << result["last_price"] << endl;
    //     cout << "Volume: " << result["stats"]["volume"] << endl;
    //     cout << "Best Ask Price: " << result["best_ask_price"] << endl;
    //     cout << "Best Bid Price: " << result["best_bid_price"] << endl;
    // }

    string placeOrder = api.placeOrder("BTC-PERPETUAL", "buy", 10, 5000, "limit");
    if (placeOrder.empty()) {
        cerr << "Failed to place order." << endl;
        return 1;
    }
    auto jsonResponsePlaceOrder = json::parse(placeOrder);

    if(jsonResponsePlaceOrder.contains("result")){
        auto result = jsonResponsePlaceOrder["result"];
        cout<< "Order ID: " << result["order"]["order_id"] << endl;
        cout<< "Instrument Name: " << result["order"]["instrument_name"] << endl;
        cout<< "Amount: " << result["order"]["amount"] << endl;
        cout<< "Price: " << result["order"]["price"] << endl;
        cout<< "Type: " << result["order"]["order_type"] << endl;
    } 
    else if(jsonResponsePlaceOrder.contains("error")){
        cout<< "Error: " << jsonResponsePlaceOrder["error"]["message"] << endl;
    }

    string openOrders = api.getOpenOrders();
    if (openOrders.empty()) {
        cerr << "Failed to fetch open orders." << endl;
        return 1;
    }
    auto jsonResponse = json::parse(openOrders);
    if(jsonResponse.contains("result")){
        auto result = jsonResponse["result"];
        for (const auto &order : result) {
            cout << "Order ID: " << order["order_id"] << endl;
            cout << "Instrument Name: " << order["instrument_name"] << endl;
            cout << "Amount: " << order["amount"] << endl;
            cout << "Price: " << order["price"] << endl;
            cout << "Type: " << order["order_type"] << endl<<endl;
        }
    }

    // string cancelOrder = api.cancelOrder("30585785830");
    // if (cancelOrder.empty()) {
    //     cerr << "Failed to cancel order." << endl;
    //     return 1;
    // }
    // cout<< "Cancel Order: " << cancelOrder << endl;


    // cout<< "Open Orders: " << openOrders << endl;

    // string modifyOrder = api.modifyOrder("30587101348", 25000, 20);
    // if (modifyOrder.empty()) {
    //     cerr << "Failed to modify order." << endl;
    //     return 1;
    // }
    // cout<< "Modify Order: " << modifyOrder << endl;

    api.startWebSocket({"BTC-PERPETUAL","ETH-PERPETUAL"});



    return 0;
}

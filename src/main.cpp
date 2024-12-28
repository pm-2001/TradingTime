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
    DeribitAPI api(clientId, clientSecret);

    if (!api.authenticate()) {
        cerr << "Failed to authenticate" << endl;
        return 1;
    }

    // string orderBook = api.getOrderBook("BTC-PERPETUAL");
    // cout << "Order Book: " << orderBook << endl;


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
    

    // string accountSummary = api.getAccountSummary("BTC");
    // if (accountSummary.empty()) {
    //     cerr << "Failed to fetch account summary." << endl;
    //     return 1;
    // }
    // auto jsonResponse = json::parse(accountSummary);
    // if (jsonResponse.contains("result")) {
    //     auto result = jsonResponse["result"];
    //     cout << "Balance: " << result["balance"] << endl;
    //     cout << "Equity: " << result["equity"] << endl;
    //     cout << "Available Funds: " << result["available_funds"] << endl;
    // }

    string ticker = api.getTicker("BTC-PERPETUAL");
    if (ticker.empty()) {
        cerr << "Failed to fetch ticker." << endl;
        return 1;
    }
    auto jsonResponse = json::parse(ticker);
    if(jsonResponse.contains("result")){
        auto result = jsonResponse["result"];
        cout<< "Timestamp: " << result["timestamp"] << endl;
        cout<< "Instrument Name: " << result["instrument_name"] << endl;
        cout<< "Current Price: " << result["last_price"] << endl;
        cout << "Volume: " << result["stats"]["volume"] << endl;
        cout << "Best Ask Price: " << result["best_ask_price"] << endl;
        cout << "Best Bid Price: " << result["best_bid_price"] << endl;
    }

    return 0;
}

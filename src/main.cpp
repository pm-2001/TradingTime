
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

void handlePlaceOrder(DeribitAPI& api) {
    string instrument_name, side, type;
    double amount, price;

    cout << "Enter Instrument Name (e.g., BTC-PERPETUAL): ";
    cin >> instrument_name;
    cout << "Enter Side (buy/sell): ";
    cin >> side;
    cout << "Enter Amount: ";
    cin >> amount;
    cout << "Enter Price: ";
    cin >> price;
    cout << "Enter Type (default: limit): ";
    cin >> type;

    string response = api.placeOrder(instrument_name, side, amount, price, type);
    cout << "Place Order Response: " << response << endl;
}

void handleCancelOrder(DeribitAPI& api) {
    string orderId;
    cout << "Enter Order ID to cancel: ";
    cin >> orderId;

    string response = api.cancelOrder(orderId);
    cout << "Cancel Order Response: " << response << endl;
}

void handleModifyOrder(DeribitAPI& api) {
    string orderId;
    double newPrice, newAmount;

    cout << "Enter Order ID to modify: ";
    cin >> orderId;
    cout << "Enter New Price: ";
    cin >> newPrice;
    cout << "Enter New Amount: ";
    cin >> newAmount;

    string response = api.modifyOrder(orderId, newPrice, newAmount);
    cout << "Modify Order Response: " << response << endl;
}

void handleAccountSummary(DeribitAPI& api) {
    string currency;
    cout << "Enter Currency (e.g., BTC): ";
    cin >> currency;

    string accountSummary = api.getAccountSummary(currency);
    if (accountSummary.empty()) {
        cerr << "Failed to fetch account summary." << endl;
        return;
    }
    auto jsonResponseAccountSummary = json::parse(accountSummary);
    if (jsonResponseAccountSummary.contains("result")) {
        auto result = jsonResponseAccountSummary["result"];
        cout<<"-----------------------------"<<endl;
        cout << "Account Summary:" << endl;
        cout << "\tBalance: " << result["balance"] << endl;
        cout << "\tEquity: " << result["equity"] << endl;
        cout << "\tAvailable Funds: " << result["available_funds"] << endl;
        cout<<"-----------------------------"<<endl;
    }
}

void handleMarketData(DeribitAPI& api) {
    string choice, symbol, currency, kind;

    cout << "1. Order Book\n2. Ticker\n3. Instruments\nChoose Market Data Type: ";
    cin >> choice;

    if (choice == "1") {
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        cout << "Order Book: " << api.getOrderBook(symbol) << endl;
    } else if (choice == "2") {
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        cout << "Ticker: " << api.getTicker(symbol) << endl;
    } else if (choice == "3") {
        cout << "Enter Currency (e.g., BTC): ";
        cin >> currency;
        cout << "Enter Kind (default: future): ";
        cin >> kind;
        cout << "Instruments: " << api.getInstruments(currency, kind) << endl;
    } else {
        cout << "Invalid choice." << endl;
    }
}

void handleWebSocket(DeribitAPI& api) {
    vector<string> symbols;
    int count;

    cout << "Enter number of symbols to subscribe to: ";
    cin >> count;

    for (int i = 0; i < count; ++i) {
        string symbol;
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        symbols.push_back(symbol);
    }

    api.startWebSocket(symbols);
}

int main() {
    try {
        string clientId = getEnvVar("CLIENT_ID");
        string clientSecret = getEnvVar("CLIENT_SECRET");
        DeribitAPI api(clientId, clientSecret);

        if (!api.authenticate()) {
            cerr << "Failed to authenticate." << endl;
            return 1;
        }

        while (true) {
            string choice;
            cout << "\n=== Deribit API Menu ===\n";
            cout << "1. Place Order\n2. Cancel Order\n3. Modify Order\n4. Account Summary\n5. Market Data\n6. WebSocket\n7. Exit\nChoose an option: ";
            cin >> choice;

            if (choice == "1") {
                handlePlaceOrder(api);
            } else if (choice == "2") {
                handleCancelOrder(api);
            } else if (choice == "3") {
                handleModifyOrder(api);
            } else if (choice == "4") {
                handleAccountSummary(api);
            } else if (choice == "5") {
                handleMarketData(api);
            } else if (choice == "6") {
                handleWebSocket(api);
            } else if (choice == "7") {
                break;
            } else {
                cout << "Invalid option." << endl;
            }
        }

    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}

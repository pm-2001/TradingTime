#include "deribit_api.h"
#include "json.hpp"
#include <iostream>
#include <iomanip>
using namespace std;
using json = nlohmann::json;

string getEnvVar(const string &varName)
{
    const char *value = getenv(varName.c_str());
    if (!value)
    {
        throw runtime_error(varName + " environment variable not set");
    }
    return string(value);
}

void handlePlaceOrder(DeribitAPI &api)
{
    string instrument_name, side, orderType;
    double amount, price;

    cout << "------------------------------PLACE AN ORDER------------------------------" << endl;
    cout << "\tEnter Instrument Name (e.g., BTC-PERPETUAL): ";
    cin >> instrument_name;
    cout << "\tEnter Side (buy/sell): ";
    cin >> side;
    cout << "\tEnter Amount: ";
    cin >> amount;
    cout << "\tEnter Price: ";
    cin >> price;
    cout << "\tEnter 1 for limit order or 2 for market order: ";
    cin >> orderType;
    while (orderType != "1" && orderType != "2")
    {
        cout << "\tInvalid Type. Enter 1 for limit order or 2 for market order: ";
        cin >> orderType;
    }
    string type = (orderType == "1") ? "limit" : "market";

    string response = api.placeOrder(instrument_name, side, amount, price, type);
    auto jsonResponse = json::parse(response);
    if (jsonResponse.contains("result") && jsonResponse["result"].contains("order"))
    {
        string orderState = jsonResponse["result"]["order"]["order_state"];
        if (orderState == "open")
            cout << "\t--------Order Placed Succesfully--------" << endl;
        else
            cout << "\t--------Order not placed--------" << endl;
    }
    else if (jsonResponse.contains("error"))
        cout << "\tError placing order." << endl
             << "Message: " << jsonResponse["error"]["message"] << endl;
    else
        cerr << "\tInvalid JSON response or missing 'order_state' field." << endl;
    cout << "--------------------------------------------------------------------------" << endl;
}

void handleOpenOrder(DeribitAPI &api)
{
    string openOrders = api.getOpenOrders();
    cout << "---------------------------------------OPEN ORDERS---------------------------------------" << endl;
    if (openOrders.empty())
        cerr << "Failed to fetch open orders." << endl;
    else
    {
        auto jsonResponse = json::parse(openOrders);
        if (jsonResponse.contains("result"))
        {
            cout << left << setw(15) << "Order ID" << left << setw(20) << "Instrument Name" << left << setw(15) << "Amount" << left << setw(15) << "Price" << left << setw(10) << "Type" << endl;
            cout << string(75, '-') << endl;

            auto result = jsonResponse["result"];
            for (const auto &order : result)
            {
                cout << left << setw(15) << order["order_id"]
                     << left << setw(20) << order["instrument_name"]
                     << left << setw(15) << order["amount"]
                     << left << setw(15) << order["price"]
                     << left << setw(10) << order["order_type"] << endl;
            }
        }
    }
    cout << "----------------------------------------------------------------------------------------" << endl;
}

void handleCancelOrder(DeribitAPI &api)
{
    cout << "---------------------------------------CANCEL ORDER---------------------------------------" << endl;
    string orderId;
    cout << "\tEnter Order ID to cancel: ";
    cin >> orderId;

    string response = api.cancelOrder(orderId);
    auto parsedJson = json::parse(response);

    if (parsedJson.contains("result") && parsedJson["result"].contains("order_state"))
    {
        string orderState = parsedJson["result"]["order_state"];
        if (orderState == "cancelled")
            cout << "\tOrder cancelled successfully." << endl;
        else
            cout << "\tOrder not cancelled." << endl;
    }
    else
        cerr << "\tInvalid JSON response or missing 'order_state' field." << endl;
    cout << "----------------------------------------------------------------------------------------" << endl;
}

void handleModifyOrder(DeribitAPI &api)
{
    string orderId;
    double newPrice, newAmount;
    cout<< "---------------------------------------MODIFY ORDER---------------------------------------" << endl;
    cout << "\tEnter Order ID to modify: ";
    cin >> orderId;
    cout << "\tEnter New Price: ";
    cin >> newPrice;
    cout << "\tEnter New Amount: ";
    cin >> newAmount;

    string response = api.modifyOrder(orderId, newPrice, newAmount);
    auto jsonResponse = json::parse(response);
    if (jsonResponse.contains("result") && jsonResponse["result"].contains("order"))
    {
        string orderState = jsonResponse["result"]["order"]["order_state"];
        if (orderState == "open")
            cout << "\t--------Order Modified Succesfully with Order_id :"<<jsonResponse["result"]["order"]["order_id"]<<"--------" << endl;
        else
            cout << "\t--------Order Not Modified--------" << endl;
    }
    else if (jsonResponse.contains("error"))
        cout << "\tError placing order." << endl
             << "Message: " << jsonResponse["error"]["message"] << endl;
    else
        cerr << "\tInvalid JSON response or missing 'order_state' field." << endl;
    cout << "----------------------------------------------------------------------------------------" << endl;
}

void handleAccountSummary(DeribitAPI &api)
{
    string currency;
    cout << "Enter Currency (e.g., BTC): ";
    cin >> currency;

    string accountSummary = api.getAccountSummary(currency);
    cout<<accountSummary<<endl;
    cout<< "---------------------------------------ACCOUNT SUMMARY---------------------------------------" << endl;
    if (accountSummary.empty())
    {
        cerr << "Failed to fetch account summary." << endl;
        return;
    }
    auto jsonResponseAccountSummary = json::parse(accountSummary);
    if (jsonResponseAccountSummary.contains("result"))
    {
        auto result = jsonResponseAccountSummary["result"];
        cout << "+" << setw(15) << setfill('-') << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(20) << "-" << "+" << endl;

    cout << "|" << setw(14) << setfill(' ') << left << "Currency"
         << "|" << setw(14) << left << "Balance"
         << "|" << setw(14) << left << "Equity"
         << "|" << setw(19) << left << "Available Funds" << "|" << endl;

    cout << "+" << setw(15) << setfill('-') << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(20) << "-" << "+" << endl;

    // Table Content
    cout << "|" << setw(22-result["currency"].size()) << setfill(' ') << left << result["currency"]
         << "|" << setw(14) << left << result["balance"]
         << "|" << setw(14) << left << result["equity"]
         << "|" << setw(19) << left << result["available_funds"] << "|" << endl;

    cout << "+" << setw(15) << setfill('-') << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(15) << "-"
         << "+" << setw(20) << "-" << "+" << endl;
    }
    cout << "----------------------------------------------------------------------------------------" << endl;
}

void handleMarketData(DeribitAPI &api)
{
    string choice, symbol, currency, kind;

    cout << "1. Order Book\n2. Ticker\n3. Instruments\nChoose Market Data Type: ";
    cin >> choice;

    if (choice == "1")
    {
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        cout << "Order Book: " << api.getOrderBook(symbol) << endl;
    }
    else if (choice == "2")
    {
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        cout << "Ticker: " << api.getTicker(symbol) << endl;
    }
    else if (choice == "3")
    {
        cout << "Enter Currency (e.g., BTC): ";
        cin >> currency;
        cout << "Enter Kind (default: future): ";
        cin >> kind;
        cout << "Instruments: " << api.getInstruments(currency, kind) << endl;
    }
    else
    {
        cout << "Invalid choice." << endl;
    }
}

void handleWebSocket(DeribitAPI &api)
{
    vector<string> symbols;
    int count;

    cout << "Enter number of symbols to subscribe to: ";
    cin >> count;

    for (int i = 0; i < count; ++i)
    {
        string symbol;
        cout << "Enter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        symbols.push_back(symbol);
    }

    api.startWebSocket(symbols);
}

int main()
{
    try
    {
        string clientId = getEnvVar("CLIENT_ID");
        string clientSecret = getEnvVar("CLIENT_SECRET");
        DeribitAPI api(clientId, clientSecret);

        if (!api.authenticate())
        {
            cerr << "Failed to authenticate." << endl;
            return 1;
        }

        while (true)
        {
            string choice;
            cout << "\n=== Deribit API Menu ===\n";
            cout << "1. Place Order\n2. Open Orders\n3. Cancel Order\n4. Modify Order\n5. Account Summary\n6. Market Data\n7. WebSocket\n8. Exit\nChoose an option: ";
            cin >> choice;

            if (choice == "1")
            {
                handlePlaceOrder(api);
            }
            else if (choice == "2")
            {
                handleOpenOrder(api);
            }
            else if (choice == "3")
            {
                handleCancelOrder(api);
            }
            else if (choice == "4")
            {
                handleModifyOrder(api);
            }
            else if (choice == "5")
            {
                handleAccountSummary(api);
            }
            else if (choice == "6")
            {
                handleMarketData(api);
            }
            else if (choice == "7")
            {
                handleWebSocket(api);
            }
            else if (choice == "8")
            {
                break;
            }
            else
            {
                cout << "Invalid option." << endl;
            }
        }
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}

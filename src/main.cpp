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
    cout<<string(40,'-')<<"OPEN ORDERS"<<string(40,'-')<<endl<<endl;
    if (openOrders.empty())
        cerr << "Failed to fetch open orders." << endl;
    else
    {
        auto jsonResponse = json::parse(openOrders);
        if (jsonResponse.contains("result"))
        {
            cout << left << setw(24) <<setfill(' ') << "ORDER ID" << left << setw(24) << "INSTRUMENT NAME" << left << setw(15) << "AMOUNT" << left << setw(15) << "PRICE" << left << setw(10) << "TYPE" << endl;
            cout << string(91, '-') << endl;

            auto result = jsonResponse["result"];
            for (const auto &order : result)
            {
                string orderId = order["order_id"];
                string instrumentName = order["instrument_name"];
                double amount = order["amount"];
                double price = order["price"];
                string type = order["order_type"];
                cout<<left<<setw(24)<<setfill(' ') <<orderId<<left<<setw(24)<<instrumentName<<left<<setw(15)<<amount<<left<<setw(15)<<price<<left<<setw(10)<<type<<endl;
            }
        }
    }
    cout<<string(91,'-')<<endl;
}

void handleCancelOrder(DeribitAPI &api)
{
    cout<<string(40,'-')<<"CANCEL ORDER"<<string(40,'-')<<endl;
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
    cout<<string(91,'-')<<endl;
}

void handleModifyOrder(DeribitAPI &api)
{
    string orderId;
    double newPrice, newAmount;
    cout<<string(40,'-')<<"MODIFY ORDER"<<string(40,'-')<<endl;
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
    cout<<string(91,'-')<<endl;
}

void handleAccountSummary(DeribitAPI &api)
{
    string currency;
    cout << "Enter Currency (e.g., BTC): ";
    cin >> currency;

    string accountSummary = api.getAccountSummary(currency);
    cout<<string(35,'-')<<"ACCOUNT SUMMARY"<<string(35,'-')<<endl;
    if (accountSummary.empty())
    {
        cerr << "Failed to fetch account summary." << endl;
        return;
    }
    auto jsonResponseAccountSummary = json::parse(accountSummary);
    if (jsonResponseAccountSummary.contains("result"))
    {
        auto result = jsonResponseAccountSummary["result"];
        string currency = result["currency"];
        double balance = result["balance"];
        double equity = result["equity"];
        double available_funds = result["available_funds"];
        cout<<"\t+"<<setw(14)<<setfill('-')<<"-"<<"+"<<setw(14)<<"-"<<"+"<<setw(14)<<"-"<<"+"<<setw(19)<<"-"<<"+"<<endl;
        cout<<"\t|"<<setw(14)<<setfill(' ')<<left<<"Currency"<<"|"<<setw(14)<<left<<"Balance"<<"|"<<setw(14)<<left<<"Equity"<<"|"<<setw(19)<<left<<"Available Funds"<<"|"<<endl;
        cout<<"\t+"<<setw(14)<<setfill('-')<<"-"<<"+"<<setw(14)<<"-"<<"+"<<setw(14)<<"-"<<"+"<<setw(19)<<"-"<<"+"<<endl;

        cout<<"\t|"<<setw(14)<<setfill(' ')<<left<<currency<<"|"<<setw(14)<<left<<balance<<"|"<<setw(14)<<left<<equity<<"|"<<setw(19)<<left<<available_funds<<"|"<<endl;
        cout << "\t+" << setw(14) << setfill('-') << "-"<< "+" << setw(14) << "-"<< "+" << setw(14) << "-"<< "+" << setw(19) << "-" << "+" << endl;
    }
    cout<<string(85,'-')<<endl;
}

void handleMarketData(DeribitAPI &api)
{
    string choice, symbol, currency, kind;
    cout<<string(56,'-')<<"MARKET DATA"<<string(56,'-')<<endl;
    cout << "\t1. Order Book\n\t2. Ticker\n\t3. Instruments\n\tChoose Market Data Type: ";
    cin >> choice;
    string header = choice == "1" ? "ORDER BOOK" : choice == "2" ? "TICKER" : "INSTRUMENTS";
    cout<<string(53,'-')<<header<<string(53,'-')<<endl;
    if (choice == "1")
    {
        cout << "\t\tEnter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        string orderBook = api.getOrderBook(symbol);
        if (orderBook.empty())
        {
            cerr << "Failed to fetch order book." << endl;
            return;
        }
        auto jsonResponseOrderBook = json::parse(orderBook);
        if (jsonResponseOrderBook.contains("result"))
        {
            auto result = jsonResponseOrderBook["result"];
            string instrumentName = result["instrument_name"];
            double bestBidAmount = result["best_bid_amount"];
            double bestAskPrice = result["best_ask_price"];
            double bestAskAmount = result["best_ask_amount"];
            double lastPrice = result["last_price"];
            double indexPrice = result["index_price"];
            auto bids = result["bids"];
            auto asks = result["asks"];
            
            cout << fixed << setprecision(2);
            cout << "\t\tInstrument: " <<instrumentName<< endl;
            cout << "\t\tLast Price: " << lastPrice << endl;
            cout << "\t\tIndex Price: " << indexPrice << endl;
            cout << "\t\tBest Bid: " <<result["best_bid_price"]<< " (" << bestBidAmount << ")" << endl;
            cout << "\t\tBest Ask: " << bestAskPrice << " (" << bestAskAmount << ")" << endl;
            cout << "\n\t\tOrder Book Depth (Top 5):\n";
            cout <<"\t\t"<<setw(15)<<setfill(' ')<< "Bids" << setw(15) << "Amount" << "   |   " << setw(15) << "Asks" << setw(15) << "Amount" << endl;
            for (size_t i = 0; i < 5; ++i) {
                double bidPrice = bids[i][0];
                double bidAmount = bids[i][1];
                double askPrice = asks[i][0];
                double askAmount = asks[i][1];
                cout <<"\t\t"<< setw(15)<<setfill(' ')<< bidPrice << setw(15) << bidAmount << "   |   " << setw(15) << askPrice << setw(15) << askAmount << endl;
            }
        }
    }
    else if (choice == "2")
    {
        cout << "\t\tEnter Symbol (e.g., BTC-PERPETUAL): ";
        cin >> symbol;
        string ticker = api.getTicker(symbol);
        if (ticker.empty())
        {
            cerr << "Failed to fetch ticker." << endl;
            return;
        }
        auto jsonResponseTicker = json::parse(ticker);
        if (jsonResponseTicker.contains("result"))
        {
            auto result = jsonResponseTicker["result"];
            string instrumentName = result["instrument_name"];
            double lastPrice = result["last_price"];
            double indexPrice = result["index_price"];
            double markPrice = result["mark_price"];
            double highPrice = result["stats"]["high"];
            double lowPrice = result["stats"]["low"];
            double priceChange = result["stats"]["price_change"];
            double volume = result["stats"]["volume"];
            double volumeUSD = result["stats"]["volume_usd"];
            double bestAskPrice = result["best_ask_price"];
            double bestAskAmount = result["best_ask_amount"];
            double bestBidPrice = result["best_bid_price"];
            double bestBidAmount = result["best_bid_amount"];
            double openInterest = result["open_interest"];
            double currentFunding = result["current_funding"];
            double funding8h = result["funding_8h"];

            // Display the information
            cout << fixed << setprecision(2);
            cout << "\t\tInstrument: " << instrumentName << endl;
            cout << "\t\tLast Price: " << lastPrice << endl;
            cout << "\t\tIndex Price: " << indexPrice << endl;
            cout << "\t\tMark Price: " << markPrice << endl;
            cout << "\t\t24H High: " << highPrice << "   24H Low: " << lowPrice << endl;
            cout << "\t\tPrice Change (24H): " << priceChange * 100 << "%" << endl;
            cout << "\t\tVolume (Contracts): " << volume << "   Volume (USD): $" << volumeUSD << endl;
            cout << "\t\tBest Ask: " << bestAskPrice << " (" << bestAskAmount << ")" << endl;
            cout << "\t\tBest Bid: " << bestBidPrice << " (" << bestBidAmount << ")" << endl;
            cout << "\t\tOpen Interest: " << openInterest << endl;
            cout << "\t\tCurrent Funding Rate: " << currentFunding * 100 << "%" << endl;
            cout << "\t\tFunding (8H): " << funding8h * 100 << "%" << endl;
        }
    }
    else if (choice == "3")
    {
        cout << "\t\tEnter Currency (e.g., BTC): ";
        cin >> currency;
        cout << "\t\tEnter Kind (default: future): ";
        cin >> kind;
        string instruments = api.getInstruments(currency, kind);
        if (instruments.empty())
        {
            cerr << "Failed to fetch instruments." << endl;
            return;
        }
        auto jsonResponseInstruments = json::parse(instruments);
        if (jsonResponseInstruments.contains("result"))
        {
            auto result = jsonResponseInstruments["result"];
            cout<<"+"<<setw(20)<<setfill('-')<<"-"<<"+"<<setw(20)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<endl;
            cout<<"|"<<left<<setw(20)<<setfill(' ') << "INSTRUMENT NAME"<<"|"<<setw(20) << "EXPIRATION TIME"<<"|"<<setw(15) << "CONTRACT SIZE"<<"|"<<setw(15) << "MAKER FEE"<<"|"<<setw(15) << "TAKER FEE"<<"|"<<setw(15) << "MIN TRADE"<<"|"<<setw(15) << "TICK SIZE"<<"|"<<endl;
            cout<<"+"<<setw(20)<<setfill('-')<<"-"<<"+"<<setw(20)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<endl;

            for (const auto& instrument : result) {
            string inst_name = instrument["instrument_name"];
            long long exp_timestamp = instrument["expiration_timestamp"];
            double contract_size = instrument["contract_size"];
            double maker_commission = instrument["maker_commission"];
            double taker_commission = instrument["taker_commission"];
            double min_trade_amt = instrument["min_trade_amount"];
            double tick_size = instrument["tick_size"];
            cout<<"|"<<left<<setw(20)<<setfill(' ') << inst_name<<"|"<<setw(20) << exp_timestamp<<"|"<<setw(15) <<contract_size<<"|"<<setw(15) <<maker_commission<<"|"<<setw(15) <<taker_commission<<"|"<<setw(15) <<min_trade_amt<<"|"<<setw(15) <<tick_size<<"|"<<endl;
            cout<<"+"<<setw(20)<<setfill('-')<<"-"<<"+"<<setw(20)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<setw(15)<<"-"<<"+"<<endl;
            }
        }
    }
    else
    {
        cout << "\t\tInvalid choice." << endl;
    }
    cout<<string(123,'-')<<endl;
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

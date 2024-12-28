#ifndef DERIBIT_API_H
#define DERIBIT_API_H

#include <string>
#include <curl/curl.h>
using namespace std;

class DeribitAPI {
private:
    string base_url;
    string client_id;
    string client_secret;
    string access_token;

    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, string *output);

public:
    DeribitAPI(const string &client_id, const string &client_secret);

    bool authenticate();
    string getOrderBook(const string &symbol);
    string getInstruments(const string &currency, const string &kind = "future");
    string getAccountSummary(const string &currency);
    string getTicker(const string &symbol);
    string placeOrder(const string &instrument_name, const string &side, double amount, double price, const string &type="limit");
    string getOpenOrders();
    string cancelOrder(const string &orderId);
    string modifyOrder(const string &orderId, double newPrice, double newAmount);
    void startWebSocket(const string &symbol);

};

#endif

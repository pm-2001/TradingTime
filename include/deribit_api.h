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

};

#endif

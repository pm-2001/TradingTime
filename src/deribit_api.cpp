#include "deribit_api.h"
#include <iostream>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

DeribitAPI::DeribitAPI(const string &client_id, const string &client_secret)
    : base_url("https://test.deribit.com/api/v2/"), client_id(client_id), client_secret(client_secret) {}

size_t DeribitAPI::WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
}

bool DeribitAPI::authenticate() {
    string url = base_url + "public/auth";
    string payload = R"({
        "id": 2,
        "method": "public/auth",
        "params": {
            "grant_type": "client_credentials",
            "scope": "session:apiconsole-8nbzq15dnbj expires:2592000 account:read",
            "client_id": ")" + client_id + R"(",
            "client_secret": ")" + client_secret + R"("
        },
        "jsonrpc": "2.0"
    })";

    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return false;
    }

    string response;
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Authentication failed: " << curl_easy_strerror(res) << endl;
        return false;
    }

    try {
        auto jsonResponse = json::parse(response);
        if (jsonResponse.contains("result") && jsonResponse["result"].contains("access_token")) {
            access_token = jsonResponse["result"]["access_token"];
            cout<<"Access Token: "<<access_token<<endl;
        } else {
            cerr << "Authentication response missing 'access_token'" << endl;
            return false;
        }
    } catch (json::parse_error &e) {
        cerr << "Error parsing authentication response: " << e.what() << endl;
        return false;
    }

    return true;
}

string DeribitAPI::getOrderBook(const string &symbol) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        cerr << "Failed to initialize CURL." << endl;
        return "";
    }

    string url = base_url + "public/get_order_book?instrument_name=" + symbol;
    string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to fetch order book: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}

string DeribitAPI::getInstruments(const string &currency, const string &kind) {
    CURL *curl = curl_easy_init();
    if (!curl) return "";

    string url = base_url + "public/get_instruments?currency=" + currency + "&kind=" + kind;
    string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to fetch instruments: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}

string DeribitAPI::getAccountSummary(const string &currency) {
    CURL *curl = curl_easy_init();
    if (!curl) return "";

    string url = base_url + "private/get_account_summary";
    string response;

    string post_fields = R"({
        "jsonrpc": "2.0", 
        "id": 42, 
        "method": "private/get_account_summary", 
        "params": {
            "currency": ")" +currency + R"("
        }
    })";

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_fields.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to fetch account summary: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}


string DeribitAPI::getTicker(const string &instrument_name) {
    CURL *curl = curl_easy_init();
    if (!curl) return "";

    string url = base_url + "public/ticker?instrument_name=" + instrument_name;
    string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        cerr << "Failed to fetch ticker: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}



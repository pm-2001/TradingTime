#include "deribit_api.h"
#include <iostream>
#include <sstream>
#include "json.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_tls_client.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WebSocketClient;

using json = nlohmann::json;
using namespace std;

DeribitAPI::DeribitAPI(const string &client_id, const string &client_secret)
    : base_url("https://test.deribit.com/api/v2/"), client_id(client_id), client_secret(client_secret) {}

size_t DeribitAPI::WriteCallback(void *contents, size_t size, size_t nmemb, string *output) {
    size_t totalSize = size * nmemb;
    output->append((char *)contents, totalSize);
    return totalSize;
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






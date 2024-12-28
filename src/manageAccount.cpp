#include "deribit_api.h"
#include <iostream>

using namespace std;

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
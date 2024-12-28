#include "deribit_api.h"
#include <iostream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

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

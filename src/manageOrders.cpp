#include "deribit_api.h"
#include <iostream>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

string DeribitAPI::placeOrder(const string &instrument_name, const string &side, double amount, double price, const string &type)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return "";

    string url = base_url + "private/" + side; // "private/buy" or "private/sell"
    string response;
    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 42},
        {"method", "private/" + side},
        {"params", {{"instrument_name", instrument_name}, {"amount", amount}, {"price", price}, {"type", type}}}};

    string payload_str = payload.dump();

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "Failed to place order: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}

string DeribitAPI::getOpenOrders()
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return "";

    string url = base_url + "private/get_open_orders";
    string response;

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "Failed to fetch open orders: " << curl_easy_strerror(res) << endl;
        return "";
    }
    return response;
}

string DeribitAPI::cancelOrder(const string &order_id)
{
    CURL *curl = curl_easy_init();
    if (!curl)
        return "";

    string url = base_url + "private/cancel";
    string response;

    json payload = {
        {"jsonrpc", "2.0"},
        {"id", 42},
        {"method", "private/cancel"},
        {"params", {{"order_id", order_id}}}};

    string payload_str = payload.dump();

    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, ("Authorization: Bearer " + access_token).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload_str.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
    {
        cerr << "Failed to cancel order: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}

string DeribitAPI::modifyOrder(const string &order_id, double new_price, double new_amount)
{
    string cancel_response = cancelOrder(order_id);

    try
    {
        auto jsonResponse = json::parse(cancel_response);
        if (!jsonResponse.contains("result"))
        {
            std::cerr << "Failed to cancel order for modification" << std::endl;
            return cancel_response;
        }
        else
        {
            string instrument_name = jsonResponse["result"]["instrument_name"];
            string side = jsonResponse["result"]["direction"];
            string type = jsonResponse["result"]["order_type"];
            return placeOrder(instrument_name, side, new_amount, new_price, type);
        }
    }
    catch (json::parse_error &e)
    {
        std::cerr << "Error parsing cancel response: " << e.what() << std::endl;
        return cancel_response;
    }
    return "Error modifying order";
}

string DeribitAPI::getOrderBook(const string &symbol)
{
    CURL *curl = curl_easy_init();
    if (!curl)
    {
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

    if (res != CURLE_OK)
    {
        cerr << "Failed to fetch order book: " << curl_easy_strerror(res) << endl;
        return "";
    }

    return response;
}

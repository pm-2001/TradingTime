#include "deribit_api.h"
#include <iostream>
using namespace std;

string getEnvVar(const string& varName) {
    const char* value = getenv(varName.c_str());
    if (!value) {
        throw runtime_error(varName + " environment variable not set");
    }
    return string(value);
}

int main() {
    string clientId = getEnvVar("CLIENT_ID");
    string clientSecret = getEnvVar("CLIENT_SECRET");
    DeribitAPI api(clientId, clientSecret);

    if (!api.authenticate()) {
        cerr << "Failed to authenticate" << endl;
        return 1;
    }

    string orderBook = api.getOrderBook("BTC-PERPETUAL");
    cout << "Order Book: " << orderBook << endl;

    return 0;
}

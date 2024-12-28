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

void DeribitAPI::startWebSocket(const string &symbol) {
    try {
        WebSocketClient ws_client;

        // Initialize the ASIO library
        ws_client.init_asio();

        // Set up SSL/TLS context for secure connection
        ws_client.set_tls_init_handler([](websocketpp::connection_hdl) {
            auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(
                boost::asio::ssl::context::tlsv12);
            ctx->set_options(boost::asio::ssl::context::default_workarounds |
                             boost::asio::ssl::context::no_sslv2 |
                             boost::asio::ssl::context::no_sslv3 |
                             boost::asio::ssl::context::single_dh_use);
            return ctx;
        });

        ws_client.set_message_handler([this, &ws_client](websocketpp::connection_hdl hdl, WebSocketClient::message_ptr msg) {
            string payload = msg->get_payload();
            auto jsonPayload = json::parse(payload);
            if(jsonPayload.contains("params") && jsonPayload["params"].contains("data")){
                auto data = jsonPayload["params"]["data"];
                cout<<"ASKS: "<<data["asks"]<<endl;
                cout<<"BIDS: "<<data["bids"]<<endl;
                cout<<"Instrument Name: "<<data["instrument_name"]<<endl;
                cout<<"Change Id: "<<data["change_id"]<<endl;
                cout<<"Timestamp: "<<data["timestamp"]<<endl<<endl;
            }
        });

        ws_client.set_open_handler([&ws_client, symbol](websocketpp::connection_hdl hdl) {
            string subscribe_msg = R"({
                "jsonrpc": "2.0",
                "id": 1,
                "method": "public/subscribe",
                "params": {
                    "channels": ["book.)" + symbol + R"(.none.10.100ms"]
                }
            })";
            ws_client.send(hdl, subscribe_msg, websocketpp::frame::opcode::text);
        });

        websocketpp::lib::error_code ec;
        WebSocketClient::connection_ptr con = ws_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);
        if (ec) {
            cerr << "WebSocket Connection Error: " << ec.message() << endl;
            return;
        }

        ws_client.connect(con);
        ws_client.run();

    } catch (const exception &e) {
        cerr << "WebSocket Exception: " << e.what() << endl;
    }
}
#include "deribit_api.h"
#include <iostream>
#include <sstream>
#include "json.hpp"
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <boost/asio/ssl/context.hpp>

typedef websocketpp::client<websocketpp::config::asio_tls_client> WebSocketClient;

using json = nlohmann::json;
using namespace std;


void reconnect_websocket(WebSocketClient &ws_client, websocketpp::connection_hdl hdl) {
    int reconnect_attempts = 0;
    const int max_reconnect_attempts = 5;
    const int reconnect_delay_ms = 3000; // 3 seconds

    while (reconnect_attempts < max_reconnect_attempts) {
        try {
            cout << "Reconnect attempt " << reconnect_attempts + 1 << "..." << endl;
            
            // Reinitialize connection
            websocketpp::lib::error_code ec;
            WebSocketClient::connection_ptr con = ws_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);
            if (ec) {
                cerr << "WebSocket Connection Error: " << ec.message() << endl;
                reconnect_attempts++;
                std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_delay_ms));
                continue;
            }

            ws_client.connect(con);
            cout << "Reconnected successfully!" << endl;
            return; // Exit the function upon successful reconnection
        } catch (const exception &e) {
            cerr << "WebSocket Reconnection Exception: " << e.what() << endl;
        }

        reconnect_attempts++;
        std::this_thread::sleep_for(std::chrono::milliseconds(reconnect_delay_ms));
    }

    cerr << "Failed to reconnect after " << max_reconnect_attempts << " attempts." << endl;
}
// void DeribitAPI::startWebSocket(const string &symbol) {
//     try {
//         WebSocketClient ws_client;
//         ws_client.init_asio();

//         // Add TLS initialization
//         ws_client.set_tls_init_handler([](websocketpp::connection_hdl hdl) -> websocketpp::lib::shared_ptr<boost::asio::ssl::context> {
//             auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
//             ctx->set_options(boost::asio::ssl::context::default_workarounds |
//                              boost::asio::ssl::context::no_sslv2 |
//                              boost::asio::ssl::context::no_sslv3 |
//                              boost::asio::ssl::context::single_dh_use);
//             return ctx;
//         });

//         ws_client.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
//             string payload = msg->get_payload();
//             auto jsonPayload = json::parse(payload);
//             if(jsonPayload.contains("params") && jsonPayload["params"].contains("data")){
//                 auto data = jsonPayload["params"]["data"];
//                 cout<<"ASKS: "<<data["asks"]<<endl;
//                 cout<<"BIDS: "<<data["bids"]<<endl;
//                 cout<<"Instrument Name: "<<data["instrument_name"]<<endl;
//                 cout<<"Change Id: "<<data["change_id"]<<endl;
//                 cout<<"Timestamp: "<<data["timestamp"]<<endl<<endl;
//             }
//         });

//         ws_client.set_close_handler([this, &ws_client](websocketpp::connection_hdl hdl) {
//             cout << "WebSocket Disconnected. Attempting to reconnect..." << endl;
//             reconnect_websocket(ws_client, hdl);
//         });

//         ws_client.set_open_handler([this, &ws_client, symbol](websocketpp::connection_hdl hdl) {
//             cout << "WebSocket Connected!" << endl;

//             string subscribe_msg = R"({
//                 "jsonrpc": "2.0",
//                 "id": 1,
//                 "method": "public/subscribe",
//                 "params": {
//                     "channels": ["book.)" + symbol + R"(.none.10.100ms"]
//                 }
//             })";
//             ws_client.send(hdl, subscribe_msg, websocketpp::frame::opcode::text);
//         });

//         // Establish Connection
//         websocketpp::lib::error_code ec;
//         WebSocketClient::connection_ptr con = ws_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);
//         if (ec) {
//             cerr << "WebSocket Connection Error: " << ec.message() << endl;
//             return;
//         }

//         ws_client.connect(con);
//         ws_client.run();
//     } catch (const exception &e) {
//         cerr << "WebSocket Exception: " << e.what() << endl;
//     }
// }




void DeribitAPI::startWebSocket(const std::vector<std::string> &symbols) {
    try {
        WebSocketClient ws_client;
        ws_client.init_asio();

        ws_client.set_tls_init_handler([](websocketpp::connection_hdl hdl) -> websocketpp::lib::shared_ptr<boost::asio::ssl::context> {
            auto ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::tlsv12);
            ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::no_sslv3 | boost::asio::ssl::context::single_dh_use);
            return ctx;
        });

        ws_client.set_message_handler([this](websocketpp::connection_hdl, WebSocketClient::message_ptr msg) {
            string payload = msg->get_payload();
            auto jsonPayload = json::parse(payload);
            if(jsonPayload.contains("params") && jsonPayload["params"].contains("data") && jsonPayload["params"]["data"].contains("change_id")){
                auto data = jsonPayload["params"]["data"];
                cout<<"ASKS: "<<data["asks"]<<endl;
                cout<<"BIDS: "<<data["bids"]<<endl;
                cout<<"Instrument Name: "<<data["instrument_name"]<<endl;
                cout<<"Change Id: "<<data["change_id"]<<endl;
                cout<<"Timestamp: "<<data["timestamp"]<<endl<<endl;
            }
        });

        ws_client.set_open_handler([this, &ws_client, symbols](websocketpp::connection_hdl hdl) {
            std::cout << "WebSocket Connected!" << std::endl;

            // Subscribe to all specified channels
            for (const auto &symbol : symbols) {
                std::vector<std::string> channels = {
                    "book." + symbol + ".none.10.100ms",
                    "trades." + symbol + ".raw",
                    "ticker." + symbol + ".100ms"
                };

                for (const auto &channel : channels) {
                    std::string subscribe_msg = R"({
                        "jsonrpc": "2.0",
                        "id": 1,
                        "method": "public/subscribe",
                        "params": {
                            "channels": [")" + channel + R"("]
                        }
                    })";
                    ws_client.send(hdl, subscribe_msg, websocketpp::frame::opcode::text);
                    std::cout << "Subscribed to channel: " << channel << std::endl;
                }
            }
        });

        // Establish Connection
        websocketpp::lib::error_code ec;
        WebSocketClient::connection_ptr con = ws_client.get_connection("wss://test.deribit.com/ws/api/v2", ec);
        if (ec) {
            std::cerr << "WebSocket Connection Error: " << ec.message() << std::endl;
            return;
        }

        ws_client.connect(con);
        ws_client.run();
    } catch (const std::exception &e) {
        std::cerr << "WebSocket Exception: " << e.what() << std::endl;
    }
}


void subscribeToChannel(WebSocketClient &ws_client, websocketpp::connection_hdl hdl, const std::string &channel) {
    std::string subscribe_msg = R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "public/subscribe",
        "params": {
            "channels": [")" + channel + R"("]
        }
    })";
    ws_client.send(hdl, subscribe_msg, websocketpp::frame::opcode::text);
    std::cout << "Subscribed to channel: " << channel << std::endl;
}


void unsubscribeFromChannel(WebSocketClient &ws_client, websocketpp::connection_hdl hdl, const std::string &channel) {
    std::string unsubscribe_msg = R"({
        "jsonrpc": "2.0",
        "id": 1,
        "method": "public/unsubscribe",
        "params": {
            "channels": [")" + channel + R"("]
        }
    })";
    ws_client.send(hdl, unsubscribe_msg, websocketpp::frame::opcode::text);
    std::cout << "Unsubscribed from channel: " << channel << std::endl;
}


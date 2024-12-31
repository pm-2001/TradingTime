# TradingTime
## Documentation : https://guiltless-bongo-2a6.notion.site/TradingTime-16d34cef9f1680a29c0ecd754664aad3
## Project Explanation Video : https://drive.google.com/file/d/1RnwqMqcO5nSRb0ah5oo8LpuZFRJbYIPW/view?usp=sharing
## How to run application in linux?
1. Clone this repository.
2. ``` cd TradingTime ```
3. ``` mkdir build ```

4. Run following commands to install required libraries
- ``` sudo apt install cmake ```
- ```sudo apt install build-essential ```
- ``` sudo apt install libcurl4-openssl-dev ```
- ``` sudo apt install libboost-all-dev ```

5. Create your api key at Deribit and run following command in your terminal
- ``` export CLIENT_ID= < > ```
- ``` export CLIENT_SECRET= < > ```

6. Run application
- ``` cmake .. ```
- ``` make ```
- ``` ./DeribitTrading ```

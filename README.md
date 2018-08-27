# stock_chart_wx

C++ client for stock market data at

https://www.alphavantage.co/

# Build

## Get an API key

https://www.alphavantage.co/support/#api-key

## Install dependencies (UNIX)

<pre>
sudo apt-get install build-essential
sudo apt-get install autoconf
sudo apt-get install libwxgtk3.0-dev
sudo apt-get install libcurl4-openssl-dev
</pre>

## Build GUI interface (UNIX)

<pre>
git clone https://github.com/pedro-vicente/stock_chart_wx
cd stock_chart_wx
autoreconf -vfi && ./configure && make
./stock_chart_wx -k API_KEY
</pre>

## Build command line interface

<pre>
cd build
cmake .. 
</pre>
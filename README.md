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

## Data files

Dow Jones Industrial Average and SP500 index for 2018

DJI_2018_intermediate.txt

INX_2018_intermediate.txt


![alt text](https://user-images.githubusercontent.com/6119070/44764863-ba231f80-ab1f-11e8-9645-bef795cf36cf.png)
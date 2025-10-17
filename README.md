A wxWidgets-based desktop application for visualizing and analyzing SPY (S&P 500 ETF) stock data with interactive charting capabilities for stock market data at

https://www.alphavantage.co/

## Build

Get an API key

https://www.alphavantage.co/support/#api-key

and save to file `resources/api_key.txt`

![SPY Stock Chart Visualization](https://github.com/user-attachments/assets/be82917d-2074-4af3-ae59-1f1bc398a193)

## Features

- **CSV Data Management**: Reads and parses SPY historical stock data (OHLCV format)
- **Interactive Charts**: Real-time visualization with mouse tracking and crosshair
- **Live Data Updates**: Fetches intraday data from Alpha Vantage API
- **Volume Visualization**: Displays trading volume as bars at chart bottom
- **Data Merging**: Automatically merges new data with existing records, removing duplicates
- **Candlestick Support**: Framework includes candlestick chart rendering capabilities

## Architecture

### Core Components

**trader.hh** - Main application structure and entry types
- Defines `entry_t` struct for stock data storage
- Application framework with `TraderApp`, `MainFrame`, `PanelDisplay`, `PanelInput`

**display.cc/hh** - Display panel and data management
- CSV file reading and parsing
- API data fetching and merging
- Splitter window layout management

**chart.cc/hh** - Interactive charting widget
- Real-time rendering of price data and volume
- Mouse interaction (click, drag, hover)
- Crosshair with tooltip showing price and timestamp
- Point selection and highlighting

**grafix.cc/hh** - Graphics primitives
- Coordinate transformation (data space ↔ screen space)
- Drawing utilities for lines, circles, rectangles, candlesticks
- Scale rendering with intelligent time axis formatting

**model.cc/hh** - Data model layer
- Wraps stock data for chart consumption

**get.cc/hh** - HTTP client for Alpha Vantage API
- Fetches intraday SPY data
- Reverses chronological order for time series display

**ssl_read.cc/hh** - HTTPS communication
- SSL/TLS connection handling using ASIO and OpenSSL

## Project Structure

```
trade/
├── src/
│   ├── trader.cc/hh          # Main application entry
│   ├── display.cc/hh         # Display panel and data management
│   ├── chart.cc/hh           # Interactive chart widget
│   ├── grafix.cc/hh          # Graphics primitives
│   ├── model.cc/hh           # Data model
│   ├── get.cc/hh             # Alpha Vantage API client
│   ├── ssl_read.cc/hh        # HTTPS/SSL communication
│   ├── http_client.cc        # CLI utility for API testing
│   └── sample.rc             # Windows resources
├── ext/                      # External dependencies (auto-downloaded)
│   ├── wxwidgets-3.3.1/
│   └── asio-1.30.2/
├── build/                    # Build output
│   ├── wxwidgets-3.3.1/      # wxWidgets build
│   └── trade/                # Application build
├── resources/
│   ├── SPY.csv               # Historical data
│   └── api_key.txt           # Alpha Vantage API key
├── CMakeLists.txt
├── build.wx.sh               # Build dependencies
└── build.cmake.sh            # Build application
```

## Dependencies

- **wxWidgets 3.3.1**: Cross-platform GUI toolkit (auto-downloaded)
- **ASIO 1.30.2**: Asynchronous I/O library (auto-downloaded)
- **OpenSSL**: SSL/TLS support (system-installed)
- **C++17** or later

## Build Instructions

### Prerequisites

**Linux (Ubuntu/Debian)**
```bash
sudo apt-get install cmake build-essential git
sudo apt-get install libssl-dev
sudo apt-get install libgtk-3-dev 
```

**macOS**
```bash
brew install cmake openssl git
```

**Windows**
- Visual Studio 2019 or later
- CMake 3.28+
- OpenSSL (set `OPENSSL_ROOT_DIR` environment variable)

https://slproweb.com/products/Win32OpenSSL.html

### Build Process

The project uses CMake and automatically downloads dependencies.

**1. Build wxWidgets and ASIO**
```bash
./build.wx.sh
```
This script:
- Clones wxWidgets 3.3.1 to `ext/wxwidgets-3.3.1`
- Clones ASIO 1.30.2 to `ext/asio-1.30.2`
- Builds wxWidgets as static library in `build/wxwidgets-3.3.1`

**2. Build the application**
```bash
./build.cmake.sh
```
This script:
- Creates `build/trade` directory
- Configures CMake project
- Builds `trader` executable and `http_client` utility
- Copies `SPY.csv` and `api_key.txt` to build directory

**Manual CMake build**
```bash
mkdir -p build/trade
cd build/trade
cmake ../.. --fresh
cmake --build . --config Debug --verbose
```

## Usage

### Running

**Linux/macOS**
```bash
cd build/trade
./trader
```

**Windows**
```bash
cd build/trade/Debug
trader.exe
```

**Standalone HTTP client** 

Fetches data without GUI

```bash
./http_client 
```

### Controls

- **Get Data Button**: Fetches latest intraday data from API
- **Mouse Hover**: Displays crosshair and price tooltip
- **Mouse Click**: Selects and highlights data point in red
- **Chart Auto-saves**: Renders to `chart.bmp` after each paint event

## Data Format

### CSV Structure
```
timestamp,open,high,low,close,volume
YYYY-MM-DD HH:MM:SS,float,float,float,float,integer
```

### API Response
Alpha Vantage returns CSV format that gets reversed chronologically and saved to `SPY_intraday.csv`, then merged into main `SPY.csv`.

## File Outputs

All output files are written to the build directory (`build/trade/`):
- `SPY.csv`: Merged historical data
- `SPY_intraday.csv`: Latest fetched data (reversed chronologically)
- `intraday.csv`: Raw API response
- `chart.bmp`: Last rendered chart image
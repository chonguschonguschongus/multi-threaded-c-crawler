# Web Crawler

## Overview

This is a multithreaded web crawler written in C++ that downloads web pages, extracts hyperlinks, and recursively follows them up to a specified depth. The program uses cURL for downloading web content, regex for extracting links, and multithreading for parallel crawling.

## Features

- Multi-threaded web crawling using `std::thread`
- HTML downloading with `libcurl`
- Hyperlink extraction using regex
- Depth-first crawling with a limit (default: depth 3)
- Thread synchronization with `std::mutex`

## Requirements

Before running the crawler, ensure you have the following installed:

### Dependencies

- **g++ (GCC) 4.8 or later** (supports C++11 or later)
- **libcurl** (for HTTP requests)
- **pthreads** (for multithreading)

### Installing Dependencies

#### Ubuntu/Debian:

```sh
sudo apt update
sudo apt install g++ libcurl4-openssl-dev
```

#### macOS (Homebrew):

```sh
brew install gcc curl
```

#### Windows (MinGW):

Ensure `g++` is installed with `pthread` support. You may need MSYS2:

```sh
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-curl
```

## Compilation

Use the following command to compile the program:

```sh
g++ -std=c++11 crawler.cpp -o crawler -lpthread -lcurl
```

## Running the Program

To start the crawler, execute:

```sh
./crawler
```

This will start multiple threads crawling predefined websites.

## Configuration

- Modify `main()` to change the seed URLs.
- Adjust the depth limit inside `crawl_page()` if needed.

## Notes

- Ensure internet connectivity for successful crawling.
- Some websites may block automated requests; use respectful crawling practices.
- Avoid excessive requests to prevent being flagged as a bot.

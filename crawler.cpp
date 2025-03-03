#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <curl/curl.h>
#include <regex>
#include <thread>
#include <mutex>
#include <chrono>
#include <string>

using namespace std;

mutex mtx;

/**
 * @brief Downloads HTML content from a given URL and writes it to a file.
 * 
 * @param url The URL from which to download the HTML content.
 * @param file_name The name of the file where the HTML will be saved.
 */
void download_page(const char* url, const char* file_name) {
    CURL* curl_handle = curl_easy_init(); // Initialize curl object

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    FILE* file = fopen(file_name, "w"); // Create file to store content

    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, file); // Write to file
    curl_easy_perform(curl_handle); // Download HTML content
    curl_easy_cleanup(curl_handle); // Clean up curl object
    fclose(file);
}

/**
 * @brief Extracts hyperlinks from an HTML file.
 * 
 * @param html_file_name The HTML file to extract links from.
 * @return A vector of valid hyperlinks found in the file.
 */
vector<string> extract_links_from_html(const string& html_file_name) {
    string html_content;
    ifstream file_reader(html_file_name); 

    while (!file_reader.eof()) {
        if (file_reader.eof()) break;
        char ch;
        file_reader.get(ch);
        html_content.push_back(ch); // Append character to string
    }
    file_reader.close();

    static const regex link_regex("<a href=\"(.*?)\">", regex_constants::icase); // Case insensitive regex
    vector<string> links;
    copy(sregex_token_iterator(html_content.begin(), html_content.end(), link_regex, 1), 
         sregex_token_iterator(), back_inserter(links));
    return links;
}

/**
 * @brief Cleans and validates hyperlinks by removing unnecessary parts.
 * 
 * @param links A vector of strings representing hyperlinks.
 */
void clean_links(vector<string>& links) {
    vector<string> valid_links;
    for (const auto& link : links) {
        string cleaned_link;
        for (char ch : link) {
            if (ch == ' ' || ch == 34) break; // Stop at space or double quote
            cleaned_link.push_back(ch); // Append character to string
        }

        // Validate the URL format using regex
        if (regex_match(cleaned_link, regex("((http|https)://)(www.)?[a-zA-Z0-9@:%._\\+~#?&//=]{2,256}\\.[a-z]{2,6}\\b([-a-zA-Z0-9@:%._\\+~#?&//=]*)"))) {
            valid_links.push_back(cleaned_link);
        }
    }
    links = valid_links; // Replace original links with valid ones
}

unordered_map<string, bool> visited_urls; // To track visited URLs

/**
 * @brief Performs depth-first search to crawl and extract hyperlinks from the given URL.
 * 
 * @param url The starting URL to crawl.
 * @param file_path The path where the HTML content of the URL will be saved.
 * @param depth The current depth of the crawling process.
 * @param bot_id The ID of the bot performing the crawling.
 */
void crawl_page(const char* url, const char* file_path, int depth, int bot_id) {
  if (depth == 3) return;  // Stop at max depth

  // Lock before checking and modifying visited_urls
  {
      lock_guard<mutex> lock(mtx);
      if (visited_urls[url]) return;  // Skip if already visited
      visited_urls[url] = true;  // Mark as visited
  }

  cout << "Bot ID: " << bot_id << "\tCrawling URL: " << url << endl;
  cout << "Current depth: " << depth << endl;

  // Use a unique file for each depth level
  string file_name = "bot_" + to_string(bot_id) + "_depth_" + to_string(depth) + ".txt";
  download_page(url, file_name.c_str()); 

  vector<string> links = extract_links_from_html(file_name);
  clean_links(links);

  vector<thread> threads;  // Store threads for recursive calls

  for (const auto& link : links) {
      this_thread::sleep_for(chrono::milliseconds(10));  // Avoid rate limiting
      threads.emplace_back(crawl_page, link.c_str(), file_path, depth + 1, bot_id);
  }

  // Join all spawned threads
  for (auto& t : threads) {
      t.join();
  }
}

int main() {
    const char* filename = "1.txt"; // File to store HTML content

    // Start the crawling threads
    std::thread crawler1([&]() { crawl_page("https://en.wikipedia.org/wiki/97th_Academy_Awards", filename, 0, 1); });
    std::thread crawler2([&]() { crawl_page("https://github.com/", filename, 0, 2); });
    std::thread crawler3([&]() { crawl_page("https://codeforces.com/", filename, 0, 3); });

    // Wait for threads to finish
    crawler1.join();
    crawler2.join();
    crawler3.join();

    return 0;
}
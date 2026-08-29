// Strictly handles downloading the raw web string from the internet.

#include "NetworkClient.hpp"
#include <string>
#include <curl/curl.h>
#include <iostream>

size_t WriteCallback(void* Contents, size_t Size, size_t NumberOfElements, std::string* UserData)
{
  // 1. Calculate the exact size of the incoming block of text data bytes
  size_t TotalSize = Size * NumberOfElements;

  // 💡 The Secure Modern C++ Fix: Wrap the raw data in a safe text window view
  std::string_view DataWindow(static_cast<const char*>(Contents), TotalSize);
  // 2. Append the raw text bytes directly onto our target C++ string container
  // Contents is a raw pointer block, so we tell the string to append exactly TotalSize characters from it
  UserData->append(DataWindow);

  // 3. Return the exact total size to curl to confirm we safely received the data block
  return TotalSize;
}

std::string NetworkClient::DownloadPage(const std::string& TargetUrl)
{
  std::string RC;
  CURL* CurlEngine = curl_easy_init();

  if (CurlEngine == nullptr)
  {
    std::cerr << "[ERROR] Unable to initialise CURL Engine." << std::endl;
    return RC;
  }

  curl_easy_setopt(CurlEngine, CURLOPT_URL, TargetUrl.c_str());
  curl_easy_setopt(CurlEngine, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(CurlEngine, CURLOPT_WRITEDATA, &RC);
  CURLcode ResultCode = curl_easy_perform(CurlEngine);

  if (ResultCode != CURLE_OK)
  {
    std::cerr << "[ERROR] Connection failed: " << curl_easy_strerror(ResultCode) << std::endl;
  }

  curl_easy_cleanup(CurlEngine);

  return RC;
}

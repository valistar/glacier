#include "request.h"

#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>


namespace aws {

bool OrderHeaders::operator()(const std::string &lhs, const std::string &rhs) const {
  return true; //TODO IMPLEMENT
};

Request::Request() : curl(curl_easy_init()) {

}

size_t curlWriter(char *data, size_t size, size_t nmemb, void *s) {
  static_cast<std::string *>(s)->append(data);
  return size * nmemb;
}

void Request::setHeader(std::string name, std::string value) {
  this->headers[name] = value;
}

void Request::setUrl(std::string url) {
  this->url = url;
}


void Request::send() {
  /*
  switch(this->verb) {
      case 1:
          break;
      case 2:
          break;
      case 3:
          break;
      case 4:
          break;
      default:
          throw std::domain_error("Unknown verb set");
  }
*/
  std::cout << this->url.c_str() << std::endl;

  curl_easy_setopt(this->curl, CURLOPT_URL, this->url.c_str());
  curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &this->response_headers);
  curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &this->response_body);
  curl_easy_perform(this->curl); //TODO Performance comparison of resetting opts but still reusing the handler vs not resetting opts, when connecting to same host but different url, and different host.
  curl_easy_reset(this->curl);

  std::cout << this->response_headers << std::endl << std::endl << std::endl;
  std::cout << this->response_body << std::endl;

  curl_easy_cleanup(this->curl); //TODO DEV

}

} //namespace aws
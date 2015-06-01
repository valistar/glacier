/**
 * Defines Request, the base class used to send http(s) requests. Currently utilizes libcurl.
 */

#ifndef GLACIER_REQUEST_H
#define GLACIER_REQUEST_H

#include <curl/curl.h>
#include <map>
#include <string>

namespace aws {

struct OrderHeaders {
  bool operator()(const std::string &lhs, const std::string &rhs) const;
};

class Request {
 public:
  Request();
  void setHeader(std::string name, std::string value);
  void addBody();
  void setUrl(std::string url);
  void send();
  void getResponse();

 protected:
  std::map<std::string, std::string, OrderHeaders> headers;
  std::string url;
  CURL *curl;
  std::string response_headers;
  std::string response_body;
};

} //namespace aws

#endif //GLACIER_REQUEST_H

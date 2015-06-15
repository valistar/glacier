/**
 * Defines Request, the base class used to send http(s) requests. Currently utilizes libcurl.
 */

#ifndef GLACIER_REQUEST_H
#define GLACIER_REQUEST_H

#include <curl/curl.h>
#include <map>
#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace aws {

struct OrderHeaders {
  bool operator()(const std::string &lhs, const std::string &rhs) const;
};

class Request {
 public:
  explicit Request(std::string verb);
  void setHeader(std::string name, std::string value);
  void addBody(std::string body);
  void addBodyFile(std::string filepath);
  void setUrl(std::string url);
  void setAccessKey(std::string key);
  void setSecretKey(std::string key);
  void selectRegion(std::string region);
  void selectService(std::string service);
  void send();
  void getResponse();
  static std::string headerToCanonical(std::string headerName);

 protected:
  std::map<std::string, std::string, OrderHeaders> headers;
  CURL *curl;
  std::string response_headers;
  std::string response_body;
  void signRequest();

 private:
  const std::string verb;
  boost::posix_time::ptime timestamp;
  std::string protocol;
  std::string host;
  std::string uri;
  std::string query_string;
  std::string body;
  FILE* bodyFile;
  bool is_body_file;
  std::string access_key;
  std::string secret_key;
  std::string region;
  std::string service;
};

} //namespace aws

#endif //GLACIER_REQUEST_H

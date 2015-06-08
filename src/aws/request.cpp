#include "request.h"

#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>


namespace aws {

bool OrderHeaders::operator()(const std::string &lhs, const std::string &rhs) const {
  //TODO Test this, since it might return based on memory address, not ascii char code.
  return strcmp(Request::headerToCanonical(lhs).c_str(), Request::headerToCanonical(rhs).c_str()) < 0;
};

Request::Request(std::string verb) : curl(curl_easy_init()), verb(verb) {

}

std::string Request::headerToCanonical(std::string headerName) {
  boost::algorithm::to_lower(headerName);
  return headerName;
}

size_t curlWriter(char *data, size_t size, size_t nmemb, void *s) {
  static_cast<std::string *>(s)->append(data);
  return size * nmemb;
}

void Request::setHeader(std::string name, std::string value) {
  this->headers[name] = value;
}

void Request::setUrl(std::string url) {
  std::size_t protocolPos;
  std::size_t uriPos;
  std::size_t queryPos = url.find("?");
  BOOST_LOG_TRIVIAL(info) << "Parse URL";
  if((protocolPos = url.find("http://")) == 0) {
    this->protocol = "http://";
    BOOST_LOG_TRIVIAL(info) << "HTTP detected";
  }
  else if((protocolPos = url.find("https://")) == 0) {
    this->protocol = "https://";
    BOOST_LOG_TRIVIAL(info) << "HTTPS detected";
  }
  else {
    BOOST_LOG_TRIVIAL(info) << "No protocol detected";
    this->protocol.clear();
  }

  uriPos = url.find("/", this->protocol.length() + 1);

  if(uriPos != std::string::npos && (queryPos == std::string::npos || uriPos < queryPos)) {
    this->host = url.substr(this->protocol.length() > 0 ? this->protocol.length() : 0, uriPos - this->protocol.length());
    this->uri = url.substr(uriPos, queryPos);
    BOOST_LOG_TRIVIAL(info) << "Host: " << this->host;
    BOOST_LOG_TRIVIAL(info) << "URI: " << this->uri;
  }
  else {
    this->host = url.substr(this->protocol.length() > 0 ? this->protocol.length() : 0, queryPos - this->protocol.length());
    this->uri = "/";
  }

  if(queryPos != std::string::npos) {
    this->queryString = url.substr(queryPos);
    BOOST_LOG_TRIVIAL(info) << "QS: " << this->queryString;
  }
  else {
    this->queryString.clear();
    BOOST_LOG_TRIVIAL(info) << "QS: N/A";
  }

  if(this->protocol.length() == 0) {
    this->protocol = "https://";
  }

}

void Request::signRequest() {
  //Step 1
  std::string canonicalRequest = this->verb + "\n" + this->uri + "\n" + this->queryString + "\n"; //TODO We have to create an actual canonical query string, sorted, formatted, etc.
  std::string signedHeaders;
  for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
    canonicalRequest += Request::headerToCanonical(it->first) + ":" + it->second + "\n"; //TODO We should be getting the canonical header val here too
    signedHeaders += Request::headerToCanonical(it->first);
    if(it != this->headers.end()) {
      signedHeaders += ";";
    }
  }
  canonicalRequest += signedHeaders += "\n";
  BOOST_LOG_TRIVIAL(info) << std::endl << "Canonical Request" << std::endl <<  canonicalRequest << std::endl;
  //TODO Hash body, append, then hash canonical request.

  //Step 2
  std::stringstream requestTimestamp;
  boost::posix_time::time_facet* timestampFormat = new boost::posix_time::time_facet("%Y%m%dT%H%M%S%FZ"); //TODO We have to deallocate this, don't we? Ought to keep it around for multiple requests.
  requestTimestamp.imbue(std::locale(std::locale::classic(), timestampFormat));
  requestTimestamp << this->timestamp;

  std::stringstream requestDate;
  boost::posix_time::time_facet dateFormat("%Y%m%d");
  requestDate.imbue(std::locale(std::locale::classic(), &dateFormat));
  requestDate << this->timestamp;

  std::string stringToSign = "AWS4-HMAC-SHA256\n" + requestTimestamp.str() + "\n" + requestDate.str() + "/" + this->region + "/" + this->service + "/aws4_request\n" + canonicalRequest;

  BOOST_LOG_TRIVIAL(info) << std::endl << "String to Sign" << std::endl <<  stringToSign << std::endl;

  //Step 3


}


void Request::send() {
  this->timestamp = boost::posix_time::second_clock::universal_time();

  BOOST_LOG_TRIVIAL(info) << std::endl << "URL"  << std::endl << (this->protocol + this->host + this->uri + this->queryString) << std::endl;
  this->signRequest();

  curl_easy_setopt(this->curl, CURLOPT_URL, (this->protocol + this->host + this->uri + this->queryString).c_str()); //TODO Encode as necessary
  curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &this->response_headers);
  curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &this->response_body);
  curl_easy_perform(this->curl); //TODO Performance comparison of resetting opts but still reusing the handler vs not resetting opts, when connecting to same host but different url, and different host.
  curl_easy_reset(this->curl);

  BOOST_LOG_TRIVIAL(info) << std::endl << "Response Headers"  << std::endl << this->response_headers << std::endl;
  BOOST_LOG_TRIVIAL(info) << std::endl << "Response Body"  << std::endl << this->response_body << std::endl;

  curl_easy_cleanup(this->curl); //TODO DEV

}

} //namespace aws
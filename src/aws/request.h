#ifndef GLACIER_REQUEST_H
#define GLACIER_REQUEST_H

#include "request.h"

#include <curl/curl.h>
#include <map>
#include <string>

namespace aws{

struct OrderHeaders {
    bool operator() (const std::string& lhs, const std::string& rhs) const;
};

class Request {
    std::map<std::string, std::string, OrderHeaders> headers;
    int verb; //Defaults to GET
    std::string url;
    CURL* curl;
    std::string response_headers;
    std::string response_body;
public:
    static const int kGet;
    static const int kPost;
    static const int kPut;
    static const int kDelete;
    Request();
    void setHeader(std::string name, std::string value);
    void setVerb(int verb);
    void addBody();
    void setUrl(std::string url);
    void send();
    void getResponse();
};

} //namespace aws

#endif //GLACIER_REQUEST_H

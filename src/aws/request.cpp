#include "request.h"

#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../hash/hmac.h"
#include "../hash/sha256.h"


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
    this->query_string = url.substr(queryPos);
    BOOST_LOG_TRIVIAL(info) << "QS: " << this->query_string;
  }
  else {
    this->query_string.clear();
    BOOST_LOG_TRIVIAL(info) << "QS: N/A";
  }

  if(this->protocol.length() == 0) {
    this->protocol = "https://";
  }

}

void Request::addBody(std::string body) {
  this->is_body_file = false;
  this->body = body;
}

void Request::addBodyFile(std::string filepath) {
  this->is_body_file = true;
  this->body = filepath;
}

void Request::setAccessKey(std::string key) {
  this->access_key = key;
}

void Request::setSecretKey(std::string key) {
  this->secret_key = key;
}

void Request::selectRegion(std::string region) {
  this->region = region;
}

void Request::selectService(std::string service) {
  this->service = service;
}

unsigned char* hextest(std::string input) {
  const char* chmac1 = input.c_str();
  unsigned char binhmac1[32];
  unsigned char* dapoint = binhmac1;
  size_t count = 0;
  for(count = 0; count < sizeof(binhmac1)/sizeof(binhmac1[0]); count++) {
    sscanf(chmac1, "%2hhx", &binhmac1[count]);
    chmac1 += 2;
  }

  return dapoint;

}

void Request::signRequest() {
  //Step 1
  std::string canonicalRequest = this->verb + "\n" + this->uri + "\n" + this->query_string + "\n"; //TODO We have to create an actual canonical query string, sorted, formatted, etc.
  std::string signedHeaders;
  for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
    canonicalRequest += Request::headerToCanonical(it->first) + ":" + it->second + "\n"; //TODO We should be getting the canonical header val here too
    signedHeaders += Request::headerToCanonical(it->first);
    signedHeaders += ";";
  }
  //We should avoid adding the last ';', but in the words of every arrogant C++ programmer in the world "There's no reason to be able to know if you're looking at your sets last element,
  //you must be trying to solve the wrong problem because thats dumb. Work on something else instead."
  signedHeaders.pop_back();
  canonicalRequest += "\n" + signedHeaders += "\n";
  SHA256 sha256;
  if(this->is_body_file) { //TODO Files entirely untested.
    SHA256 part_sha256;
    std::vector<unsigned char> partBuffer;
    partBuffer.reserve(SHA256::HashBytes);
    std::vector<std::vector<unsigned char>> parts;
    char* buffer = (char*) malloc (sizeof(char)*1024*1024); //TODO Something about unsigned being important here for hashing, check it.
    size_t read;
    size_t bufferSize = sizeof(buffer);

    while(true) {
      read = fread (buffer, 1, bufferSize, this->bodyFile);
      if(read > 0) {
        sha256.add(buffer, read);
        part_sha256.getHash(&partBuffer[0]);
        parts.push_back(partBuffer);
      }
      if(read != bufferSize) {
        if(ferror(this->bodyFile)) {
          //TODO Error.
        }
        break;
      }
    }
    free(buffer);
    canonicalRequest += sha256.getHash(); //TODO Will this act like hashing an empty string for an empty file?
    this->setHeader("x-amz-content-sha256", sha256.getHash());

    //Compute tree hash
    int i;
    int increment = 1;
    size_t size = parts.size();
    std::vector<unsigned char> treeBuffer;
    treeBuffer.reserve(SHA256::HashBytes*2);
    unsigned char* ptest;
    while(true) { //TODO Test empty file.
      for (i = 0; i < size; i += increment * 2) {
        if (i + increment >= size - 1) {
          //Odd number of elements.
          continue;
        }
        //TODO Can probably change this now that using vectors
        strcpy((char *) &treeBuffer[0], (char *) &parts[i][0]); //TODO Operator precedence?
        strcat((char *) &treeBuffer[0], (char *) &parts[i + increment][0]);
        part_sha256.reset();
        part_sha256.add((void*)&treeBuffer[0], SHA256::HashBytes*2); //TODO See if void pointer cast is actually required...
        part_sha256.getHash(&((parts[i])[0]));
      }
      increment *= 2;
      if (increment > parts.size()) {
        this->setHeader("x-amz-sha256-tree-hash", part_sha256.getHash());
        break;
      }
    }
  }
  else {
    //TODO Tree hash here?
    canonicalRequest += sha256(this->body);
  }

  BOOST_LOG_TRIVIAL(info) << std::endl << "Canonical Request" << std::endl <<  canonicalRequest << std::endl;
  SHA256 sha2562;
  canonicalRequest = sha2562(canonicalRequest); //TODO Make sure we can re-use sha256 hashers.
  BOOST_LOG_TRIVIAL(info) << std::endl << "Canonical Request (Hash)" << std::endl <<  canonicalRequest << std::endl;

  //Step 2
  std::stringstream requestTimestamp;
  boost::posix_time::time_facet* timestampFormat = new boost::posix_time::time_facet("%Y%m%dT%H%M%S%FZ"); //TODO We have to deallocate this, don't we? Ought to keep it around for multiple requests.
  requestTimestamp.imbue(std::locale(std::locale::classic(), timestampFormat));
  requestTimestamp << this->timestamp;

  std::stringstream requestDate;
  boost::posix_time::time_facet* dateFormat = new boost::posix_time::time_facet("%Y%m%d");
  requestDate.imbue(std::locale(std::locale::classic(), dateFormat));
  requestDate << this->timestamp;

  std::string scope = requestDate.str() + "/" + this->region + "/" + this->service + "/aws4_request";
  std::string stringToSign = "AWS4-HMAC-SHA256\n" + requestTimestamp.str() + "\n" + scope + "\n" + canonicalRequest;
  //stringToSign = "AWS4-HMAC-SHA256\n20110909T233600Z\n20110909/us-east-1/iam/aws4_request\n3511de7e95d28ecd39e9513b642aee07e54f4941150d8df8bf94b328ef7e55e2"; //TODO DEV
  BOOST_LOG_TRIVIAL(info) << std::endl << "String to Sign" << std::endl <<  stringToSign << std::endl;

  //Step 3
  unsigned char* binhmac1, * binhmac2, * binhmac3, * binhmac4; //TODO Can probably just re-use one pointer. Didnt work when I tested, but I think that was due to a typo with hmac inputs that I've since fixed.
  std::string hmac1 = hmac<SHA256>(requestDate.str(), "AWS4" + this->secret_key);
  binhmac1 = aws::hextest(hmac1); //TODO Is this a memory leak? I'm creating a char array inside the function, returning the pointer, than overwriting the pointer. But its not dynamically allocated, so I assume refcounting handles it?
  std::string hmac2 = hmac<SHA256>(this->region.c_str(), this->region.size(), binhmac1, sizeof(unsigned char) * 32);
  binhmac2 = aws::hextest(hmac2);
  std::string hmac3 = hmac<SHA256>(this->service.c_str(), this->service.size(), binhmac2, sizeof(unsigned char) * 32);
  binhmac3 = aws::hextest(hmac3);
  std::string terminationString = "aws4_request";
  std::string hmac4 = hmac<SHA256>(terminationString.c_str(), terminationString.size(), binhmac3, sizeof(unsigned char) * 32 );
  binhmac4 = aws::hextest(hmac4);
  std::string hmac5 = hmac<SHA256>(stringToSign.c_str(), stringToSign.size(), binhmac4, sizeof(unsigned char) * 32 );


  BOOST_LOG_TRIVIAL(info) << std::endl << "Signature" << std::endl <<  hmac5 << std::endl;

  std::string auth = "AWS4-HMAC-SHA256 Credential=" + this->access_key + "/" + scope + ", SignedHeaders=" + signedHeaders + ", Signature=" + hmac5;

  BOOST_LOG_TRIVIAL(info) << std::endl << "Authorization: " <<  auth << std::endl;

  this->setHeader("Authorization", auth);
}





void Request::send() {
  this->setHeader("Host", this->host);
  this->timestamp = boost::posix_time::second_clock::universal_time();

  //TODO Re-use this better.
  std::stringstream requestTimestamp;
  boost::posix_time::time_facet* timestampFormat = new boost::posix_time::time_facet("%Y%m%dT%H%M%S%FZ"); //TODO We have to deallocate this, don't we? Ought to keep it around for multiple requests.
  requestTimestamp.imbue(std::locale(std::locale::classic(), timestampFormat));
  requestTimestamp << this->timestamp;
  this->setHeader("x-amz-date", requestTimestamp.str());

  BOOST_LOG_TRIVIAL(info) << std::endl << "URL"  << std::endl << (this->protocol + this->host + this->uri + this->query_string) << std::endl;
  if(this->is_body_file) {
    this->bodyFile = fopen(this->body.c_str(), "rb"); //TODO Have to close this somewhere
    if (this->bodyFile == NULL) {
      //TODO Throw exception.
    }
    //fstat(fileno(this->bodyFile));
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_READDATA, this->bodyFile);
    this->setHeader("Content-Length", "9"); //TODO DEV

  }
  this->signRequest();

  curl_easy_setopt(this->curl, CURLOPT_URL, (this->protocol + this->host + this->uri + this->query_string).c_str()); //TODO Encode as necessary
  curl_easy_setopt(this->curl, CURLOPT_HEADERFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_HEADERDATA, &this->response_headers);
  curl_easy_setopt(this->curl, CURLOPT_WRITEFUNCTION, aws::curlWriter);
  curl_easy_setopt(this->curl, CURLOPT_WRITEDATA, &this->response_body);

  curl_easy_setopt(this->curl, CURLOPT_SSL_VERIFYPEER, FALSE); //TODO Remove this. How do I make libcurl use the OS CA bundle? I don't want to distribute my own CA bundle...
//http://curl.haxx.se/docs/sslcerts.html

  //TODO move this somewhere better. Maybe, signRequest. Maybe not.
  struct curl_slist *chunk = NULL; //TODO Needs to be freed somewhere, sometime
  for (std::map<std::string, std::string>::iterator it = this->headers.begin(); it != this->headers.end(); ++it) {
    chunk = curl_slist_append(chunk, (it->first + ": " + it->second).c_str());
  }


  curl_easy_setopt(this->curl, CURLOPT_HTTPHEADER, chunk);



  CURLcode res;
  res = curl_easy_perform(this->curl); //TODO Performance comparison of resetting opts but still reusing the handler vs not resetting opts, when connecting to same host but different url, and different host.
  BOOST_LOG_TRIVIAL(info) << "CURL Result: " << res;
  if(res != CURLE_OK) {
    BOOST_LOG_TRIVIAL(info) << curl_easy_strerror(res);
  }
  curl_easy_reset(this->curl);

  BOOST_LOG_TRIVIAL(info) << std::endl << "Response Headers"  << std::endl << this->response_headers << std::endl;
  BOOST_LOG_TRIVIAL(info) << std::endl << "Response Body"  << std::endl << this->response_body << std::endl;

  curl_easy_cleanup(this->curl); //TODO DEV

}

} //namespace aws
#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>

#include "src/aws/request.h"




using namespace std;

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    aws::Request request_test;
    BOOST_LOG_TRIVIAL(warning) << "A warning severity message";
    /*
    request_test.setVerb(aws::Request::kGet);
    request_test.setUrl("");
    request_test.setHeader("NotRealHeader", "NotRealValue");
    request_test.send();
     */
    cout << "Goodbye, World!" << endl;
    return 0;
}
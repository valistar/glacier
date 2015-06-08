#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>

#include "src/aws/request.h"
#include "src/aws/get_request.h"


using namespace std;

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    aws::GetRequest request_test;
    request_test.setUrl("http://mydevices/awstest?query1=one&query2=two");
    request_test.setHeader("NotRealHeader", "NotRealValue");
    request_test.send();

    cout << endl << "Goodbye, World!" << endl;
    return 0;
}
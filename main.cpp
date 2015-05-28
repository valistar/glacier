#include <iostream>
#include <curl/curl.h>

#include "src/aws/request.h"

using namespace std;

int main() {
    curl_global_init(CURL_GLOBAL_ALL);
    aws::Request request_test;
    request_test.setVerb(aws::Request::kGet);
    request_test.setUrl("");
    request_test.setHeader("NotRealHeader", "NotRealValue");
    request_test.send();
    cout << "Goodbye, World!" << endl;
    return 0;
}
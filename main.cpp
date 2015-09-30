#include <iostream>
#include <curl/curl.h>
#include <boost/log/trivial.hpp>

#include "src/aws/request.h"
#include "src/aws/get_request.h"
#include "src/aws/post_request.h"


using namespace std;

int main() {
  curl_global_init(CURL_GLOBAL_ALL);
  /*
  aws::GetRequest request_test;
  request_test.setUrl("https://glacier.us-east-1.amazonaws.com/-/vaults");
  request_test.selectRegion("us-east-1");
  request_test.selectService("glacier");
  request_test.setAccessKey("access");
  request_test.setSecretKey("sekret");
  request_test.setHeader("x-amz-glacier-version", "2012-06-01");
  request_test.send();
   */

  aws::PostRequest request_test;
  //request_test.setUrl("https://glacier.us-east-1.amazonaws.com/-/vaults/test_vault_manual/archives");
  request_test.setUrl("http://requestb.in/168jz8g1");
  request_test.selectRegion("us-east-1");
  request_test.selectService("glacier");
  request_test.setAccessKey("access"); //No, the old keys here arent valid.
  request_test.setSecretKey("sekret");
  request_test.setHeader("x-amz-glacier-version", "2012-06-01");
  request_test.addBodyFile("F:\\CPP\\Glacier\\tfile.txt");
  request_test.send();

  cout << endl << "Goodbye, World!" << endl;
  return 0;
}
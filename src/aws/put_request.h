#ifndef GLACIER_PUT_REQUEST_H
#define GLACIER_PUT_REQUEST_H

#include "request.h"

namespace aws {

class PutRequest : public aws::Request {
 public:
  PutRequest();
};

} //namespace aws

#endif //GLACIER_PUT_REQUEST_H

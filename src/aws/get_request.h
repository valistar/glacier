#ifndef GLACIER_GET_REQUEST_H
#define GLACIER_GET_REQUEST_H

#include "request.h"

namespace aws {

class GetRequest : public aws::Request {
 public:
  GetRequest();
};

} //namespace aws

#endif //GLACIER_GET_REQUEST_H

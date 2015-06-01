#ifndef GLACIER_POST_REQUEST_H
#define GLACIER_POST_REQUEST_H

#include "request.h"

namespace aws {

class PostRequest : public aws::Request {
 public:
  PostRequest();
};

} //namespace awsl

#endif //GLACIER_POST_REQUEST_H

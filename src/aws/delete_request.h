#ifndef GLACIER_DELETE_REQUEST_H
#define GLACIER_DELETE_REQUEST_H

#include "request.h"

namespace aws {

class DeleteRequest : public aws::Request {
 public:
    DeleteRequest();
};

} //namespace aws

#endif //GLACIER_DELETE_REQUEST_H

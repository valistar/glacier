#ifndef GLACIER_ARCHIVE_H
#define GLACIER_ARCHIVE_H

#include "delete_request.h"
#include "post_request.h"

namespace aws {

class Archive {
 public:
  explicit Archive(std::string filename);
  void upload(aws::PostRequest* request);
  void remove(aws::DeleteRequest* request);
 private:
  std::string filename;

};

} //namespace aws
#endif //GLACIER_ARCHIVE_H

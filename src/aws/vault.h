#ifndef GLACIER_VAULT_H
#define GLACIER_VAULT_H

#include "delete_request.h"

namespace aws {

class Vault {
 public:
  explicit Vault(std::string name);
  explicit Vault(std::string id, std::string name, std::string description);
  void empty(aws::DeleteRequest* request);
  void remove(aws::DeleteRequest* request);


 private:
  aws::VaultAccessPolicy access_policy;
  aws::VaultNotificationConfig notification_config;
  std::string id;
  std::string description;



};

} //namespace aws
#endif //GLACIER_VAULT_H

#ifndef GLACIER_VAULT_H
#define GLACIER_VAULT_H

namespace aws {

class Vault {
 public:
  void empty();
  void remove();


 private:
  VaultAccessPolicy access_policy;
  VaultNotificationConfig notification_config;
  std::string id;
  std::string description;



};

} //namespace aws
#endif //GLACIER_VAULT_H

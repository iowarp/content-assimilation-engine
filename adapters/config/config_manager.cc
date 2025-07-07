#include "adapters/config/config_manager.h"
#include "config_client_default.h"

namespace cae {

/** Global configuration instance */
HSHM_DEFINE_GLOBAL_PTR_VAR_CC(cae::ConfigurationManager, cae_conf)

} // namespace cae
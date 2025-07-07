# Find Python3
find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Set output header path
set(IOWARP_CAE_CONF_DEFAULT_H "${CMAKE_SOURCE_DIR}/adapters/config/config_client_default.h")

# Add custom command to generate header
add_custom_command(
  OUTPUT "${IOWARP_CAE_CONF_DEFAULT_H}"
  COMMAND ${Python3_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/generate_config_default.py"
  DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/generate_config_default.py"
          "${CMAKE_SOURCE_DIR}/config/cae_client_default.yaml"
  COMMENT "Generating default configuration header"
  VERBATIM
)

# Add custom target that depends on the generated file
add_custom_target(IOWARP_CAE_CONF_default
  DEPENDS "${IOWARP_CAE_CONF_DEFAULT_H}"
)

# Add config library
add_library(cae_config SHARED
  config_manager.cc
)

# Add dependencies
target_link_libraries(cae_config
  PUBLIC
    hermes::hermes_core_client
)

# Include directories
target_include_directories(cae_config
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

install(
        TARGETS
        cae_config
        LIBRARY DESTINATION ${CAE_INSTALL_LIB_DIR}
        ARCHIVE DESTINATION ${CAE_INSTALL_LIB_DIR}
        RUNTIME DESTINATION ${CAE_INSTALL_BIN_DIR}
)
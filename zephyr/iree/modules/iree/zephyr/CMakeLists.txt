# Only build integration if enabled in Kconfig
if(NOT CONFIG_IREE_ZEPHYR_RUNTIME)
  return()
endif()

# Module root is one level above this file
get_filename_component(MODULE_ROOT "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)

# Hand off to the real CMakeLists at the module root
include(${MODULE_ROOT}/CMakeLists.txt)
cmake_minimum_required(VERSION 3.20.0)

project(iree_mlp_minst)

message(-=========================)
message(${CMAKE_SOURCE_DIR}/../)


# list(APPEND MODULE_EXT_ROOT
  # ${CMAKE_CURRENT_LIST_DIR}
# )
# if(CONFIG_IREE_ZEPHYR_RUNTIME)
# endif()


list(APPEND ZEPHYR_EXTRA_MODULES
    ${CMAKE_CURRENT_SOURCE_DIR}/modules/iree
)

# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/modules/iree)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

set(NO_THREADSAFE_STATICS $<TARGET_PROPERTY:compiler-cpp,no_threadsafe_statics>)
zephyr_compile_options($<$<COMPILE_LANGUAGE:CXX>:${NO_THREADSAFE_STATICS}>)

file(GLOB app_sources src/*)
target_sources(app PRIVATE ${app_sources})

target_link_libraries(app PRIVATE iree_runtime)

# if(TARGET iree_runtime)
  # target_link_libraries(app PRIVATE iree_runtime)
# endif()
option(USERVER_LTO "Use link time optimizations" OFF)

if(NOT LTO_FLAG AND NOT USERVER_LTO)
  message(STATUS "LTO: disabled (local build)")
elseif(NOT USERVER_LTO)
  message(STATUS "LTO: disabled (user request)")
else()
  message(STATUS "LTO: on")
  include("${USERVER_CMAKE_DIR}/RequireLTO.cmake")
endif()
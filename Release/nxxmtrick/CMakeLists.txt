if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	find_library(COREFOUNDATION CoreFoundation "/" REQUIRED)
	find_library(SECURITY Security "/" REQUIRED)
	target_link_libraries(cpprestsdk LINK_INTERFACE_LIBRARIES ${COREFOUNDATION} ${SECURITY})
endif()

target_compile_definitions(cpprestsdk PUBLIC
  -DCPPREST_FORCE_HTTP_CLIENT_ASIO
  -DCPPREST_FORCE_HTTP_LISTENER_ASIO
  -D_NO_ASYNCRTIMP 
  -D_NO_PPLXIMP)
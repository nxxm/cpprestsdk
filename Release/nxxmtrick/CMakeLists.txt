find_library(COREFOUNDATION CoreFoundation "/" REQUIRED)
find_library(SECURITY Security "/" REQUIRED)
target_link_libraries(cpprestsdk LINK_INTERFACE_LIBRARIES ${COREFOUNDATION} ${SECURITY})

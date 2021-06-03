find_library(COREFOUNDATION CoreFoundation "/")
find_library(SECURITY Security "/")
set_property(TARGET cpprestsdk APPEND PROPERTY LINK_LIBRARIES ${COREFOUNDATION} ${SECURITY})


set(VCPKG_PACKAGES_PATH "C:/Users/kaytc/Documents/CS_Projects/Dependencies/vcpkg/vcpkg/packages")
set(CMAKE_PREFIX_PATH ${VCPKG_PACKAGES_PATH}/curl_x64-windows-static ${VCPKG_PACKAGES_PATH}/zlib_x64-windows-static)

find_package(CURL REQUIRED)
add_executable(genv command_tree.cpp studio.cpp main.cpp)

# include_directories(${VCPKG_PACKAGES_PATH}/curl_x64-windows-static/include)
# target_link_libraries(genv PUBLIC CURL::libcurl)

# set(LIBCURL_STATIC_LIB_DEBUG ${VCPKG_PACKAGES_PATH}/curl_x64-windows-static/debug/lib)
# set(LIBCURL_STATIC_LIB ${VCPKG_PACKAGES_PATH}/curl_x64-windows-static/lib)

# target_link_libraries(genv ${LIBCURL_STATIC_LIB}/libcurl.lib)
# target_link_libraries(genv ${LIBCURL_STATIC_LIB_DEBUG}/libcurl-d.lib)

# if(WIN32)
#   message("Linking ws2_32 library to project ${fname}")
#   target_link_libraries(genv ws2_32)
# endif()
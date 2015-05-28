cmake_minimum_required(VERSION 3.2)
project(Glacier)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(curl)
find_library(EAY32_LIBRARY eay32)
find_library(EAY32SSL_LIBRARY ssleay32)
find_library(SSH2_LIBRARY ssh2)
find_library(ZLIB_LIBRARY zlib1)


include_directories(${CURL_INCLUDE_DIRS})
set(SOURCE_FILES main.cpp src/aws/request.h src/aws/request.cpp src/aws/response.h src/aws/response.cpp src/aws/vault.h src/aws/archive.h src/aws/job.h ${CURL_INCLUDE_DIRS}/curl/curl.h) #How do I make this pick up off include_directories?
add_executable(Glacier ${SOURCE_FILES})



target_link_libraries (Glacier ${EAY32_LIBRARY})
target_link_libraries (Glacier ${SSH2_LIBRARY})
target_link_libraries (Glacier ${EAY32SSL_LIBRARY})
target_link_libraries (Glacier ${ZLIB_LIBRARY})
target_link_libraries (Glacier ${CURL_LIBRARY})
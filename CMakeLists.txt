cmake_minimum_required(VERSION 3.2)
project(Glacier)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(curl)
find_library(EAY32_LIBRARY eay32)
find_library(EAY32SSL_LIBRARY ssleay32)
find_library(SSH2_LIBRARY ssh2)
find_library(ZLIB_LIBRARY zlib1)

#TODO May have to add other boost defines here as well, not clear which are only relevant when building boost and when are relevant when linking.
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
find_package(Boost 1.58.0 REQUIRED COMPONENTS chrono date_time filesystem regex system thread log)
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(BOOST_LIBRARIES ${Boost_CHRONO_LIBRARY_DEBUG} ${Boost_DATE_TIME_LIBRARY_DEBUG} ${Boost_FILESYSTEM_LIBRARY_DEBUG} ${Boost_REGEX_LIBRARY_DEBUG}
    ${Boost_SYSTEM_LIBRARY_DEBUG} ${Boost_THREAD_LIBRARY_DEBUG} ${Boost_LOG_LIBRARY_DEBUG})
else()
    set(BOOST_LIBRARIES ${Boost_CHRONO_LIBRARY_RELEASE} ${Boost_DATE_TIME_LIBRARY_RELEASE} ${Boost_FILESYSTEM_LIBRARY_RELEASE} ${Boost_REGEX_LIBRARY_RELEASE}
    ${Boost_SYSTEM_LIBRARY_RELEASE} ${Boost_THREAD_LIBRARY_RELEASE} ${Boost_LOG_LIBRARY_RELEASE})
endif()

include_directories(${CURL_INCLUDE_DIRS} ${Boost_INCLUDE_DIR}) #TODO Whats this used for...
set(SOURCE_FILES main.cpp src/aws/request.h src/aws/request.cpp src/aws/response.h src/aws/response.cpp src/aws/vault.h
src/aws/archive.h src/aws/job.h  src/aws/get_request.h src/aws/get_request.cpp src/aws/post_request.h src/aws/post_request.cpp
src/aws/delete_request.h src/aws/delete_request.cpp src/aws/put_request.h src/aws/put_request.cpp) #How do I make this pick up off include_directories?
set(HASH_LIB_FILES src/hash/hmac.h src/hash/sha256.h src/hash/sha256.cpp)
add_executable(Glacier ${SOURCE_FILES} ${HASH_LIB_FILES} src/aws/archive.cpp)



target_link_libraries(Glacier ${EAY32_LIBRARY})
target_link_libraries(Glacier ${SSH2_LIBRARY})
target_link_libraries(Glacier ${EAY32SSL_LIBRARY})
target_link_libraries(Glacier ${ZLIB_LIBRARY})
target_link_libraries(Glacier ${CURL_LIBRARY})
target_link_libraries(Glacier ${BOOST_LIBRARIES})
cmake_minimum_required(VERSION 3.2)
project(Glacier)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE} )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

find_package(curl)
find_library(EAY32_LIBRARY eay32)
find_library(EAY32SSL_LIBRARY ssleay32)
find_library(SSH2_LIBRARY ssh2)
find_library(ZLIB_LIBRARY zlib1)

#TODO Fix boost library detection
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
#TODO May have to add other boost defines here as well, not clear which are only relevant when building boost and when are relevant when linking.
if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    find_library(BOOSTCHRONOD_LIBRARY boost_chrono-mgw49-mt-d-1_58)
    find_library(BOOSTDATETIMED_LIBRARY boost_date_time-mgw49-mt-d-1_58)
    find_library(BOOSTFILESYSTEMD_LIBRARY boost_filesystem-mgw49-mt-d-1_58)
    find_library(BOOSTREGEXD_LIBRARY boost_regex-mgw49-mt-d-1_58)
    find_library(BOOSTSYSTEMD_LIBRARY boost_system-mgw49-mt-d-1_58)
    find_library(BOOSTTHREADD_LIBRARY boost_thread-mgw49-mt-d-1_58)
    find_library(BOOSTLOGD_LIBRARY boost_log-mgw49-mt-d-1_58)
    set(BOOST_LIBRARIES ${BOOSTCHRONOD_LIBRARY} ${BOOSTDATETIMED_LIBRARY} ${BOOSTFILESYSTEMD_LIBRARY} ${BOOSTREGEXD_LIBRARY}
    ${BOOSTSYSTEMD_LIBRARY} ${BOOSTTHREADD_LIBRARY} ${BOOSTLOGD_LIBRARY})
else()
    find_library(BOOSTCHRONO_LIBRARY boost_chrono-mgw49-mt-1_58)
    find_library(BOOSTDATETIME_LIBRARY boost_date_time-mgw49-mt-1_58)
    find_library(BOOSTFILESYSTEM_LIBRARY boost_filesystem-mgw49-mt-1_58)
    find_library(BOOSTREGEX_LIBRARY boost_regex-mgw49-mt-1_58)
    find_library(BOOSTSYSTEM_LIBRARY boost_system-mgw49-mt-1_58)
    find_library(BOOSTTHREAD_LIBRARY boost_thread-mgw49-mt-1_58)
    find_library(BOOSTLOG_LIBRARY boost_log-mgw49-mt-1_58)
    set(BOOST_LIBRARIES ${BOOSTCHRONO_LIBRARY} ${BOOSTDATETIME_LIBRARY} ${BOOSTFILESYSTEM_LIBRARY} ${BOOSTREGEX_LIBRARY}
    ${BOOSTSYSTEM_LIBRARY} ${BOOSTTHREAD_LIBRARY} ${BOOSTLOG_LIBRARY})
endif()

include_directories(${CURL_INCLUDE_DIRS})
set(SOURCE_FILES main.cpp src/aws/request.h src/aws/request.cpp src/aws/response.h src/aws/response.cpp src/aws/vault.h
src/aws/archive.h src/aws/job.h  src/aws/get_request.h src/aws/get_request.cpp src/aws/post_request.h src/aws/post_request.cpp
src/aws/delete_request.h src/aws/delete_request.cpp src/aws/put_request.h src/aws/put_request.cpp) #How do I make this pick up off include_directories?
add_executable(Glacier ${SOURCE_FILES})



target_link_libraries(Glacier ${EAY32_LIBRARY})
target_link_libraries(Glacier ${SSH2_LIBRARY})
target_link_libraries(Glacier ${EAY32SSL_LIBRARY})
target_link_libraries(Glacier ${ZLIB_LIBRARY})
target_link_libraries(Glacier ${CURL_LIBRARY})
target_link_libraries(Glacier ${BOOST_LIBRARIES})
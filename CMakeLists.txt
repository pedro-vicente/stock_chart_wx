cmake_minimum_required(VERSION 2.8)
project (stock_chart)
set(BUILD_SHARED_LIBRARIES OFF)

if (CMAKE_VERSION VERSION_LESS "3.1")
 if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
   CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
   CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
   if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
   endif()
 endif()
else ()
  set (CMAKE_CXX_STANDARD 11)
endif()

set(SOURCES ${SOURCES} stock_chart.cc)
set(SOURCES ${SOURCES} stock_chart.hh)
set(SOURCES ${SOURCES} connect_alpha.cc)
add_executable(stock_chart ${SOURCES})

set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH})
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} "/usr/include/curl")

find_path(CURL_INCLUDE curl.h)
if(NOT CURL_INCLUDE)
  message(FATAL_ERROR "curl.h header file not found")
else()
  message("-- Found curl.h header file at: " ${CURL_INCLUDE})
endif()

find_library(CURL_LIBRARY NAMES curl)
if(NOT CURL_LIBRARY)
  message(FATAL_ERROR "curl library not found")
else()
  message("-- Found curl library at: " ${CURL_LIBRARY})
endif()

include_directories(${CURL_INCLUDE})
add_definitions(-DCURL_STATICLIB) 

#//////////////////////////
#link with libraries
#LIB_DEP contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(LIB_DEP ${LIB_DEP})
set(LIB_DEP ${LIB_DEP} ${CURL_LIBRARY})
if (MSVC)
  set(LIB_DEP ${LIB_DEP} ws2_32.lib crypt32.lib)
endif(MSVC)
target_link_libraries (stock_chart ${LIB_DEP})


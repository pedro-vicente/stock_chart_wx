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
set(SOURCES ${SOURCES} tls_socket.cc)
set(SOURCES ${SOURCES} tls_socket.hh)


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


#///////////////////////////////////////////////////////
#OpenSSL
#///////////////////////////////////////////////////////

if(OPENSSL_INCLUDE)
  if(EXISTS "${OPENSSL_INCLUDE}/openssl/ssl.h")
  else()
    message(FATAL_ERROR "${color_red}/openssl/ssl.h not found at ${OPENSSL_INCLUDE}${color_reset}")
  endif()
endif()

find_path(OPENSSL_INCLUDE openssl/ssl.h)
if(OPENSSL_INCLUDE)
  message("-- Found /openssl/ssl.h header file at: " ${OPENSSL_INCLUDE})
else()
  message("-- OpenSSL not found")
endif()

if (OPENSSL_LIBRARY)
  if(EXISTS "${OPENSSL_LIBRARY}")
  else()
    message(FATAL_ERROR "${color_red}Specified ${OPENSSL_LIBRARY} does not exist${color_reset}")
  endif()
endif()

if (OPENSSL_INCLUDE)

  find_library(OPENSSL_LIBRARY NAMES ssl libssl.lib)
  if(NOT OPENSSL_LIBRARY)
    message(FATAL_ERROR "${color_red}OpenSSL library not found${color_reset}")
  else()
    message("-- Found OpenSSL library at: " ${OPENSSL_LIBRARY})
  endif()

  if (CRYPTO_LIBRARY)
    if(EXISTS "${CRYPTO_LIBRARY}")
    else()
      message(FATAL_ERROR "${color_red}Specified ${CRYPTO_LIBRARY} does not exist${color_reset}")
    endif()
  endif()

  find_library(CRYPTO_LIBRARY NAMES crypto libcrypto.lib)
  if(NOT CRYPTO_LIBRARY)
    message(FATAL_ERROR "${color_red}CRYPTO library not found${color_reset}")
  else()
    message("-- Found CRYPTO library at: " ${CRYPTO_LIBRARY})
  endif()

  if(OPENSSL_INCLUDE AND OPENSSL_LIBRARY AND CRYPTO_LIBRARY)
    add_executable(stock_chart ${SOURCES})
    target_include_directories(stock_chart PRIVATE ${OPENSSL_INCLUDE})
    if (MSVC)
    else()
      set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -pthread")
    endif(MSVC)
    target_link_libraries (stock_chart ${LIB_DEP} ${OPENSSL_LIBRARY} ${CRYPTO_LIBRARY} ${CMAKE_DL_LIBS}) 
  endif()

endif(OPENSSL_INCLUDE)


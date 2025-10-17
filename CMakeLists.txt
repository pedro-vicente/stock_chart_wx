cmake_minimum_required(VERSION 3.28)
project (trade)

message(STATUS "Source directory is " ${CMAKE_SOURCE_DIR})
message(STATUS "Build directory is " ${CMAKE_CURRENT_BINARY_DIR})

#//////////////////////////
# asio
#//////////////////////////

set(CMAKE_CXX_STANDARD 17)
add_definitions(-DASIO_STANDALONE)
add_definitions(-DASIO_HAS_STD_ADDRESSOF)
add_definitions(-DASIO_HAS_STD_ARRAY)
add_definitions(-DASIO_HAS_CSTDINT)
add_definitions(-DASIO_HAS_STD_SHARED_PTR)
add_definitions(-DASIO_HAS_STD_TYPE_TRAITS)
add_definitions(-DASIO_HAS_VARIADIC_TEMPLATES)
add_definitions(-DASIO_HAS_STD_FUNCTION)
add_definitions(-DASIO_HAS_STD_CHRONO)
add_definitions(-DBOOST_ALL_NO_LIB)
if (MSVC)
  add_definitions(-D_WIN32_WINNT=0x0501)
  add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
endif()

include_directories(ext/asio-1.30.2/asio/include)

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

#//////////////////////////
# OpenSSL
#//////////////////////////

if (MSVC)
  if (OPEN_SSL_SOURCE)
    set(OPENSSL_ROOT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/ext/openssl-3.0.5)
    set(OPENSSL_INCLUDE_DIR ${OPENSSL_ROOT_DIR}/include)
    set(OPENSSL_CRYPTO_LIBRARY ${OPENSSL_ROOT_DIR}/libcrypto.lib)
    set(OPENSSL_SSL_LIBRARY ${OPENSSL_ROOT_DIR}/libssl.lib)
  else()
    set(OPENSSL_ROOT_DIR $ENV{OPENSSL_ROOT_DIR})
    message(STATUS "OpenSSL root folder from environment, (OPENSSL_ROOT_DIR): ${OPENSSL_ROOT_DIR}")
  endif()
endif()

find_package(OpenSSL REQUIRED)

message(STATUS "OpenSSL root: ${OPENSSL_ROOT_DIR}")
message(STATUS "OpenSSL include: ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL libs: ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY}")

if(UNIX)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

include_directories(${OPENSSL_INCLUDE_DIR})

set(lib_dep ${lib_dep} ${OPENSSL_SSL_LIBRARY} ${OPENSSL_CRYPTO_LIBRARY})
if (MSVC)
  set(lib_dep ${lib_dep} crypt32.lib)
endif()

#//////////////////////////
# source files 
#//////////////////////////

set(src ${src})
set(src ${src} src/ssl_read.hh)
set(src ${src} src/ssl_read.cc)
set(src ${src} src/get.hh)
set(src ${src} src/get.cc)

#//////////////////////////
# create static library from common source files
#//////////////////////////

add_library(get STATIC ${src})
target_link_libraries(get ${lib_dep})

#//////////////////////////
# http_client executable
#//////////////////////////

add_executable(http_client src/http_client.cc)
target_link_libraries (http_client get ${lib_dep})

#//////////////////////////
# Linux/Mac compiling flags and needed libraries 
# build/wxwidgets-3.3.1/wx-config --cppflags
# build/wxwidgets-3.3.1/wx-config --libs
#//////////////////////////

#//////////////////////////
# Linux/Mac
#//////////////////////////

if(UNIX AND NOT APPLE)
  set(LINUX TRUE)
endif()

if(APPLE)
  message(STATUS "Mac OS build...")
endif()

#//////////////////////////
# wxWidgets library locations 
#//////////////////////////

if (MSVC)
  include_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib/vc_x64_lib/mswud)
  link_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib/vc_x64_lib)
elseif(APPLE)
  include_directories(/usr/local/include/)
  include_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib/wx/include/osx_cocoa-unicode-static-3.3)
  link_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib)
  add_definitions(-DWXUSINGDLL -D__WXOSX_COCOA__ -D__WXMAC__ -D__WXOSX__)
elseif(LINUX)
  include_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib/wx/include/gtk3-unicode-static-3.3)
  link_directories(${CMAKE_SOURCE_DIR}/build/wxwidgets-3.3.1/lib)
  add_definitions(-D__WXGTK3__ -D__WXGTK__)
endif()

#//////////////////////////
# wxWidgets source
#//////////////////////////

include_directories(${CMAKE_SOURCE_DIR}/ext/wxwidgets-3.3.1/include)

#//////////////////////////
# sources 
#//////////////////////////

set(src ${src})
if(APPLE)
  set(src ${src} ${CMAKE_SOURCE_DIR}/ext/wxwidgets-3.3.1/src/osx/carbon/wxmac.icns)
endif()
if (MSVC)
  set(src ${src} ${CMAKE_SOURCE_DIR}/src/sample.rc)
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

#//////////////////////////
# trader
#//////////////////////////

set(src ${src} src/grafix.hh)
set(src ${src} src/grafix.cc)
set(src ${src} src/display.cc)
set(src ${src} src/display.hh)
set(src ${src} src/chart.hh)
set(src ${src} src/chart.cc)
set(src ${src} src/model.hh)
set(src ${src} src/model.cc)
add_executable(trader WIN32 MACOSX_BUNDLE src/trader.cc src/trader.hh ${src})
if (MSVC)
  set_target_properties(trader PROPERTIES LINK_FLAGS "/SubSystem:\"Windows\"" VS_DPI_AWARE "ON")
  set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT trader)
endif()

#//////////////////////////
# link with libraries
# lib_dep contains a cascade definition of all the libraries needed to link
#//////////////////////////

set(lib_dep ${lib_dep})

if (MSVC)
  set(lib_dep ${lib_dep} wxbase33ud wxmsw33ud_core wxmsw33ud_webview wxbase33ud_net wxpngd wxzlibd wxregexud wxmsw33ud_html)
  set(lib_dep ${lib_dep} wxscintillad wxmsw33ud_stc wxlexillad)
  set(lib_dep ${lib_dep} comctl32.lib rpcrt4.lib ws2_32.lib)
endif()

if (LINUX)
  set(lib_dep ${lib_dep} wx_gtk3u_xrc-3.3 wx_gtk3u_qa-3.3 wx_baseu_net-3.3 wx_gtk3u_html-3.3 wx_gtk3u_core-3.3)
  set(lib_dep ${lib_dep} wx_baseu-3.3)
  set(lib_dep ${lib_dep} gtk-3 gdk-3 pangocairo-1.0 pango-1.0 harfbuzz atk-1.0 cairo-gobject cairo gdk_pixbuf-2.0 gio-2.0 gobject-2.0)
  set(lib_dep ${lib_dep} X11 glib-2.0 png jpeg z xkbcommon)
endif()

if(APPLE)
  set(lib_dep ${lib_dep} wx_osx_cocoau_xrc-3.3 wx_osx_cocoau_qa-3.3 wx_baseu_net-3.3 wx_osx_cocoau_html-3.3 wx_osx_cocoau_core-3.3)
  set(lib_dep ${lib_dep} wx_baseu_xml-3.3 wx_baseu-3.3 wx_osx_cocoau_webview-3.3)
  set(lib_dep ${lib_dep} wx_osx_cocoau_core-3.3 wxregexu-3.3 wxjpeg-3.3 wxpng-3.3 wxtiff-3.3 z iconv)
  set(lib_dep ${lib_dep} wx_osx_cocoau_stc-3.3)
  set(lib_dep ${lib_dep} wxscintilla-3.3 wxlexilla-3.3)
  find_library(COREFOUNDATION CoreFoundation)
  if (NOT COREFOUNDATION)
      message(FATAL_ERROR "CoreFoundation not found")
  endif()
  find_library(SECURITY Security)
  find_library(CARBON Carbon)
  find_library(COCOA Cocoa)
  find_library(IOKIT IOKit)
  find_library(WEBKIT WebKit)
  find_library(ANIMATION QuartzCore)
  set(lib_dep ${lib_dep} ${COREFOUNDATION} ${SECURITY} ${CARBON} ${COCOA} ${IOKIT} ${WEBKIT} ${ANIMATION})
endif()

#//////////////////////////
# LINUX
# order of the link libraries matters
#//////////////////////////

if(LINUX)
  set(lib_dep ${lib_dep} pthread dl)
  find_program(LSB_RELEASE_EXEC lsb_release)
  execute_process(COMMAND ${LSB_RELEASE_EXEC} -is OUTPUT_VARIABLE LSB_RELEASE_ID_SHORT OUTPUT_STRIP_TRAILING_WHITESPACE)
  message(STATUS "Building in " ${LSB_RELEASE_ID_SHORT})
endif()

target_link_libraries(trader ${lib_dep})

#//////////////////////////
# APPLE
#//////////////////////////

if(APPLE)
  set(VERSION "1.0")
  set(COPYRIGHT "Sarsat")
  if(NOT IPHONE)
    set_target_properties(trader PROPERTIES
      MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/ext/wxwidgets-3.3.1/src/osx/carbon/Info.plist.in"
      RESOURCE "${CMAKE_SOURCE_DIR}/ext/wxwidgets-3.3.1/src/osx/carbon/wxmac.icns")
  endif()
  set_target_properties(trader PROPERTIES
    MACOSX_BUNDLE_ICON_FILE wxmac.icns
    MACOSX_BUNDLE_LONG_VERSION_STRING "${VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${VERSION}"
    MACOSX_BUNDLE_VERSION "${VERSION}"
    MACOSX_BUNDLE_COPYRIGHT "${COPYRIGHT}"
    MACOSX_BUNDLE_GUI_IDENTIFIER "org.wxwidgets.qms"
    )
  message(STATUS "MacOS bundle version: " ${VERSION})
endif()

#//////////////////////////
# copy data to build folder
#//////////////////////////

message(STATUS "Copying resources to: ${CMAKE_BINARY_DIR}")
file(COPY "${CMAKE_SOURCE_DIR}/resources/SPY.csv" DESTINATION ${CMAKE_BINARY_DIR})
file(COPY "${CMAKE_SOURCE_DIR}/resources/api_key.txt" DESTINATION ${CMAKE_BINARY_DIR})

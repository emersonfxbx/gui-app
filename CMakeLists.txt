cmake_minimum_required(VERSION 3.20)

project(gui-app
    VERSION 0.99.0.1
    DESCRIPTION "Gtk application template"
    HOMEPAGE_URL "http://somesite.com/gui-app")

set(APP_COPYRIGHT_NOTE "Copyright (C) 2021")
set(APP_COMPANY_AUTHOR "emersonfxbx@msn.com")

find_package(PkgConfig REQUIRED)
pkg_check_modules(Gtkmm4 REQUIRED IMPORTED_TARGET gtkmm-4.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(gtk)

add_executable(${PROJECT_NAME})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    "include")

target_sources(${PROJECT_NAME} PRIVATE
    "src/app_window.cpp"
    "src/main.cpp")
configure_file("include/version.h.in" "version.h")
gtk_add_gresources(TARGET ${PROJECT_NAME} RES_FILE "res/app.gresources.xml")

target_link_libraries(${PROJECT_NAME} PRIVATE PkgConfig::Gtkmm4)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
    target_compile_definitions(${PROJECT_NAME} PRIVATE
        "-D_WIN32_WINNT=_WIN32_WINNT_WIN10"
        "-DUNICODE"
        "-D_UNICODE")
    target_sources(${PROJECT_NAME} PRIVATE
        "res/win32/app.rc"
        "src/win32/theme_synchronizer.cpp")
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        gtk4_win_deploy(TARGET ${PROJECT_NAME})
    endif()
# MSYS2/MINGW windres missing fix
# ln -s $MINGW_PREFIX/bin/windres.exe $MINGW_PREFIX/bin/x86_64-w64-mingw32-windres.exe
endif()

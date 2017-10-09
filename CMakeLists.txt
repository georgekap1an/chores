cmake_minimum_required(VERSION 2.8)

project(com.github.georgekap1an.chores)

find_package(PkgConfig)

pkg_check_modules(GTKMM gtkmm-3.0)

# Translation files
set (GETTEXT_PACKAGE "${CMAKE_PROJECT_NAME}")
add_definitions (-DGETTEXT_PACKAGE=\"${GETTEXT_PACKAGE}\")
add_subdirectory (po)

add_subdirectory (src)

add_subdirectory (data)



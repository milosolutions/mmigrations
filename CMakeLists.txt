if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif ()

cmake_minimum_required(VERSION 3.5)

project(mmigrations
  VERSION 0.0.1
  DESCRIPTION "Milo SQL Migrations System"
  LANGUAGES CXX
)

find_package(Qt5 COMPONENTS REQUIRED Core Sql)

set(CMAKE_AUTOMOC ON)

set(SOURCES
  connectionproviders/connectionproviderbase.h
  connectionproviders/connectionprovidersqlite.h
  connectionproviders/connectionprovidersqlitecipher.h
  connectionproviders/connectionprovidersqlitesee.h
  dbhelpers.h
  migration.h
  migrationmanager.h
  abstractmigrationmanager.h
  migrationsdata.h
  connectionproviders/connectionproviderbase.cpp
  connectionproviders/connectionprovidersqlite.cpp
  connectionproviders/connectionprovidersqlitecipher.cpp
  connectionproviders/connectionprovidersqlitesee.cpp
  dbhelpers.cpp
  migration.cpp
  abstractmigrationmanager.cpp
)

set(OTHER_FILES README.md mmigrations.doxyfile)

add_library(mmigrations STATIC ${SOURCES} ${OTHER_FILES})

target_include_directories(mmigrations
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(mmigrations
  Qt5::Core
  Qt5::Sql
)

add_subdirectory(tst_mmigrations)
add_subdirectory(example)

if (POLICY CMP0048)
        cmake_policy(SET CMP0048 NEW)
endif ()

cmake_minimum_required(VERSION 3.5)

project(mmigrations
  VERSION 0.0.1
  DESCRIPTION "Milo SQL Migrations System"
  LANGUAGES CXX
)

find_package(Qt5 COMPONENTS Core Sql REQUIRED)

set(SOURCES 
  connectionproviders/connectionproviderbase.h
  connectionproviders/connectionprovidersqlite.h
  connectionproviders/connectionprovidersqlitecipher.h
  dbhelpers.h
  migration.h
  migrationmanager.h
  abstractmigrationmanager.h
  migrationsdata.h
  connectionproviders/connectionproviderbase.cpp
  connectionproviders/connectionprovidersqlite.cpp
  connectionproviders/connectionprovidersqlitecipher.cpp
  dbhelpers.cpp
  migration.cpp
  abstractmigrationmanager.cpp
)

set(OTHER_FILES README.md mmigrations.doxyfile)

add_library(mmigrations OBJECT ${SOURCES} ${OTHER_FILES})

target_include_directories(mmigrations
  PUBLIC
  $<INSTALL_INTERFACE:include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

qt5_use_modules(mmigrations Core Sql)

enable_testing()
add_subdirectory(tst_mmigrations)
add_test(NAME MMigrationsTest COMMAND mmigrations_Test)


project(mmigrations
  VERSION 0.0.1
  DESCRIPTION "Milo SQL Migrations System"
  LANGUAGES CXX
)

find_package(Qt5 COMPONENTS REQUIRED Core Sql)

set(SOURCES connectionproviders/connectionproviderbase.h
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

target_link_libraries(mmigrations
  Qt5::Core
  Qt5::Sql
)

message("MMigrations: missing unit test!")
#add_subdirectory(tst_mmigrations)

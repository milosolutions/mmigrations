find_package(Qt5 COMPONENTS REQUIRED Core Sql)

set(CMAKE_AUTOMOC ON)

add_executable(example-migrations dbmigrationsdata.cpp main.cpp)

target_link_libraries(example-migrations mmigrations
  Qt5::Core Qt5::Sql
)

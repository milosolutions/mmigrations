find_package(Qt5 COMPONENTS REQUIRED Core Sql Test)

set(CMAKE_AUTOMOC ON)

add_executable(mmigrations_test tst_migrations.cpp)

target_link_libraries(mmigrations_test mmigrations
  Qt5::Core Qt5::Sql Qt5::Test
)

add_test(mmigrations_test mmigrations_test)

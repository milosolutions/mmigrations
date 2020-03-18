find_package(Qt5 COMPONENTS Core Sql REQUIRED)

set(CMAKE_AUTOMOC ON)

set(TEST_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/tst_migrations.cpp	
)

add_executable(mmigrations_test ${TEST_SRC})

qt5_use_modules(mmigrations_test Core Sql)

target_link_libraries(mmigrations_test mmigrations)

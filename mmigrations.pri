QT *= sql

INCLUDEPATH += $$PWD

SOURCES += \
        $$PWD/connectionproviders/connectionproviderbase.cpp \
        $$PWD/connectionproviders/connectionprovidersqlite.cpp \
        $$PWD/connectionproviders/connectionprovidersqlitecipher.cpp \
        $$PWD/dbhelpers.cpp \
        $$PWD/migration.cpp \
        $$PWD/abstractmigrationmanager.cpp

HEADERS += \
        $$PWD/connectionproviders/connectionproviderbase.h \
        $$PWD/connectionproviders/connectionprovidersqlite.h \
        $$PWD/connectionproviders/connectionprovidersqlitecipher.h \
        $$PWD/dbhelpers.h \
        $$PWD/migration.h \
        $$PWD/migrationmanager.h \
        $$PWD/abstractmigrationmanager.h
        $$PWD/migrationsdata.h

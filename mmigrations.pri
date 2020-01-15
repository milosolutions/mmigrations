QT *= sql

INCLUDEPATH += $$PWD

SOURCES += \
        $$PWD/connectionproviders/dbconnectionproviderbase.cpp \
        $$PWD/connectionproviders/dbconnectionprovidersqlite.cpp \
        $$PWD/dbhelpers.cpp \
        $$PWD/dbmigration.cpp \
        $$PWD/mmigrations.cpp \
        $$PWD/dbmigrationmanager.cpp

HEADERS += \
        $$PWD/connectionproviders/dbconnectionproviderbase.h \
        $$PWD/connectionproviders/dbconnectionprovidersqlite.h \
        $$PWD/dbhelpers.h \
        $$PWD/dbmigration.h \
        $$PWD/dbmigrationmanager.h \
        $$PWD/mmigrations.h
        $$PWD/dbmigrationsdata.h

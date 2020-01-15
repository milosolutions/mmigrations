QT += core
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = exampleMigration
TEMPLATE = app

SOURCES +=  main.cpp \
            dbmigrationsdata.cpp

include(../mmigrations.pri)

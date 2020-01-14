QT += core
QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle
TARGET = exampleMigration
TEMPLATE = app

SOURCES +=  main.cpp \
            databasemanager.cpp \
            dbmigrationsdata.cpp

HEADERS += databasemanager.h
include(../mmigrations.pri)

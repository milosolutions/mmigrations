QT += core
QT -= gui

CONFIG += c++17
CONFIG -= app_bundle
TARGET = exampleMigration
TEMPLATE = app

SOURCES += main.cpp

include(../mmigrations.pri)

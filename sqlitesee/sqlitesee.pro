QT += core sql sql-private

android {
  TARGET = plugins_sqldrivers_qsqlitesee
} else {
  TARGET = qsqlitesee
}

TEMPLATE = lib

CONFIG += c++14 plugin
DEFINES += "SQLITE_THREADSAFE=1" \
   SQLITE_ENABLE_FTS3 SQLITE_ENABLE_FTS3_PARENTHESIS \
   SQLITE_ENABLE_FTS4 \
   SQLITE_ENABLE_FTS5 \
   SQLITE_ENABLE_JSON1 \
   SQLITE_ENABLE_RTREE \
   SQLITE_ENABLE_EXPLAIN_COMMENTS \
   SQLITE_ENABLE_COLUMN_METADATA \
   SQLITE_OMIT_LOAD_EXTENSION \
   SQLITE_OMIT_COMPLETE \
   HAVE_USLEEP HAVE_READLINE \
   QT_NO_CAST_TO_ASCII QT_NO_CAST_FROM_ASCII

HEADERS += qsql_sqlite_p.h sqlite3.h
SOURCES += qsql_sqlite.cpp smain.cpp sqlite3-see-aes256-ofb.c
OTHER_FILES += sqlitesee.json

QT_FOR_CONFIG += sqldrivers-private
PLUGIN_CLASS_NAME = QSQLiteSeeDriverPlugin
PLUGIN_TYPE = sqldrivers

target.path = $$[QT_INSTALL_PLUGINS]/sqldrivers/
INSTALLS += target

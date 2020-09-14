#!/bin/bash

if [ "${1}" = "-h" ] || [ "${1}" = "--help" ]; then
  echo "Usage: sqlitesee_install.sh [path-to-qt-sources]"
  echo
  echo "Compiles and installs SQLiteSee for given Qt version. Plugin is "
  echo "installed. qmake has to be in path - plugin will be installed into Qt "
  echo "that qmake is coming from."
  echo "For Android, make sure ANDROID_NDK_ROOT is properly set."
  echo
  echo "Directory 'sqlitesee-source' must contain SQLiteSee source code "
  echo "(amalgamated)."
  echo
  echo "If no path to Qt sources is given, script will download it automatically "
  echo "using Qt version provided by qmake."
  echo
  echo "Run this script only from within milo/mmigrations/sqlitesee directory!"
  exit
fi

QT_SOURCE="${1}"

if [ -z $QT_SOURCE ]; then
  echo "Qt source directory has not been set! Qt source code will be downloaded "
  echo "based on qmake version in PATH."
fi

if [ ! -x "$(command -v qmake)" ]; then
  echo "qmake binary has not been found!"
  exit 1
fi

if [ -z $QT_SOURCE ]; then
  QT_VER=$(qmake -query QT_VERSION)
  IFS='.' read -ra split <<< "$QT_VER"
  QT_VER_MINOR=${split[0]}.${split[1]}
  echo "Downloading Qt source code, version: "${QT_VER}", "${QT_VER_MINOR}
  
  if [ ! -d "qt-src" ]; then  
    mkdir -p qt-src
  fi
  
  cd qt-src
  
  if [ ! -d "qtbase-everywhere-src-${QT_VER}" ]; then  
    wget https://download.qt.io/archive/qt/${QT_VER_MINOR}/${QT_VER}/submodules/qtbase-everywhere-src-${QT_VER}.tar.xz
    tar -xaf qtbase-everywhere-src-${QT_VER}.tar.xz
  fi
  QT_SOURCE=${PWD}/qtbase-everywhere-src-${QT_VER}
  QSQL_SOURCE="${QT_SOURCE}/src/plugins/sqldrivers"
  
  cd ..
else
  QSQL_SOURCE="${QT_SOURCE}/qtbase/src/plugins/sqldrivers"
fi

QSQLITE_SOURCE="${QSQL_SOURCE}/sqlite"

SQLITE_SEE_SOURCE="${PWD}/sqlitesee-sources"
PLUGIN_BUILD="${PWD}/plugin-build"

if [ ! -d $QT_SOURCE ]; then
  echo "Qt source directory has not been found!"
  exit 3
fi

if [ ! -d $QSQL_SOURCE ]; then
  echo "Qt plugins source directory has not been found!"
  exit 4
fi

if [ ! -d $QSQLITE_SOURCE ]; then
  echo "Qt SQLite plugin source directory has not been found!"
  exit 5
fi

if [ ! -d $SQLITE_SEE_SOURCE ]; then
  echo "Qt SQLite SEE source directory has not been found!"
  exit 6
fi

echo "Preparing QSQLiteSee plugin"

cp -r ${QSQLITE_SOURCE} ${PLUGIN_BUILD}

cp ${SQLITE_SEE_SOURCE}/sqlite3-see-aes256-ofb.c ${PLUGIN_BUILD}/sqlite3-see-aes256-ofb.c
cp ${SQLITE_SEE_SOURCE}/sqlite3.h ${PLUGIN_BUILD}/sqlite3.h

cp -f smain.cpp ${PLUGIN_BUILD}/smain.cpp
cp -f sqlitesee.json ${PLUGIN_BUILD}/sqlitesee.json
cp -f sqlitesee.pro ${PLUGIN_BUILD}/sqlitesee.pro
rm ${PLUGIN_BUILD}/sqlite.json
rm ${PLUGIN_BUILD}/sqlite.pro

cd "${PLUGIN_BUILD}"

echo "Compiling QSQLiteSee plugin in ${PWD}"

qmake sqlitesee.pro
make -j 4

IS_ANDROID=$(qmake -query QMAKE_XSPEC | grep -c android)
if [ "$IS_ANDROID" = "1" ]; then
  # Android, qmake INSTALLS directive fails. Use bash as workaround...
  QT_PLUGIN_DIR=$(qmake -query QT_INSTALL_PLUGINS)
  cp libplugins_sqldrivers_qsqlitesee* "${QT_PLUGIN_DIR}/sqldrivers/"
else
  make install
fi

cd ..

echo "Cleanup"
rm -rf ${PLUGIN_BUILD}

echo "Done. You can now use it by loading QSQLITESEE plugin in Qt code"

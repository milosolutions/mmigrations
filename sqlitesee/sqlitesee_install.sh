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

if [ ! -x "$(command -v qmake)" ]; then
  echo "qmake binary has not been found in PATH!"
  exit 1
fi

IS_ANDROID=$(qmake -query QMAKE_XSPEC | grep -c android)
IS_MSVC=$(qmake -query QMAKE_XSPEC | grep -c msvc)

if [ "$IS_MSVC" = "1" ]; then
  MSVC_VERSION=${1}
  QT_SOURCE="${2}"

  if [ -z $MSVC_VERSION ]; then
    echo "MSVC version not provided. It should be something like 2017 or 2019."
    exit 2
  fi
else
  QT_SOURCE="${1}"
fi

if [ -z $QT_SOURCE ]; then
  echo "Qt source directory has not been set! Qt source code will be downloaded "
  echo "based on qmake version in PATH."
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

if [ "$IS_MSVC" = "1" ]; then
  MSVC_PATH="c:/Program Files (x86)/Microsoft Visual Studio/${MSVC_VERSION}/Community/VC/Auxiliary/Build/vcvarsall.bat"
  cmd.exe /c "\"${MSVC_PATH}\" x86_amd64 && dir && qmake sqlitesee.pro && nmake && nmake install"
else
  qmake sqlitesee.pro
  make -j 4

  if [ "$IS_ANDROID" = "1" ]; then
    # Android, qmake INSTALLS directive fails. Use bash as workaround...
    QT_PLUGIN_DIR=$(qmake -query QT_INSTALL_PLUGINS)
    cp libplugins_sqldrivers_qsqlitesee* "${QT_PLUGIN_DIR}/sqldrivers/"
  else
    make install
  fi
fi

cd ..

echo "Cleanup"
rm -rf ${PLUGIN_BUILD}

echo "Done. You can now use it by loading QSQLITESEE plugin in Qt code"

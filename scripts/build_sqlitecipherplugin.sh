#!/bin/bash

# erase "--branch 'v1.2' --single-branch" to try most recent code
git clone --branch 'v1.2' --single-branch https://github.com/devbean/QtCipherSqlitePlugin.git
mkdir cipherPluginBuild

cd cipherPluginBuild

# uncomment and adaptfollowing line to provide desired version of qt
#QMAKE=$HOME/Qt/5.14.0/gcc_64/bin/qmake
if [ -z "$QMAKE" ]; then
    # if qmake was not provided than use default
    echo Using default qmake
    echo This will probably fail because plugins needs private headers
    echo which are not provided in systems qt.
    QMAKE=qmake
fi

$QMAKE ../QtCipherSqlitePlugin/QtCipherSqlitePlugin.pro
make
# will copy created plugin to qt plugins directory (related to provided qmake)
make install

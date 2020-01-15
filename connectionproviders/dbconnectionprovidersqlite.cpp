#include "dbconnectionprovidersqlite.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

void db::ConnectionProviderSQLite::setupConnectionData(
                    const QString &databasePath, const QString &connectionName)
{
    if (!databaseExist(databasePath)) {
        createDatabase(databasePath);
    }

    auto db = createDatabaseConnection(connectionName);
    db.setDatabaseName(databasePath);
}

bool db::ConnectionProviderSQLite::databaseExist(const QString &databasePath)
{
    return QFile::exists(databasePath);
}

bool db::ConnectionProviderSQLite::createDatabase(const QString &databasePath)
{
    if (!QFileInfo(databasePath).absoluteDir().mkpath(".")) {
        qCCritical(migrations) << "Cannot create a directory for database: " << databasePath;
        return false;
    }
    return true;
}

db::ConnectionProviderSQLite::ConnectionProviderSQLite()
    : ConnectionProviderBase("QSQLITE")
{}

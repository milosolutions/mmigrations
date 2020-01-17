#include "dbconnectionprovidersqlite.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

mdatabase::ConnectionProviderSQLite &mdatabase::ConnectionProviderSQLite::instance()
{
    static ConnectionProviderSQLite cp;
    return cp;
}

void mdatabase::ConnectionProviderSQLite::setupConnectionData(
                    const QString &databasePath, const QString &connectionName)
{
    if (!databaseExist(databasePath)) {
        createDatabase(databasePath);
    }

    auto db = createDatabaseConnection(connectionName);
    db.setDatabaseName(databasePath);
}

bool mdatabase::ConnectionProviderSQLite::databaseExist(const QString &databasePath)
{
    return QFile::exists(databasePath);
}

bool mdatabase::ConnectionProviderSQLite::createDatabase(const QString &databasePath)
{
    if (!QFileInfo(databasePath).absoluteDir().mkpath(".")) {
        qCCritical(migrations) << "Cannot create a directory for database: " << databasePath;
        return false;
    }
    return true;
}

mdatabase::ConnectionProviderSQLite::ConnectionProviderSQLite()
    : ConnectionProviderBase("QSQLITE")
{}

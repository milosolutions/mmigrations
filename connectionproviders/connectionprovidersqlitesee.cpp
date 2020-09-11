#include "connectionprovidersqlitesee.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

mdatabase::ConnectionProviderSQLiteSee &mdatabase::ConnectionProviderSQLiteSee::instance()
{
    static ConnectionProviderSQLiteSee cp;
    return cp;
}

void mdatabase::ConnectionProviderSQLiteSee::setupConnectionData(
                    const QString &databasePath, const QString &connectionName)
{
    if (!databaseExist(databasePath)) {
        createDatabase(databasePath);
    }

    auto db = createDatabaseConnection(connectionName);
    db.setDatabaseName(databasePath);
}

bool mdatabase::ConnectionProviderSQLiteSee::databaseExist(const QString &databasePath)
{
    return QFile::exists(databasePath);
}

bool mdatabase::ConnectionProviderSQLiteSee::createDatabase(const QString &databasePath)
{
    if (!QFileInfo(databasePath).absoluteDir().mkpath(".")) {
        qCCritical(migrations) << "Cannot create a directory for database: " << databasePath;
        return false;
    }
    return true;
}

mdatabase::ConnectionProviderSQLiteSee::ConnectionProviderSQLiteSee()
    : ConnectionProviderBase("QSQLITESEE")
{}

mdatabase::ConnectionProviderSQLiteSee::ConnectionProviderSQLiteSee(const QString &type)
    : ConnectionProviderBase(type)
{}

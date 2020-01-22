#include "connectionprovidersqlitecipher.h"

#include <QDebug>
#include <QFileInfo>
#include <QFile>
#include <QDir>
#include <QLoggingCategory>

Q_DECLARE_LOGGING_CATEGORY(migrations)

mdatabase::ConnectionProviderSQLiteCipher &mdatabase::ConnectionProviderSQLiteCipher::instance()
{
    static ConnectionProviderSQLiteCipher cp;
    return cp;
}

mdatabase::ConnectionProviderSQLiteCipher::ConnectionProviderSQLiteCipher() :
    ConnectionProviderSQLite("SQLITECIPHER")
{

}

void mdatabase::ConnectionProviderSQLiteCipher::setPassword(const QString &password)
{
    mPassword = password;
}

QSqlDatabase mdatabase::ConnectionProviderSQLiteCipher::databaseConnection(const QString &connectionName) const
{
    auto db = ConnectionProviderBase::databaseConnection(connectionName);
    db.setPassword(mPassword);
    return db;
}

void mdatabase::ConnectionProviderSQLiteCipher::setupConnectionData(
                    const QString &databasePath, const QString &connectionName)
{
    if (!databaseExist(databasePath)) {
        createDatabase(databasePath);
    }

    auto db = createDatabaseConnection(connectionName);
    db.setDatabaseName(databasePath);
    db.setPassword(mPassword);
}

bool mdatabase::ConnectionProviderSQLiteCipher::checkPluginAvailable() const
{
    return !QSqlDatabase::drivers().filter("SQLITECIPHER").empty();
}

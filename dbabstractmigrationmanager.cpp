#include "dbabstractmigrationmanager.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QStandardPaths>

Q_DECLARE_LOGGING_CATEGORY(migrations)

//extern template class db::MigrationManager<db::ConnectionProviderSQLite>;

namespace db {

const QLatin1String AbstractMigrationManager::scDbName = QLatin1String("local.db");

AbstractMigrationManager::AbstractMigrationManager(QObject *parent)
    : QObject(parent), cDbPath(QStandardPaths::writableLocation(
                            QStandardPaths::AppDataLocation) + "/" + scDbName)
{
    connect(&mMigrationProgress, &QFutureWatcher<bool>::finished,
            this, [this]() {
        if (!mMigrationProgress.result()) {
            emit databaseUpdateError();
        }

        emit databaseReady();
    });
}

}


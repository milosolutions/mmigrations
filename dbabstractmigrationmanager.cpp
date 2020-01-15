#include "dbabstractmigrationmanager.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QStandardPaths>

Q_DECLARE_LOGGING_CATEGORY(migrations)

//extern template class db::MigrationManager<db::ConnectionProviderSQLite>;

namespace db {

AbstractMigrationManager::AbstractMigrationManager(QObject *parent)
    : QObject(parent)
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


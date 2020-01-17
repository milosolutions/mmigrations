#include "abstractmigrationmanager.h"

#include <QFuture>
#include <QLoggingCategory>
#include <QStandardPaths>

Q_DECLARE_LOGGING_CATEGORY(migrations)

namespace mdatabase {

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


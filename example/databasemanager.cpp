#include "databasemanager.h"

#include <QFuture>
#include <QtConcurrent/QtConcurrent>

extern template class db::MigrationManager<db::ConnectionProviderSQLite>;

const QLatin1String DatabaseManager::sc_dbName = QLatin1String("local.db");

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent), c_dbPath(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/" + sc_dbName)
{
    connect(&m_migrationProgress, &QFutureWatcher<bool>::finished,
            this, [this]() {
        if (!m_migrationProgress.result()) {
            emit databaseUpdateError();
        }

        emit databaseReady();
    });
}

void DatabaseManager::setupDatabase()
{
    Q_ASSERT_X(!m_setupDone, __PRETTY_FUNCTION__, "Trying to setup database twice");
    if (!m_setupDone) {
        db::ConnectionProviderSQLite::instance().setupConnectionData(c_dbPath);
        qDebug() << "Database path:" << c_dbPath;

        m_migrationManager.loadVersion();
        if (m_migrationManager.needsUpdate()) {
            emit databaseUpdateStarted();

            m_migrationRunner = QtConcurrent::run(std::bind(&MigrationManager::update, &m_migrationManager));
            m_migrationProgress.setFuture(m_migrationRunner);
        } else {
            emit databaseReady();
        }

        m_setupDone = true;
    }
}

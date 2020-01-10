#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QFutureWatcher>

#include "connectionproviders/dbconnectionprovidersqlite.h"
#include "dbmigrationmanager.h"

class DatabaseManager : public QObject
{
    Q_OBJECT
public:
    using ConnectionProvider = db::ConnectionProviderSQLite;
    explicit DatabaseManager(QObject *parent = nullptr);

    void setupDatabase();

signals:
    void databaseReady();

    void databaseUpdateStarted() const;
    void databaseUpdateError() const;

private:
    using MigrationManager = db::MigrationManager<ConnectionProvider>;

    static const QLatin1String sc_dbName;
    const QString c_dbPath;

    bool m_setupDone = false;

    MigrationManager m_migrationManager;
    QFuture<bool> m_migrationRunner;
    QFutureWatcher<bool> m_migrationProgress;

    void onMigrationFinished(bool result);
};

#endif // DATABASEMANAGER_H

#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QFutureWatcher>

namespace db {

class AbstractMigrationManager : public QObject
{
    Q_OBJECT
public:
    explicit AbstractMigrationManager(QObject* parent = nullptr);

    virtual void setupDatabase() = 0;

signals:
    void databaseReady();

    void databaseUpdateStarted() const;
    void databaseUpdateError() const;

protected:
    bool mSetupDone = false;

    QFuture<bool> mMigrationRunner;
    QFutureWatcher<bool> mMigrationProgress;

    void onMigrationFinished(bool result);
};

}
#endif // DATABASEMANAGER_H

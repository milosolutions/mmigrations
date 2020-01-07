#ifndef DBMIGRATIONMANAGER_H
#define DBMIGRATIONMANAGER_H

#include <QObject>
#include <QVersionNumber>
#include <functional>

namespace db {
class Migration;

class MigrationManager final : public QObject
{
    Q_OBJECT
public:
    MigrationManager(const QString &dbPath, QObject *parent = nullptr);

    bool checkAndCreate();

    bool needsUpdate();
    bool update();

private:
    const QString c_dbPath;
    const QString c_dbConnectionName;

    QVersionNumber m_dbVersion;

    bool dbExist() const;
    bool openDb();

    QVersionNumber getVersionNumber() const;
    bool updateDb();

    template<typename It>
    It findMigrationNumber(It begin, It end, const QVersionNumber &number);

    template<typename It>
    bool applyMigrations(It begin, It end, std::function<bool(const Migration &)> const &handler, bool forward);
};
}
#endif // DBMIGRATIONMANAGER_H

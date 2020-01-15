#include "dbmigration.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

#include "dbhelpers.h"

db::Migration::Migration(const QVersionNumber &number, 
                         const std::function<bool (QSqlDatabase &)> &forward,
                         const std::function<bool (QSqlDatabase &)> &backward)
    : mNumber(number), mForward(forward), mBackward(backward)
{}

bool db::Migration::RunForward(const Migration &m, QSqlDatabase &db)
{
    m.runCommon(db);
    auto result = m.mForward(db);

    if (result) {
        static const QLatin1String addMigrationEntryQuery("INSERT INTO Migrations"
                           "(`timestamp`, `version`) VALUES (:timestamp, :number)");
        auto query = QSqlQuery(db);
        query.prepare(addMigrationEntryQuery);
        query.bindValue(":timestamp", QDateTime::currentDateTime().toSecsSinceEpoch());
        query.bindValue(":number", m.mNumber.toString());

        Helpers::execQuery(query);
        result &= !Helpers::hasError(query);
    }

    return result;
}

bool db::Migration::RunBackward(const Migration &m, QSqlDatabase &db)
{
    m.runCommon(db);
    auto result = m.mBackward(db);

    if (result) {
        static const QLatin1String removeMigrationEntryQuery = 
            QLatin1String("DELETE FROM `Migrations` WHERE `version` IS :number");
        auto query = QSqlQuery(db);
        query.prepare(removeMigrationEntryQuery);
        query.bindValue(":number", m.mNumber.toString());

        db::Helpers::execQuery(query);
        result &= !db::Helpers::hasError(query);
    }

    return result;
}

QVersionNumber db::Migration::number() const
{
    return mNumber;
}

void db::Migration::runCommon(QSqlDatabase &db) const
{
    Q_ASSERT_X(db.isOpen(), Q_FUNC_INFO,
               "Database object should always be opened for migration!");
}

db::MigrationBuilder db::MigrationBuilder::builder()
{
    return db::MigrationBuilder();
}

db::Migration db::MigrationBuilder::build()
{
    Q_ASSERT_X(!mVersion.isNull(), Q_FUNC_INFO,
               "Version was not set!");
    Q_ASSERT_X(!mForward.empty(), Q_FUNC_INFO,
               "Forward queries collection is empty!");
    Q_ASSERT_X(!mBackward.empty(), Q_FUNC_INFO,
               "Backward queries collection is empty!");

    auto forward = [f = std::move(mForward)](const QSqlDatabase &db) {
                        return Helpers::runQueries(db, f);
                   };
    auto backward = [b = std::move(mBackward)](const QSqlDatabase &db) {
                        return Helpers::runQueries(db, b);
                   };
    return db::Migration(mVersion, forward, backward);
}

db::MigrationBuilder&& db::MigrationBuilder::setVersion(const QVersionNumber &version)
{
    mVersion = version;
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::setVersion(const QString &version)
{
    mVersion = QVersionNumber::fromString(version);
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::addForwardQuery(const QLatin1String &query)
{
    mForward.push_back(query);
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::addForwardQuery(const char* query)
{
    return addForwardQuery(QLatin1String(query));
}

db::MigrationBuilder &&db::MigrationBuilder::addBackwardQuery(const QLatin1String &query)
{
    mBackward.push_back(query);
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::addBackwardQuery(const char* query)
{
    return addBackwardQuery(QLatin1String(query));
}

db::MigrationBuilder &&db::MigrationBuilder::setForwardQueries(const db::Helpers::Queries &queries)
{
    Q_ASSERT_X(mForward.empty(), Q_FUNC_INFO, "mForward collection is not empty!");
    mForward = queries;
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::setBackwardQueries(const db::Helpers::Queries &queries)
{
    Q_ASSERT_X(mBackward.empty(), Q_FUNC_INFO, "mForward collection is not empty!");
    mBackward = queries;
    return std::move(*this);
}

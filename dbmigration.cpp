#include "dbmigration.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

#include "dbhelpers.h"

db::Migration::Migration(const QVersionNumber &number, const std::function<bool (QSqlDatabase &)> &forward, const std::function<bool (QSqlDatabase &)> &backward)
    : m_number(number), m_forward(forward), m_backward(backward)
{}

bool db::Migration::RunForward(const Migration &m, QSqlDatabase &db)
{
    m.runCommon(db);
    auto result = m.m_forward(db);

    if (result) {
        static const QLatin1String addMigrationEntryQuery = QLatin1String("INSERT INTO Migrations (`timestamp`, `version`) VALUES (:timestamp, :number)");
        auto query = QSqlQuery(db);
        query.prepare(addMigrationEntryQuery);
        query.bindValue(":timestamp", QDateTime::currentDateTime().toSecsSinceEpoch());
        query.bindValue(":number", m.m_number.toString());

        Helpers::execQuery(query);
        result &= !Helpers::hasError(query);
    }

    return result;
}

bool db::Migration::RunBackward(const Migration &m, QSqlDatabase &db)
{
    m.runCommon(db);
    auto result = m.m_backward(db);

    if (result) {
        static const QLatin1String removeMigrationEntryQuery = QLatin1String("DELETE FROM `Migrations` WHERE `version` IS :number");
        auto query = QSqlQuery(db);
        query.prepare(removeMigrationEntryQuery);
        query.bindValue(":number", m.m_number.toString());

        db::Helpers::execQuery(query);
        result &= !db::Helpers::hasError(query);
    }

    return result;
}

QVersionNumber db::Migration::number() const
{
    return m_number;
}

void db::Migration::runCommon(QSqlDatabase &db) const
{
    Q_ASSERT_X(db.isOpen(), __PRETTY_FUNCTION__,
               "Database object should always be opened for migration!");
}

db::MigrationBuilder db::MigrationBuilder::builder()
{
    return db::MigrationBuilder();
}

db::Migration db::MigrationBuilder::build()
{
    // TODO sanity checks
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

db::MigrationBuilder &&db::MigrationBuilder::addForwardQuery(const QString &query)
{
    return addForwardQuery(QLatin1String(query.toLatin1()));
}

db::MigrationBuilder &&db::MigrationBuilder::addBackwardQuery(const QLatin1String &query)
{
    mBackward.push_back(query);
    return std::move(*this);
}

db::MigrationBuilder &&db::MigrationBuilder::addBackwardQuery(const QString &query)
{
    return addBackwardQuery(QLatin1String(query.toLatin1()));
}

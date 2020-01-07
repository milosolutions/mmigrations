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

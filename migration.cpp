#include "migration.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>
#include <QDateTime>

#include "dbhelpers.h"

#include <QLoggingCategory>


Q_DECLARE_LOGGING_CATEGORY(migrations)

#include "migrationsdata.h"
QVector<mdatabase::Migration> mdatabase::MIGRATIONS::sMigrations;
QVersionNumber mdatabase::MIGRATIONS::sVersion;

mdatabase::Migration::Migration(const QVersionNumber &number, 
                         const std::function<bool (QSqlDatabase &)> &forward,
                         const std::function<bool (QSqlDatabase &)> &backward)
    : mNumber(number), mForward(forward), mBackward(backward)
{}

bool mdatabase::Migration::RunForward(const Migration &m, QSqlDatabase &db)
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

bool mdatabase::Migration::RunBackward(const Migration &m, QSqlDatabase &db)
{
    m.runCommon(db);
    auto result = m.mBackward(db);

    if (result) {
        static const QLatin1String removeMigrationEntryQuery = 
            QLatin1String("DELETE FROM `Migrations` WHERE `version` IS :number");
        auto query = QSqlQuery(db);
        query.prepare(removeMigrationEntryQuery);
        query.bindValue(":number", m.mNumber.toString());

        mdatabase::Helpers::execQuery(query);
        result &= !mdatabase::Helpers::hasError(query);
    }

    return result;
}

QVersionNumber mdatabase::Migration::number() const
{
    return mNumber;
}

void mdatabase::Migration::runCommon(QSqlDatabase &db) const
{
    Q_ASSERT_X(db.isOpen(), Q_FUNC_INFO,
               "Database object should always be opened for migration!");
}


/*!
 * \class MigrationBuilder
 * This class simplifies creation of migrations in list.
 * Is uses fluent builder patter (modified because there is no need
 * for inheritance (Migration is final)
 */

/**
 * This static method returns very first migration
 * which creates Migrations table with version and a timestamp
 */
mdatabase::Migration mdatabase::MigrationBuilder::migration001()
{
    return MigrationBuilder::builder()
            .setVersion("0.0.1")
            .addForwardQuery(
                "CREATE TABLE `Migrations` "
                "(`id` INTEGER primary key AUTOINCREMENT,"
                " `timestamp` INTEGER NOT NULL, `version` TEXT NOT NULL)")
            .addBackwardQuery("DROP TABLE `Migrations`")
            .build();
}

/**
 *  Creates a builder object to start building
 */
mdatabase::MigrationBuilder mdatabase::MigrationBuilder::builder()
{
    return mdatabase::MigrationBuilder();
}

/**
 *  Finalizes building and returns final Migration object
 */
mdatabase::Migration mdatabase::MigrationBuilder::build()
{
    Q_ASSERT_X(!mVersion.isNull(), Q_FUNC_INFO,
               "Version was not set!");
    Q_ASSERT_X(!mForward.empty(), Q_FUNC_INFO,
               "Forward queries collection is empty!");
    Q_ASSERT_X(!mBackward.empty(), Q_FUNC_INFO,
               "Backward queries collection is empty!");

    auto forward = [f = std::move(mForward)](const QSqlDatabase &mdatabase) {
                        return Helpers::runQueries(mdatabase, f);
                   };
    auto backward = [b = std::move(mBackward)](const QSqlDatabase &mdatabase) {
                        return Helpers::runQueries(mdatabase, b);
                   };
    return mdatabase::Migration(mVersion, forward, backward);
}

/**
 * Sets migration version (major, minor, micro numbers)
 */
mdatabase::MigrationBuilder&& mdatabase::MigrationBuilder::setVersion(const QVersionNumber &version)
{
    mVersion = version;
    return std::move(*this);
}

/**
 * Sets migration version. Version should have format acceptable by QVersionNumber,
 * like this : "1.1.0" (major, minor, micro numbers)
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::setVersion(const QString &version)
{
    mVersion = QVersionNumber::fromString(version);
    return std::move(*this);
}

/**
 * Adds query that is used in forward migration. Every invokation of this method
 * will add new query after last one.
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::addForwardQuery(const QLatin1String &query)
{
    mForward.push_back(query);
    return std::move(*this);
}

/**
 * Adds query that is used in forward migration. Every invokation of this method
 * will add new query after last one.
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::addForwardQuery(const char* query)
{
    return addForwardQuery(QLatin1String(query));
}

/**
 * Adds query that is used in backward migration. Every invokation of this method
 * will add new query after last one.
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::addBackwardQuery(const QLatin1String &query)
{
    mBackward.push_back(query);
    return std::move(*this);
}

/**
 * Adds query that is used in backward migration. Every invokation of this method
 * will add new query after last one.
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::addBackwardQuery(const char* query)
{
    return addBackwardQuery(QLatin1String(query));
}

/**
 * Sets all queries that are used in forward migration. It is assumed that no forward queries
 * were added before this method invokation
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::setForwardQueries(const mdatabase::Helpers::Queries &queries)
{
    Q_ASSERT_X(mForward.empty(), Q_FUNC_INFO, "mForward collection is not empty!");
    mForward = queries;
    return std::move(*this);
}

/**
 * Sets all queries that are used in backward migration. It is assumed that no backward queries
 * were added before this method invokation
 */
mdatabase::MigrationBuilder &&mdatabase::MigrationBuilder::setBackwardQueries(const mdatabase::Helpers::Queries &queries)
{
    Q_ASSERT_X(mBackward.empty(), Q_FUNC_INFO, "mForward collection is not empty!");
    mBackward = queries;
    return std::move(*this);
}

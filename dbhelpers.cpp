#include "dbhelpers.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

#include <QLoggingCategory>

Q_LOGGING_CATEGORY(migrations, "MMigrations")

bool mdatabase::Helpers::hasError(const QSqlQuery &query)
{
    return query.lastError().isValid();
}

bool mdatabase::Helpers::execQuery(QSqlQuery &query)
{
    query.exec();
    if (hasError(query)) {
        qCWarning(migrations) << "Cannot apply query: (" << query.lastQuery() 
                   << " ) error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool mdatabase::Helpers::execQuery(const QSqlDatabase &db, const QLatin1String &queryStr)
{
    auto query = QSqlQuery(db);
    query.prepare(queryStr);
    execQuery(query);
    return !hasError(query);
}

bool mdatabase::Helpers::runQueries(const QSqlDatabase &db,
                                    const QVector<QLatin1String> &queries)
{
    return std::all_of(queries.constBegin(), queries.constEnd(),
                       [&](const QLatin1String& query) {
                           return execQuery(db, query);
                        });
}

#include "dbhelpers.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

bool db::Helpers::hasError(const QSqlQuery &query)
{
    return query.lastError().isValid();
}

bool db::Helpers::execQuery(QSqlQuery &query)
{
    query.exec();
    if (hasError(query)) {
        qWarning() << "Cannot apply query: (" << query.lastQuery() << " ) error:" << query.lastError().text();
        return false;
    }

    return true;
}

bool db::Helpers::execQuery(const QSqlDatabase &db, const QLatin1String &queryStr)
{
    auto query = QSqlQuery(db);
    query.prepare(queryStr);
    execQuery(query);
    return !hasError(query);
}

bool db::Helpers::runQueries(const QSqlDatabase &db, const QVector<QLatin1String> &queries)
{
    return std::all_of(queries.constBegin(), queries.constEnd(),
                       [&](const QLatin1String& query) {
                           return execQuery(db, query);
                        });
}

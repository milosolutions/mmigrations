#include "dbhelpers.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>

bool db::Helpers::hasError(const QSqlQuery &query)
{
    return query.lastError().isValid();
}

void db::Helpers::execQuery(QSqlQuery &query)
{
    query.exec();
    if (hasError(query)) {
        qWarning() << "Cannot apply query: (" << query.lastQuery() << " ) error:" << query.lastError().text();
    }
}

bool db::Helpers::execQuery(const QSqlDatabase &db, const QLatin1String &queryStr)
{
    auto query = QSqlQuery(db);
    query.prepare(queryStr);
    execQuery(query);
    return !hasError(query);
}

bool db::Helpers::runQueries(const QSqlDatabase &db, const QList<QLatin1String> &queries)
{
    return std::all_of(queries.constBegin(), queries.constEnd(),
                       std::bind(qOverload<const QSqlDatabase &, const QLatin1String &>(&execQuery),
                                 db, std::placeholders::_1));
}

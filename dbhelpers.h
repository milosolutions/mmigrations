#ifndef DBHELPERS_H
#define DBHELPERS_H

#include <QString>

class QSqlDatabase;
class QSqlQuery;

namespace mdatabase {
class Helpers
{
public:
    using Queries = QVector<QLatin1String>;
    static bool hasError(const QSqlQuery &query);

    static bool execQuery(QSqlQuery &query);
    static bool execQuery(const QSqlDatabase &mdatabase, const QLatin1String &queryStr);

    static bool runQueries(const QSqlDatabase &mdatabase, const Queries &queries);
};

}

#endif // DBHELPERS_H

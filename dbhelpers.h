#ifndef DBHELPERS_H
#define DBHELPERS_H

#include <QString>

class QSqlDatabase;
class QSqlQuery;

namespace db {
class Helpers
{
public:
    static bool hasError(const QSqlQuery &query);

    static void execQuery(QSqlQuery &query);
    static bool execQuery(const QSqlDatabase &db, const QLatin1String &queryStr);

    static bool runQueries(const QSqlDatabase &db, const QList<QLatin1String> &queries);
};

}

#endif // DBHELPERS_H

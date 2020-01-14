#ifndef DBMIGRATION_H
#define DBMIGRATION_H

#include <QVersionNumber>
#include <functional>
#include <QVector>

#include "dbhelpers.h"

class QSqlDatabase;

namespace db {
class Migration final {
public:
    Migration(const QVersionNumber &m_number,
              const std::function<bool(QSqlDatabase &)> &m_forward,
              const std::function<bool(QSqlDatabase &)> &m_backward);

    static bool RunForward(const Migration &m, QSqlDatabase &db);
    static bool RunBackward(const Migration &m, QSqlDatabase &db);

    QVersionNumber number() const;

private:
    QVersionNumber m_number;
    std::function<bool(QSqlDatabase &)> m_forward;
    std::function<bool(QSqlDatabase &)> m_backward;

    void runCommon(QSqlDatabase &db) const;
};


class MigrationBuilder
{
public:
    static MigrationBuilder builder();
    Migration build();
    MigrationBuilder&& setVersion(const QVersionNumber& version);
    MigrationBuilder&& setVersion(const QString& version);
    MigrationBuilder&& addForwardQuery(const QLatin1String& query);
    MigrationBuilder&& addForwardQuery(const QString& query);
    MigrationBuilder&& addBackwardQuery(const QLatin1String& query);
    MigrationBuilder&& addBackwardQuery(const QString& query);

private:
    Helpers::Queries mForward;
    Helpers::Queries mBackward;
    QVersionNumber mVersion;
};

}
#endif // DBMIGRATION_H

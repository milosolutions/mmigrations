#ifndef DBMIGRATION_H
#define DBMIGRATION_H

#include <QVersionNumber>
#include <functional>
#include <QVector>

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
}
#endif // DBMIGRATION_H

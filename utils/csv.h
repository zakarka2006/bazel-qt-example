//
// Created by Pavel Kasila on 7.05.24.
//

#ifndef CREATIVE_CSV_H
#define CREATIVE_CSV_H

#include <QSqlQuery>
#include <QString>

namespace outfit::utils::csv {
QString EscapeCSV(QString unexc);

void SaveQuery(const QString& header, QSqlQuery& query);
}  // namespace outfit::utils::csv

#endif  // CREATIVE_CSV_H

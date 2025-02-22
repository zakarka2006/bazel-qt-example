//
// Created by Pavel Kasila on 7.05.24.
//

#include "csv.h"

#include <QFile>
#include <QFileDialog>
#include <QLatin1Char>
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QString>
#include <QStringLiteral>
#include <QTextStream>
#include <QtCore/qstringconverter_base.h>

QString outfit::utils::csv::EscapeCSV(QString unexc) {
    if (!unexc.contains(QLatin1Char(','))) {
        return unexc;
    }
    return '\"' + unexc.replace(QLatin1Char('\"'), QStringLiteral("\"\"")) + '\"';
}

void outfit::utils::csv::SaveQuery(const QString& header, QSqlQuery& query) {
    const QString file_name =
        QFileDialog::getSaveFileName(nullptr, "export.csv", ".", "CSV (*.csv)");
    if (file_name == "") {
        return;
    }
    QFile csv_file(file_name);
    if (!csv_file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox msg;
        msg.setText("failed to open file");
        msg.exec();
        return;
    }
    if (!query.exec()) {
        QMessageBox msg;
        msg.setText("failed to run query");
        msg.exec();
        return;
    }
    QTextStream out_stream(&csv_file);
    out_stream << header << "\n";
    out_stream.setEncoding(QStringConverter::Utf8);
    while (query.next()) {
        const QSqlRecord record = query.record();
        for (int i = 0, rec_count = record.count(); i < rec_count; ++i) {
            if (i > 0) {
                out_stream << ',';
            }
            out_stream << utils::csv::EscapeCSV(record.value(i).toString());
        }
        out_stream << '\n';
    }
}
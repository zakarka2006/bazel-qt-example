#ifndef TASK_TYPES_H
#define TASK_TYPES_H

#include <QString>
#include <QStringList>

struct TranslationTask {
    QString id;
    QString textToTranslate;
    QString correctAnswer;
    QString hint;

    TranslationTask(QString _id, QString _text, QString _answer, QString _hint = "")
        : id(_id), textToTranslate(_text), correctAnswer(_answer), hint(_hint) {}
};

struct GrammarTask {
    QString id;
    QString description;
    QStringList options;
    int correctAnswerIndex;
    QString hint;

    GrammarTask(QString _id, QString _desc, QStringList _opts, int _correctIdx, QString _hint = "")
        : id(_id), description(_desc), options(_opts), correctAnswerIndex(_correctIdx), hint(_hint) {}
};

#endif // TASK_TYPES_H
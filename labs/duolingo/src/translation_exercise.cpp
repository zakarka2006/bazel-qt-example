#include "translation_exercise.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QDebug>

TranslationExerciseWidget::TranslationExerciseWidget(QWidget *parent)
    : QWidget(parent), m_answerProcessed(false) {
    setupUi();
}

TranslationExerciseWidget::~TranslationExerciseWidget() {}

void TranslationExerciseWidget::setupUi() {
    mainLayout = new QVBoxLayout(this);

    taskLabel = new QLabel(tr("Translate the following:"), this);
    taskLabel->setAlignment(Qt::AlignCenter);
    QFont taskFont = taskLabel->font();
    taskFont.setPointSize(14);
    taskLabel->setFont(taskFont);

    answerLineEdit = new QLineEdit(this);
    answerLineEdit->setPlaceholderText(tr("Enter your translation here"));
    QFont answerFont = answerLineEdit->font();
    answerFont.setPointSize(12);
    answerLineEdit->setFont(answerFont);

    submitButton = new QPushButton(tr("Submit"), this);

    feedbackLabel = new QLabel(this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    QFont feedbackFont = feedbackLabel->font();
    feedbackFont.setPointSize(12);
    feedbackLabel->setFont(feedbackFont);

    mainLayout->addWidget(taskLabel);
    mainLayout->addWidget(answerLineEdit);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch(1);
    
    connect(submitButton, &QPushButton::clicked, this, &TranslationExerciseWidget::handleSubmit);
    taskLabel->setText(tr("Loading translation task..."));
}

void TranslationExerciseWidget::setTask(const QString& taskText, const QString& correctAnswer) {
    taskLabel->setText(tr("Translate: \"%1\"").arg(taskText));
    currentCorrectAnswer = correctAnswer;
    answerLineEdit->clear();
    feedbackLabel->clear();
    answerLineEdit->setFocus();

    m_answerProcessed = false;
    submitButton->setText(tr("Submit"));
    answerLineEdit->setReadOnly(false);
}

void TranslationExerciseWidget::handleSubmit() {
    if (m_answerProcessed) {
        emit nextQuestionRequested();
        return;
    }

    QString userAnswer = answerLineEdit->text().trimmed();
    if (userAnswer.isEmpty()) {
        feedbackLabel->setText(tr("<font color='orange'>Please enter an answer.</font>"));
        emit answerSubmitted(false, userAnswer);
        return;
    }

    bool isCorrect = (userAnswer.compare(currentCorrectAnswer, Qt::CaseInsensitive) == 0);

    if (isCorrect) {
        qDebug() << "Correct! User answer:" << userAnswer << "Expected:" << currentCorrectAnswer;
        feedbackLabel->setText(tr("<font color='green'>Correct!</font>"));
        submitButton->setText(tr("Next Question"));
        answerLineEdit->setReadOnly(true);
        m_answerProcessed = true;
    } else {
        qDebug() << "Incorrect. User answer:" << userAnswer << "Expected:" << currentCorrectAnswer;
        feedbackLabel->setText(tr("<font color='red'>Incorrect. Try again!</font>"));
    }
    emit answerSubmitted(isCorrect, userAnswer);
}

void TranslationExerciseWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        if (answerLineEdit && answerLineEdit->hasFocus()) {
            answerLineEdit->clearFocus();
            qDebug() << "TranslationExerciseWidget: Esc pressed, cleared focus from answerLineEdit.";
        } else {
            QWidget::keyPressEvent(event);
        }
    } else {
        QWidget::keyPressEvent(event);
    }
}
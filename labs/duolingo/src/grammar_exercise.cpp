#include "grammar_exercise.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QDebug>
#include <utility>

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent)
    : QWidget(parent), currentCorrectAnswerIndex(-1), m_answerProcessed(false) {
    setupUi();
}

GrammarExerciseWidget::~GrammarExerciseWidget() {
    clearOptions();
}

void GrammarExerciseWidget::setupUi() {
    mainLayout = new QVBoxLayout(this);

    taskDescriptionLabel = new QLabel(tr("Complete the sentence:"), this);
    QFont taskFont = taskDescriptionLabel->font();
    taskFont.setPointSize(14);
    taskDescriptionLabel->setFont(taskFont);
    taskDescriptionLabel->setWordWrap(true);

    optionsGroupBox = new QGroupBox(tr("Choose an option:"), this);
    optionsLayout = new QVBoxLayout(optionsGroupBox);

    optionsGroup = new QButtonGroup(this);

    submitButton = new QPushButton(tr("Submit"), this);

    feedbackLabel = new QLabel(this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    QFont feedbackFont = feedbackLabel->font();
    feedbackFont.setPointSize(12);
    feedbackLabel->setFont(feedbackFont);

    mainLayout->addWidget(taskDescriptionLabel);
    mainLayout->addWidget(optionsGroupBox);
    mainLayout->addWidget(submitButton);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addStretch(1);

    connect(submitButton, &QPushButton::clicked, this, &GrammarExerciseWidget::handleSubmit);
    taskDescriptionLabel->setText(tr("Loading grammar task..."));
}

void GrammarExerciseWidget::clearOptions() {
    for (QRadioButton *rb : currentRadioButtons) {
        optionsLayout->removeWidget(rb);
        optionsGroup->removeButton(rb);
        delete rb;
    }
    currentRadioButtons.clear();
}

void GrammarExerciseWidget::setTask(const QString& taskDescription, const QStringList& options, int correctAnswerIndex) {
    taskDescriptionLabel->setText(taskDescription);
    this->currentCorrectAnswerIndex = correctAnswerIndex;
    clearOptions();

    for (int i = 0; i < options.size(); ++i) {
        QRadioButton *radioButton = new QRadioButton(options.at(i), this);
        optionsLayout->addWidget(radioButton);
        optionsGroup->addButton(radioButton, i);
        currentRadioButtons.append(radioButton);
        radioButton->setEnabled(true);
    }

    feedbackLabel->clear();
    m_answerProcessed = false;
    submitButton->setText(tr("Submit"));
}

void GrammarExerciseWidget::handleSubmit() {
    if (m_answerProcessed) {
        emit nextQuestionRequested();
        return;
    }

    int checkedId = optionsGroup->checkedId();
    if (checkedId == -1) {
        feedbackLabel->setText(tr("<font color='orange'>Please select an option.</font>"));
        emit answerSubmitted(false, checkedId);
        return;
    }

    bool isCorrect = (checkedId == currentCorrectAnswerIndex);

    if (isCorrect) {
        feedbackLabel->setText(tr("<font color='green'>Correct!</font>"));
        submitButton->setText(tr("Next Question"));
        for (QRadioButton *rb : std::as_const(currentRadioButtons)) {
            rb->setEnabled(false);
        }
        m_answerProcessed = true;
    } else {
        feedbackLabel->setText(tr("<font color='red'>Incorrect. Try again!</font>"));
    }
    emit answerSubmitted(isCorrect, checkedId);
}
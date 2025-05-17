#ifndef GRAMMAR_EXERCISE_H
#define GRAMMAR_EXERCISE_H

#include <QWidget>
#include <QStringList>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QRadioButton;
class QButtonGroup;
class QGroupBox;

class GrammarExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarExerciseWidget(QWidget *parent = nullptr);
    ~GrammarExerciseWidget();

    void setTask(const QString& taskDescription, const QStringList& options, int correctAnswerIndex);

signals:
    void answerSubmitted(bool isCorrect, int chosenAnswerIndex);
    void nextQuestionRequested();

private slots:
    void handleSubmit();

private:
    void setupUi();
    void clearOptions();

    QLabel *taskDescriptionLabel;
    QGroupBox *optionsGroupBox;
    QVBoxLayout *optionsLayout;
    QButtonGroup *optionsGroup;
    QPushButton *submitButton;
    QLabel *feedbackLabel;
    QVBoxLayout *mainLayout;

    int currentCorrectAnswerIndex;
    QList<QRadioButton*> currentRadioButtons;
    bool m_answerProcessed;
};

#endif // GRAMMAR_EXERCISE_H
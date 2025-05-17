#ifndef TRANSLATION_EXERCISE_H
#define TRANSLATION_EXERCISE_H

#include <QWidget>

// Forward declarations
class QLabel;
class QLineEdit;
class QPushButton;
class QVBoxLayout;

class TranslationExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationExerciseWidget(QWidget *parent = nullptr);
    ~TranslationExerciseWidget();

    void setTask(const QString& taskText, const QString& correctAnswer);

signals:
    void answerSubmitted(bool isCorrect, const QString& userAnswer);
    void nextQuestionRequested();

private slots:
    void handleSubmit();
protected:
    void keyPressEvent(QKeyEvent *event) override;
private:
    void setupUi();

    QLabel *taskLabel;          
    QLineEdit *answerLineEdit;  
    QPushButton *submitButton;  
    QLabel *feedbackLabel;      

    QVBoxLayout *mainLayout;

    QString currentCorrectAnswer;
    bool m_answerProcessed;
};

#endif // TRANSLATION_EXERCISE_H
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include "task_types.h"
#include "difficulty_dialog.h"

class QStackedWidget;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QMenuBar;
class QMenu;
class QAction;
class QLabel;
class QSoundEffect;
class QProgressBar;
class QTimer;

class GrammarExerciseWidget;
class TranslationExerciseWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void showWelcomePage();
    void showTranslationExercisePage();
    void showGrammarExercisePage();
    void handleChangeDifficulty();

    void handleTranslationAnswer(bool isCorrect, const QString& userAnswer);
    void handleGrammarAnswer(bool isCorrect, int chosenAnswerIndex);
    void proceedToNextTranslationTask();
    void proceedToNextGrammarTask();
    void handleSessionTimeout();

private:
    void setupUI();
    void createMenus();
    void createNavigationBar();
    void createExerciseArea();
    void setupLayouts();
    void connectSignals();

    void startTranslationSession();
    void loadNextTranslationTask();
    void startGrammarSession();
    void loadNextGrammarTask();
    void applyDifficultySettings();

    QMenuBar *appMenuBar;
    QMenu *settingsMenu;
    QAction *changeDifficultyAction;

    QWidget *centralAreaWidget;
    QVBoxLayout *mainVerticalLayout;
    QWidget *navigationBarWidget;
    QHBoxLayout *navigationBarLayout;
    QPushButton *homeButton;
    QPushButton *translationButton;
    QPushButton *grammarButton;
    QStackedWidget *exercisePagesWidget;
    QWidget *welcomePageWidget;
    QLabel *welcomeImageLabel;
    QLabel *welcomeTextLabel;
    QProgressBar *taskProgressBar;
    QLabel *scoreLabel;

    TranslationExerciseWidget *translationExercise;
    GrammarExerciseWidget *grammarExercise;

    QSoundEffect *correctSound;
    QSoundEffect *wrongSound;

    QTimer *exerciseSessionTimer;

    std::vector<TranslationTask> allTranslationTasks;
    std::vector<GrammarTask> allGrammarTasks;
    std::vector<TranslationTask> currentSessionTranslationTasks;
    std::vector<GrammarTask> currentSessionGrammarTasks;
    int currentTranslationTaskIndex;
    int currentGrammarTaskIndex;

    DifficultyLevel currentDifficulty;
    int totalScore;
    int incorrectAttemptsInSession;

    int m_tasksPerSession;
    int m_sessionDurationSeconds;
    int m_maxIncorrectAttempts;
    int m_pointsPerSession;
};

#endif // MAINWINDOW_H
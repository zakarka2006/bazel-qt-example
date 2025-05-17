#include "mainwindow.h"
#include "translation_exercise.h"
#include "grammar_exercise.h"
#include "difficulty_dialog.h" 

#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QDir>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QStackedWidget>
#include <QLabel>
#include <QMessageBox>
#include <QPixmap>
#include <QSoundEffect>
#include <QProgressBar>
#include <QUrl>
#include <QTimer>
#include <QKeyEvent>
#include <QVariant>     
#include <random>       
#include <algorithm>    

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    currentTranslationTaskIndex(-1),
    currentGrammarTaskIndex(-1),
    totalScore(0),
    incorrectAttemptsInSession(0),
    currentDifficulty(DifficultyLevel::Medium) 
{
    qRegisterMetaType<DifficultyLevel>("DifficultyLevel");

    
    allTranslationTasks.emplace_back("t1", "Hello", "Привет", "Common greeting.");
    allTranslationTasks.emplace_back("t2", "Goodbye", "Пока", "Said when leaving.");
    allTranslationTasks.emplace_back("t3", "Thank you", "Спасибо", "Expressing gratitude.");
    allTranslationTasks.emplace_back("t4", "Cat", "Кошка", "A small domesticated carnivorous mammal.");
    allTranslationTasks.emplace_back("t5", "Dog", "Собака", "A domesticated canid, Canis familiaris.");
    allTranslationTasks.emplace_back("t6", "Water", "Вода", "H₂O, a transparent fluid.");
    allTranslationTasks.emplace_back("t7", "Book", "Книга", "A set of written or printed sheets of paper.");
    allTranslationTasks.emplace_back("t8", "Friend", "Друг", "A person who gives assistance and support.");
    allTranslationTasks.emplace_back("t9", "School", "Школа", "An institution for educating children.");
    allTranslationTasks.emplace_back("t10", "Computer", "Компьютер", "An electronic device for processing data.");
    allTranslationTasks.emplace_back("t11", "Red", "Красный", "Color at the end of the spectrum next to orange.");
    allTranslationTasks.emplace_back("t12", "To eat", "Кушать", "To take food into the mouth and swallow it."); 
    allTranslationTasks.emplace_back("t13", "To drink", "Пить", "To take liquid into the mouth and swallow it.");
    allTranslationTasks.emplace_back("t14", "To sleep", "Спать", "To rest your mind and body by closing your eyes.");
    allTranslationTasks.emplace_back("t15", "Today", "Сегодня", "This present day.");
    allTranslationTasks.emplace_back("t16", "Apple", "Яблоко", "A round fruit with firm, white flesh.");
    allTranslationTasks.emplace_back("t17", "House", "Дом", "A building for human habitation.");
    allTranslationTasks.emplace_back("t18", "Car", "Машина", "A road vehicle, typically with four wheels.");
    allTranslationTasks.emplace_back("t19", "Sun", "Солнце", "The star around which the earth orbits.");
    allTranslationTasks.emplace_back("t20", "Moon", "Луна", "The natural satellite of the earth.");


    
    allGrammarTasks.emplace_back("g1", "I ___ a doctor.", QStringList{"is", "am", "are"}, 1, "Verb 'to be', 1st person singular.");
    allGrammarTasks.emplace_back("g2", "She ___ apples.", QStringList{"like", "likes", "liking"}, 1, "Present Simple, 3rd person singular.");
    allGrammarTasks.emplace_back("g3", "They ___ playing now.", QStringList{"is", "are", "was"}, 1, "Present Continuous, plural.");
    allGrammarTasks.emplace_back("g4", "He ___ to the cinema yesterday.", QStringList{"go", "goes", "went"}, 2, "Past Simple, irregular verb.");
    allGrammarTasks.emplace_back("g5", "This is ___ interesting book.", QStringList{"a", "an", "the"}, 1, "Article before a vowel sound.");
    allGrammarTasks.emplace_back("g6", "My keys are ___ the table.", QStringList{"on", "in", "at"}, 0, "Preposition of place: surface.");
    allGrammarTasks.emplace_back("g7", "What ___ you do last night?", QStringList{"did", "do", "does"}, 0, "Past Simple auxiliary verb for questions.");
    allGrammarTasks.emplace_back("g8", "Listen! The baby ___.", QStringList{"cry", "cries", "is crying"}, 2, "Present Continuous for action happening now.");
    allGrammarTasks.emplace_back("g9", "There isn't ___ milk in the fridge.", QStringList{"some", "any", "no"}, 1, "'Any' in negative sentences.");
    allGrammarTasks.emplace_back("g10", "I have ___ visited Paris.", QStringList{"ever", "never", "already"}, 1, "Present Perfect with 'never'.");
    allGrammarTasks.emplace_back("g11", "This car is ___ than that one.", QStringList{"more fast", "faster", "fastest"}, 1, "Comparative adjective.");
    allGrammarTasks.emplace_back("g12", "If I ___ you, I would study more.", QStringList{"am", "was", "were"}, 2, "Second conditional, subjunctive mood.");
    allGrammarTasks.emplace_back("g13", "She has been working here ___ 2010.", QStringList{"since", "for", "ago"}, 0, "Present Perfect Continuous with 'since'.");
    allGrammarTasks.emplace_back("g14", "Can you tell me where ___?", QStringList{"is the bank", "the bank is", "does the bank is"}, 1, "Indirect question word order.");
    allGrammarTasks.emplace_back("g15", "___ you like some tea?", QStringList{"Do", "Would", "Are"}, 1, "Polite offer.");
    allGrammarTasks.emplace_back("g16", "He is afraid ___ spiders.", QStringList{"of", "from", "about"}, 0, "Preposition 'of' with 'afraid'.");
    allGrammarTasks.emplace_back("g17", "I ___ to London next week.", QStringList{"go", "am going", "will go"}, 1, "Future plan or arrangement (Present Continuous)."); 
    allGrammarTasks.emplace_back("g18", "She ___ a beautiful song when I entered.", QStringList{"sings", "sang", "was singing"}, 2, "Past Continuous for an ongoing action interrupted.");
    allGrammarTasks.emplace_back("g19", "You ___ smoke here. It's forbidden.", QStringList{"mustn't", "don't have to", "shouldn't"}, 0, "Prohibition.");
    allGrammarTasks.emplace_back("g20", "The book ___ by Mark Twain.", QStringList{"written", "was written", "wrote"}, 1, "Passive voice (Past Simple).");


    exerciseSessionTimer = new QTimer(this);
    exerciseSessionTimer->setSingleShot(true);
    connect(exerciseSessionTimer, &QTimer::timeout, this, &MainWindow::handleSessionTimeout);

    applyDifficultySettings(); 

    setupUI();
    setWindowTitle(tr("Duolingo Clone"));
    resize(800, 600);

    correctSound = new QSoundEffect(this);
    wrongSound = new QSoundEffect(this);
    const QString relativePathPrefix = "labs/duolingo/resources/";
    QString runfilesPath = QDir::currentPath();
    QString correctWavPath = runfilesPath + "/" + relativePathPrefix + "correct.wav";
    QString wrongWavPath = runfilesPath + "/" + relativePathPrefix + "wrong.wav";
    correctSound->setSource(QUrl::fromLocalFile(correctWavPath));
    correctSound->setVolume(0.8f);
    wrongSound->setSource(QUrl::fromLocalFile(wrongWavPath));
    wrongSound->setVolume(0.8f);
}

MainWindow::~MainWindow() {}

void MainWindow::applyDifficultySettings() {
    switch (currentDifficulty) {
        case DifficultyLevel::Easy:
            m_tasksPerSession = 3;
            m_sessionDurationSeconds = 120;
            m_maxIncorrectAttempts = 5;
            m_pointsPerSession = 5;
            break;
        case DifficultyLevel::Medium:
        default:
            m_tasksPerSession = 5;
            m_sessionDurationSeconds = 90;
            m_maxIncorrectAttempts = 3;
            m_pointsPerSession = 10;
            break;
        case DifficultyLevel::Hard:
            m_tasksPerSession = 7;
            m_sessionDurationSeconds = 60;
            m_maxIncorrectAttempts = 2;
            m_pointsPerSession = 15;
            break;
    }
}

void MainWindow::setupUI() {
    createMenus();
    createNavigationBar();

    taskProgressBar = new QProgressBar(this);
    taskProgressBar->setValue(0);
    taskProgressBar->setTextVisible(true);
    taskProgressBar->setFormat(tr("%v / %m"));
    taskProgressBar->setVisible(false);

    createExerciseArea();
    setupLayouts();
    connectSignals();
    setCentralWidget(centralAreaWidget);
    showWelcomePage();
}

void MainWindow::createMenus() {
    appMenuBar = menuBar();
    settingsMenu = appMenuBar->addMenu(tr("&Settings"));
    changeDifficultyAction = new QAction(tr("&Change Difficulty..."), this);
    settingsMenu->addAction(changeDifficultyAction);
}

void MainWindow::createNavigationBar() {
    navigationBarWidget = new QWidget(this);
    navigationBarLayout = new QHBoxLayout(navigationBarWidget);

    homeButton = new QPushButton(tr("Home"), this);
    translationButton = new QPushButton(tr("Translation"), this);
    grammarButton = new QPushButton(tr("Grammar"), this);

    homeButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    translationButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    grammarButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    navigationBarLayout->addWidget(homeButton);
    navigationBarLayout->addWidget(translationButton);
    navigationBarLayout->addWidget(grammarButton);
    navigationBarLayout->addStretch(1);

    scoreLabel = new QLabel(this);
    scoreLabel->setText(tr("Score: %1").arg(totalScore));
    QFont scoreFont = scoreLabel->font();
    scoreFont.setPointSize(12);
    scoreLabel->setFont(scoreFont);
    navigationBarLayout->addWidget(scoreLabel);

    navigationBarLayout->setContentsMargins(5, 5, 5, 5);
    navigationBarLayout->setSpacing(10);
}

void MainWindow::createExerciseArea() {
    exercisePagesWidget = new QStackedWidget(this);
    welcomePageWidget = new QWidget(this);
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomePageWidget);

    welcomeTextLabel = new QLabel(tr("Welcome to Duolingo Clone!\nSelect an exercise to begin."), this);
    welcomeTextLabel->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    QFont font = welcomeTextLabel->font();
    font.setPointSize(16);
    welcomeTextLabel->setFont(font);
    welcomeTextLabel->setWordWrap(true);
    welcomeLayout->addWidget(welcomeTextLabel);

    welcomeImageLabel = new QLabel(this);
    QPixmap welcomePixmap("labs/duolingo/resources/home_pic.png");
    if (welcomePixmap.isNull()) {
        welcomeImageLabel->setText("Image not found (labs/duolingo/resources/home_pic.png)");
    } else {
        welcomeImageLabel->setPixmap(welcomePixmap.scaled(400, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    welcomeImageLabel->setAlignment(Qt::AlignCenter);
    welcomeLayout->addWidget(welcomeImageLabel, 1);

    translationExercise = new TranslationExerciseWidget(this);
    grammarExercise = new GrammarExerciseWidget(this);

    exercisePagesWidget->addWidget(welcomePageWidget);
    exercisePagesWidget->addWidget(translationExercise);
    exercisePagesWidget->addWidget(grammarExercise);
}

void MainWindow::setupLayouts() {
    centralAreaWidget = new QWidget(this);
    mainVerticalLayout = new QVBoxLayout(centralAreaWidget);

    mainVerticalLayout->addWidget(navigationBarWidget);
    mainVerticalLayout->addWidget(taskProgressBar);
    mainVerticalLayout->addWidget(exercisePagesWidget, 1);

}

void MainWindow::connectSignals() {
    connect(homeButton, &QPushButton::clicked, this, &MainWindow::showWelcomePage);
    connect(translationButton, &QPushButton::clicked, this, &MainWindow::showTranslationExercisePage);
    connect(grammarButton, &QPushButton::clicked, this, &MainWindow::showGrammarExercisePage);
    connect(changeDifficultyAction, &QAction::triggered, this, &MainWindow::handleChangeDifficulty); 

    if (translationExercise) {
        connect(translationExercise, &TranslationExerciseWidget::answerSubmitted,
                this, &MainWindow::handleTranslationAnswer);
        connect(translationExercise, &TranslationExerciseWidget::nextQuestionRequested,
                this, &MainWindow::proceedToNextTranslationTask);
    }
    if (grammarExercise) {
        connect(grammarExercise, &GrammarExerciseWidget::answerSubmitted,
                this, &MainWindow::handleGrammarAnswer);
        connect(grammarExercise, &GrammarExerciseWidget::nextQuestionRequested,
                this, &MainWindow::proceedToNextGrammarTask);
    }
}

void MainWindow::showWelcomePage() {
    exercisePagesWidget->setCurrentWidget(welcomePageWidget);
    if(taskProgressBar) {
        taskProgressBar->setVisible(false);
    }
    if(exerciseSessionTimer && exerciseSessionTimer->isActive()) {
        exerciseSessionTimer->stop();
    }
}

void MainWindow::showTranslationExercisePage() {
    exercisePagesWidget->setCurrentWidget(translationExercise);
    startTranslationSession();
}

void MainWindow::showGrammarExercisePage() {
    exercisePagesWidget->setCurrentWidget(grammarExercise);
    startGrammarSession();
}

void MainWindow::handleChangeDifficulty() {
    DifficultyDialog dialog(currentDifficulty, this); 
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.getSelectedDifficulty();
        applyDifficultySettings();
        QMessageBox::information(this, tr("Difficulty Changed"),
                                 tr("Difficulty level has been updated. Applied on next session."));
    }
}

void MainWindow::startTranslationSession() {
    incorrectAttemptsInSession = 0;
    currentSessionTranslationTasks.clear();

    std::vector<TranslationTask> availableTasks = allTranslationTasks; 
    if (availableTasks.empty()) {
        QMessageBox::warning(this, tr("No Tasks"), tr("No translation tasks available."));
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
        return;
    }
    
    std::shuffle(availableTasks.begin(), availableTasks.end(), std::mt19937(std::random_device()()));
    int count = std::min(m_tasksPerSession, (int)availableTasks.size());
    for (int i = 0; i < count; ++i) {
        currentSessionTranslationTasks.push_back(availableTasks[i]);
    }

    currentTranslationTaskIndex = 0;
    if (!currentSessionTranslationTasks.empty()) {
        if(taskProgressBar) {
            taskProgressBar->setRange(0, currentSessionTranslationTasks.size());
            taskProgressBar->setValue(0);
            taskProgressBar->setVisible(true);
        }
        loadNextTranslationTask();
        exerciseSessionTimer->start(m_sessionDurationSeconds * 1000);
    } else {
        QMessageBox::information(this, tr("Session Info"), tr("No tasks for this session."));
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
    }
}

void MainWindow::startGrammarSession() {
    incorrectAttemptsInSession = 0;
    currentSessionGrammarTasks.clear();

    std::vector<GrammarTask> availableTasks = allGrammarTasks;
    if (availableTasks.empty()) {
        QMessageBox::warning(this, tr("No Tasks"), tr("No grammar tasks available."));
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
        return;
    }
    std::shuffle(availableTasks.begin(), availableTasks.end(), std::mt19937(std::random_device()()));
    int count = std::min(m_tasksPerSession, (int)availableTasks.size());
    for (int i = 0; i < count; ++i) {
        currentSessionGrammarTasks.push_back(availableTasks[i]);
    }

    currentGrammarTaskIndex = 0;
    if (!currentSessionGrammarTasks.empty()) {
        if(taskProgressBar) {
            taskProgressBar->setRange(0, currentSessionGrammarTasks.size());
            taskProgressBar->setValue(0);
            taskProgressBar->setVisible(true);
        }
        loadNextGrammarTask();
        exerciseSessionTimer->start(m_sessionDurationSeconds * 1000);
    } else {
        QMessageBox::information(this, tr("Session Info"), tr("No tasks for this session."));
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
    }
}

void MainWindow::loadNextTranslationTask() {
    if (currentTranslationTaskIndex >= 0 && currentTranslationTaskIndex < static_cast<int>(currentSessionTranslationTasks.size())) {
        if(taskProgressBar) taskProgressBar->setValue(currentTranslationTaskIndex);
        const auto& task = currentSessionTranslationTasks[currentTranslationTaskIndex];
        translationExercise->setTask(task.textToTranslate, task.correctAnswer);
    } else { 
        if (exerciseSessionTimer->isActive()) exerciseSessionTimer->stop();
        if(taskProgressBar && !currentSessionTranslationTasks.empty()) {
             taskProgressBar->setValue(taskProgressBar->maximum());
        }

        if (incorrectAttemptsInSession < m_maxIncorrectAttempts) {
            totalScore += m_pointsPerSession;
            scoreLabel->setText(tr("Score: %1").arg(totalScore));
            QMessageBox::information(this, tr("Session Finished"),
                                     tr("Translation exercise completed successfully! You earned %1 points.")
                                     .arg(m_pointsPerSession));
        } else {
            
             QMessageBox::information(this, tr("Session Finished"),
                                     tr("Translation exercise completed, but with too many mistakes. No points awarded."));
        }
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
    }
}

void MainWindow::loadNextGrammarTask() {
    if (currentGrammarTaskIndex >= 0 && currentGrammarTaskIndex < static_cast<int>(currentSessionGrammarTasks.size())) {
        if(taskProgressBar) taskProgressBar->setValue(currentGrammarTaskIndex);
        const auto& task = currentSessionGrammarTasks[currentGrammarTaskIndex];
        grammarExercise->setTask(task.description, task.options, task.correctAnswerIndex);
    } else { 
        if (exerciseSessionTimer->isActive()) exerciseSessionTimer->stop();
        if(taskProgressBar && !currentSessionGrammarTasks.empty()) {
            taskProgressBar->setValue(taskProgressBar->maximum());
        }
        if (incorrectAttemptsInSession < m_maxIncorrectAttempts) {
            totalScore += m_pointsPerSession;
            scoreLabel->setText(tr("Score: %1").arg(totalScore));
            QMessageBox::information(this, tr("Session Finished"),
                                     tr("Grammar exercise completed successfully! You earned %1 points.")
                                     .arg(m_pointsPerSession));
        } else {
             QMessageBox::information(this, tr("Session Finished"),
                                     tr("Grammar exercise completed, but with too many mistakes. No points awarded."));
        }
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
    }
}

void MainWindow::handleTranslationAnswer(bool isCorrect, const QString& userAnswer) {
    if (isCorrect) {
        if (correctSound->isLoaded()) correctSound->play();
    } else {
        if (wrongSound->isLoaded()) wrongSound->play();
        incorrectAttemptsInSession++;
        if (incorrectAttemptsInSession >= m_maxIncorrectAttempts) {
            if (exerciseSessionTimer->isActive()) exerciseSessionTimer->stop();
            QMessageBox::warning(this, tr("Too Many Mistakes"),
                                 tr("You've made too many mistakes. Exercise failed."));
            if(taskProgressBar) taskProgressBar->setVisible(false);
            showWelcomePage();
            return;
        }
    }
}

void MainWindow::handleGrammarAnswer(bool isCorrect, int chosenAnswerIndex) {
    if (isCorrect) {
        if (correctSound->isLoaded()) correctSound->play();
    } else {
        if (wrongSound->isLoaded()) wrongSound->play();
        incorrectAttemptsInSession++;
        if (incorrectAttemptsInSession >= m_maxIncorrectAttempts) {
            if (exerciseSessionTimer->isActive()) exerciseSessionTimer->stop();
            QMessageBox::warning(this, tr("Too Many Mistakes"),
                                 tr("You've made too many mistakes. Exercise failed."));
            if(taskProgressBar) taskProgressBar->setVisible(false);
            showWelcomePage();
            return;
        }
    }
}

void MainWindow::proceedToNextTranslationTask() {
    if (currentTranslationTaskIndex < static_cast<int>(currentSessionTranslationTasks.size()) - 1 ) {
        currentTranslationTaskIndex++;
        loadNextTranslationTask();
    } else if (currentTranslationTaskIndex == static_cast<int>(currentSessionTranslationTasks.size()) - 1) {
        currentTranslationTaskIndex++;
        loadNextTranslationTask();
    }
}

void MainWindow::proceedToNextGrammarTask() {
    if (currentGrammarTaskIndex < static_cast<int>(currentSessionGrammarTasks.size()) - 1) {
        currentGrammarTaskIndex++;
        loadNextGrammarTask();
    } else if (currentGrammarTaskIndex == static_cast<int>(currentSessionGrammarTasks.size()) - 1) {
        currentGrammarTaskIndex++;
        loadNextGrammarTask();
    }
}

void MainWindow::handleSessionTimeout() {
    if (exercisePagesWidget->currentWidget() == translationExercise ||
        exercisePagesWidget->currentWidget() == grammarExercise) {
        QMessageBox::information(this, tr("Time's Up!"),
                                 tr("The time allotted for this exercise has expired."));
        if(taskProgressBar) taskProgressBar->setVisible(false);
        showWelcomePage();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        QString hintText = "No hint available for the current view.";
        QWidget *currentPage = exercisePagesWidget->currentWidget();

        if (currentPage == translationExercise) {
            if (currentTranslationTaskIndex >= 0 && currentTranslationTaskIndex < static_cast<int>(currentSessionTranslationTasks.size())) {
                hintText = currentSessionTranslationTasks[currentTranslationTaskIndex].hint;
                if (hintText.isEmpty()) {
                    hintText = "No specific hint for this translation task.";
                }
            } else {
                hintText = "Translation exercise is not active or no task loaded.";
            }
            QMessageBox::information(this, tr("Translation Hint"), hintText);
        } else if (currentPage == grammarExercise) {
            if (currentGrammarTaskIndex >= 0 && currentGrammarTaskIndex < static_cast<int>(currentSessionGrammarTasks.size())) {
                hintText = currentSessionGrammarTasks[currentGrammarTaskIndex].hint;
                if (hintText.isEmpty()) {
                    hintText = "No specific hint for this grammar task.";
                }
            } else {
                hintText = "Grammar exercise is not active or no task loaded.";
            }
            QMessageBox::information(this, tr("Grammar Hint"), hintText);
        } else if (currentPage == welcomePageWidget) {
            hintText = tr("Select an exercise from the top menu to begin learning!");
            QMessageBox::information(this, tr("Help"), hintText);
        } else {
            QMainWindow::keyPressEvent(event);
            return;
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}
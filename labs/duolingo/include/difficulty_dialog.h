#ifndef DIFFICULTY_DIALOG_H
#define DIFFICULTY_DIALOG_H

#include <QDialog>

class QComboBox;
class QDialogButtonBox;
class QVBoxLayout;
class QLabel;

enum class DifficultyLevel {
    Easy,
    Medium,
    Hard
};

class DifficultyDialog : public QDialog {
    Q_OBJECT

public:
    explicit DifficultyDialog(DifficultyLevel currentLevel, QWidget *parent = nullptr);
    ~DifficultyDialog();

    DifficultyLevel getSelectedDifficulty() const;

private:
    void setupUi();

    QComboBox *difficultyComboBox;
    QDialogButtonBox *buttonBox;
    QVBoxLayout *mainLayout;

    DifficultyLevel initialDifficulty;
};

#endif // DIFFICULTY_DIALOG_H
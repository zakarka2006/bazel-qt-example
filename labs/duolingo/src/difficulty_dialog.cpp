#include "difficulty_dialog.h"

#include <QLabel>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QVBoxLayout>
#include <QVariant>

DifficultyDialog::DifficultyDialog(DifficultyLevel currentLevel, QWidget *parent)
    : QDialog(parent), initialDifficulty(currentLevel) {
    setupUi();
    setWindowTitle(tr("Change Difficulty"));
}

DifficultyDialog::~DifficultyDialog() {
}

void DifficultyDialog::setupUi() {
    mainLayout = new QVBoxLayout(this);

    QLabel *infoLabel = new QLabel(tr("Select the desired difficulty level:"), this);
    mainLayout->addWidget(infoLabel);

    difficultyComboBox = new QComboBox(this);
    difficultyComboBox->addItem(tr("Easy"), QVariant::fromValue(DifficultyLevel::Easy));
    difficultyComboBox->addItem(tr("Medium"), QVariant::fromValue(DifficultyLevel::Medium));
    difficultyComboBox->addItem(tr("Hard"), QVariant::fromValue(DifficultyLevel::Hard));

    int currentIndex = difficultyComboBox->findData(QVariant::fromValue(initialDifficulty));
    if (currentIndex != -1) {
        difficultyComboBox->setCurrentIndex(currentIndex);
    } else {
        currentIndex = difficultyComboBox->findData(QVariant::fromValue(DifficultyLevel::Medium));
        if (currentIndex != -1) difficultyComboBox->setCurrentIndex(currentIndex);
    }

    mainLayout->addWidget(difficultyComboBox);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DifficultyDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DifficultyDialog::reject);
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);
    setMinimumWidth(300);
}

DifficultyLevel DifficultyDialog::getSelectedDifficulty() const {
    if (difficultyComboBox->currentData().canConvert<DifficultyLevel>()) {
        return difficultyComboBox->currentData().value<DifficultyLevel>();
    }
    return initialDifficulty;
}
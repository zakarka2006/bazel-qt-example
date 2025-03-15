#include "MainWindow.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QTimer>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QFile>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    resize(400, 400);

    QWidget *centralWidget = new QWidget(this);
    auto *mainLayout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    QList<QWidget*> groups;
    groups.append(createSliderExample());
    groups.append(createComboboxExample());
    groups.append(createClickerExample());
    groups.append(createRollExample());
    groups.append(createRandomExample());

    for (auto group : groups) {
        group->setVisible(false);
        mainLayout->addWidget(group, 0, Qt::AlignTop);
    }

    QMenu *menu = menuBar()->addMenu("Примеры");
    menu->addAction("Слайдер", [this]() { switchGroup(0); });
    menu->addAction("Комбобоксы", [this]() { switchGroup(1); });
    menu->addAction("Счётчик", [this]() { switchGroup(2); });
    menu->addAction("Барабан", [this]() { switchGroup(3); });
    menu->addAction("Рандом", [this]() { switchGroup(4); });

    groups[0]->setVisible(true);
    centralWidget->setLayout(mainLayout);
}

void MainWindow::switchGroup(int index) {
    for (int i = 0; i < centralWidget()->layout()->count(); i++)
        centralWidget()->layout()->itemAt(i)->widget()->setVisible(i == index);
}

void MainWindow::savePhoneToFile(const QString &phone) {
    QWidget* parent = this;
    QString fileName = QFileDialog::getSaveFileName(
        parent,
        "Добавить номер телефона в файл",
        "phone.txt",
        "Текстовые файлы (*.txt);;Все файлы (*)"
    );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&file);
            out << phone << "\n";
            file.close();

            QMessageBox::information(parent, "Успех", "Номер успешно сохранён.");
        } else {
            QMessageBox::warning(parent, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

QWidget* MainWindow::createSliderExample() {
    QGroupBox *group = new QGroupBox("");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QVBoxLayout *layout = new QVBoxLayout(group);
    QSlider *slider = new QSlider(Qt::Horizontal, group);
    slider->setMinimum(1);
    slider->setMaximum(999999999);
    QLabel *label = new QLabel(group);
    auto updateLabel = [label, slider](int value) {
        QString formatted = QString("%1").arg(value, 9, 10, QChar('0'));
        label->setText("+375" + formatted);
    };
    connect(slider, &QSlider::valueChanged, updateLabel);
    updateLabel(slider->value());
    layout->addWidget(slider);
    layout->addWidget(label);
    QPushButton *saveBtn = new QPushButton("Сохранить", group);
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, [this, label]() {
        savePhoneToFile(label->text());
    });
    return group;
}
QWidget* MainWindow::createComboboxExample() {
    QGroupBox *group = new QGroupBox("");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(group);
    QLabel *prefix = new QLabel("+375", group);
    QComboBox *combo1 = new QComboBox(group);
    QComboBox *combo2 = new QComboBox(group);
    QComboBox *combo3 = new QComboBox(group);
    for (int i = 0; i <= 99; i++)
        combo1->addItem(QString("%1").arg(i, 2, 10, QChar('0')));
    for (int i = 0; i <= 999; i++)
        combo2->addItem(QString("%1").arg(i, 3, 10, QChar('0')));
    for (int i = 0; i <= 9999; i++)
        combo3->addItem(QString("%1").arg(i, 4, 10, QChar('0')));
    layout->addWidget(prefix);
    layout->addWidget(combo1);
    layout->addWidget(combo2);
    layout->addWidget(combo3);
    QPushButton *saveBtn = new QPushButton("Сохранить", group);
    layout->addWidget(saveBtn);
    connect(saveBtn, &QPushButton::clicked, [this, prefix, combo1, combo2, combo3]() {
        QString phone = prefix->text() + combo1->currentText() + combo2->currentText() + combo3->currentText();
        savePhoneToFile(phone);
    });
    return group;
}
QWidget* MainWindow::createClickerExample() {
    QGroupBox *group = new QGroupBox("");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(group);
    QLabel *prefix = new QLabel("+375", group);
    QLineEdit *counter = new QLineEdit(group);
    counter->setReadOnly(true);
    counter->setText("000000000");
    QPushButton *incBtn = new QPushButton("+1", group);
    layout->addWidget(prefix);
    layout->addWidget(counter);
    layout->addWidget(incBtn);
    QPushButton *saveBtn = new QPushButton("Сохранить", group);
    layout->addWidget(saveBtn);
    int *currentValue = new int(0);
    connect(incBtn, &QPushButton::clicked, [counter, currentValue]() mutable {
        (*currentValue)++;
        counter->setText(QString("%1").arg(*currentValue, 9, 10, QChar('0')));
    });
    connect(saveBtn, &QPushButton::clicked, [this, prefix, counter]() {
        QString phone = prefix->text() + counter->text();
        savePhoneToFile(phone);
    });
    return group;
}
QWidget* MainWindow::createRollExample() {
    QGroupBox *group = new QGroupBox("");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QVBoxLayout *layout = new QVBoxLayout(group);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *digitsLayout = new QHBoxLayout;
    digitsLayout->setSpacing(0);
    digitsLayout->setContentsMargins(0, 0, 0, 0);
    QList<QLineEdit*> digits;
    QLabel *plusLabel = new QLabel("+", group);
    plusLabel->setFixedWidth(20);
    plusLabel->setAlignment(Qt::AlignCenter);
    digitsLayout->addWidget(plusLabel);
    for (int i = 0; i < 12; i++) {
        QLineEdit *edit = new QLineEdit(group);
        edit->setFocusPolicy(Qt::NoFocus);
        edit->setReadOnly(true);
        edit->setFixedWidth(20);
        edit->setAlignment(Qt::AlignCenter);
        edit->setText("0");
        digits.append(edit);
        digitsLayout->addWidget(edit);
    }
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setSpacing(0);
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    QPushButton *resetBtn = new QPushButton("Сброс", group);
    QPushButton *fixBtn = new QPushButton("След. цифра", group);
    buttonsLayout->addWidget(resetBtn);
    buttonsLayout->addWidget(fixBtn);
    layout->addLayout(digitsLayout);
    layout->addLayout(buttonsLayout);
    QPushButton *saveBtn = new QPushButton("Сохранить", group);
    layout->addWidget(saveBtn);
    QTimer *timer = new QTimer(group);
    timer->setInterval(100);
    int *currentIndex = new int(0);
    int *currentValue = new int(0);
    auto updateHighlight = [digits, currentIndex]() {
        for (int i = 0; i < digits.size(); i++) {
            if (i == *currentIndex)
                digits[i]->setStyleSheet("background-color: lightblue;");
            else
                digits[i]->setStyleSheet("");
        }
    };
    updateHighlight();
    connect(timer, &QTimer::timeout, group, [digits, currentIndex, currentValue, updateHighlight]() mutable {
        if (*currentIndex < digits.size()) {
            *currentValue = (*currentValue + 1) % 10;
            digits[*currentIndex]->setText(QString::number(*currentValue));
            updateHighlight();
        }
    });
    connect(fixBtn, &QPushButton::clicked, group, [currentIndex, currentValue, digits, updateHighlight]() mutable {
        if (*currentIndex < digits.size()) {
            (*currentIndex)++;
            *currentValue = 0;
            updateHighlight();
        }
    });
    connect(resetBtn, &QPushButton::clicked, group, [digits, currentIndex, currentValue, updateHighlight]() mutable {
        *currentIndex = 0;
        *currentValue = 0;
        for (int i = 0; i < digits.size(); i++)
            digits[i]->setText("0");
        updateHighlight();
    });
    connect(saveBtn, &QPushButton::clicked, group, [this, plusLabel, digits]() {
        QString phone = plusLabel->text();
        for (QLineEdit *edit : digits) {
            phone += edit->text();
        }
        savePhoneToFile(phone);
    });
    timer->start();
    return group;
}
QWidget* MainWindow::createRandomExample() {
    QGroupBox *group = new QGroupBox("");
    group->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    QHBoxLayout *layout = new QHBoxLayout(group);
    QLabel *label = new QLabel(group);
    QPushButton *btn = new QPushButton("Генерировать", group);
    layout->addWidget(label);
    layout->addWidget(btn);
    QPushButton *saveBtn = new QPushButton("Сохранить", group);
    layout->addWidget(saveBtn);
    auto generate = [label]() {
        qint64 value = QRandomGenerator::global()->bounded(1000000000000LL);
        QString formatted = QString("%1").arg(value, 12, 10, QChar('0'));
        label->setText("+" + formatted);
    };
    connect(btn, &QPushButton::clicked, generate);
    connect(saveBtn, &QPushButton::clicked, [this, label]() {
        savePhoneToFile(label->text());
    });
    generate();
    return group;
}
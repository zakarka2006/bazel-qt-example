#include "MainWindow.h"

#include <QFile>
#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTextStream>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi();
    setupConnections();

    updateTicketsCount(10);
    ticketsCountSpin->setValue(10);
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    setWindowTitle("Новое название приложения");

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    ticketsCountSpin = new QSpinBox;
    ticketsCountSpin->setRange(1, 100);

    ticketsView = new QTableWidget;
    ticketsView->setColumnCount(1);
    ticketsView->horizontalHeader()->setStretchLastSection(true);
    ticketsView->horizontalHeader()->hide();
    ticketsView->verticalHeader()->setDefaultAlignment(Qt::AlignHCenter);
    ticketsView->verticalHeader()->setMinimumWidth(30);
    ticketsView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ticketsView->verticalHeader()->setDefaultSectionSize(30);
    ticketsView->setSelectionMode(QAbstractItemView::NoSelection);
    ticketsView->setFocusPolicy(Qt::NoFocus);

    nextQuestionButton = new QPushButton("След. вопрос");
    previousQuestionButton = new QPushButton("Пред. вопрос");
    saveProgressButton = new QPushButton("Сохранить прогресс");
    loadProgressButton = new QPushButton("Восстановить прогресс");

    int buttonMinWidth = 180;
    nextQuestionButton->setMinimumWidth(buttonMinWidth);
    previousQuestionButton->setMinimumWidth(buttonMinWidth);
    saveProgressButton->setMinimumWidth(buttonMinWidth);
    loadProgressButton->setMinimumWidth(buttonMinWidth);

    nextQuestionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    previousQuestionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    saveProgressButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    loadProgressButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QGroupBox* questionGroup = new QGroupBox("Данные билета:");
    QGridLayout* questionLayout = new QGridLayout(questionGroup);

    numberLabel = new QLabel("Номер билета: -");
    nameLabel = new QLabel("Название билета: -");
    nameEdit = new QLineEdit;
    statusCombo = new QComboBox;
    statusCombo->addItems({"Не просмотрен", "Надо повторить", "Выучен"});

    questionLayout->addWidget(numberLabel, 0, 0);
    questionLayout->addWidget(nameLabel, 1, 0);
    questionLayout->addWidget(nameEdit, 2, 0);
    questionLayout->addWidget(statusCombo, 3, 0);
    questionGroup->setLayout(questionLayout);

    totalProgress = new QProgressBar;
    greenProgress = new QProgressBar;

    mainLayout->addWidget(ticketsCountSpin);
    mainLayout->addWidget(ticketsView);
    mainLayout->addWidget(questionGroup);

    mainLayout->addWidget(new QLabel("Прогресс (желтый + зеленый):"));
    mainLayout->addWidget(totalProgress);
    mainLayout->addWidget(new QLabel("Прогресс (только зеленый):"));
    mainLayout->addWidget(greenProgress);

    QHBoxLayout* buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(previousQuestionButton);
    buttonLayout->addWidget(nextQuestionButton);
    mainLayout->addLayout(buttonLayout);

    QHBoxLayout* buttonProgressLayout = new QHBoxLayout;
    buttonProgressLayout->addWidget(saveProgressButton);
    buttonProgressLayout->addWidget(loadProgressButton);
    mainLayout->addLayout(buttonProgressLayout);

    resize(600, 700);
}

void MainWindow::setupConnections() {
    connect(ticketsCountSpin, &QSpinBox::valueChanged, this, &MainWindow::updateTicketsCount);
    connect(nextQuestionButton, &QPushButton::clicked, this, &MainWindow::nextQuestion);
    connect(previousQuestionButton, &QPushButton::clicked, this, &MainWindow::previousQuestion);
    connect(ticketsView, &QTableWidget::cellClicked, this, &MainWindow::ticketCellClicked);
    connect(
        ticketsView, &QTableWidget::cellDoubleClicked, this, &MainWindow::ticketCellDoubleClicked);
    connect(nameEdit, &QLineEdit::editingFinished, this, &MainWindow::updateTicketInfo);
    connect(statusCombo, &QComboBox::currentTextChanged, this, &MainWindow::changeTicketStatus);
    connect(saveProgressButton, &QPushButton::clicked, this, &MainWindow::saveProgressToFile);
    connect(loadProgressButton, &QPushButton::clicked, this, &MainWindow::loadProgressFromFile);
}

void MainWindow::updateTicketsCount(int count) {
    if (count == tickets.size()) {
        return;
    }

    tickets.resize(count);
    ticketsView->setRowCount(count);
    questionHistory.clear();

    for (int i = 0; i < count; ++i) {
        tickets[i].name = QString("Билет %1").arg(i + 1);
        tickets[i].status = "Не просмотрен";

        QTableWidgetItem* item = new QTableWidgetItem(tickets[i].name);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ticketsView->setItem(i, 0, item);

        updateTicketCellColor(i);

        if (!ticketsView->verticalHeaderItem(i)) {
            QTableWidgetItem* headerItem = new QTableWidgetItem(QString::number(i + 1));
            headerItem->setTextAlignment(Qt::AlignCenter);
            ticketsView->setVerticalHeaderItem(i, headerItem);
        } else {
            ticketsView->verticalHeaderItem(i)->setText(QString::number(i + 1));
        }
    }

    currentTicketIndex = -1;
    numberLabel->setText("Номер билета: -");
    nameLabel->setText("Название билета: -");
    nameEdit->clear();
    statusCombo->setCurrentIndex(0);
    updateProgressBars();
}

void MainWindow::ticketCellClicked(int row, int column) {
    if (row < 0 || row >= tickets.size()) {
        return;
    }

    QFont defaultFont;
    for (int i = 0; i < tickets.size(); ++i) {
        if (ticketsView->item(i, 0)) {
            ticketsView->item(i, 0)->setFont(defaultFont);
        }
        if (ticketsView->verticalHeaderItem(i)) {
            ticketsView->verticalHeaderItem(i)->setFont(defaultFont);
        }
    }

    QFont ultraBoldFont;
    ultraBoldFont.setBold(true);
    ultraBoldFont.setWeight(QFont::Black);
    if (ticketsView->item(row, 0)) {
        ticketsView->item(row, 0)->setFont(ultraBoldFont);
    }

    if (ticketsView->verticalHeaderItem(row)) {
        ticketsView->verticalHeaderItem(row)->setFont(ultraBoldFont);
    }

    currentTicketIndex = row;
    numberLabel->setText(QString("Номер билета: %1").arg(row + 1));
    nameLabel->setText("Название билета: " + tickets[row].name);
    nameEdit->setText(tickets[row].name);
    statusCombo->setCurrentText(tickets[row].status);
}

void MainWindow::ticketCellDoubleClicked(int row, int column) {
    if (row < 0 || row >= tickets.size()) {
        return;
    }

    if (tickets[row].status == "Не просмотрен" || tickets[row].status == "Надо повторить") {
        tickets[row].status = "Выучен";
    } else if (tickets[row].status == "Выучен") {
        tickets[row].status = "Надо повторить";
    }

    updateTicketCellColor(row);
    if (currentTicketIndex == row) {
        statusCombo->setCurrentText(tickets[row].status);
    }

    updateProgressBars();
}

void MainWindow::updateTicketCellColor(int row) {
    if (!ticketsView->item(row, 0)) {
        return;
    }

    QColor color;
    if (tickets[row].status == "Не просмотрен") {
        color = Qt::transparent;
    } else if (tickets[row].status == "Надо повторить") {
        color = QColor(255, 235, 156);
    } else if (tickets[row].status == "Выучен") {
        color = QColor(180, 230, 180);
    }

    ticketsView->item(row, 0)->setBackground(color);
}

void MainWindow::updateTicketInfo() {
    if (currentTicketIndex < 0 || currentTicketIndex >= tickets.size()) {
        return;
    }

    if (nameEdit->text().isEmpty()) {
        return;
    }

    tickets[currentTicketIndex].name = nameEdit->text();
    nameLabel->setText("Название билета: " + nameEdit->text());
    ticketsView->item(currentTicketIndex, 0)->setText(tickets[currentTicketIndex].name);
}

void MainWindow::changeTicketStatus(const QString& status) {
    if (currentTicketIndex < 0 || currentTicketIndex >= tickets.size()) {
        return;
    }

    tickets[currentTicketIndex].status = status;
    updateTicketCellColor(currentTicketIndex);
    updateProgressBars();
}

void MainWindow::nextQuestion() {
    if (tickets.isEmpty()) {
        return;
    }

    QVector<int> availableTickets;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status != "Выучен" && currentTicketIndex != i) {
            availableTickets.append(i);
        }
    }

    if (availableTickets.isEmpty()) {
        return;
    }

    int randomIndex =
        availableTickets[QRandomGenerator::global()->bounded(availableTickets.size())];

    if (currentTicketIndex != -1) {
        questionHistory.push(currentTicketIndex);
    }

    ticketCellClicked(randomIndex, 0);
}

void MainWindow::previousQuestion() {
    if (questionHistory.isEmpty()) {
        return;
    }

    int previousIndex = questionHistory.pop();

    ticketCellClicked(previousIndex, 0);
}

void MainWindow::updateProgressBars() {
    int totalCount = tickets.size();
    int countYellowOrGreen = 0;
    int countGreen = 0;

    for (auto& t : tickets) {
        if (t.status == "Надо повторить" || t.status == "Выучен") {
            countYellowOrGreen++;
        }
        if (t.status == "Выучен") {
            countGreen++;
        }
    }

    totalProgress->setRange(0, totalCount);
    totalProgress->setValue(countYellowOrGreen);

    greenProgress->setRange(0, totalCount);
    greenProgress->setValue(countGreen);
}

void MainWindow::saveProgressToFile() {
    QString fileName = QFileDialog::getSaveFileName(
        this, tr("Сохранить прогресс"), QString(), tr("Text Files (*.txt);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this, tr("Ошибка"),
            tr("Не удалось открыть файл для записи:\n%1").arg(file.errorString()));
        return;
    }

    QTextStream out(&file);

    out << tickets.size() << "\n";

    for (const auto& t : tickets) {
        out << t.name << ";" << t.status << "\n";
    }

    file.close();
}

void MainWindow::loadProgressFromFile() {
    QString fileName = QFileDialog::getOpenFileName(
        this, tr("Загрузить прогресс"), QString(), tr("Text Files (*.txt);;All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(
            this, tr("Ошибка"),
            tr("Не удалось открыть файл для чтения:\n%1").arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    QString firstLine = in.readLine();
    bool ok = false;
    int newSize = firstLine.toInt(&ok);
    if (!ok) {
        QMessageBox::warning(
            this, tr("Ошибка"),
            tr("Некорректный формат файла: не могу прочитать количество билетов."));
        return;
    }

    tickets.resize(newSize);
    ticketsView->setRowCount(newSize);

    for (int i = 0; i < newSize; ++i) {
        if (in.atEnd()) {
            break;
        }
        QString line = in.readLine();

        int lastSemicolonIndex = line.lastIndexOf(';');

        if (lastSemicolonIndex == -1) {
            tickets[i].name = line;
            tickets[i].status = "Не просмотрено";
        } else {
            tickets[i].name = line.left(lastSemicolonIndex);
            tickets[i].status = line.mid(lastSemicolonIndex + 1);
        }
    }

    file.close();

    questionHistory.clear();
    for (int i = 0; i < newSize; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem(tickets[i].name);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable);
        ticketsView->setItem(i, 0, item);

        updateTicketCellColor(i);

        if (!ticketsView->verticalHeaderItem(i)) {
            QTableWidgetItem* headerItem = new QTableWidgetItem(QString::number(i + 1));
            headerItem->setTextAlignment(Qt::AlignCenter);
            ticketsView->setVerticalHeaderItem(i, headerItem);
        } else {
            ticketsView->verticalHeaderItem(i)->setText(QString::number(i + 1));
        }
    }

    currentTicketIndex = -1;
    numberLabel->setText("Номер билета: -");
    nameLabel->setText("Название билета: -");
    nameEdit->clear();
    statusCombo->setCurrentIndex(0);
    ticketsCountSpin->setValue(newSize);

    updateProgressBars();
}

MainWindow::~MainWindow() {
}

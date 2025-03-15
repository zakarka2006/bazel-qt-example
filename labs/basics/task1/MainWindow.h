#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QProgressBar>
#include <QVector>
#include <QStack>

struct Ticket {
    QString name;
    QString status;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateTicketsCount(int count);
    void nextQuestion();
    void previousQuestion();
    void ticketCellClicked(int row, int column);
    void ticketCellDoubleClicked(int row, int column);
    void updateTicketInfo();
    void changeTicketStatus(const QString &status);
    void saveProgressToFile();
    void loadProgressFromFile();

private:
    QSpinBox *ticketsCountSpin;
    QTableWidget *ticketsView;
    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QPushButton *nextQuestionButton;
    QPushButton *previousQuestionButton;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
    QPushButton *saveProgressButton;    
    QPushButton *loadProgressButton;    

    QVector<Ticket> tickets;
    QStack<int> questionHistory;
    int currentTicketIndex = -1;

    void setupUi();
    void setupConnections();
    void updateTicketCellColor(int row);
    void updateProgressBars();
};

#endif // MAINWINDOW_H

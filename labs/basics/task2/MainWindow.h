#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QVBoxLayout;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    QVBoxLayout *mainLayout;
    QList<QWidget*> groups;

    QWidget* createDefaultPage();
    QWidget* createSliderExample();
    QWidget* createComboboxExample();
    QWidget* createClickerExample();
    QWidget* createRollExample();
    QWidget* createRandomExample();

    void switchGroup(int index);
    void savePhoneToFile(const QString &phone);
};

#endif // MAINWINDOW_H

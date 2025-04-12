#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QCheckBox>
#include "drawingarea.h"
#include "controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPolygonModeClicked();
    void onLightModeClicked();
    void onStaticLightsModeClicked();
    void onClearPolygonsClicked();
    void onClearStaticLightsClicked();
    void onDynamicLightCollisionsChanged(int state);

private:
    void setupUI();
    void createModeButtons();
    void createUtilityButtons();

    DrawingArea* drawingArea;
    Controller* controller;
    
    QPushButton* polygonModeButton;
    QPushButton* lightModeButton;
    QPushButton* staticLightsModeButton;
    
    QPushButton* clearPolygonsButton;
    QPushButton* clearStaticLightsButton;
    QCheckBox* dynamicLightCollisionsCheckbox;
    
    QVBoxLayout* mainLayout;
    QHBoxLayout* modeButtonsLayout;
    QHBoxLayout* utilityButtonsLayout;
    QWidget* centralWidget;
};

#endif
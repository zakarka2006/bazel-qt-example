#include "mainwindow.h"
#include "drawingarea.h"
#include "controller.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QCheckBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , drawingArea(nullptr)
    , controller(nullptr)
{
    controller = new Controller(this);
    setupUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    centralWidget = new QWidget(this);
    mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    modeButtonsLayout = new QHBoxLayout();
    modeButtonsLayout->setSpacing(10);
    createModeButtons();
    mainLayout->addLayout(modeButtonsLayout);

    drawingArea = new DrawingArea(centralWidget);
    drawingArea->setController(controller);
    mainLayout->addWidget(drawingArea);

    utilityButtonsLayout = new QHBoxLayout();
    utilityButtonsLayout->setSpacing(10);
    createUtilityButtons();
    mainLayout->addLayout(utilityButtonsLayout);

    setCentralWidget(centralWidget);
    resize(800, 600);
}

void MainWindow::createModeButtons()
{
    polygonModeButton = new QPushButton("Polygon Mode", centralWidget);
    lightModeButton = new QPushButton("Light Mode", centralWidget);
    staticLightsModeButton = new QPushButton("Static Lights", centralWidget);

    lightModeButton->setChecked(true);

    modeButtonsLayout->addWidget(polygonModeButton);
    modeButtonsLayout->addWidget(lightModeButton);
    modeButtonsLayout->addWidget(staticLightsModeButton);

    connect(polygonModeButton, &QPushButton::clicked, this, &MainWindow::onPolygonModeClicked);
    connect(lightModeButton, &QPushButton::clicked, this, &MainWindow::onLightModeClicked);
    connect(staticLightsModeButton, &QPushButton::clicked, this, &MainWindow::onStaticLightsModeClicked);
}

void MainWindow::createUtilityButtons()
{
    clearPolygonsButton = new QPushButton("Clear Polygons", centralWidget);
    clearStaticLightsButton = new QPushButton("Clear Static Lights", centralWidget);
    dynamicLightCollisionsCheckbox = new QCheckBox("Dynamic Light Collisions", centralWidget);
    dynamicLightCollisionsCheckbox->setChecked(true);

    utilityButtonsLayout->addWidget(clearPolygonsButton);
    utilityButtonsLayout->addWidget(clearStaticLightsButton);
    utilityButtonsLayout->addWidget(dynamicLightCollisionsCheckbox);

    connect(clearPolygonsButton, &QPushButton::clicked, this, &MainWindow::onClearPolygonsClicked);
    connect(clearStaticLightsButton, &QPushButton::clicked, this, &MainWindow::onClearStaticLightsClicked);
    connect(dynamicLightCollisionsCheckbox, &QCheckBox::stateChanged, this, &MainWindow::onDynamicLightCollisionsChanged);
}

void MainWindow::onPolygonModeClicked()
{
    controller->setMode(Controller::Mode::POLYGON);
    drawingArea->setMode(Controller::Mode::POLYGON);
}

void MainWindow::onLightModeClicked()
{
    controller->setMode(Controller::Mode::LIGHT);
    drawingArea->setMode(Controller::Mode::LIGHT);
}

void MainWindow::onStaticLightsModeClicked()
{
    controller->setMode(Controller::Mode::STATIC_LIGHTS);
    drawingArea->setMode(Controller::Mode::STATIC_LIGHTS);
}

void MainWindow::onClearPolygonsClicked()
{
    while (controller->GetPolygons().size() > 1) {
        controller->RemoveLastPolygon();
    }
    drawingArea->CancelPolygonDrawing();
    drawingArea->updateBackgroundCache();
    drawingArea->update();
}

void MainWindow::onClearStaticLightsClicked()
{
    auto& lightSources = controller->GetLightSources();
    for (size_t i = lightSources.size(); i > 0; --i) {
        if (lightSources[i-1].isStatic) {
            controller->RemoveLightSource(i-1);
        }
    }
    drawingArea->update();
}

void MainWindow::onDynamicLightCollisionsChanged(int state)
{
    controller->SetDynamicLightCollisions(state == Qt::Checked);
}

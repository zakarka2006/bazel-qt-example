#ifndef DRAWINGAREA_H
#define DRAWINGAREA_H

#include <QWidget>
#include <QPoint>
#include <QTimer>
#include <QPixmap>
#include <vector>
#include <QPainter>
#include <QMouseEvent>
#include <QSlider>
#include "controller.h"

class DrawingArea : public QWidget
{
    Q_OBJECT

public:
    explicit DrawingArea(QWidget *parent = nullptr);
    ~DrawingArea();

    void setController(Controller* ctrl);
    void setMode(bool lightMode);
    void setMode(Controller::Mode newMode);
    void updateBackgroundCache();
    void CancelPolygonDrawing();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

private:
    void createBoundaryPolygon();
    void updateLightSource();
    void drawPolygons(QPainter& painter);
    void drawLightSource(QPainter& painter);
    bool hasSelfIntersections(const std::vector<QPointF>& polygon) const;
    bool polygonsIntersect(const std::vector<QPointF>& poly1, const std::vector<QPointF>& poly2) const;
    bool doSegmentsIntersect(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) const;
    bool isPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) const;

    Controller::Mode mode;
    Controller* controller;
    bool isDrawingPolygon;
    QSize lastSize;
    QPixmap* cachedBackground;
    QPointF cursorPos;
    QTimer updateTimer;
    QSlider* radiusSlider;
    size_t selectedLightIndex;

private slots:
    void onRadiusChanged(int value);
};

#endif
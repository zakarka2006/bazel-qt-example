#include "drawingarea.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QSlider>
#include <QPushButton>
#include <QCheckBox>

DrawingArea::DrawingArea(QWidget *parent)
    : QWidget(parent)
    , mode(Controller::Mode::LIGHT)
    , controller(nullptr)
    , isDrawingPolygon(false)
    , lastSize(size())
    , cachedBackground(nullptr)
    , cursorPos(0, 0)
    , selectedLightIndex(0)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    radiusSlider = new QSlider(Qt::Horizontal, this);
    radiusSlider->setRange(50, 700);
    radiusSlider->setValue(350);
    radiusSlider->setFixedWidth(200);
    radiusSlider->move(10, 10);
    radiusSlider->hide();
    connect(radiusSlider, &QSlider::valueChanged, this, &DrawingArea::onRadiusChanged);
    updateTimer.setInterval(8);
    connect(&updateTimer, &QTimer::timeout, this, [this]() {
        update();
    });
    updateTimer.start();
}

DrawingArea::~DrawingArea()
{
    delete cachedBackground;
}

void DrawingArea::setController(Controller* ctrl)
{
    controller = ctrl;
    createBoundaryPolygon();
}

void DrawingArea::setMode(bool lightMode)
{
    setMode(lightMode ? Controller::Mode::LIGHT : Controller::Mode::POLYGON);
}

void DrawingArea::setMode(Controller::Mode newMode)
{
    if (mode == Controller::Mode::POLYGON && newMode != Controller::Mode::POLYGON && isDrawingPolygon) {
        isDrawingPolygon = false;
        controller->RemoveLastPolygon();
    }
    mode = newMode;
    radiusSlider->setVisible(mode == Controller::Mode::STATIC_LIGHTS);
    update();
}

void DrawingArea::createBoundaryPolygon()
{
    if (!controller) return;
    std::vector<QPointF> boundaryVertices = {
        QPointF(0, 0),
        QPointF(width(), 0),
        QPointF(width(), height()),
        QPointF(0, height())
    };
    controller->AddPolygon(Polygon(boundaryVertices));
    lastSize = size();
}

void DrawingArea::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!controller) return;
    std::vector<QPointF> boundary = {
        QPointF(0, 0),
        QPointF(width(), 0),
        QPointF(width(), height()),
        QPointF(0, height())
    };
    if (controller->GetPolygons().empty()) {
        controller->AddPolygon(Polygon(boundary));
    } else {
        controller->UpdateBoundaryPolygon(boundary);
    }
    delete cachedBackground;
    cachedBackground = new QPixmap(size());
    updateBackgroundCache();
    lastSize = size();
    update();
}

void DrawingArea::updateBackgroundCache()
{
    if (!controller || !cachedBackground) return;
    cachedBackground->fill(Qt::black);
    QPainter painter(cachedBackground);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 2));
    for (const auto& polygon : controller->GetPolygons()) {
        const auto& vertices = polygon.getVertices();
        painter.drawPolygon(vertices.data(), vertices.size());
    }
}

void DrawingArea::updateLightSource()
{
}

void DrawingArea::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    if (cachedBackground) {
        painter.drawPixmap(0, 0, *cachedBackground);
    }
    if (mode == Controller::Mode::LIGHT || mode == Controller::Mode::STATIC_LIGHTS) {
        for (const auto& light : controller->GetLightSources()) {
            auto sightPolygons = controller->GetSightPolygons(light.position);
            if (!sightPolygons.empty()) {
                if (!light.isStatic) {
                    for (size_t i = 1; i < sightPolygons.size(); ++i) {
                        QPolygonF offsetPolygon;
                        for (const auto& point : sightPolygons[i]) {
                            offsetPolygon << point;
                        }
                        QPainterPath path;
                        path.addPolygon(offsetPolygon);
                        painter.fillPath(path, QColor(255, 255, 255, 20));
                    }
                }
                QPolygonF mainPolygon;
                for (const auto& point : sightPolygons[0]) {
                    mainPolygon << point;
                }
                QPainterPath mainPath;
                mainPath.addPolygon(mainPolygon);
                QRadialGradient gradient(light.position, light.isStatic ? light.radius : 300.0);
                if (light.isStatic) {
                    gradient.setColorAt(0.0, QColor(100, 150, 255, 180));
                    gradient.setColorAt(1.0, QColor(100, 150, 255, 0));
                } else {
                    gradient.setColorAt(0.0, QColor(255, 255, 200, 180));
                    gradient.setColorAt(1.0, QColor(255, 255, 200, 0));
                }
                painter.fillPath(mainPath, gradient);
            }
            painter.setPen(Qt::NoPen);
            if (light.isStatic) {
                painter.setBrush(QColor(56, 128, 221));
                const int numSources = 5;
                const double radius = 3.0;
                const double circleRadius = 3.0;
                for (int i = 0; i < numSources - 1; ++i) {
                    double angle = i * (2 * M_PI / (numSources - 1));
                    QPointF offset(
                        light.position.x() + circleRadius * std::cos(angle),
                        light.position.y() + circleRadius * std::sin(angle)
                    );
                    painter.drawEllipse(offset, radius, radius);
                }
                painter.drawEllipse(light.position, radius, radius);
            } else {
                const int numSources = 11;
                const double radius = 2.0;
                const double circleRadius = 15.0;
                QRadialGradient dotGradient(light.position, circleRadius);
                dotGradient.setColorAt(0, QColor(255, 255, 255, 200));
                dotGradient.setColorAt(1, QColor(255, 255, 255, 0));
                painter.setBrush(dotGradient);
                painter.drawEllipse(light.position, circleRadius, circleRadius);
                painter.setBrush(QColor(221, 56, 56));
                painter.drawEllipse(light.position, radius, radius);
                for (int i = 0; i < numSources - 1; ++i) {
                    double angle = i * (2 * M_PI / (numSources - 1));
                    QPointF offset(
                        light.position.x() + circleRadius * std::cos(angle),
                        light.position.y() + circleRadius * std::sin(angle)
                    );
                    painter.drawEllipse(offset, radius, radius);
                }
            }
        }
    } else if (mode == Controller::Mode::POLYGON && isDrawingPolygon) {
        const auto& currentPolygon = controller->GetPolygons().back();
        const auto& vertices = currentPolygon.getVertices();
        if (!vertices.empty()) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::white);
            for (const auto& vertex : vertices) {
                painter.drawEllipse(vertex, 4, 4);
            }
            painter.setPen(QPen(Qt::white, 2));
            for (size_t i = 0; i < vertices.size() - 1; ++i) {
                painter.drawLine(vertices[i], vertices[i + 1]);
            }
            {
                QPointF intersectionPoint;
                bool intersect = false;
                const auto& polygons = controller->GetPolygons();
                for (const auto& polygon : polygons) {
                    const auto& polyVertices = polygon.getVertices();
                    for (size_t i = 0; i < polyVertices.size(); ++i) {
                        QPointF current = polyVertices[i];
                        QPointF next = polyVertices[(i + 1) % polyVertices.size()];
                        QLineF edge(current, next);
                        if (QLineF(vertices.back(), cursorPos).intersects(edge, &intersectionPoint) == QLineF::BoundedIntersection) {
                            if (QLineF(intersectionPoint, current).length() > 0.001 &&
                                QLineF(intersectionPoint, next).length() > 0.001) {
                                intersect = true;
                                break;
                            }
                        }
                    }
                    if (intersect) break;
                }
                QPen previewPen(intersect ? Qt::red : Qt::white, 2);
                painter.setPen(previewPen);
                painter.drawLine(vertices.back(), cursorPos);
                if (intersect) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::red);
                    painter.drawEllipse(intersectionPoint, 4, 4);
                }
            }
            if (vertices.size() >= 2) {
                QLineF closingLine(cursorPos, vertices.front());
                bool closeIntersect = false;
                QPointF interValid;
                const double eps = 0.001;
                const auto& polygons = controller->GetPolygons();
                for (size_t i = 1; i < polygons.size() && !closeIntersect; ++i) {
                    const auto& polyVertices = polygons[i].getVertices();
                    for (size_t j = 0; j < polyVertices.size(); ++j) {
                        QPointF current = polyVertices[j];
                        QPointF next = polyVertices[(j + 1) % polyVertices.size()];
                        QLineF edge(current, next);
                        QPointF tempInter;
                        if (closingLine.intersects(edge, &tempInter) == QLineF::BoundedIntersection) {
                            if (QLineF(tempInter, cursorPos).length() < eps ||
                                QLineF(tempInter, vertices.front()).length() < eps) {
                                continue;
                            }
                            closeIntersect = true;
                            interValid = tempInter;
                            break;
                        }
                    }
                }
                QPen dashedPen(closeIntersect ? Qt::red : Qt::white, 2, Qt::DashLine);
                painter.setPen(dashedPen);
                painter.drawLine(cursorPos, vertices.front());
                if (closeIntersect) {
                    painter.setPen(Qt::NoPen);
                    painter.setBrush(Qt::red);
                    painter.drawEllipse(interValid, 4, 4);
                }
            }
        } else {
            painter.setPen(Qt::NoPen);
            painter.setBrush(Qt::white);
            painter.drawEllipse(cursorPos, 4, 4);
        }
    }
}

void DrawingArea::drawPolygons(QPainter& painter)
{
    if (!controller) return;
    if (mode == Controller::Mode::LIGHT) {
        for (const auto& light : controller->GetLightSources()) {
            Polygon lightArea = controller->CreateLightArea(light);
            painter.setPen(Qt::NoPen);
            painter.setBrush(light.color);
            const auto& vertices = lightArea.getVertices();
            if (vertices.size() >= 3) {
                QPolygonF polygon;
                for (const auto& vertex : vertices) {
                    polygon << vertex;
                }
                painter.drawPolygon(polygon);
            }
        }
    }
    painter.setPen(QPen(Qt::black, 2));
    painter.setBrush(Qt::transparent);
    for (const auto& polygon : controller->GetPolygons()) {
        const auto& vertices = polygon.getVertices();
        if (vertices.empty()) continue;
        for (size_t i = 0; i < vertices.size(); ++i) {
            const auto& current = vertices[i];
            const auto& next = vertices[(i + 1) % vertices.size()];
            painter.drawLine(current.toPoint(), next.toPoint());
        }
        painter.setPen(QPen(Qt::blue, 2));
        for (const auto& vertex : vertices) {
            painter.drawEllipse(vertex.toPoint(), 4, 4);
        }
        painter.setPen(QPen(Qt::black, 2));
    }
}

void DrawingArea::drawLightSource(QPainter& painter)
{
    for (size_t i = 0; i < controller->GetLightSources().size(); ++i) {
        const auto& light = controller->GetLightSources()[i];
        painter.setPen(QPen(light.isStatic ? Qt::blue : Qt::red, 2));
        painter.setBrush(light.isStatic ? Qt::blue : Qt::red);
        painter.drawEllipse(light.position.toPoint(), 6, 6);
        if (light.radius > 0) {
            painter.setPen(QPen(Qt::gray, 1, Qt::DashLine));
            painter.setBrush(Qt::NoBrush);
            int radius = static_cast<int>(light.radius);
            painter.drawEllipse(light.position.toPoint(), radius, radius);
        }
    }
}

void DrawingArea::mousePressEvent(QMouseEvent* event)
{
    if (!controller) return;
    QPointF pos = event->pos();
    if (mode == Controller::Mode::POLYGON) {
        if (event->button() == Qt::LeftButton) {
            if (!isDrawingPolygon) {
                isDrawingPolygon = true;
                controller->AddPolygon(Polygon({pos}));
            } else {
                const auto& currentPolygon = controller->GetPolygons().back();
                const auto& vertices = currentPolygon.getVertices();
                if (!vertices.empty()) {
                    const QPointF& lastVertex = vertices.back();
                    if (controller->WouldEdgeIntersect(lastVertex, pos)) {
                        QMessageBox::warning(this, "Invalid Edge", "The new edge would intersect with an existing polygon");
                        return;
                    }
                }
                controller->AddVertexToLastPolygon(pos);
            }
        } else if (event->button() == Qt::RightButton && isDrawingPolygon) {
            const auto& currentPolygon = controller->GetPolygons().back();
            const auto& vertices = currentPolygon.getVertices();
            isDrawingPolygon = false;
            if (vertices.size() >= 3) {
                const QPointF& firstVertex = vertices.front();
                const QPointF& lastVertex = vertices.back();
                if (!controller->WouldEdgeIntersect(lastVertex, firstVertex)) {
                    controller->AddVertexToLastPolygon(firstVertex);
                    updateBackgroundCache();
                } else {
                    QMessageBox::warning(this, "Invalid Edge", "Cannot close the polygon - the closing edge would intersect with an existing polygon");
                    controller->RemoveLastPolygon();
                    return;
                }
            } else {
                controller->RemoveLastPolygon();
            }
            controller->AddPolygon(Polygon({}));
        }
    } else if (mode == Controller::Mode::LIGHT) {
        controller->UpdateLightSourcePosition(0, pos);
    } else if (mode == Controller::Mode::STATIC_LIGHTS) {
        if (event->button() == Qt::LeftButton) {
            controller->AddLightSource(pos, true);
            selectedLightIndex = controller->GetLightSources().size() - 1;
            radiusSlider->setValue(controller->GetLightSources().back().radius);
        } else if (event->button() == Qt::RightButton) {
            for (size_t i = 0; i < controller->GetLightSources().size(); ++i) {
                const auto& light = controller->GetLightSources()[i];
                if (light.isStatic) {
                    double distance = QLineF(light.position, pos).length();
                    if (distance <= 10) {
                        selectedLightIndex = i;
                        radiusSlider->setValue(light.radius);
                        break;
                    }
                }
            }
        }
    }
    update();
}

void DrawingArea::mouseMoveEvent(QMouseEvent *event)
{
    if (!controller) return;
    QPointF pos(event->pos());
    cursorPos = pos;
    switch (mode) {
        case Controller::Mode::POLYGON:
            break;
        case Controller::Mode::LIGHT:
            if (!controller->GetLightSources().empty()) {
                controller->UpdateLightSourcePosition(0, pos);
            }
            break;
        case Controller::Mode::STATIC_LIGHTS:
            break;
    }
    update();
}

void DrawingArea::mouseReleaseEvent(QMouseEvent *event)
{
    if (!controller) return;
    switch (mode) {
        case Controller::Mode::POLYGON:
            break;
        case Controller::Mode::LIGHT:
            break;
        case Controller::Mode::STATIC_LIGHTS:
            break;
    }
    update();
}

bool DrawingArea::hasSelfIntersections(const std::vector<QPointF>& polygon) const
{
    if (polygon.size() < 3) return false;
    for (size_t i = 0; i < polygon.size(); ++i) {
        const QPointF& p1 = polygon[i];
        const QPointF& p2 = polygon[(i + 1) % polygon.size()];
        for (size_t j = i + 2; j < polygon.size(); ++j) {
            const QPointF& p3 = polygon[j];
            const QPointF& p4 = polygon[(j + 1) % polygon.size()];
            if (doSegmentsIntersect(p1, p2, p3, p4)) {
                return true;
            }
        }
    }
    return false;
}

bool DrawingArea::polygonsIntersect(const std::vector<QPointF>& poly1, const std::vector<QPointF>& poly2) const
{
    for (size_t i = 0; i < poly1.size(); ++i) {
        const QPointF& p1 = poly1[i];
        const QPointF& p2 = poly1[(i + 1) % poly1.size()];
        for (size_t j = 0; j < poly2.size(); ++j) {
            const QPointF& p3 = poly2[j];
            const QPointF& p4 = poly2[(j + 1) % poly2.size()];
            if (doSegmentsIntersect(p1, p2, p3, p4)) {
                return true;
            }
        }
    }
    if (isPointInPolygon(poly1[0], poly2) || isPointInPolygon(poly2[0], poly1)) {
        return true;
    }
    return false;
}

bool DrawingArea::doSegmentsIntersect(const QPointF& p1, const QPointF& p2, const QPointF& p3, const QPointF& p4) const
{
    auto orientation = [](const QPointF& a, const QPointF& b, const QPointF& c) {
        double val = (b.y() - a.y()) * (c.x() - b.x()) - (b.x() - a.x()) * (c.y() - b.y());
        if (val == 0) return 0;
        return (val > 0) ? 1 : 2;
    };
    auto onSegment = [](const QPointF& p, const QPointF& q, const QPointF& r) {
        return q.x() <= std::max(p.x(), r.x()) && q.x() >= std::min(p.x(), r.x()) &&
               q.y() <= std::max(p.y(), r.y()) && q.y() >= std::min(p.y(), r.y());
    };
    int o1 = orientation(p1, p2, p3);
    int o2 = orientation(p1, p2, p4);
    int o3 = orientation(p3, p4, p1);
    int o4 = orientation(p3, p4, p2);
    if (o1 != o2 && o3 != o4) return true;
    if (o1 == 0 && onSegment(p1, p3, p2)) return true;
    if (o2 == 0 && onSegment(p1, p4, p2)) return true;
    if (o3 == 0 && onSegment(p3, p1, p4)) return true;
    if (o4 == 0 && onSegment(p3, p2, p4)) return true;
    return false;
}

bool DrawingArea::isPointInPolygon(const QPointF& point, const std::vector<QPointF>& polygon) const
{
    if (polygon.size() < 3) return false;
    bool inside = false;
    for (size_t i = 0, j = polygon.size() - 1; i < polygon.size(); j = i++) {
        if (((polygon[i].y() > point.y()) != (polygon[j].y() > point.y())) &&
            (point.x() < (polygon[j].x() - polygon[i].x()) * (point.y() - polygon[i].y()) /
                        (polygon[j].y() - polygon[i].y()) + polygon[i].x())) {
            inside = !inside;
        }
    }
    return inside;
}

void DrawingArea::onRadiusChanged(int value)
{
    if (controller && selectedLightIndex < controller->GetLightSources().size()) {
        controller->UpdateStaticLightRadius(selectedLightIndex, value);
    }
}

void DrawingArea::CancelPolygonDrawing()
{
    isDrawingPolygon = false;
    updateBackgroundCache();
    update();
}
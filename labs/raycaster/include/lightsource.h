#ifndef LIGHTSOURCE_H
#define LIGHTSOURCE_H

#include <QPointF>
#include <QColor>
#include <QGradient>

struct LightSource {
    QPointF position;
    QColor color;
    double radius;
    bool isStatic;
    QRadialGradient gradient;
};

#endif
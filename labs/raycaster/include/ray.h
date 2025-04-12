#ifndef RAY_H
#define RAY_H

#include <QPointF>
#include <cmath>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end, double angle)
        : m_begin(begin)
        , m_end(end)
        , m_angle(angle)
    {}

    const QPointF& getBegin() const { return m_begin; }
    const QPointF& getEnd() const { return m_end; }
    double getAngle() const { return m_angle; }

    void setBegin(const QPointF& begin) { m_begin = begin; }
    void setEnd(const QPointF& end) { m_end = end; }
    void setAngle(double angle) { m_angle = angle; }

    Ray Rotate(double angle) const {
        double newAngle = m_angle + angle;
        double dx = m_end.x() - m_begin.x();
        double dy = m_end.y() - m_begin.y();
        double length = std::sqrt(dx*dx + dy*dy);
        QPointF newEnd(
            m_begin.x() + length * std::cos(newAngle),
            m_begin.y() + length * std::sin(newAngle)
        );
        return Ray(m_begin, newEnd, newAngle);
    }

private:
    QPointF m_begin;
    QPointF m_end;
    double m_angle;
};

#endif

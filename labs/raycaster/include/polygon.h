#ifndef POLYGON_H
#define POLYGON_H

#include <vector>
#include <optional>
#include <QPointF>
#include <cmath>
#include "ray.h"

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF>& vertices)
        : m_vertices(vertices)
    {}

    const std::vector<QPointF>& getVertices() const { return m_vertices; }
    
    void AddVertex(const QPointF& vertex) {
        m_vertices.push_back(vertex);
    }

    void UpdateLastVertex(const QPointF& new_vertex) {
        if (!m_vertices.empty()) {
            m_vertices.back() = new_vertex;
        }
    }

    std::optional<QPointF> IntersectRay(const Ray& ray) const {
        if (m_vertices.size() < 2) return std::nullopt;
        QPointF p = ray.getBegin();
        QPointF r = ray.getEnd() - ray.getBegin();
        bool found = false;
        QPointF closestIntersection;
        double minT = std::numeric_limits<double>::max();
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            QPointF a = m_vertices[i];
            QPointF b = m_vertices[(i + 1) % m_vertices.size()];
            QPointF s = b - a;
            double rxs = r.x() * s.y() - r.y() * s.x();
            if (std::fabs(rxs) < 1e-9) continue;
            QPointF diff = a - p;
            double t = (diff.x() * s.y() - diff.y() * s.x()) / rxs;
            double u = (diff.x() * r.y() - diff.y() * r.x()) / rxs;
            if (t >= 0 && u >= 0 && u <= 1.0) {
                if (t < minT) {
                    minT = t;
                    closestIntersection = QPointF(p.x() + t * r.x(), p.y() + t * r.y());
                    found = true;
                }
            }
        }
        return found ? std::optional<QPointF>(closestIntersection) : std::nullopt;
    }

    bool IsPointInside(const QPointF& point) const {
        if (m_vertices.size() < 3) return false;

        bool inside = false;
        for (size_t i = 0, j = m_vertices.size() - 1; i < m_vertices.size(); j = i++) {
            if (((m_vertices[i].y() > point.y()) != (m_vertices[j].y() > point.y())) &&
                (point.x() < (m_vertices[j].x() - m_vertices[i].x()) * (point.y() - m_vertices[i].y()) / 
                (m_vertices[j].y() - m_vertices[i].y()) + m_vertices[i].x())) {
                inside = !inside;
            }
        }
        return inside;
    }

    bool DoesEdgeIntersect(const QPointF& p1, const QPointF& p2) const {
        QPointF r = p2 - p1;
        for (size_t i = 0; i < m_vertices.size(); ++i) {
            QPointF a = m_vertices[i];
            QPointF b = m_vertices[(i + 1) % m_vertices.size()];
            QPointF s = b - a;
            double rxs = r.x() * s.y() - r.y() * s.x();
            if (std::fabs(rxs) < 1e-9) continue;
            QPointF diff = a - p1;
            double t = (diff.x() * s.y() - diff.y() * s.x()) / rxs;
            double u = (diff.x() * r.y() - diff.y() * r.x()) / rxs;
            if (t > 1e-9 && t < 1 - 1e-9 && u > 1e-9 && u < 1 - 1e-9)
                return true;
        }
        return false;
    }

private:
    std::vector<QPointF> m_vertices;
};

#endif
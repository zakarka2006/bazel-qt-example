#include "controller.h"
#include <QDebug>
#include <set>
#include <functional>
#include <QRadialGradient>
#include <cmath>

Controller::Controller(QObject *parent)
    : QObject(parent)
    , m_mode(Mode::LIGHT)
    , m_cacheValid(false)
    , dynamicLightCollisions(true)
{
    LightSource mainLight;
    mainLight.position = QPointF(400, 300);
    mainLight.color = QColor(255, 255, 255, 255);
    mainLight.radius = -1;
    mainLight.isStatic = false;
    m_lightSources.push_back(mainLight);
}

Controller::~Controller()
{
}

void Controller::setMode(Mode mode)
{
    m_mode = mode;
    emit modeChanged(mode == Mode::LIGHT || mode == Mode::STATIC_LIGHTS);
}

Controller::Mode Controller::getMode() const
{
    return m_mode;
}

bool Controller::isLightMode() const
{
    return m_mode == Mode::LIGHT || m_mode == Mode::STATIC_LIGHTS;
}

const std::vector<Polygon>& Controller::GetPolygons() const {
    return m_polygons;
}

void Controller::AddPolygon(const Polygon& polygon) {
    qDebug() << "add polygon " << polygon.getVertices();
    m_polygons.push_back(polygon);
    m_cacheValid = false;
}

void Controller::AddVertexToLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) {
        m_polygons.back().AddVertex(new_vertex);
        m_cacheValid = false;
    }
}

void Controller::UpdateLastPolygon(const QPointF& new_vertex) {
    if (!m_polygons.empty()) {
        m_polygons.back().UpdateLastVertex(new_vertex);
        m_cacheValid = false;
    }
}

void Controller::UpdateBoundaryPolygon(const std::vector<QPointF>& vertices) {
    if (!m_polygons.empty()) {
        m_polygons[0] = Polygon(vertices);
    }
}

void Controller::RemoveLastPolygon() {
    if (!m_polygons.empty()) {
        m_polygons.pop_back();
        m_cacheValid = false;
    }
}

const std::vector<LightSource>& Controller::GetLightSources() const {
    return m_lightSources;
}

void Controller::AddLightSource(const QPointF& position, bool isStatic)
{
    LightSource newLight;
    newLight.position = position;
    newLight.isStatic = isStatic;
    if (isStatic) {
        newLight.color = QColor(100, 150, 255, 150);
        newLight.radius = 350.0;
    } else {
        newLight.color = QColor(255, 255, 255, 255);
        newLight.radius = -1;
    }
    m_lightSources.push_back(newLight);
}

void Controller::RemoveLightSource(size_t index) {
    if (index < m_lightSources.size()) {
        m_lightSources.erase(m_lightSources.begin() + index);
    }
}

void Controller::UpdateLightSourcePosition(size_t index, const QPointF& newPosition)
{
    if (index < m_lightSources.size())
    {
        if (index == 0 && dynamicLightCollisions) {
            const int numSources = 10;
            const double radius = 4.0;
            const double circleRadius = 15.0;
            auto pointDistance = [](const QPointF& a, const QPointF& b) {
                double dx = a.x() - b.x();
                double dy = a.y() - b.y();
                return std::sqrt(dx*dx + dy*dy);
            };
            if (IsPointInsidePolygon(m_lightSources[0].position))
                return;
            if (IsPointInsidePolygon(newPosition))
                return;
            for (int i = 0; i < numSources; ++i) {
                double angle = 2 * M_PI * i / numSources;
                QPointF ballCenter(newPosition.x() + circleRadius * std::cos(angle),
                                   newPosition.y() + circleRadius * std::sin(angle));
                for (int j = 0; j < 4; ++j) {
                    double ballAngle = 2 * M_PI * j / 4;
                    QPointF point(ballCenter.x() + radius * std::cos(ballAngle),
                                  ballCenter.y() + radius * std::sin(ballAngle));
                    if (IsPointInsidePolygon(point))
                        return;
                }
            }
        }
        if (m_polygons.empty() || m_polygons[0].IsPointInside(newPosition))
        {
            m_lightSources[index].position = newPosition;
        }
    }
}

void Controller::UpdateStaticLightRadius(size_t index, double radius)
{
    if (index < m_lightSources.size() && m_lightSources[index].isStatic)
    {
        m_lightSources[index].radius = radius;
        m_cacheValid = false;
    }
}

bool Controller::IsPointInsidePolygon(const QPointF& point) const {
    for (size_t i = 1; i < m_polygons.size(); ++i) {
        const auto& vertices = m_polygons[i].getVertices();
        if (vertices.size() < 3) continue;
        bool inside = false;
        for (size_t i = 0, j = vertices.size() - 1; i < vertices.size(); j = i++) {
            if (((vertices[i].y() > point.y()) != (vertices[j].y() > point.y())) &&
                (point.x() < (vertices[j].x() - vertices[i].x()) * (point.y() - vertices[i].y()) /
                (vertices[j].y() - vertices[i].y()) + vertices[i].x())) {
                inside = !inside;
            }
        }
        if (inside) return true;
    }
    return false;
}

bool Controller::WouldEdgeIntersect(const QPointF& p1, const QPointF& p2) const
{
    return doesEdgeIntersectPolygons(p1, p2);
}

void Controller::updateVertexCache()
{
    m_vertexCache.clear();
    for (size_t polyIndex = 0; polyIndex < m_polygons.size(); ++polyIndex) {
        const auto& vertices = m_polygons[polyIndex].getVertices();
        for (size_t vertexIndex = 0; vertexIndex < vertices.size(); ++vertexIndex) {
            const QPointF& vertex = vertices[vertexIndex];
            if (m_vertexCache.find(vertex) == m_vertexCache.end()) {
                VertexData data;
                data.point = vertex;
                data.polygonIndex = polyIndex;
                data.vertexIndex = vertexIndex;
                m_vertexCache[vertex] = data;
            }
        }
    }
    m_cacheValid = true;
}

std::vector<double> Controller::calculateAngles(const QPointF& source, const QPointF& vertex) const
{
    double baseAngle = std::atan2(vertex.y() - source.y(), vertex.x() - source.x());
    return {
        baseAngle - ANGLE_OFFSET,
        baseAngle,
        baseAngle + ANGLE_OFFSET
    };
}

std::vector<Ray> Controller::CastRays(const LightSource& light)
{
    if (!m_cacheValid) {
        updateVertexCache();
    }
    std::vector<Ray> rays;
    rays.reserve(m_vertexCache.size() * 3);
    for (const auto& [vertex, data] : m_vertexCache) {
        if (data.polygonIndex == 0) continue;
        double dx = light.position.x() - vertex.x();
        double dy = light.position.y() - vertex.y();
        double distance = std::sqrt(dx*dx + dy*dy);
        if (light.radius > 0 && distance > light.radius)
            continue;
        auto angles = calculateAngles(light.position, vertex);
        for (double angle : angles) {
            QPointF direction(std::cos(angle), std::sin(angle));
            double rayLength = light.isStatic ? light.radius : 10000.0;
            QPointF end(light.position.x() + direction.x() * rayLength, 
                        light.position.y() + direction.y() * rayLength);
            rays.emplace_back(light.position, end, angle);
        }
    }
    return rays;
}

void Controller::IntersectRays(std::vector<Ray>* rays, const LightSource& light)
{
    if (!rays) return;
    auto distanceBetween = [](const QPointF& a, const QPointF& b) {
        double dx = a.x() - b.x();
        double dy = a.y() - b.y();
        return std::sqrt(dx*dx + dy*dy);
    };
    for (size_t i = 0; i < rays->size(); ++i) {
        Ray& ray = (*rays)[i];
        double minDistance = distanceBetween(ray.getBegin(), ray.getEnd());
        QPointF closestIntersection = ray.getEnd();
        for (const auto& polygon : m_polygons) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                double d = distanceBetween(ray.getBegin(), *intersection);
                if (d < minDistance) {
                    minDistance = d;
                    closestIntersection = *intersection;
                }
            }
        }
        if (closestIntersection != ray.getEnd()) {
            ray.setEnd(closestIntersection);
        }
    }
}

void Controller::RemoveAdjacentRays(std::vector<Ray>* rays)
{
    if (!rays || rays->empty()) return;
    std::sort(rays->begin(), rays->end(), 
        [](const Ray& a, const Ray& b) {
            return a.getAngle() < b.getAngle();
        });
    std::set<QPointF, std::function<bool(const QPointF&, const QPointF&)>> uniqueEndpoints(
        [](const QPointF& a, const QPointF& b) {
            double dx = a.x() - b.x();
            double dy = a.y() - b.y();
            return std::sqrt(dx*dx + dy*dy) > ADJACENT_THRESHOLD;
        }
    );
    std::vector<Ray> filteredRays;
    filteredRays.reserve(rays->size());
    for (const auto& ray : *rays) {
        if (uniqueEndpoints.insert(ray.getEnd()).second) {
            filteredRays.push_back(ray);
        }
    }
    *rays = std::move(filteredRays);
}

Polygon Controller::CreateLightArea(const LightSource& light) {
    if (light.isStatic) {
        std::vector<QPointF> vertices;
        const int numPoints = 36;
        for (int i = 0; i < numPoints; ++i) {
            double angle = 2 * M_PI * i / numPoints;
            QPointF point(
                light.position.x() + light.radius * std::cos(angle),
                light.position.y() + light.radius * std::sin(angle)
            );
            vertices.push_back(point);
        }
        return Polygon(vertices);
    } else {
        std::vector<Ray> rays = CastRays(light);
        IntersectRays(&rays, light);
        RemoveAdjacentRays(&rays);
        std::vector<QPointF> vertices;
        vertices.reserve(rays.size());
        for (const auto& ray : rays) {
            vertices.push_back(ray.getEnd());
        }
        return Polygon(vertices);
    }
}

void Controller::AddExamplePolygons() {
    m_polygons.push_back(Polygon({QPointF(111,104), QPointF(153,188), QPointF(221,105)}));
    m_polygons.push_back(Polygon({QPointF(556,98), QPointF(541,167), QPointF(602,132), QPointF(634,60)}));
    m_polygons.push_back(Polygon({QPointF(245,268), QPointF(263,416), QPointF(452,481), QPointF(456,356)}));
    m_polygons.push_back(Polygon({QPointF(544,273), QPointF(573,402), QPointF(720,234), QPointF(608,229)}));
    m_polygons.push_back(Polygon({QPointF(107,270), QPointF(77,407), QPointF(160,354)}));
}

std::vector<QPointF> Controller::GetRayIntersections(const QPointF& source) const
{
    struct Intersection {
        QPointF point;
        double angle;
        double param;
    };
    std::vector<Intersection> intersections;
    const double angleOffset = 0.00001;
    std::vector<QPointF> uniqueVertices;
    for (const auto& polygon : m_polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            QString key = QString("%1,%2").arg(vertex.x()).arg(vertex.y());
            bool isUnique = true;
            for (const auto& existing : uniqueVertices) {
                if (QString("%1,%2").arg(existing.x()).arg(existing.y()) == key) {
                    isUnique = false;
                    break;
                }
            }
            if (isUnique)
                uniqueVertices.push_back(vertex);
        }
    }
    std::vector<double> uniqueAngles;
    for (const auto& vertex : uniqueVertices) {
        double angle = std::atan2(vertex.y() - source.y(), vertex.x() - source.x());
        uniqueAngles.push_back(angle - angleOffset);
        uniqueAngles.push_back(angle);
        uniqueAngles.push_back(angle + angleOffset);
    }
    auto distanceBetween = [](const QPointF& a, const QPointF& b) {
        double dx = a.x() - b.x();
        double dy = a.y() - b.y();
        return std::sqrt(dx*dx + dy*dy);
    };
    for (double angle : uniqueAngles) {
        QPointF direction(std::cos(angle), std::sin(angle));
        QPointF end(source.x() + direction.x() * 10000.0, 
                    source.y() + direction.y() * 10000.0);
        Ray ray(source, end, angle);
        QPointF closestIntersection;
        double minDistance = std::numeric_limits<double>::max();
        bool hasIntersection = false;
        for (const auto& polygon : m_polygons) {
            auto intersection = polygon.IntersectRay(ray);
            if (intersection) {
                double d = distanceBetween(source, *intersection);
                if (d < minDistance) {
                    minDistance = d;
                    closestIntersection = *intersection;
                    hasIntersection = true;
                }
            }
        }
        if (hasIntersection) {
            intersections.push_back({closestIntersection, angle, minDistance});
        }
    }
    std::sort(intersections.begin(), intersections.end(),
        [](const auto& a, const auto& b) { return a.angle < b.angle; });
    std::vector<QPointF> result;
    for (const auto& intersection : intersections) {
        result.push_back(intersection.point);
    }
    return result;
}

std::vector<std::vector<QPointF>> Controller::GetSightPolygons(const QPointF& source) const
{
    const int numRays = 8;
    const double fuzzyRadius = 20.0;
    std::vector<std::vector<QPointF>> result;
    result.push_back(GetRayIntersections(source));
    for (int i = 0; i < numRays; ++i) {
        double angle = i * (2 * M_PI / numRays);
        QPointF offset(
            source.x() + fuzzyRadius * std::cos(angle),
            source.y() + fuzzyRadius * std::sin(angle)
        );
        result.push_back(GetRayIntersections(offset));
    }
    return result;
}

void Controller::SetDynamicLightCollisions(bool enabled)
{
    dynamicLightCollisions = enabled;
}

bool Controller::doesEdgeIntersectPolygons(const QPointF& p1, const QPointF& p2) const
{
    for (size_t i = 1; i < m_polygons.size(); ++i) {
        if (m_polygons[i].DoesEdgeIntersect(p1, p2)) {
            return true;
        }
    }
    return false;
}
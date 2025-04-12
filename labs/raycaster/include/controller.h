#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <vector>
#include <QPointF>
#include <unordered_map>
#include "polygon.h"
#include "ray.h"

class Controller : public QObject {
    Q_OBJECT

public:
    enum class Mode {
        POLYGON,
        LIGHT,
        STATIC_LIGHTS
    };

    explicit Controller(QObject *parent = nullptr);
    ~Controller();

    void setMode(Mode mode);
    Mode getMode() const;
    bool isLightMode() const;

    const std::vector<Polygon>& GetPolygons() const;
    void AddPolygon(const Polygon& polygon);
    void AddVertexToLastPolygon(const QPointF& new_vertex);
    void UpdateLastPolygon(const QPointF& new_vertex);
    void UpdateBoundaryPolygon(const std::vector<QPointF>& vertices);
    void RemoveLastPolygon();

    const std::vector<LightSource>& GetLightSources() const;
    void AddLightSource(const QPointF& position, bool isStatic);
    void RemoveLightSource(size_t index);
    void UpdateLightSourcePosition(size_t index, const QPointF& newPosition);
    void UpdateStaticLightRadius(size_t index, double radius);

    bool IsPointInsidePolygon(const QPointF& point) const;
    bool WouldEdgeIntersect(const QPointF& p1, const QPointF& p2) const;

    std::vector<Ray> CastRays(const LightSource& light);
    void IntersectRays(std::vector<Ray>* rays, const LightSource& light);
    void RemoveAdjacentRays(std::vector<Ray>* rays);

    Polygon CreateLightArea(const LightSource& light);
    void AddExamplePolygons();

    std::vector<QPointF> GetRayIntersections(const QPointF& source) const;
    std::vector<std::vector<QPointF>> GetSightPolygons(const QPointF& source) const;

    void SetDynamicLightCollisions(bool enabled);

signals:
    void modeChanged(bool isLightMode);

private:
    struct VertexData {
        QPointF point;
        size_t polygonIndex;
        size_t vertexIndex;
    };

    void updateVertexCache();
    bool doesEdgeIntersectPolygons(const QPointF& p1, const QPointF& p2) const;

    Mode m_mode;
    std::vector<Polygon> m_polygons;
    std::vector<LightSource> m_lightSources;
    std::unordered_map<QPointF, VertexData> m_vertexCache;
    bool m_cacheValid;
    bool dynamicLightCollisions;

    static constexpr double ANGLE_OFFSET = 0.0001;
    static constexpr double ADJACENT_THRESHOLD = 0.01;
};

#endif

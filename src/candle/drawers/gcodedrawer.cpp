// This file is a part of "Candle" application.
// Copyright 2015-2021 Hayrullin Denis Ravilevich

#include "gcodedrawer.h"

GcodeDrawer::GcodeDrawer() : QObject()
{   
    m_geometryUpdated = false;
    m_pointSize = 6;
    m_ignoreZ = false;
    m_grayscaleSegments = false;
    m_grayscaleCode = GcodeDrawer::S;
    m_grayscaleMin = 0;
    m_grayscaleMax = 255;

    connect(&m_timerVertexUpdate, &QTimer::timeout, this, &GcodeDrawer::onTimerVertexUpdate);
    m_timerVertexUpdate.start(100);
}

void GcodeDrawer::update()
{
    m_indexes.clear();
    m_geometryUpdated = false;
    ShaderDrawable::update();
}

void GcodeDrawer::update(QList<int> indexes)
{
    // Store segments to update
    m_indexes += indexes;
}

bool GcodeDrawer::updateData(GLPalette &palette)
{
    if (m_indexes.isEmpty()) return prepareVectors(palette); else return updateVectors(palette);
}

QVector3D GcodeDrawer::initialNormal(QVector3D p1, QVector3D p2)
{
    QVector3D direction = p2 - p1;
    QVector3D normal;

    if (direction.z() == 0) {
        normal = QVector3D(0.0, -1.0, 0.1);
    } else {
        normal = QVector3D(-direction.z(), 0.0, direction.x());
    }

    assert(normal.length());

    return normal.normalized();
}

void GcodeDrawer::computeNormals()
{
    QVector3D normal;
    QVector3D lastNormal;
    QVector<VertexData> newPoints;
    for (int i = 0; i < m_lines.count() - 2; i += 2) {
        QVector3D tangent = m_lines[i + 1].position - m_lines[i].position;
        tangent.normalize();
        if (i == 0) {
            normal = QVector3D(0, 0, 1); // Dowolny wektor normalny dla pierwszego odcinka
            if (QVector3D::dotProduct(normal, tangent) != 0) {
                // Korekta normalnej, aby była ortogonalna do tangenta
                normal = QVector3D::crossProduct(tangent, QVector3D(0, 1, 0));
                normal.normalize();
            }
            m_lines[i].start = normal;
        } else {
            // Korekcja normalnej na podstawie poprzedniego kierunku
            QVector3D projectedNormal = QVector3D::crossProduct(tangent, lastNormal);
            normal = QVector3D::crossProduct(projectedNormal, tangent);
            normal.normalize();
            m_lines[i - 1].start = normal;
            m_lines[i].start = normal;

            // newPoints.append(VertexData(m_lines[i - 1].position, Util::colorToVector(Qt::black), normal));
            // QVector3D p3 = m_lines[i - 1].position;
            // p3 += normal * 2.0;
            // newPoints.append(VertexData(p3, Util::colorToVector(Qt::black), normal));
        }
        normal.normalize();
        lastNormal = normal;
    }
}

bool GcodeDrawer::prepareVectors(GLPalette &palette)
{
    qDebug() << "preparing vectors" << this;

    QList<LineSegment> list = m_viewParser->getLines();
    VertexData vertex;

    qDebug() << "lines count" << list.count();

    // Clear all vertex data
    m_lines.clear();
    m_points.clear();
    m_triangles.clear();

    // Delete texture on mode change
    // if (m_texture) {
    //     m_texture->destroy();
    //     delete m_texture;
    //     m_texture = NULL;
    // }

    bool drawFirstPoint = true;
    for (int i = 0; i < list.count(); i++) {

        if (qIsNaN(list[i].getEnd().z())) {
            continue;
        }

        // Find first point of toolpath
        if (drawFirstPoint) {

            if (qIsNaN(list[i].getEnd().x()) || qIsNaN(list[i].getEnd().y())) continue;

            // Draw first toolpath point
            vertex.color = palette.color(m_colorStart);
            vertex.position = list[i].getEnd();
            if (m_ignoreZ) vertex.position.setZ(0);
            vertex.start = QVector3D(sNan, sNan, m_pointSize);
            m_points.append(vertex);

            drawFirstPoint = false;
            continue;
        }

        // Prepare vertices
        if (list[i].isFastTraverse()) vertex.start = list[i].getStart();
        else vertex.start = QVector3D(sNan, sNan, sNan);

        // Simplify geometry
        int j = i;
        if (m_simplify && i < list.count() - 1) {
            QVector3D start = list[i].getEnd() - list[i].getStart();
            QVector3D next;
            double length = start.length();
            bool straight = false;

            do {
                list[i].setVertexIndex(m_lines.count()); // Store vertex index
                i++;
                if (i < list.count() - 1) {
                    next = list[i].getEnd() - list[i].getStart();
                    length += next.length();
//                    straight = start.crossProduct(start.normalized(), next.normalized()).length() < 0.025;
                }
            // Split short & straight lines
            } while ((length < m_simplifyPrecision || straight) && i < list.count()
                     && getSegmentType(list[i]) == getSegmentType(list[j]));
            i--;
        } else {
            list[i].setVertexIndex(m_lines.count()); // Store vertex index
        }

        vertex.color = palette.color(getSegmentColor(list[i]));

        // ignore shorter than 0.0001
        if ((list[i].getEnd() - list[j].getStart()).length() > 0.0001) {
            // Line start
            vertex.position = list[j].getStart();
            if (m_ignoreZ) vertex.position.setZ(0);
            m_lines.append(vertex);

            // Line end
            vertex.position = list[i].getEnd();
            if (m_ignoreZ) vertex.position.setZ(0);
            m_lines.append(vertex);
        }

        // Draw last toolpath point
        if (i == list.count() - 1) {
            vertex.color = palette.color(m_colorEnd);
            vertex.position = list[i].getEnd();
            if (m_ignoreZ) vertex.position.setZ(0);
            vertex.start = QVector3D(sNan, sNan, m_pointSize);
            m_points.append(vertex);
        }
    }

    computeNormals();

    // if (m_lines.count())
    // {
    //     // static float rot = 0;
    //     // QMatrix4x4 m;
    //     // m.setToIdentity();
    //     // m.rotate(rot, 0, 0, 1);
    //     // rot += 5;

    //     //qDebug() << m.map(m_eye);

    //     // m_lines.append(VertexData(QVector3D(100,100,100), Util::colorToVector(Qt::black), QVector3D(0, 0, 1)));
    //     // m_lines.append(VertexData(QVector3D(0,0,0), Util::colorToVector(Qt::black), QVector3D(0, 0, 1)));

    //     // Calculate normals
    //     int il = 0;
    //     int vc = m_lines.count() / 2;
    //     QVector<QVector3D> tangents;

    //     QVector<QVector3D> normals;
    //     normals.append(initialNormal(m_lines[0].position, m_lines[2].position));
    //     assert((m_lines[0].position - m_lines[2].position).length());

    //     for (int i = 0; i < vc - 1; i++) {
    //         QVector3D T_i = (m_lines[il + 2].position - m_lines[il].position).normalized();
    //         tangents.append(T_i);
    //         assert(T_i.length());

    //         if (il > 0) {
    //             QVector3D N_i = QVector3D::crossProduct(
    //                 QVector3D::crossProduct(tangents[i - 1], normals.last()),
    //                 tangents[i]
    //             ).normalized();
    //             if (QVector3D::dotProduct(N_i, normals.last()) < 0) {
    //                 N_i = -N_i;
    //             }

    //             if (!N_i.length()) {
    //                 // qDebug() << N_i << m_lines[il + 2].position << m_lines[il].position << m_lines[il - 2].position << tangents[i] << tangents[i - 1] << normals.last()
    //                 //          << QVector3D::crossProduct(tangents[i - 1], normals.last());
    //             }
    //             assert(N_i.length());

    //             normals.append(N_i);
    //         }

    //         il+=2;
    //     }
    //     normals.append(normals.last());

    //     il = 0;
    //     // QVector<VertexData> normalVertices;
    //     // QVector3D black = Util::colorToVector(Qt::black);
    //     for (auto &v : m_lines) {
    //         v.start = normals[il/2];
    //         il++;
    //         //m_lines.append(v);

    //         // normalVertices.append(VertexData(v.position, black, QVector3D(0,0,1)));
    //         // normalVertices.append(VertexData(v.position + v.start * 1, black, QVector3D(0,0,1)));
    //     }

    //     // m_lines += normalVertices;

    //     qDebug() << m_lines.count() << normals.count();
    // }

    qDebug() << m_lines.count();

    m_geometryUpdated = true;
    m_indexes.clear();

    return true;
}

bool GcodeDrawer::updateVectors(GLPalette &palette)
{
    // Update vertices
    QList<LineSegment>& list = m_viewParser->getLines();

    // Map buffer
    VertexData* data = (VertexData*)m_vbo.map(QOpenGLBuffer::WriteOnly);

    // Update vertices for each line segment
    int vertexIndex;
    foreach (int i, m_indexes) {
        // Update vertex pair
        if (i < 0 || i > list.count() - 1) continue;
        vertexIndex = list[i].vertexIndex();
        if (vertexIndex >= 0) {
            // Update vertex array            
            if (data) {
                data[vertexIndex].color = palette.color(getSegmentColor(list[i]));
                data[vertexIndex + 1].color = data[vertexIndex].color;
            } else {
                m_lines[vertexIndex].color = palette.color(getSegmentColor(list[i]));
                m_lines[vertexIndex + 1].color = m_lines.at(vertexIndex).color;
            }
        }
    }

    m_indexes.clear();
    if (data) m_vbo.unmap();

    return !data;
}

// void GcodeDrawer::setImagePixelColor(QImage &image, double x, double y, QRgb color) const
// {
//     if (qIsNaN(x) || qIsNaN(y)) {
//         qDebug() << "Error updating pixel" << x << y;
//         return;
//     };

//     uchar* pixel = image.scanLine((int)y);

//     *(pixel + (int)x * 3) = qRed(color);
//     *(pixel + (int)x * 3 + 1) = qGreen(color);
//     *(pixel + (int)x * 3 + 2) = qBlue(color);
// }

QVector3D GcodeDrawer::getSegmentColorVector(LineSegment& segment)
{
    return Util::colorToVector(getSegmentColor(segment));
}

QColor GcodeDrawer::getSegmentColor(LineSegment& segment)
{
    if (segment.drawn()) return m_colorDrawn;//QVector3D(0.85, 0.85, 0.85);
    else if (segment.isHightlight()) return m_colorHighlight;//QVector3D(0.57, 0.51, 0.9);
    else if (segment.isFastTraverse()) return m_colorNormal;// QVector3D(0.0, 0.0, 0.0);
    else if (segment.isZMovement()) return m_colorZMovement;//QVector3D(1.0, 0.0, 0.0);
    else if (m_grayscaleSegments) switch (m_grayscaleCode) {
    case GcodeDrawer::S:
        return QColor::fromHsl(0, 0, qBound<int>(0, 255 - 255.0 / (m_grayscaleMax - m_grayscaleMin) * segment.getSpindleSpeed(), 255));
    case GcodeDrawer::Z:
        return QColor::fromHsl(0, 0, qBound<int>(0, 255 - 255.0 / (m_grayscaleMax - m_grayscaleMin) * segment.getStart().z(), 255));
    }
    return m_colorNormal;//QVector3D(0.0, 0.0, 0.0);
}

int GcodeDrawer::getSegmentType(LineSegment& segment)
{
    return segment.isFastTraverse() + segment.isZMovement() * 2;
}

QVector3D GcodeDrawer::getSizes()
{
    QVector3D min = m_viewParser->getMinimumExtremes();
    QVector3D max = m_viewParser->getMaximumExtremes();

    return QVector3D(max.x() - min.x(), max.y() - min.y(), max.z() - min.z());
}

QVector3D GcodeDrawer::getMinimumExtremes()
{
    QVector3D v = m_viewParser->getMinimumExtremes();
    if (m_ignoreZ) v.setZ(0);

    return v;
}

QVector3D GcodeDrawer::getMaximumExtremes()
{
    QVector3D v = m_viewParser->getMaximumExtremes();
    if (m_ignoreZ) v.setZ(0);

    return v;
}

void GcodeDrawer::setViewParser(GCodeViewParser* viewParser)
{
    // do not delete old parser!
    m_viewParser = viewParser;
}

GCodeViewParser *GcodeDrawer::viewParser()
{
    return m_viewParser;
}

bool GcodeDrawer::simplify() const
{
    return m_simplify;
}

void GcodeDrawer::setSimplify(bool simplify)
{
    m_simplify = simplify;
}

double GcodeDrawer::simplifyPrecision() const
{
    return m_simplifyPrecision;
}

void GcodeDrawer::setSimplifyPrecision(double simplifyPrecision)
{
    m_simplifyPrecision = simplifyPrecision;
}

bool GcodeDrawer::geometryUpdated()
{
    return m_geometryUpdated;
}

QColor GcodeDrawer::colorNormal() const
{
    return m_colorNormal;
}

void GcodeDrawer::setColorNormal(const QColor &colorNormal)
{
    m_colorNormal = colorNormal;
}

QColor GcodeDrawer::colorHighlight() const
{
    return m_colorHighlight;
}

void GcodeDrawer::setColorHighlight(const QColor &colorHighlight)
{
    m_colorHighlight = colorHighlight;
}
QColor GcodeDrawer::colorZMovement() const
{
    return m_colorZMovement;
}

void GcodeDrawer::setColorZMovement(const QColor &colorZMovement)
{
    m_colorZMovement = colorZMovement;
}

QColor GcodeDrawer::colorDrawn() const
{
    return m_colorDrawn;
}

void GcodeDrawer::setColorDrawn(const QColor &colorDrawn)
{
    m_colorDrawn = colorDrawn;
}

QColor GcodeDrawer::colorStart() const
{
    return m_colorStart;
}

void GcodeDrawer::setColorStart(const QColor &colorStart)
{
    m_colorStart = colorStart;
}

QColor GcodeDrawer::colorEnd() const
{
    return m_colorEnd;
}

void GcodeDrawer::setColorEnd(const QColor &colorEnd)
{
    m_colorEnd = colorEnd;
}

bool GcodeDrawer::getIgnoreZ() const
{
    return m_ignoreZ;
}

void GcodeDrawer::setIgnoreZ(bool ignoreZ)
{
    m_ignoreZ = ignoreZ;
}

void GcodeDrawer::onTimerVertexUpdate()
{
    if (!m_indexes.isEmpty()) ShaderDrawable::update();
}

int GcodeDrawer::grayscaleMax() const
{
    return m_grayscaleMax;
}

void GcodeDrawer::setGrayscaleMax(int grayscaleMax)
{
    m_grayscaleMax = grayscaleMax;
}

int GcodeDrawer::grayscaleMin() const
{
    return m_grayscaleMin;
}

void GcodeDrawer::setGrayscaleMin(int grayscaleMin)
{
    m_grayscaleMin = grayscaleMin;
}

GcodeDrawer::GrayscaleCode GcodeDrawer::grayscaleCode() const
{
    return m_grayscaleCode;
}

void GcodeDrawer::setGrayscaleCode(const GrayscaleCode &grayscaleCode)
{
    m_grayscaleCode = grayscaleCode;
}

bool GcodeDrawer::getGrayscaleSegments() const
{
    return m_grayscaleSegments;
}

void GcodeDrawer::setGrayscaleSegments(bool grayscaleSegments)
{
    m_grayscaleSegments = grayscaleSegments;
}

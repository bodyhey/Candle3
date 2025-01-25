#ifndef SHADERDRAWABLE_H
#define SHADERDRAWABLE_H

#include <QObject>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include "utils/util.h"
#include "../widgets/glpalette.h"

struct VertexData
{
    VertexData() {}

    VertexData(QVector3D pos, GLfloat col, QVector3D sta) {
        position = pos;
        color = col;
        start = sta;
    }

    QVector3D position;
    GLfloat color;
    QVector3D start;
};

struct _2DTexturedVertexData
{
    _2DTexturedVertexData() {}
    _2DTexturedVertexData(QVector2D pos, QVector2D tex) {
        position = pos;
        texCoord = tex;
    }

    QVector2D position;
    QVector2D texCoord;
};

class ShaderDrawable : protected QOpenGLFunctions
{
public:
    enum class ProgramType {
        Default,
        GCode,
    };

    explicit ShaderDrawable();
    ~ShaderDrawable();
    void update();
    void draw(QOpenGLShaderProgram *shaderProgram);

    bool needsUpdateGeometry() const;
    virtual void updateGeometry(QOpenGLShaderProgram *shaderProgram, GLPalette &palette);

    virtual QVector3D getSizes();
    virtual QVector3D getMinimumExtremes();
    virtual QVector3D getMaximumExtremes();
    virtual int getVertexCount();

    double lineWidth() const;
    void setLineWidth(double lineWidth);

    bool visible() const;
    void setVisible(bool visible);

    double pointSize() const;
    void setPointSize(double pointSize);

    QList<VertexData>& lines() { return m_lines; }
    virtual bool updateData(GLPalette &palette);

    virtual ProgramType programType() { return ProgramType::Default; };

signals:

public slots:

protected:
    double m_lineWidth;
    double m_pointSize;
    bool m_visible;
    float m_globalAlpha = 1;

    QVector<VertexData> m_lines;
    QVector<VertexData> m_points;
    QVector<VertexData> m_triangles;
    QOpenGLTexture *m_texture;

    QOpenGLBuffer m_vbo; // Protected for direct vbo access

    void init();
    virtual void bindAttributes(QOpenGLShaderProgram *&shaderProgram);

private:
    QOpenGLVertexArrayObject m_vao;

    bool m_needsUpdateGeometry;
};

#endif // SHADERDRAWABLE_H

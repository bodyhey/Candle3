#include "vertexdataexporter.h"
#include <QFile>

VertexDataExporter::VertexDataExporter() {
}

void VertexDataExporter::exportToJsFile(const QString &fileName, const QVector<VertexData> &data)
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);

    QTextStream out(&file);

    out << "var vertexData = [\n";

    for (auto &vertex : data) {
        out << QString("%1, %2, %3, %4, %5, %6, %7, %8, %9,\n")
            .arg(vertex.position.x())
            .arg(vertex.position.y())
            .arg(vertex.position.z())
            .arg(vertex.color.x())
            .arg(vertex.color.y())
            .arg(vertex.color.z())
            .arg(vertex.start.x())
            .arg(vertex.start.y())
            .arg(vertex.start.z());
    }

    out << "];\n";

    file.close();
}


class VertexData {
    m_position;
    m_color;
    m_start;

    constructor(position, color, start) {
        this.m_position = position;
        this.m_color = color;
        this.m_start = start;
    }
}

class Vertexes extends Array {
    constructor() {
        super();
    }

    pushVertex(position, color, start) {
        this.push(new VertexData(position, color, start));
    }

    toRawArray() {
        const raw = new Array(this.length * 9);
        let index = 0;
        for (const vertex of this) {
            raw[index++] = vertex.m_position[0];
            raw[index++] = vertex.m_position[1];
            raw[index++] = vertex.m_position[2];
            raw[index++] = vertex.m_color[0];
            raw[index++] = vertex.m_color[1];
            raw[index++] = vertex.m_color[2];
            raw[index++] = vertex.m_start[0];
            raw[index++] = vertex.m_start[1];
            raw[index++] = vertex.m_start[2];
        }
        return raw;
    }
}

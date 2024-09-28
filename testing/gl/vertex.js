
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
        const raw = [];
        for (const vertex of this) {
            raw.push(...vertex.m_position);
            raw.push(...vertex.m_color);
            raw.push(...vertex.m_start);
        }
        return raw;
        return new Float32Array(raw)    ;
    }
}

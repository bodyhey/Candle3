
class ShaderDrawable1 {
    m_needsUpdateGeometry = true;
    m_visible = true;
    m_lineWidth = 0.1;
    m_pointSize = 1.0;
    m_globalAlpha = 1.0;
    m_texture = null;
    //m_vao;
    m_vbo = null;
    m_lines = [];
    m_triangles = [];
    m_points = [];
    m_program = null;

    constructor(shaderProgram) {
        this.m_program = shaderProgram;
    }

    init()
    {
        // Create buffers
        //this.m_vao = gl.createVertexArray();
        this.m_vbo = gl.createBuffer();
    }

    update()
    {
        this.m_needsUpdateGeometry = true;
    }

    updateData()
    {
        // Test data
        // this.m_lines = new Vertexes();
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(0, 0, 0), glMatrix.vec3.fromValues(1, 0, 0), glMatrix.vec3.fromValues(NaN, 0, 0)));
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(10, 0, 0), glMatrix.vec3.fromValues(1, 0, 0), glMatrix.vec3.fromValues(NaN, 0, 0)));
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(0, 0, 0), glMatrix.vec3.fromValues(0, 1, 0), glMatrix.vec3.fromValues(NaN, 0, 0)));
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(0, -10, 0), glMatrix.vec3.fromValues(1, 1, 0), glMatrix.vec3.fromValues(NaN, 0, 0)));
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(0, 0, 0), glMatrix.vec3.fromValues(0, 0, 1), glMatrix.vec3.fromValues(NaN, 0, 0)));
        // this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(0, 0, 10), glMatrix.vec3.fromValues(0, 0, 1), glMatrix.vec3.fromValues(NaN, 0, 0)));

        // Grid
        const SIZE = 50;
        const color1 = glMatrix.vec3.fromValues(1, 0, 0);
        const color2 = glMatrix.vec3.fromValues(0, 0, 1);

        // this.m_lines = new Vertexes();

        // for (let i = -SIZE; i < SIZE; i++) {
        //     this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(i * 10, -SIZE * 10, 0), color1, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
        //     this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(i * 10, SIZE * 10, 0), color2, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
        //     this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(-SIZE * 10, i * 10, 0), color1, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
        //     this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(SIZE * 10, i * 10, 0), color2, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
        // }

        return true;
    }

    updateGeometry()
    {
        this.m_program.bind();

        // Init in context
        if (this.m_vbo == null) this.init();

        // console.log(this.m_vao);
        //if (this.m_vao.isCreated()) {
            // Prepare vao
            //this.m_vao.bind();
        //gl.bindVertexArray(this.m_vao);
        //}

        // Prepare vbo
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_vbo);
        //this.m_vbo.bind();

        // Update vertex buffer
        if (this.updateData()) {
            // Fill vertices buffer
            const vertexData = [];
//                        vertexData.push(...this.m_triangles);
            //vertexData.push(...this.m_lines);
//                        vertexData.push(...this.m_points);
            //
            // QVector<VertexData> vertexData(this.m_triangles);
            // vertexData += this.m_lines;
            // vertexData += this.m_points;
            //this.m_vbo.allocate(vertexData, vertexData.length * VertexData.sizeof());
            //console.log(this.m_lines);
            //gl.bufferData(gl.ARRAY_BUFFER, this.m_lines.toRawArray(), gl.STATIC_DRAW);
        } else {
            this.m_vbo.release();
            if (this.m_vao.isCreated()) m_vao.release();
            this.m_needsUpdateGeometry = false;
            this.m_program.release();
            return;
        }

        this.m_program.release();
        return;

        //if (m_vao.isCreated())
        {
            // // Offset for position
            // //quintptr ??
            // let offset = 0;

            // // Tell OpenGL programmable pipeline how to locate vertex position data
            // const vertexLocation = shaderProgram.attributeLocation("a_position");
            // shaderProgram.enableAttributeArray(vertexLocation);
            // shaderProgram.setAttributeBuffer(vertexLocation, gl.FLOAT, offset, 3, VertexData.sizeof());

            // // Offset for color
            // offset += 3; //  QVector3D.sizeof();

            // // Tell OpenGL programmable pipeline how to locate vertex color data
            // const colorLocation = shaderProgram.attributeLocation("a_color");
            // shaderProgram.enableAttributeArray(colorLocation);
            // shaderProgram.setAttributeBuffer(colorLocation, gl.FLOAT, offset, 3, VertexData.sizeof());

            // // Offset for line start point
            // offset += 3; //QVector3D.sizeof();

            // // Tell OpenGL programmable pipeline how to locate vertex line start point
            // const startLocation = shaderProgram.attributeLocation("a_start");
            // shaderProgram.enableAttributeArray(startLocation);
            // shaderProgram.setAttributeBuffer(startLocation, gl.FLOAT, offset, 3, VertexData.sizeof());

            // //this.m_vao.release();
        }

        //this.m_vbo.release();

        this.m_needsUpdateGeometry = false;
    }

    getMinimumExtremes() {
        return glMatrix.vec3.fromValues(-500, -500, -1);
    }

    getMaximumExtremes() {
        return glMatrix.vec3.fromValues(500, 500, 1);
    }

    getVertexCount() {
        return 0;
    }

    calculateNormal(P1, P2, P3) {
        // Oblicz wektory krawędzi
        let v1 = glMatrix.vec3.create();
        let v2 = glMatrix.vec3.create();
        glMatrix.vec3.sub(v1, P2, P1);
        glMatrix.vec3.sub(v2, P3, P1);

        // Iloczyn wektorowy
        let normal = glMatrix.vec3.create();
        glMatrix.vec3.cross(normal, v1, v2);

        // Normalizacja wektora
        glMatrix.vec3.normalize(normal, normal);

        return normal;
    }

    once = true;
    m_lines = null;

    draw(eye)
    {
        if (!this.m_visible) return;

        this.m_program.bind();

        // if (this.m_vao.isCreated()) {
        //     // Prepare vao
        //     m_vao.bind();
        // } else {
            // Prepare vbo
            gl.bindBuffer(gl.ARRAY_BUFFER, this.m_vbo);
            //this.m_vbo.bind();

            // Offset for position
            //quintptr
            let offset = 0;

            // Tell OpenGL programmable pipeline how to locate vertex position data
            const vertexLocation = this.m_program.attributeLocation("a_position");
            this.m_program.enableAttributeArray(vertexLocation);
            this.m_program.setAttributeBuffer(vertexLocation, gl.FLOAT, offset, 3, VERTEX_SIZE);

            // Offset for color
            offset += VECTOR3D_SIZE;

            // console.log('Vertex size: ' + VERTEX_SIZE);
            // console.log('Vector3D size: ' + VECTOR3D_SIZE);

            // Tell OpenGL programmable pipeline how to locate vertex color data
            const colorLocation = this.m_program.attributeLocation("a_color");
            this.m_program.enableAttributeArray(colorLocation);
            this.m_program.setAttributeBuffer(colorLocation, gl.FLOAT, offset, 3, VERTEX_SIZE);

            // Offset for line start point
            offset += VECTOR3D_SIZE;

            // Tell OpenGL programmable pipeline how to locate vertex line start point
            const startLocation = this.m_program.attributeLocation("a_normal");
            this.m_program.enableAttributeArray(startLocation);
            this.m_program.setAttributeBuffer(startLocation, gl.FLOAT, offset, 3, VERTEX_SIZE);

            //shaderProgram.setAttributeValue("a_alpha", this.m_globalAlpha);
        // }

        //if (this.m_lines.length)
        {
            //console.log('draw lines', this.m_lines.length);

            const color1 = glMatrix.vec3.fromValues(0, 1, 0);
            const color2 = glMatrix.vec3.fromValues(0, 1, 0);
            const color3 = glMatrix.vec3.fromValues(0, 1, 0);
            const color4 = glMatrix.vec3.fromValues(0, 0, 0);

            this.m_triangles = new Vertexes();

            const SIZE = 200.0;
            const STEP = 1;
            let normal;
            const MAP_HEIGHT = 50.0;
            const MAP_SCALE = 0.025;

            const grid = [];
            if (this.once)
            for (let x = -SIZE; x < SIZE; x+=STEP) {
                const col = [];
                for (let y = -SIZE; y < SIZE; y+=STEP)
                {
                    col[y] = Math.sin(y * MAP_SCALE) * Math.cos(x * MAP_SCALE) * MAP_HEIGHT;
                }
                grid[x] = col;
            }

            // if (this.once) {
            //     //console.log(grid);
            //     for (let x = -SIZE; x < SIZE; x++) {
            //         console.log(grid[x][0]);
            //     }
            // }

            function cubicInterpolate(p0, p1, p2, p3, t) {
                return p1 + 0.5 * t * (p2 - p0 + t * (2.0 * p0 - 5.0 * p1 + 4.0 * p2 - p3 + t * (3.0 * (p1 - p2) + p3 - p0)));
            }

            function bicubicInterpolate(x, y) {
                // 'grid' to dwuwymiarowa tablica przechowująca wartości Z
                // 'x' i 'y' to współrzędne w zakresie od -STEP do STEP (wewnątrz siatki)

                // Wyznaczenie indeksów całkowitych i reszt dla X i Y
                const xInt = Math.floor(x);
                const yInt = Math.floor(y);
                const xFrac = x - xInt;
                const yFrac = y - yInt;

                // Wybieranie 4x4 siatki punktów wokół (x, y)
                let result = [];
                for (let i = -1; i <= 2; i++) {
                    let row = [];
                    for (let j = -1; j <= 2; j++) {
                        const xi = Math.max(-SIZE, Math.min(SIZE - 1, xInt + i));
                        const yi = Math.max(-SIZE, Math.min(SIZE - 1, yInt + j));
                        row.push(grid[xi][yi]);
                    }
                    result.push(row);
                }

                // if (this.once) {
                //     //  console.log(result);
                // }

                // Interpolacja w kierunku X dla każdej kolumny
                let colInterpolations = [];
                for (let i = 0; i < 4; i++) {
                    colInterpolations.push(cubicInterpolate(result[i][0], result[i][1], result[i][2], result[i][3], xFrac));
                }

                // Interpolacja końcowa w kierunku Y
                return cubicInterpolate(colInterpolations[0], colInterpolations[1], colInterpolations[2], colInterpolations[3], yFrac);
            }

            const MICROSTEP = 4;

            if (this.once) {
                this.m_lines = new Vertexes();
             //   console.log(this.m_lines);
            }

            if (this.once) {
                for (let i = -SIZE; i <= SIZE; i += MICROSTEP) {
                    for (let j = -SIZE; j <= SIZE; j += MICROSTEP) {
                        const z1 = bicubicInterpolate(i, j);
                        const z2 = bicubicInterpolate(i + MICROSTEP, j);

                        const p1 = glMatrix.vec3.fromValues(i, j, z1);
                        const p2 = glMatrix.vec3.fromValues(i + (MICROSTEP), j, z2);

                        const direction = glMatrix.vec3.create();
                        glMatrix.vec3.sub(direction, p2, p1);

                        let normal = glMatrix.vec3.create();
                        if (direction.z == 0) {
                            normal = glMatrix.vec3.fromValues(0.0, -1.0, 0.1);
                        } else {
                            normal = glMatrix.vec3.fromValues(-direction[2], 0.0, direction[0]);
                        }
                        glMatrix.vec3.normalize(normal, normal);

                        this.m_lines.push(new VertexData(p1, color1, normal));
                        this.m_lines.push(new VertexData(p2, color1, normal));

                        this.m_lines.push(new VertexData(p1, color1, normal));
                        const p3 = glMatrix.vec3.fromValues(p1[0], p1[1], p1[2]);
                        glMatrix.vec3.add(p3, p1, normal);
                        this.m_lines.push(new VertexData(p3, color1, normal));
                    }
                }
                console.log('Lines: ' + this.m_lines.length);
            }

            if (this.once) {
                //console.log(this.m_lines);
            }

            this.once = false;

            const t0 = performance.now();
            const ra = this.m_lines.toRawArray().concat(this.m_triangles.toRawArray())
            //console.log('Draw time: ' + (performance.now() - t0));
            gl.bufferData(gl.ARRAY_BUFFER,
                new Float32Array(ra), gl.STATIC_DRAW);

//            gl.drawArrays(gl.TRIANGLES, this.m_lines.length, this.m_triangles.length);
            gl.drawArrays(gl.LINES, 0, this.m_lines.length);

            //console.log(new Float32Array(ra), this.m_lines.length, this.m_triangles.length);
        }

        //if (m_vao.isCreated()) m_vao.release(); else m_vbo.release();
    }

    needsUpdateGeometry() {
        return this.m_needsUpdateGeometry;
    }

    visible() {
        return true;
    }
}

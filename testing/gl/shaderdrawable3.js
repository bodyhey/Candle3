
class ShaderDrawable3 {
    m_needsUpdateGeometry = true;
    m_visible = true;
    m_lineWidth = 0.1;
    m_pointSize = 1.0;
    m_globalAlpha = 1.0;
    m_texture = null;
    m_vao;
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
        this.m_vao = gl.createVertexArray();
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

        this.m_lines = new Vertexes();

        for (let i = -SIZE; i < SIZE; i++) {
            this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(i * 10, -SIZE * 10, 0), color1, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
            this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(i * 10, SIZE * 10, 0), color2, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
            this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(-SIZE * 10, i * 10, 0), color1, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
            this.m_lines.push(new VertexData(glMatrix.vec3.fromValues(SIZE * 10, i * 10, 0), color2, glMatrix.vec3.fromValues(NaN, NaN, NaN)));
        }

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
            gl.bufferData(gl.ARRAY_BUFFER, this.m_lines.toRawArray(), gl.STATIC_DRAW);
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

    vertexToScreen(P, mvpMatrix) {
        const P2 = glMatrix.vec3.create();
        glMatrix.vec3.transformMat4(P2, P, mvpMatrix);

        return glMatrix.vec3.fromValues(0, 0, P2[2]);
        return P2;
    }

    pointsCenter(mvpMatrix, points) {
        let center = glMatrix.vec3.create();
        const leng = points.length;
        if (leng < 2) {
            throw new Error('Not enough points');
        }
        glMatrix.vec3.add(center, this.vertexToScreen(points.shift(), mvpMatrix), this.vertexToScreen(points.shift(), mvpMatrix));
        for (const P of points) {
            glMatrix.vec3.add(center, center, this.vertexToScreen(P, mvpMatrix));
        }
        glMatrix.vec3.scale(center, center, 1.0 / leng);
        return center;
    }

    ofs = 0.0;

    draw(eye, mvpMatrix)
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

            // Tell OpenGL programmable pipeline how to locate vertex color data
            // const colorLocation = this.m_program.attributeLocation("a_color");
            // this.m_program.enableAttributeArray(colorLocation);
            // this.m_program.setAttributeBuffer(colorLocation, gl.FLOAT, offset, 3, VERTEX_SIZE);

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

            this.m_lines = new Vertexes();
            this.m_triangles = new Vertexes();

            const SIZE = 150;

            const c1 = glMatrix.vec3.fromValues(1, 0, 0);
            const c2 = glMatrix.vec3.fromValues(0, 1, 0);
            const c3 = glMatrix.vec3.fromValues(0, 0, 1);
            const c4 = glMatrix.vec3.fromValues(1, 1, 0);
            const c5 = glMatrix.vec3.fromValues(1, 0, 1);
            const c6 = glMatrix.vec3.fromValues(0, 1, 1);

            const cubeQuads = [];
            const Z = 25;
            for (let i = -SIZE; i <= SIZE; i+=10) {
                for (let j = -SIZE; j <= SIZE; j+=10) {
                    const j_ = j + this.ofs;
                    const i_ = i + this.ofs;
                    const z0 = Math.sin(j_ * Utils.MAP_SCALE) * Math.cos(i_ * Utils.MAP_SCALE) * Utils.MAP_HEIGHT - Z;
                    const z1 = Math.sin(j_ * Utils.MAP_SCALE) * Math.cos((i_+10) * Utils.MAP_SCALE) * Utils.MAP_HEIGHT - Z;
                    const z2 = Math.sin((j_+10) * Utils.MAP_SCALE) * Math.cos((i_+10) * Utils.MAP_SCALE) * Utils.MAP_HEIGHT - Z;
                    const z3 = Math.sin((j_+10) * Utils.MAP_SCALE) * Math.cos(i_ * Utils.MAP_SCALE) * Utils.MAP_HEIGHT - Z;
                    cubeQuads.push(
                        [
                            [i, j, z0],  [i+10, j, z1],  [i+10, j+10, z2],
                            [i, j, z0],  [i+10, j+10, z2],  [i, j+10, z3],
                        ]
                    );
                    if (j > -SIZE && j < SIZE) {
                        this.m_lines.push(
                            new VertexData(
                                glMatrix.vec3.fromValues(i, j, z0),
                                c1,
                                glMatrix.vec3.fromValues(NaN, NaN, NaN)
                            ),
                            new VertexData(
                                glMatrix.vec3.fromValues(i+10, j, z1),
                                c1,
                                glMatrix.vec3.fromValues(NaN, NaN, NaN)
                            )
                        );                            
                    }
                    if (i > -SIZE && i < SIZE) {
                        this.m_lines.push(
                            new VertexData(
                                glMatrix.vec3.fromValues(i, j, z0),
                                c1,
                                glMatrix.vec3.fromValues(NaN, NaN, NaN)
                            ),
                            new VertexData(
                                glMatrix.vec3.fromValues(i, j+10, z3),
                                c1,
                                glMatrix.vec3.fromValues(NaN, NaN, NaN)
                            )
                        );
                    }
                }                
            }
            this.ofs += 0.8;
            //console.log(cubeQuads.length);

            const cubeTrianglesSorted = new Array(cubeQuads.length * 2);
            let index = 0;
            for (const points of cubeQuads) {
                const center = this.pointsCenter(mvpMatrix, [points[0], points[1], points[2], points[4]]);
                const length = glMatrix.vec3.length(center);
                cubeTrianglesSorted[index++] = [center, length, [points[0], points[1], points[2]]];
                cubeTrianglesSorted[index++] = [center, length, [points[3], points[4], points[5]]];
            }

            cubeTrianglesSorted.sort((a, b) => b[1] - a[1]);

            const max = cubeTrianglesSorted[0][1];
            const min = cubeTrianglesSorted[cubeTrianglesSorted.length - 1][1];

            //console.log([min, max]);

            const colors = [c1, c2, c3, c4, c5, c6];
            const color = c1;

            let ic = 0;
            for (const [center,length,triangle] of cubeTrianglesSorted) {
                ic = (length - min) / (max - min);
                //console.log(triangle);
                for (const vertex of triangle) {
                    this.m_triangles.push(
                        new VertexData(
                            glMatrix.vec3.fromValues(...vertex),
                            glMatrix.vec3.fromValues(
                                ic,
                                ic,
                                ic
                            ),
                            glMatrix.vec3.fromValues(0, 0, 1)
                        )
                    );
                    // this.m_lines.push(
                    //     new VertexData(
                    //         glMatrix.vec3.fromValues(...vertex),
                    //         color,
                    //         glMatrix.vec3.fromValues(NaN, NaN, NaN)
                    //     ),
                    //     1
                    // );
                }
                // if (ic < 1) {
                //     ic+=0.015;
                // }
            }

            this.m_lines.push(
                new VertexData(
                    glMatrix.vec3.fromValues(0, 0, 0),
                    c1,
                    glMatrix.vec3.fromValues(0, 0, 1)
                ),
                new VertexData(
                    glMatrix.vec3.fromValues(0, 0, 100),
                    c1,
                    glMatrix.vec3.fromValues(0, 0, 1)
                )
            );

            gl.bufferData(gl.ARRAY_BUFFER,
                new Float32Array(this.m_lines.toRawArray().concat(this.m_triangles.toRawArray())), gl.STATIC_DRAW);
            gl.drawArrays(gl.TRIANGLES, this.m_lines.length, this.m_triangles.length);
            gl.drawArrays(gl.LINES, 0, this.m_lines.length);
        }
    }

    needsUpdateGeometry() {
        return this.m_needsUpdateGeometry;
    }

    visible() {
        return true;
    }
}

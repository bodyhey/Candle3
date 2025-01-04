class ShaderProgram {
    m_vertexShader;
    m_fragmentShader;
    m_program;

    program() {
        return this.m_program;
    }

    constructor(vss, fss) {
        this.m_program = gl.createProgram();

        this.m_vertexShader = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(this.m_vertexShader, vss);
        gl.compileShader(this.m_vertexShader);

        if (!gl.getShaderParameter(this.m_vertexShader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(this.m_vertexShader));
            return null;
        }

        // now compile the fragment shader
        this.m_fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(this.m_fragmentShader, fss);
        gl.compileShader(this.m_fragmentShader);

        if (!gl.getShaderParameter(this.m_fragmentShader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(this.m_fragmentShader));
            return null;
        }

        console.log("ShaderProgram created");

        gl.attachShader(this.m_program, this.m_fragmentShader);
        gl.attachShader(this.m_program, this.m_vertexShader);
        gl.linkProgram(this.m_program);

        if (!gl.getProgramParameter(this.m_program, gl.LINK_STATUS)) {
            alert("Could not initialise shaders");
        }
    }

    bind() {
        //console.log("ShaderProgram bind");
        gl.useProgram(this.m_program);
    }

    setUniformValueMatrix(name, matrix) {
        const location = gl.getUniformLocation(this.m_program, name);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }
        gl.uniformMatrix4fv(location, false, matrix.values().toArray());
        gl.uniform
    }

    setUniformValueMatrix3(name, matrix) {
        const location = gl.getUniformLocation(this.m_program, name);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }
        gl.uniformMatrix3fv(location, false, matrix.values().toArray());
        gl.uniform
    }

    setUniformValueVec3(name, vec3) {
        const location = gl.getUniformLocation(this.m_program, name);
        //console.log(name, location);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }
        gl.uniform3fv(location, vec3);
    }

    release() {
        gl.useProgram(null);
    }

    attributeLocation(name) {
        const loc = gl.getAttribLocation(this.m_program, name);
        if (loc == -1 || loc == null) {
            throw "Could not find " + name;
        }

        return loc;
    }

    uniformLocation(name) {
        const loc = gl.getUniformLocation(this.m_program, name);
        if (loc == -1 || loc == null) {
            throw "Could not find " + name;
        }

        return loc;
    }

    enableAttributeArray(location) {
        gl.enableVertexAttribArray(location);
    }
//\/setAttributeBuffer(color, gl.gl.FLOAT, offset, 3, VertexData.sizeof());

    setAttributeBuffer(location, type, offset, size, stride) {
        //console.log("setAttributeBuffer", location, type, offset, size, stride);
        gl.vertexAttribPointer(location, size, type, false, stride, offset);
    }

    setAttributeValue(name, value) {
        const location = gl.getAttribLocation(this.m_program, name);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }
        gl.vertexAttrib1f(location, value);
    }

    setUniformValue(name, value) {
        const location = gl.getUniformLocation(this.m_program, name);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }

        gl.uniform1f(location, value);
    }

    setUniformValuei(name, value) {
        const location = gl.getUniformLocation(this.m_program, name);
        if (location == -1 || location == null) {
            throw "Could not find " + name;
        }

        gl.uniform1i(location, value);
    }
}

class ShaderProgram2d extends ShaderProgram {
    m_vbo = gl.createBuffer();

    constructor() {
        super(vertexShaderSource2d, fragmentShaderSource2d);
    }

    setGcodeBgMode(value) {
        this.setUniformValue("u_gcodeBgMode", value);
    }

    bind() {
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_vbo);
        super.bind();

        let offset = 0;
        const vertexLocation = this.attributeLocation("a_position");
        this.enableAttributeArray(vertexLocation);
        this.setAttributeBuffer(vertexLocation, gl.FLOAT, offset, 2, 2 * 4 * 2);

        offset += Utils.VECTOR2D_SIZE;

        const textureLocation = this.attributeLocation("a_texcoord");
        this.enableAttributeArray(textureLocation);
        this.setAttributeBuffer(textureLocation, gl.FLOAT, offset, 2, 2 * 4 * 2);

        //gl.depthMask(gl.FALSE);
    }

    release() {
        super.release();

        gl.bindBuffer(gl.ARRAY_BUFFER, null);

        //gl.depthMask(gl.TRUE);
    }
}

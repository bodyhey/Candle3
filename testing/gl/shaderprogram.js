class ShaderProgram {
    m_vertexShader;
    m_fragmentShader;
    m_program;

    constructor() {
        this.m_program = gl.createProgram();

        this.m_vertexShader = gl.createShader(gl.VERTEX_SHADER);
        gl.shaderSource(this.m_vertexShader, vertexShaderSource);
        gl.compileShader(this.m_vertexShader);

        if (!gl.getShaderParameter(this.m_vertexShader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(this.m_vertexShader));
            return null;
        }

        // now compile the fragment shader
        this.m_fragmentShader = gl.createShader(gl.FRAGMENT_SHADER);
        gl.shaderSource(this.m_fragmentShader, fragmentShaderSource);
        gl.compileShader(this.m_fragmentShader);

        if (!gl.getShaderParameter(this.m_fragmentShader, gl.COMPILE_STATUS)) {
            console.error(gl.getShaderInfoLog(this.m_fragmentShader));
            return null;
        }

        //console.log("ShaderProgram created");

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
        if (location == -1) {
            throw "Could not find " + name;
        }
        gl.uniformMatrix4fv(location, false, matrix.values().toArray());
        gl.uniform
    }

    setUniformValueVec3(name, vec3) {
        const location = gl.getUniformLocation(this.m_program, name);
        if (location == -1) {
            throw "Could not find " + name;
        }
        gl.uniform3fv(location, vec3);
    }

    release() {
    }

    attributeLocation(name) {
        const loc = gl.getAttribLocation(this.m_program, name);
        if (loc == -1) {
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
        if (location == -1) {
            throw "Could not find " + name;
        }
        gl.vertexAttrib1f(location, value);
    }
}

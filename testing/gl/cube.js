class Cube {
    constructor() {
        this.m_eye = glMatrix.vec3.create();
        this.m_lookAt = glMatrix.vec3.create();
        this.m_viewMatrix = glMatrix.mat4.create();
        this.m_projectionMatrix = glMatrix.mat4.create();
        this.lightRot = 0.2;
        this.m_zoomDistance = Utils.DEFAULT_ZOOM;
        this.m_fov = 36.0;
        this.m_near = 1.5;
        this.m_far = 1000;
        this.m_perspective = false;
        this.m_xRot = 0.0;
        this.m_yRot = 0.0;
        this.m_fbo = gl.createFramebuffer();
        this.m_vbo = gl.createBuffer();
        gl.createRenderbuffer();
        this.renderTexture = this.createTexture(512, 512);
        this.m_program = new ShaderProgram(vertexShaderSource1, fragmentShaderSource1);
        this.draw();
    }
    createTexture(width, height) {
        var texture = gl.createTexture();
        gl.bindTexture(gl.TEXTURE_2D, texture);
        gl.texImage2D(
        // Always gl.TEXTURE_2D for a 2D texture.
        gl.TEXTURE_2D, 
        // Mipmap level.  Always 0.
        0, 
        // Internal format of each pixel.  Here we want an RGBA texture.
        gl.RGBA, 
        // Width of the texture.
        width, 
        // Height of the texture.
        height, 
        // Width of the border of the texture.  Always 0.
        0, 
        // The pixel format of the data that is going to be uploaded to the GPU.
        // We have no data here, so use something that matches the internal format.
        gl.RGBA, 
        // The type of each component of the pixel that is going to be uploaded.
        // Here we want a floating point texture.
        gl.FLOAT, 
        // The data that is going to be uploaded.
        // We don't have any data, so we give null.
        // WebGL will just allocate the texture and leave it blank.
        null);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
        gl.bindTexture(gl.TEXTURE_2D, null);
        return texture;
    }
    width() {
        return canvas.width;
    }
    height() {
        return canvas.height;
    }
    updateProjection() {
        // Reset projection
        //this.m_projectionMatrix.setToIdentity();
        glMatrix.mat4.identity(this.m_projectionMatrix);
        // const aspectRatio = this.width() / this.height();
        // const orthoSize = this.m_zoomDistance * Math.tan((this.m_fov * 0.0174533) / 2.0);
        // // perspective / orthographic projection
        // if (this.m_perspective)
        //     //perspective(out, fovy, aspect, near, far) {
        //     glMatrix.mat4.perspective(this.m_projectionMatrix, this.m_fov, aspectRatio, this.m_near, this.m_far);
        // else
        //     //export function ortho(out, left, right, bottom, top, near, far) {
        //     glMatrix.mat4.ortho(this.m_projectionMatrix, -orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, -this.m_far/2.0, this.m_far/2.0);
        glMatrix.mat4.ortho(this.m_projectionMatrix, -10, 10, -10, 10, 1, 70);
    }
    updateView() {
        // Set view matrix
        // this.m_viewMatrix.setToIdentity();
        glMatrix.mat4.identity(this.m_viewMatrix);
        // console.log(this.m_viewMatrix);
        const angY = Utils.toRadians(Math.random() * 360);
        const angX = Utils.toRadians(0);
        //onsole.log(this.m_xRot, this.m_yRot, angX, angY);
        //console.log('ang', angX, angY, Math.cos(angX));
        const eye = glMatrix.vec3.fromValues(0, 0, -10);
        const up = glMatrix.vec3.fromValues(0, 1, 0
        // Math.abs(this.m_xRot) == 90 ? -Math.sin(angY + (this.m_xRot < 0 ? Utils.M_PI : 0)) : 0,
        // Math.cos(angX),
        // Math.abs(this.m_xRot) == 90 ? -Math.cos(angY + (this.m_xRot < 0 ? Utils.M_PI : 0)) : 0
        );
        // zamiast `eye * m_zoomDistance`
        const eye2 = glMatrix.vec3.create();
        //        glMatrix.vec3.multiply(eye, eye, glMatrix.vec3.fromValues(this.m_zoomDistance, this.m_zoomDistance, this.m_zoomDistance));
        glMatrix.mat4.lookAt(this.m_viewMatrix, [1, 0, 50], [0, 0, 0], [0, 1, 0]);
        //this.m_viewMatrix.lookAt(eye * m_zoomDistance, QVector3D(0,0,0), up.normalized());
        //glMatrix.mat4.rotate(this.m_viewMatrix, this.m_viewMatrix, Utils.toRadians(-90), glMatrix.vec3.fromValues(1.0, 0.0, 0.0));
        // this.m_viewMatrix.rotate(-90, 1.0, 0.0, 0.0);
        // const lookAt = glMatrix.vec3.create();
        // glMatrix.vec3.multiply(lookAt, this.m_lookAt, glMatrix.vec3.fromValues(-1, -1, -1));
        //glMatrix.mat4.translate(this.m_viewMatrix, this.m_viewMatrix, lookAt);
        this.m_eye = eye;
    }
    drawCube() {
        let offset = 0;
        this.m_program.bind();
        this.updateProjection();
        this.updateView();
        //
        const mvpMatrix = glMatrix.mat4.create();
        glMatrix.mat4.multiply(mvpMatrix, this.m_projectionMatrix, this.m_viewMatrix);
        // Set modelview-projection matrix
        this.m_program.setUniformValueMatrix("mvp_matrix", mvpMatrix);
        this.m_program.setUniformValueMatrix("mv_matrix", this.m_viewMatrix);
        this.m_program.setUniformValueVec3("eye", this.m_eye);
        this.m_program.setUniformValueVec3("u_light_color", glMatrix.vec3.fromValues(0.9, 0.6, 0.9));
        this.m_program.setUniformValueVec3("u_object_color", glMatrix.vec3.fromValues(1.0, 1.0, 1.0));
        const light_pos = glMatrix.vec3.fromValues(1110, 1111, 11);
        //console.log(light_pos);
        const light_matrix = glMatrix.mat4.create();
        glMatrix.mat4.identity(light_matrix);
        glMatrix.mat4.rotateZ(light_matrix, light_matrix, this.lightRot);
        glMatrix.vec3.transformMat4(light_pos, light_pos, light_matrix);
        this.m_program.setUniformValueVec3("u_light_position", light_pos);
        //
        gl.bindBuffer(gl.ARRAY_BUFFER, this.m_vbo);
        // Tell OpenGL programmable pipeline how to locate vertex position data
        const vertexLocation = this.m_program.attributeLocation("a_position");
        this.m_program.enableAttributeArray(vertexLocation);
        this.m_program.setAttributeBuffer(vertexLocation, gl.FLOAT, offset, 3, Utils.VERTEX_SIZE);
        // Offset for color
        offset += Utils.VECTOR3D_SIZE;
        // Tell OpenGL programmable pipeline how to locate vertex color data
        const colorLocation = this.m_program.attributeLocation("a_color");
        this.m_program.enableAttributeArray(colorLocation);
        this.m_program.setAttributeBuffer(colorLocation, gl.FLOAT, offset, 3, Utils.VERTEX_SIZE);
        // Offset for line start point
        offset += Utils.VECTOR3D_SIZE;
        // Tell OpenGL programmable pipeline how to locate vertex line start point
        const startLocation = this.m_program.attributeLocation("a_normal");
        this.m_program.enableAttributeArray(startLocation);
        this.m_program.setAttributeBuffer(startLocation, gl.FLOAT, offset, 3, Utils.VERTEX_SIZE);
        gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(cube), gl.STATIC_DRAW);
        // gl.clearColor(0, 1, 1, 0.8);
        // gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
        gl.drawArrays(gl.LINES, 0, cube.length / 9);
        this.m_program.release();
        gl.flush();
    }
    draw() {
        gl.bindFramebuffer(gl.FRAMEBUFFER, this.m_fbo);
        gl.viewport(0, 0, 512, 512);
        // // Step 2: Attach the texture to the FBO.
        gl.framebufferTexture2D(
        // First argument is always gl.FRAMEBUFFER
        gl.FRAMEBUFFER, 
        // The second argument indicates the "attachment slot" of the FBO.
        // Basically, it indicates the function of the texture that you are attaching.
        // gl.COLOR_ATTACHMENT0 means that the texture will serve as the zeroth color buffer.
        // By default, this is the only color buffer attachment slot of that you can use in vanially WebGL.
        // To use other color buffer attachment slot, you need an extension.  (We will cover this later.)
        gl.COLOR_ATTACHMENT0, 
        // The third argument indicates the kind of texture we are attaching.
        // Since we are attaching a TEXTURE_2D, we give it gl.TEXTURE_2D
        gl.TEXTURE_2D, 
        // The fourth argument is the texture that you want to attach.
        // Of course, this must be created before hand.
        this.renderTexture, 
        // The fifth argument is the mipmap level of the texture.
        // This is always 0.
        0);
        // Step 3: Do your rendering as usual.
        {
            gl.clearColor(0, 0, 1, 0.8);
            gl.clear(gl.COLOR_BUFFER_BIT);
            this.drawCube();
            // Step 4: Flush the buffer just to be sure everything is rendered to the texture.
            gl.flush();
        }
        // Step 4: Detach the texture.
        // Use the same arguments as the command in Step 2, but now the texture is null.
        gl.framebufferTexture2D(gl.FRAMEBUFFER, gl.COLOR_ATTACHMENT0, gl.TEXTURE_2D, null, // null texture means we are detaching.
        0);
        // Step 5: Unbind the FBO.
        gl.bindFramebuffer(gl.FRAMEBUFFER, null);
    }
}


class GLWidget {
    m_zBuffer = true;
    m_antialiasing = false;
    m_msaa = false;
    m_colorBackground = new QColor(1, 1, 1);
    m_shaderProgram1 = new ShaderProgram(vertexShaderSource1, fragmentShaderSource1);
    m_shaderProgram2 = new ShaderProgram(vertexShaderSource2, fragmentShaderSource2);
    m_shaderDrawables = [
        new ShaderDrawable1(this.m_shaderProgram1),
        //new ShaderDrawable2(this.m_shaderProgram2),
        new ShaderDrawable3(this.m_shaderProgram2),
    ];
    m_projectionMatrix = glMatrix.mat4.create();// new QMatrix4x4();
    m_viewMatrix = glMatrix.mat4.create();// new QMatrix4x4();
    m_xRot;
    m_yRot;
    // m_xRotTarget;
    // m_yRotTarget;
    m_zoomDistance;
    m_lastPos = {x: 0, y: 0};
    m_lookAt = glMatrix.vec3.create(0, 0, 0);
    m_eye = glMatrix.vec3.create();
    m_eye2 = glMatrix.vec3.create();
    m_perspective = false;
    m_fov = 30; m_near = 0.5; m_far = 17000.0;
    m_xMin = 0; m_xMax = 0;
    m_yMin = 0; m_yMax = 0;
    m_zMin = 0; m_zMax = 0;
    m_xSize = 0; m_ySize = 0; m_zSize = 0;

    constructor() {
        this.m_xRot = this.m_xRotTarget = 10;
        this.m_yRot = this.m_yRotTarget = 0;

        this.m_zoomDistance = DEFAULT_ZOOM;

        this.updateProjection();
        this.updateView();

        setTimeout(() => {
            this.setIsometricView();
        }, 100);
    }

    rotate() {
        //this.m_xRot++;
        //this.m_yRot+=0.2;
        this.updateView();
        this.updateProjection();
    }

    width() {
        return canvas.width;
    }

    height() {
        return canvas.height;
    }

    updateProjection()
    {
        // Reset projection
        //this.m_projectionMatrix.setToIdentity();
        glMatrix.mat4.identity(this.m_projectionMatrix);

        const aspectRatio = this.width() / this.height();
        const orthoSize = this.m_zoomDistance * Math.tan((this.m_fov * 0.0174533) / 2.0);

        // perspective / orthographic projection
        if (this.m_perspective)
            //perspective(out, fovy, aspect, near, far) {
            glMatrix.mat4.perspective(this.m_projectionMatrix, this.m_fov, aspectRatio, this.m_near, this.m_far);
        else
            //export function ortho(out, left, right, bottom, top, near, far) {
            glMatrix.mat4.ortho(this.m_projectionMatrix, -orthoSize * aspectRatio, orthoSize * aspectRatio, -orthoSize, orthoSize, -this.m_far/2.0, this.m_far/2.0);

        //const m = glMatrix.mat4.create();
//        glMatrix.mat4.invert(m, this.m_projectionMatrix);
        //glMatrix.mat4.identity(m);
        //glMatrix.vec3.transformMat4(this.m_eye2, this.m_eye, m);
        this.m_eye2 = this.m_eye;
    }

    updateView()
    {
        // Set view matrix
        // this.m_viewMatrix.setToIdentity();
        glMatrix.mat4.identity(this.m_viewMatrix);
        // console.log(this.m_viewMatrix);

        const angY = toRadians(this.m_yRot);
        const angX = toRadians(this.m_xRot);
        //onsole.log(this.m_xRot, this.m_yRot, angX, angY);
        //console.log('ang', angX, angY, Math.cos(angX));

        const eye = glMatrix.vec3.fromValues(Math.cos(angX) * Math.sin(angY), Math.sin(angX), Math.cos(angX) * Math.cos(angY));
        const up = glMatrix.vec3.fromValues(
            Math.abs(this.m_xRot) == 90 ? -Math.sin(angY + (this.m_xRot < 0 ? M_PI : 0)) : 0,
            Math.cos(angX),
            Math.abs(this.m_xRot) == 90 ? -Math.cos(angY + (this.m_xRot < 0 ? M_PI : 0)) : 0
        );

        // zamiast `eye * m_zoomDistance`
        const eye2 = glMatrix.vec3.create();
        glMatrix.vec3.multiply(eye, eye, glMatrix.vec3.fromValues(this.m_zoomDistance, this.m_zoomDistance, this.m_zoomDistance));

        glMatrix.mat4.lookAt(this.m_viewMatrix, eye, glMatrix.vec3.create(), glMatrix.vec3.normalize(up, up));
        //this.m_viewMatrix.lookAt(eye * m_zoomDistance, QVector3D(0,0,0), up.normalized());
        glMatrix.mat4.rotate(this.m_viewMatrix, this.m_viewMatrix, toRadians(-90), glMatrix.vec3.fromValues(1.0, 0.0, 0.0));
        // this.m_viewMatrix.rotate(-90, 1.0, 0.0, 0.0);
        const lookAt = glMatrix.vec3.create();
        glMatrix.vec3.multiply(lookAt, this.m_lookAt, glMatrix.vec3.fromValues(-1, -1, -1));
        glMatrix.mat4.translate(this.m_viewMatrix, this.m_viewMatrix, lookAt);

        this.m_eye = eye;
    }

    fitDrawable(drawable)
    {
        // stopViewAnimation();

        this.m_zoomDistance = DEFAULT_ZOOM;

        if (drawable != null) {
            this.updateExtremes(drawable);

            const largestSize = Math.max(Math.max(this.m_xSize, this.m_ySize), this.m_zSize);

            const newZoom = largestSize / (MAGIC_ZOOM_MULTIPLIER * Math.tan((this.m_fov * ONE_DEG_IN_RAD) / 2.0));
            this.m_zoomDistance = newZoom > 0 ? Math.max(newZoom, MIN_ZOOM) : DEFAULT_ZOOM;

            this.m_lookAt = glMatrix.vec3.fromValues(
                this.m_xSize / 2 + this.m_xMin,
                this.m_ySize / 2 + this.m_yMin,
                this.m_zSize / 2 + this.m_zMin
            );
        } else {
            this.m_lookAt = glMatrix.vec3.create();

            this.m_xMin = 0;
            this.m_xMax = 0;
            this.m_yMin = 0;
            this.m_yMax = 0;
            this.m_zMin = 0;
            this.m_zMax = 0;

            this.m_xSize = 0;
            this.m_ySize = 0;
            this.m_zSize = 0;
        }

        this.updateProjection();
        this.updateView();
    }

    setTopView() {
        this.m_xRot = 90;
        this.m_yRot = this.m_yRot > 180 ? 360 : 0;
        this.updateView();
    }

    setFrontView() {
        this.m_xRot = 0;
        this.m_yRot = this.m_yRot > 180 ? 360 : 0;
        this.updateView();
    }

    setLeftView() {
        this.m_xRot = 0;
        this.m_yRot = this.m_yRot > 270 ? 450 : 90;
        this.updateView();
    }

    setIsometricView() {
        this.m_perspective = false;
        this.updateProjection();
        this.m_xRot = 35.264;
        this.m_yRot = this.m_yRot > 180 ? 405 : 45;
        this.updateView();
    }

    fit() {
        this.fitDrawable(this.m_shaderDrawables[0]);
    }

    zoom(event) {
        event.preventDefault();

        if (this.m_zoomDistance > MIN_ZOOM && event.deltaY < 0)
            this.m_zoomDistance /= ZOOMSTEP;
        else if (event.deltaY > 0)
            this.m_zoomDistance *= ZOOMSTEP;

        if (!this.m_perspective) this.updateProjection();
        else this.updateView();
    }

    btn = -1;

    onMouseDown(event) {
        event.preventDefault();

        this.btn = event.button;
        this.m_lastPos = {x: event.clientX, y: event.clientY};
        this.m_xLastRot = this.m_xRot;
        this.m_yLastRot = this.m_yRot;
    }

    normalizeAngle(angle)
    {
        while (angle < 0) angle += 360;
        while (angle > 360) angle -= 360;

        return angle;
    }

    onMouseMove(event) {
        event.preventDefault();

        if (this.btn == -1) return;

        if (this.btn == 0 && !event.ctrlKey) {
            this.m_yRot = this.normalizeAngle(this.m_yLastRot - (event.clientX - this.m_lastPos.x) * 0.5);
            this.m_xRot = this.m_xLastRot + (event.clientY - this.m_lastPos.y) * 0.5;

            if (this.m_xRot < -90) this.m_xRot = -90;
            if (this.m_xRot > 90) this.m_xRot = 90;

            this.updateView();
        }

        if (this.btn == 0 && event.ctrlKey) {
            // Get world to clip
            const mvp = glMatrix.mat4.create();
            glMatrix.mat4.multiply(mvp, this.m_projectionMatrix, this.m_viewMatrix);
            // Get clip to world
            const mvpi = glMatrix.mat4.create();
            glMatrix.mat4.invert(mvpi, mvp);

            const centerVector = glMatrix.vec4.create();
            glMatrix.vec4.transformMat4(centerVector, glMatrix.vec4.fromValues(...this.m_lookAt, 1.0), mvp);

            // Get last mouse XY in clip
            const lastMouseInWorld = glMatrix.vec4.fromValues(
                (this.m_lastPos.x / this.width()) * 2.0 - 1.0,
                -((this.m_lastPos.y / this.height()) * 2.0 - 1.0),
                0,
                1.0
            );

            // Project last mouse pos to world
            // lastMouseInWorld = mvpi * lastMouseInWorld * centerVector.w();
            glMatrix.vec4.multiply(lastMouseInWorld, lastMouseInWorld, glMatrix.vec4.fromValues(centerVector[3], centerVector[3], centerVector[3], centerVector[3]));
            glMatrix.vec4.transformMat4(lastMouseInWorld, lastMouseInWorld, mvp);

            // Get current mouse XY in clip
            const currentMouseInWorld = glMatrix.vec4.fromValues(
                (event.clientX / this.width()) * 2.0 - 1.0,
                -((event.clientY / this.height()) * 2.0 - 1.0),
                0,
                1.0
            );
            // Project current mouse pos to world
            glMatrix.vec4.multiply(currentMouseInWorld, currentMouseInWorld, glMatrix.vec4.fromValues(centerVector[3], centerVector[3], centerVector[3], centerVector[3]));
            glMatrix.vec4.transformMat4(currentMouseInWorld, currentMouseInWorld, mvpi);
            // currentMouseInWorld = mvpi * currentMouseInWorld * centerVector.w();

            // Get difference
            const difference = glMatrix.vec4.create();
            glMatrix.vec4.subtract(difference, currentMouseInWorld, lastMouseInWorld);

            //console.log(difference);

            // Subtract difference from center point
            glMatrix.vec3.subtract(this.m_lookAt, this.m_lookAt, glMatrix.vec3.fromValues(...difference));

            this.m_lastPos = {x: event.clientX, y: event.clientY};

            this.updateView();
        }
    }

    onMouseUp(event) {
        event.preventDefault();

        this.btn = -1;
    }

    updateExtremes(drawable) {
        const minExtremes = drawable.getMinimumExtremes();
        const maxExtremes = drawable.getMaximumExtremes();

        this.m_xMin = !(minExtremes[0]) ? minExtremes[0] : 0;
        this.m_xMax = !qIsNaN(maxExtremes[0]) ? maxExtremes[0] : 0;
        this.m_yMin = !qIsNaN(minExtremes[1]) ? minExtremes[1] : 0;
        this.m_yMax = !qIsNaN(maxExtremes[1]) ? maxExtremes[1] : 0;
        this.m_zMin = !qIsNaN(minExtremes[2]) ? minExtremes[2] : 0;
        this.m_zMax = !qIsNaN(maxExtremes[2]) ? maxExtremes[2] : 0;

        this.m_xSize = this.m_xMax - this.m_xMin;
        this.m_ySize = this.m_yMax - this.m_yMin;
        this.m_zSize = this.m_zMax - this.m_zMin;
    }

    lightRot = 0;

    paintEvent() {
        let vertices = 0;

        // Clear viewport
        gl.clearColor(this.m_colorBackground.redF(), this.m_colorBackground.greenF(), this.m_colorBackground.blueF(), 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
        //gl.alphaTest(gl.GREATER, 0.0);
        // gl.polygonMode(gl.FRONT_AND_BACK, gl.LINE);

        // Shader drawable points
        //gl.enable(gl.PROGRAM_POINT_SIZE);

        // Update settings
        if (this.m_antialiasing) {
            if (this.m_msaa) {
                //gl.enable(gl.MULTISAMPLE);
            } else {
                gl.hint(gl.LINE_SMOOTH_HINT, gl.NICEST);
                gl.enable(gl.LINE_SMOOTH);
                gl.hint(gl.POINT_SMOOTH_HINT, gl.NICEST);
                gl.enable(gl.POINT_SMOOTH);

                gl.enable(gl.BLEND);
            }
        }
        gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
        gl.enable(gl.BLEND);
        //        if (this.m_zBuffer)
        const bindEverything = program => {
            program.bind();
            //console.log(this.m_projectionMatrix, this.m_viewMatrix);

            // zamiast `m_projectionMatrix * m_viewMatrix`
            const mvpMatrix = glMatrix.mat4.create();
            glMatrix.mat4.multiply(mvpMatrix, this.m_projectionMatrix, this.m_viewMatrix);

            // Set modelview-projection matrix
            program.setUniformValueMatrix("mvp_matrix", mvpMatrix);
            program.setUniformValueMatrix("mv_matrix", this.m_viewMatrix);
            program.setUniformValueVec3("eye", this.m_eye);
            program.setUniformValueVec3("u_light_color", glMatrix.vec3.fromValues(0.9, 0.6, 0.9));
            program.setUniformValueVec3("u_object_color", glMatrix.vec3.fromValues(1.0, 1.0, 1.0));

            const light_pos = glMatrix.vec3.fromValues(1110, 1111, 11);
            //console.log(light_pos);
            const light_matrix = glMatrix.mat4.create();
            glMatrix.mat4.identity(light_matrix);
            glMatrix.mat4.rotateZ(light_matrix, light_matrix, this.lightRot);
            glMatrix.vec3.transformMat4(light_pos, light_pos, light_matrix);
            program.setUniformValueVec3("u_light_position", light_pos);
            //console.log(light_pos);

            program.release();

            return [mvpMatrix]
        }
        const [mvpMatrix] = bindEverything(this.m_shaderProgram1);
        bindEverything(this.m_shaderProgram2);

        // Update geometries in current opengl context
        for (const drawable of this.m_shaderDrawables) {
            if (drawable.needsUpdateGeometry()) drawable.updateGeometry();
        }

        // Draw geometries
        // for (const drawable of this.m_shaderDrawables) {
        //     drawable.draw(this.m_eye2);
        //     if (drawable.visible()) vertices += drawable.getVertexCount();
        // }
        gl.depthMask(true);
        gl.enable(gl.DEPTH_TEST);
//        this.m_shaderDrawables[0].draw(this.m_eye2, mvpMatrix);
        gl.depthMask(false);
        // gl.blendFunc(gl.SRC_ALPHA, gl.SRC_ALPHA);
        gl.blendFunc(gl.DST_COLOR, gl.ZERO);
        gl.enable(gl.DEPTH_TEST);
        this.m_shaderDrawables[1].draw(this.m_eye2, mvpMatrix);
        gl.depthMask(true);
        gl.enable(gl.DEPTH_TEST);

        this.lightRot += 0.01;

        // Draw 2D
        gl.disable(gl.DEPTH_TEST);
        //gl.disable(gl.MULTISAMPLE);
        //gl.disable(gl.LINE_SMOOTH);
        gl.disable(gl.BLEND);
    }

}

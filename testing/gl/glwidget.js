
class GLWidget {
    m_zBuffer = true;
    m_antialiasing = false;
    m_msaa = false;
    m_colorBackground = new QColor(1, 1, 1);
    m_shaderProgram = new ShaderProgram();
    m_shaderDrawables = [
        new ShaderDrawable()
    ];
    m_projectionMatrix = glMatrix.mat4.create();// new QMatrix4x4();
    m_viewMatrix = glMatrix.mat4.create();// new QMatrix4x4();
    m_xRot;
    m_yRot;
    m_xRotTarget;
    m_yRotTarget;
    m_zoomDistance;
    m_lookAt = glMatrix.vec3.create(0, 0, 0);
    m_eye = glMatrix.vec3.create();
    m_perspective = false;
    m_fov = 30; m_near = 0.5; m_far = 5000.0;
    m_xMin = 0; m_xMax = 0;
    m_yMin = 0; m_yMax = 0;
    m_zMin = 0; m_zMax = 0;
    m_xSize = 0; m_ySize = 0; m_zSize = 0;

    constructor() {
        this.m_xRot = this.m_xRotTarget = 35;
        this.m_yRot = this.m_yRotTarget = 0;

        this.m_zoomDistance = DEFAULT_ZOOM;

        this.updateProjection();
        this.updateView();
    }

    rotate() {
        //this.m_xRot++;
        this.m_yRot+=0.2;
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
        //const eye2 = glMatrix.vec3.create();
        glMatrix.vec3.multiply(eye, eye, glMatrix.vec3.fromValues(this.m_zoomDistance, this.m_zoomDistance, this.m_zoomDistance));

        glMatrix.mat4.lookAt(this.m_viewMatrix, eye, glMatrix.vec3.create(), glMatrix.vec3.normalize(up, up));
        //this.m_viewMatrix.lookAt(eye * m_zoomDistance, QVector3D(0,0,0), up.normalized());
        glMatrix.mat4.rotate(this.m_viewMatrix, this.m_viewMatrix, toRadians(-90), glMatrix.vec3.fromValues(1.0, 0.0, 0.0));
        // this.m_viewMatrix.rotate(-90, 1.0, 0.0, 0.0);
        glMatrix.mat4.translate(this.m_viewMatrix, this.m_viewMatrix, this.m_lookAt);
        // this.m_viewMatrix.translate(-m_lookAt);

        //console.log('vm', this.m_viewMatrix);
//        const eye2 = glMatrix.vec3.create();
        this.m_eye = eye;
        //console.log('eye', up);
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

    paintEvent() {
        let vertices = 0;

        // Clear viewport
        gl.clearColor(this.m_colorBackground.redF(), this.m_colorBackground.greenF(), this.m_colorBackground.blueF(), 1.0);
        gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

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

                gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
                gl.enable(gl.BLEND);
            }
        }
        if (this.m_zBuffer) gl.enable(gl.DEPTH_TEST);

        if (this.m_shaderProgram) {
            // Draw 3d
            this.m_shaderProgram.bind();

            //console.log(this.m_projectionMatrix, this.m_viewMatrix);

            // zamiast `m_projectionMatrix * m_viewMatrix`
            const mvpMatrix = glMatrix.mat4.create();
            glMatrix.mat4.multiply(mvpMatrix, this.m_projectionMatrix, this.m_viewMatrix);

            // Set modelview-projection matrix
            this.m_shaderProgram.setUniformValueMatrix("mvp_matrix", mvpMatrix);
            this.m_shaderProgram.setUniformValueMatrix("mv_matrix", this.m_viewMatrix);
            this.m_shaderProgram.setUniformValueVec3("eye", this.m_eye);

            // Update geometries in current opengl context
            for (const drawable of this.m_shaderDrawables)
                if (drawable.needsUpdateGeometry()) drawable.updateGeometry(this.m_shaderProgram);

            // Draw geometries
            for (const drawable of this.m_shaderDrawables) {
                drawable.draw(this.m_shaderProgram);
                //console.log(this.m_eye);
                if (drawable.visible()) vertices += drawable.getVertexCount();
            }

            this.m_shaderProgram.release();
        }

        // Draw 2D
        gl.disable(gl.DEPTH_TEST);
        //gl.disable(gl.MULTISAMPLE);
        //gl.disable(gl.LINE_SMOOTH);
        gl.disable(gl.BLEND);
    }

}

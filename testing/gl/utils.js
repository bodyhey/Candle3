
class Utils {
    static toRadians(degrees) {
        return glMatrix.glMatrix.toRadian(degrees);
    }

    static qIsNaN(value) {
        return isNaN(value);
    }

    static ZOOMSTEP = 1.1;
    static DEFAULT_ZOOM = 1300.0;
    static MIN_ZOOM = 0.2;
    static ONE_DEG_IN_RAD = 0.0174533;
    static MAGIC_ZOOM_MULTIPLIER = 1.9;
    static M_PI = Math.PI;
    static GL_FLOAT_SIZE = 4;
    static VECTOR3D_SIZE = 3 * 4;
    static VECTOR2D_SIZE = 2 * 4;
    static VERTEX_SIZE = 3 * 3 * 4;
}
const ZOOMSTEP = 1.1;
const DEFAULT_ZOOM = 1300.0;
const MIN_ZOOM = 0.2;
const ONE_DEG_IN_RAD = 0.0174533;
const MAGIC_ZOOM_MULTIPLIER = 1.9;
const M_PI = Math.PI;

const GL_FLOAT_SIZE = 4;
const VECTOR3D_SIZE = 3 * GL_FLOAT_SIZE;
const VERTEX_SIZE = 3 * VECTOR3D_SIZE;

window.toRadians = glMatrix.glMatrix.toRadian;

function qIsNaN(value) {
    return isNaN(value);
}

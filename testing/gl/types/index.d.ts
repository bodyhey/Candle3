import type * as glMatrixType from 'gl-matrix';

export declare global {
    declare var glMatrix: {
        vec3: typeof glMatrixType.vec3;
        vec4: typeof glMatrixType.vec4;
        mat3: typeof glMatrixType.mat3;
        mat4: typeof glMatrixType.mat4;
    }

    declare class ShaderProgram {
        constructor(vertexShaderSource: string, fragmentShaderSource: string);
        bind(): void;
        release(): void;
        setUniformValueMatrix(name: string, matrix: glMatrixType.mat4): void;
        setUniformValueMatrix3(name: string, matrix: glMatrixType.mat3): void;
        setUniformValueVec3(name: string, vec: any): void;
        setUniformValue(name: string, value: any): void;
        setUniformValuei(name: string, value: any): void;
        attributeLocation(name: string): number;
        uniformLocation(name: string): number;
        enableAttributeArray(location: number): void;
        setAttributeBuffer(location: number, type: number, offset: number, size: number, stride: number): void;
    }
    declare class ShaderProgram2d extends ShaderProgram {
        setGcodeBgMode(mode: bool): void;
    }
    declare interface Cube {
        renderTexture: WebGLTexture | null;
        updateRotation(x: number, y: number, z: number);
    }
    declare class ShaderDrawable1 {
        constructor(shaderProgram: ShaderProgram);
        needsUpdateGeometry(): boolean;
        updateGeometry(): void;
        draw(eye2: any, mvpMatrix: any, lightPosRot: any): void;
        getVertexCount(): any;
    }
    declare class ShaderDrawable2 {
        constructor(shaderProgram: ShaderProgram);
        needsUpdateGeometry(): boolean;
        updateGeometry(): void;
        draw(eye2: any, mvpMatrix: any): void;
        getVertexCount(): any;
    }
    declare class ShaderDrawable3 {
        constructor(shaderProgram: ShaderProgram);
        needsUpdateGeometry(): boolean;
        updateGeometry(): void;
        draw(eye2: any, mvpMatrix: any): void;
        getVertexCount(): any;
    }
    declare class QColor {
        constructor(r: number, g: number, b: number);
        redF(): number;
        greenF(): number;
        blueF(): number;
    }
    declare class Utils {
        static DEFAULT_ZOOM: number;
        static MIN_ZOOM: number;
        static ZOOMSTEP: number;
        static ONE_DEG_IN_RAD: number;
        static MAGIC_ZOOM_MULTIPLIER: number;
        static M_PI: number;
        static VERTEX_SIZE: number;
        static VECTOR3D_SIZE: number;
        static VECTOR2D_SIZE: number;
        static GL_FLOAT_SIZE: number;
        static SIZE: number;
        static qIsNaN(value: number): boolean;
        static toRadians(degrees: number): number;
        static initBicubicInterpolation(): void;
        static bicubicInterpolate(x: number, y: number): number;
    }
}

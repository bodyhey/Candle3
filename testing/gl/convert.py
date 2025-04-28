import os
import math

def read_file(filename):
    with open(filename, 'r') as f:
        return f.readlines()
    
lines = read_file('Kostka.obj')

vertexes = []
normals = []
textures = []
faces = []
index = {}
mtls = []

def to_int(x):
    return int(x)

def to_int_minus_one(x):
    return int(x) - 1

def to_float(x):
    return float(x)

mtl = None
for line in lines:
    elements = line.strip().split(" ")
    type = elements.pop(0)
    if type == 'v':
        elements = list(map(to_float, elements))
        vertexes.append(elements)
    elif type == 'usemtl':
        print("usemtl", elements)
        mtl = elements[0]
    elif type == 'vn':
        elements = list(map(to_float, elements))
        normals.append(elements)
    elif type == 'vt':
        elements = list(map(to_float, elements))
        textures.append(elements)
    elif type == 'f':
        # vertex_index/texture_index/normal_index
        face = []
        for element in elements:
            [vi, vti, vni] = list(map(to_int_minus_one, element.split("/")))
            face.append([vi, vni, mtl])
        faces.append(face)

print(vertexes)
print(textures)
print(normals)
print(faces)
print(index)

colors = [
    [1.0, 0.0, 0.0],
    [0.0, 1.0, 0.0],
    [0.0, 0.0, 1.0],
    [1.0, 1.0, 0.0],
    [1.0, 0.0, 1.0],
    [0.0, 1.0, 1.0],
    [1.0, 1.0, 1.0],
    [0.5, 0.5, 0.5],
    [0.5, 0.0, 0.0],
    [0.0, 0.5, 0.0],
    [0.0, 0.0, 0.5],
    [0.5, 0.5, 0.0],
    [0.5, 0.0, 0.5],
    [0.0, 0.5, 0.5],
    [0.5, 0.5, 0.5],
    [0.25, 0.25, 0.25],
    [0.25, 0.0, 0.0],
    [0.0, 0.25, 0.0],
    [0.0, 0.0, 0.25],
    [0.25, 0.25, 0.0],
    [0.25, 0.0, 0.25],
    [0.0, 0.25, 0.25],
    [0.25, 0.25, 0.25],
    [1.0, 0.5, 0.0],
    [1.0, 0.0, 0.5],
    [0.5, 1.0, 0.0],
    [0.5, 0.0, 1.0],
    [0.0, 1.0, 0.5],
    [0.0, 0.5, 1.0],
    [1.0, 0.5, 0.5],
    [1.0, 0.0, 1.0],
    [0.5, 1.0, 0.5],
    [0.5, 0.0, 1.0],
    [0.0, 1.0, 0.5],
    [0.0, 0.5, 1.0],
    [1.0, 0.5, 1.0],
    [1.0, 1.0, 0.5],
    [0.5, 1.0, 1.0],
    [0.5, 1.0, 1.0],
    [0.5, 0.5, 1.0],
    [1.0, 0.5, 1.0],
    [1.0, 1.0, 0.5],
    [0.5, 1.0, 1.0],
    [0.5, 1.0, 1.0],
    [0.5, 0.5, 1.0],
    [1.0, 0.5, 1.0],
]

# usemtl Back
# f 14/73/73 6/74/74 9/75/75
# f 9/75/75 6/74/74 2/76/76
# usemtl Left
# f 11/77/77 3/78/78 18/79/79
# f 18/79/79 3/78/78 5/80/80
# usemtl Bottom
# f 16/81/81 4/82/82 22/83/83
# f 22/83/83 4/82/82 13/84/84
# usemtl Right
# f 23/85/85 15/86/86 21/87/87
# f 21/87/87 15/86/86 8/88/88
# usemtl Front
# f 20/89/89 12/90/90 24/91/91
# f 24/91/91 12/90/90 17/92/92
# usemtl Top

# colors2 = {
#     'Front' : [1.0, 0.5, 0.0],
#     'Back' : [1.0, 0.0, 0.5],
#     'Top' : [0.5, 1.0, 0.0],
#     'Bottom' : [0.5, 0.0, 1.0],
#     'Left' : [0.0, 1.0, 0.5],
#     'Right' : [0.0, 0.5, 1.0],
#     'BackLeft' : [1.0, 0.5, 0.5],
#     'BackRight' : [1.0, 0.0, 1.0],
#     'FrontLeft' : [0.5, 1.0, 0.5],
#     'FrontRight' : [0.5, 0.0, 1.0],
#     'FrontTop' : [0.0, 1.0, 0.5],
#     'FrontBottom' : [0.0, 0.5, 1.0],
#     'BackTop' : [1.0, 0.5, 1.0],
#     'BackBottom' : [1.0, 1.0, 0.5],
#     'LeftTop' : [0.5, 1.0, 1.0],
#     'RightTop' : [0.5, 1.0, 1.0],
#     'LeftBottom' : [0.5, 0.5, 1.0],
#     'RightBottom' : [1.0, 0.5, 1.0],
# }

colors2 = {
    'Default_Generic' : 0,
    'Front' : 1,
    'Back' : 2,
    'Top' : 3,
    'Bottom' : 4,
    'Left' : 5,
    'Right' : 6,
    'Other' : 7,
}    

thd = 1.0 / 9

# textures = {
#     'Default_Generic' : 0,
#     'Front' : 8,
#     'Back' : 7,
#     'Top' : 6,
#     'Bottom' : 5,
#     'Left' : 4,
#     'Right' : 3,
# }

def color_(mtl):
    if mtl == None or mtl == 'Default_Generic':
        return 0 
    else:
        try:
            return colors2[mtl]
        except KeyError:
            return colors2['Other']
        
# def texture_(mtl):
#     if mtl == None or mtl == 'Default_Generic':
#         return 0 
#     else:
#         try:
#             return textures[mtl]
#         except KeyError:
#             return textures['Other']        

def write_vertex(f, vertex, iface):
    [vi, vni, mtl] = vertex
    v = vertexes[vi]
    vn = normals[vni]
    color = color_(mtl)
            
    print("%f, %f, %f, %d, %f, %f, %f," % (v[0], v[1], v[2], color, vn[0], vn[1], vn[2]))
    f.write("%f, %f, %f, %d, %f, %f, %f,\n" % (v[0], v[1], v[2], color, vn[0], vn[1], vn[2]))
    
def write_cpp_vertex(f, vertex, iface):
    [vi, vni, mtl] = vertex
    v = vertexes[vi]
    vn = normals[vni]
    vt = textures[vti]
    color = color_(mtl)
            
    f.write("  VertexData(QVector3D(%f, %f, %f), %d, QVector3D(%f, %f, %f)),\n" % (v[0], v[1], v[2], color, vn[0], vn[1], vn[2]))

def write_lines(iface, face, write_vertex, f):
    write_vertex(f, face[0], iface)
    write_vertex(f, face[1], iface)
    write_vertex(f, face[1], iface)
    write_vertex(f, face[2], iface)
    write_vertex(f, face[2], iface)
    write_vertex(f, face[0], iface)
    
def write_face(iface, face, write_vertex, f):
    write_vertex(f, face[0], iface)
    write_vertex(f, face[1], iface)
    write_vertex(f, face[2], iface)

def write_cpp_face(iface, face, write_vertex, f):
    write_cpp_vertex(f, face[0], iface)
    write_cpp_vertex(f, face[1], iface)
    write_cpp_vertex(f, face[2], iface)

with open("cube_.js", 'w') as f:
    print("const cube = [")
    f.write("const cube = [\n")
    for iface, face in enumerate(faces):
        
        f.write("  // f %d %s\n" % (iface, face[0][2]))
        # write_lines(face, write_vertex, f)
        write_face(math.floor(iface / 2), face, write_vertex, f)
        
    print("];")
    f.write("];\n")

with open("cube2.cpp", 'w') as f2:
    # f2.write("#include \"cubedrawer.h\"\n")
    # f2.write("void CubeDrawer::generateIndex() {\n")
    # f2.write("m_index = QVector<VertexData>{\n")
    # f2.write("\n");
    # f2.write("};\n")
    # f2.write("}\n")
    
    with open("cube.h", 'w') as f:
        f.write("#ifndef CUBE_H\n")
        f.write("#define CUBE_H\n")
        
        f.write("#include \"shaderdrawable.h\"\n")
        f.write("\n");
        f.write("static const QVector<VertexData> cube = {\n")
        vi = 0
        for iface, face in enumerate(faces):
            mtl = face[0][2]
            if (mtl != 'Default_Generic'):
                try:
                    mtl_index = mtls.index(mtl)
                except ValueError:
                    mtls.append(mtl)
                    mtl_index = mtls.index(mtl)
                    index[mtl_index] = []

                index[mtl_index].append(vi)
                index[mtl_index].append(vi+1)
                index[mtl_index].append(vi+2)

            f.write("  // f %d %s\n" % (iface, face[0][2]))
            # write_lines(face, write_vertex, f)
            write_cpp_face(math.floor(iface / 2), face, write_vertex, f)
            vi += 3

        f.write("};\n\n")
        
        f.write("enum class CubeClickableFace {\n");
        f.write("    None = -1,\n")
        for i, mtl in enumerate(mtls):
            f.write("    %s = %d,\n" % (mtl, i))
        f.write("};\n\n")
        
        f.write("static const int clickables[][6] = {\n")
        for i, mtl in enumerate(mtls):
            f.write("    {%s},\n" % ", ".join(map(str, index[i])))
        f.write("};\n\n")
        
        f.write("#endif // CUBE_H\n\n");

print(mtls)
print(index)

import os
import math

def read_file(filename):
    with open(filename, 'r') as f:
        return f.readlines()
    
lines = read_file('Kostka.obj')

vertexes = []
normals = []
faces = []

def to_int(x):
    return int(x)

def to_int_minus_one(x):
    return int(x) - 1

def to_float(x):
    return float(x)

for line in lines:
    elements = line.strip().split(" ")
    type = elements.pop(0)
    if type == 'v':
        elements = list(map(to_float, elements))
        vertexes.append(elements)
    elif type == 'vn':
        elements = list(map(to_float, elements))
        normals.append(elements)
    elif type == 'f':
        # vertex_index/texture_index/normal_index
        face = []
        for element in elements:
            [vi, vti, vni] = list(map(to_int_minus_one, element.split("/")))
            face.append([vi, vni])
        faces.append(face)

print(vertexes)
print(normals)
print(faces)

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

def write_vertex(f, vertex, color):
    [vi, vni] = vertex
    v = vertexes[vi]
    vn = normals[vni]
            
    print("%f, %f, %f, %f, %f, %f, %f, %f, %f," % (v[0], v[1], v[2], color[0], color[1], color[2], vn[0], vn[1], vn[2]))
    f.write("%f, %f, %f, %f, %f, %f, %f, %f, %f,\n" % (v[0], v[1], v[2], color[0], color[1], color[2], vn[0], vn[1], vn[2]))
    
def write_lines(iface, face, write_vertex, f):
    write_vertex(f, face[0], colors[iface])
    write_vertex(f, face[1], colors[iface])
    write_vertex(f, face[1], colors[iface])
    write_vertex(f, face[2], colors[iface])
    write_vertex(f, face[2], colors[iface])
    write_vertex(f, face[0], colors[iface])
    
def write_face(iface, face, write_vertex, f):
    write_vertex(f, face[0], colors[iface])
    write_vertex(f, face[1], colors[iface])
    write_vertex(f, face[2], colors[iface])

with open("cube_.js", 'w') as f:
    print("const cube = [")
    f.write("const cube = [\n")
    for iface, face in enumerate(faces):
        
        f.write("// f %d\n" % iface)
        # write_lines(face, write_vertex, f)
        write_face(math.floor(iface / 2), face, write_vertex, f)
        
    print("];")
    f.write("];\n")

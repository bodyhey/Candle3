import os

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

def new_func(f, vertex):
    [vi, vni] = vertex
    v = vertexes[vi]
    vn = normals[vni]
            
    print("%f, %f, %f, 1.0, 0.0, 0.0, %f, %f, %f," % (v[0], v[1], v[2], vn[0], vn[1], vn[2]))
    f.write("%f, %f, %f, 1.0, 0.0, 0.0, %f, %f, %f,\n" % (v[0], v[1], v[2], vn[0], vn[1], vn[2]))

with open("cube_.js", 'w') as f:
    print("const cube = [")
    f.write("const cube = [\n")
    for iface, face in enumerate(faces):
        f.write("// f %d\n" % iface)
        new_func(f, face[0])
        new_func(f, face[1])
        new_func(f, face[1])
        new_func(f, face[2])
        new_func(f, face[2])
        new_func(f, face[0])
        
    print("];")
    f.write("];\n")

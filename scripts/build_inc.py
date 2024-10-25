import sys
import os

projectPath = sys.argv[1] + "/"
header = projectPath + "build.h"
txt = projectPath + "build.txt"

def incNumber():
    if not os.path.isfile(txt):
        raise Exception("Create initial build version file: " + txt)

    f = open(txt, "r+")

    txtVal = f.readline().strip()
    print(txtVal)
    old = int(txtVal)
    new = old + 1

    f.truncate(0)
    f.seek(0)

    f.write("%d" % new)

    f.close()

    print("%d -> %d" % (old, new))

    return new

def writeHeader(build):
    f = open(header, "w")
    f.write("#define BUILD %d\n" % build)
    f.close()

build = incNumber()
writeHeader(build)

import os
import sys
import shutil

sourcePath = sys.argv[1] + "\\"
destPath = sys.argv[2] + "\\"

def copyFile(sourceSubpath, filename):
    if os.path.isfile(destPath + filename):
        os.remove(destPath + filename)

    shutil.copyfile(sourcePath + sourceSubpath + filename, destPath + filename)

copyFile("src\\candle\\", "GPilot.exe")
copyFile("src\\vendor\\uCNC\\", "uCNC.dll")
copyFile("src\\vendor\\PropertyEditor\\", "PropertyEditor.dll")

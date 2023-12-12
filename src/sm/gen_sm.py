from dataclasses import replace
import os
import shutil
import argparse
        
def copytree(src, dst, symlinks=False, ignore=None):
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            shutil.copytree(s, d, symlinks, ignore)
        else:
            shutil.copy2(s, d)

class SM:
    def __init__(self, tarName, srcName, path, testPath):
        self.tarName = tarName
        self.srcName = srcName
        self.path = path
        self.testPath = testPath

    def changeNameDirectory(self):
        folderNames = [None, 'ie', 'test', 'enc', 'dec']
        for folderName in folderNames:
            path = self.path + "/" + folderName if folderName is not None else self.path
            for directoryName in os.listdir(path):
                if self.srcName in directoryName:
                    os.rename(path + "/" + directoryName, path + "/" + directoryName.replace(self.srcName, self.tarName))

    def modifyFile(self, fileNames, folder=None):
        curPath = self.path + "/" + folder + "/" if folder is not None else self.path + "/"
        for fileName in fileNames:
            with open(curPath + fileName, 'r') as file:
                # Read a list of lines into data
                data = file.readlines()
                for idx, line in enumerate(data):
                    if self.srcName in line:
                        data[idx] = line.replace(self.srcName, self.tarName)
                    if self.srcName.upper() in data[idx]:
                        data[idx] = data[idx].replace(self.srcName.upper(), self.tarName.upper())
            with open(curPath + fileName, 'w') as file:
                file.writelines(data)

    def modifyTestFile(self):
        testFileNames = ['CMakeLists.txt', 'main.c']
        curPath = self.testPath + "/"
        for fileName in testFileNames:
            with open(curPath + fileName, 'r') as file:
                # Read a list of lines into data
                data = file.readlines()
                for idx, line in enumerate(data):
                    if self.srcName in line:
                        data[idx] = line.replace(self.srcName, self.tarName)
                    if self.srcName.upper() in data[idx]:
                        data[idx] = data[idx].replace(self.srcName.upper(), self.tarName.upper())
            with open(curPath + fileName, 'w') as file:
                file.writelines(data)


def main():
    ROOT_DIR = os.path.dirname(os.path.abspath(__file__))

    parser = argparse.ArgumentParser(description='Generate new service model')
    parser.add_argument('--name', type=str, help='name of service model')
    parser.add_argument('--src', type=str, help="source of folder template")
    args = parser.parse_args()

    srcName = os.path.basename(os.path.normpath(args.src)).replace('_sm', '') if args.src is not None else 'rlc'
    tarName = args.name if args.name is not None else "new"

    encFileNames = [srcName + '_enc_generic.h', srcName + '_enc_plain.c', srcName + '_enc_plain.h',
                    srcName + '_enc_fb.h', srcName + '_enc_fb.c', srcName + '_enc_asn.h', 
                    srcName + '_enc_asn.c',]
    decFileNames = [ srcName + '_dec_generic.h', srcName + '_dec_plain.c', srcName + '_dec_plain.h',
                     srcName + '_dec_fb.h', srcName + '_dec_fb.c', srcName + '_dec_asn.h', 
                     srcName + '_dec_asn.c',]
    testFileNames = ['CMakeLists.txt', 'main.c']
    ieFileNames = [ srcName + '_data_ie.c', srcName  + '_data_ie.h']
    mainFileNames = [ srcName + '_sm_agent.c', srcName + '_sm_agent.h', 'CMakeLists.txt',
                srcName + '_sm_id.h', srcName + '_sm_ric.c', srcName + '_sm_ric.h']
    
    path = os.path.join(ROOT_DIR, tarName + "_sm")
    testPath = os.path.join(os.path.dirname(os.path.dirname(ROOT_DIR)) + "/test/sm/", tarName + "_sm")

    if not os.path.isdir(path) and not os.path.isdir(testPath):
        os.mkdir(path=path) # If not exist, create a new directory
        os.mkdir(path=testPath)
        copytree(ROOT_DIR + "/" + srcName + "_sm", path) # Copy the source file to current path
        copytree(os.path.join(os.path.dirname(os.path.dirname(ROOT_DIR))) + "/test/sm/" + srcName + "_sm", testPath)
    
        newSM = SM(tarName=tarName, srcName=srcName, path=path, testPath=testPath)
        newSM.modifyFile(mainFileNames)
        newSM.modifyFile(ieFileNames, 'ie')
        newSM.modifyFile(encFileNames, 'enc')
        newSM.modifyFile(decFileNames, 'dec')
        newSM.modifyFile(testFileNames, 'test')
        newSM.modifyTestFile()
        newSM.changeNameDirectory()

if __name__=="__main__":
    main()

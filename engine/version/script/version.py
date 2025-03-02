"""! @brief Use git ignore style files to filter files to generate hash from and generate a version number """
##
# @file version.py

# <1:git root (to get ignore)> <2:relative dir from git root to gather hash> <3:output dir>
#C:\Python310\python.exe C:\development\game04\support\s01_d3dx12_engine\Version00\Script\version.py C:\development\game04 support\s01_d3dx12_engine C:\development\game04\support\s01_d3dx12_engine\Version00\Output 0 0 0 0

import sys
import os
import re
import hashlib
import platform
import json

#os.environ["PYTHONHASHSEED"] = "0"

## read file in 64kb chunks
BUF_SIZE = 65536  
def FastHash(hashValue, filePath, verbose):
	"""! 
		append the given hash value with a hash generated from file on given path
		https://stackoverflow.com/questions/22058048/hashing-a-file-in-python
	"""
	#print("FastHash {0}".format(filePath))
	value = 0
	with open(filePath, 'rb') as f:
		while True:
			data = f.read(BUF_SIZE)
			if not data:
				break
			#value ^= hash(data)
			hashValue.update(data)
			if True == verbose:
				print(hashValue.hexdigest())
	#return value

def DealItem(setFilesIgnore, line): 
	"""! given an input line form a .ignore file, append to the set of files to ignore 
		pre compute the regular expression to make checking files easier
	"""
	if line in setFilesIgnore:
		return

	line = line.replace(".", "\.")
	line = line.replace("*", ".*")
	prog = re.compile(line)
	setFilesIgnore[line] = prog

#foo = set(), bar = {1,2,3}
class GitIgnore:
	"""! Hold the current state of the git ignore
	"""
	def __init__(self, setFilesIgnore, setFolderIgnore):
		"""! The GitIgnore class initializer.
		"""
		## The dictionatry of regulare expression string and compiled pattern
		self.setFilesIgnore = setFilesIgnore 
		## The set of folder names to ignore
		self.setFolderIgnore = setFolderIgnore
	def IsFileValid(self, fileName):
		"""! Given a file name, test if valid
			@return bool, true if folder is valid/ not ignored
		"""
		for value in self.setFilesIgnore.values():
			if None != value.fullmatch(fileName):
				return False
		return True

	def IsFolderValid(self, folderName):
		"""! Given a folder name, test if valid
			@return bool, true if folder is valid/ not ignored
		"""
		return folderName not in self.setFolderIgnore

	def MakeChild(self, folder):
		"""! Make a child GitIgnore class object of the given folder 
		"""
		return GitIgnore.Factory(folder, self);
	@classmethod
	def Factory(cls, folder, parentGitIgnoreOrNone=None):
		"""! given the folder name, and the optional parent git ignore object, make a new GitIgnore class object
		"""
		setFilesIgnore = {}
		setFolderIgnore = set()
		if parentGitIgnoreOrNone is not None:
			setFilesIgnore = parentGitIgnoreOrNone.setFilesIgnore.copy()
			setFolderIgnore = parentGitIgnoreOrNone.setFolderIgnore.copy()

		gitIgnorePath = os.path.join(folder, ".gitignore")
		if True == os.path.exists(gitIgnorePath):
			with open(gitIgnorePath) as file:
				for line in file:
					line = line.strip()
					if not line:
						continue
					if line[0] == '#':
						continue
					if line[-1] == '/':
						folder = line[:-1]
						if folder not in setFolderIgnore:
							setFolderIgnore.add(folder)
					else:
						DealItem(setFilesIgnore, line) 
		return GitIgnore(setFilesIgnore, setFolderIgnore)


def TraversData(outputFileList, folderPath, gitIgnoreOrNull, relativePathSplit, verbose):
	"""! Recursive function to generate a file list of files that pass the git ignore filter
		relativePathSplit is an array of folder names to descend before looking for files to add to outputFileList
	"""

	ignoreLocal = GitIgnore.Factory(folderPath, gitIgnoreOrNull)
	if True == verbose:
		print("TraversData {0} {1}".format(folderPath, relativePathSplit))
	for (dirpath, folderList, fileList) in os.walk(folderPath):

		if 0 < len(relativePathSplit):
			front = relativePathSplit.pop(0)
			if front not in folderList:
				if True == verbose:
					print("{0} not found under {1}".format(front, folderPath))
				return
			newDir = os.path.join(folderPath, front)
			TraversData(outputFileList, newDir, ignoreLocal, relativePathSplit, verbose)
		else:
			for file in fileList:
				newFile = os.path.join(folderPath, file)
				if False == ignoreLocal.IsFileValid(file):
					if True == verbose:
						print("ignore {0}".format(newFile))
					continue
				outputFileList.append(newFile)
			for folder in folderList:
				newDir = os.path.join(folderPath, folder)
				if False == ignoreLocal.IsFolderValid(folder):
					if True == verbose:
						print("ignore {0}".format(newDir))
					continue
				TraversData(outputFileList, newDir, ignoreLocal, relativePathSplit, verbose)
		break

def WriteStringToFile(filePath, stringData):
	"""! Write a string to the given file path
	"""
	os.makedirs(os.path.dirname(filePath), exist_ok=True)
	file = open(filePath, "w")
	file.write(stringData)
	file.close()

def ReadStringFromFile(filePath, defaultData = ""):
	"""! Read the contents of a file as a string
		@return string of file contents, else the defaultData if no file loaded
	"""
	#print("ReadStringFromFile {0} {1}".format(filePath, defaultData))
	result = defaultData
	if True == os.path.exists(filePath):
		file = open(filePath, "r")
		result = file.read()
		file.close()
	#print(bytes(result, 'utf-8'))
	return result

def WriteStringToFileIfDifferent(filePath, stringData):
	"""! update file on disk if contents are different to stringData
	"""
	data = ReadStringFromFile(filePath)
	if (stringData != data):
		WriteStringToFile(filePath, stringData)

def DealHash(hashValue, outputDir, seedMajor, seedMinor, seedPatch, seedStore):
	"""! Update the version file under the given path, incement if hash different to previous
	"""

	hashFilePath = os.path.join(outputDir, "build_hash.txt")
	oldHash = ReadStringFromFile(hashFilePath, "")

	if oldHash == hashValue:
		print("Hash value match {0}".format(hashValue))
		return


	majorFilePath = os.path.join(outputDir, "build_version_major.txt")
	major = int(ReadStringFromFile(majorFilePath, str(seedMajor)).strip())

	minorFilePath = os.path.join(outputDir, "build_version_minor.txt")
	minor = int(ReadStringFromFile(minorFilePath, str(seedMinor)).strip())

	patchFilePath = os.path.join(outputDir, "build_version_patch.txt")
	patch = int(ReadStringFromFile(patchFilePath, str(seedPatch)).strip())

	storeFilePath = os.path.join(outputDir, "build_version_store.txt")
	store = int(ReadStringFromFile(storeFilePath, str(seedStore)).strip())

	patch += 1
	version = "\"{0}.{1}.{2}.{3}\"".format(major, minor, patch, store)
	versionComma = "\"{0},{1},{2},{3}\"".format(major, minor, patch, store)

	WriteStringToFile(majorFilePath, str(major))
	WriteStringToFile(minorFilePath, str(minor))
	WriteStringToFile(patchFilePath, str(patch))
	WriteStringToFile(storeFilePath, str(store))

	versionFilePath = os.path.join(outputDir, "build_version.txt")
	WriteStringToFile(versionFilePath, version)

	versionCommaFilePath = os.path.join(outputDir, "build_version_comma.txt")
	WriteStringToFile(versionCommaFilePath, versionComma)

	WriteStringToFile(hashFilePath, hashValue)

	print("Hash {0} {1} version {2}".format(hashValue, oldHash, version))

def DealHostName(outputDir):
	"""! Write to a given dir a file with the computers host name
	"""
	hostFilePath = os.path.join(outputDir, "build_host.txt")
	host = "\"{0}\"".format(platform.node())
	WriteStringToFileIfDifferent(hostFilePath, host)

def main(*args):
	"""! Main program entry."""
	#print (len(args))
	if len(args) < 4:
		print("usage: version.py <1:git root (to get ignore)> <2:relative dir from git root to gather hash> <3:output dir> [4:seed major] [5:seed minor] [6:seed patch] [7:seed store] [8:bool verbose flag]")
		return
	gitRoot = args[1]
	relativePath = args[2]
	outputDir = args[3]
	seedMajor = 0
	if 4 < len(args):
		seedMajor = int(args[4])
	seedMinor = 0
	if 5 < len(args):
		seedMinor = int(args[5])
	seedPatch = 0
	if 6 < len(args):
		seedPatch = int(args[6])
	seedStore = 0
	if 7 < len(args):
		seedStore = int(args[7])
	verbose = False
	if 8 < len(args):
		verbose = json.loads(args[8].lower())

	relativePathSplit = relativePath.split("\\")
	fileList = []
	TraversData(fileList, gitRoot, None, relativePathSplit, verbose)

	if True == verbose:
		print("  FileList:")

	hashValue = hashlib.md5()
	for file in fileList:
		if True == verbose:
			print(file)
		#hashValue ^= FastHash(file)
		FastHash(hashValue, file, verbose)
		#break

	#print(hashValue.hexdigest())
	DealHash(hashValue.hexdigest(), outputDir, seedMajor, seedMinor, seedPatch, seedStore)
	DealHostName(outputDir)

if __name__ == "__main__":
	main(*sys.argv)

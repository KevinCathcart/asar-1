import asar
from os import path
import sys

if len(sys.argv) != 2:
	print("Usage: test-???.py [path_to_tests_directory]")
	sys.exit(1)

if not asar.init():
	print("Unable to initialize Asar")
	sys.exit(1)
asar.close()

if not asar.init():
	print("Unable to re-initialize Asar")
	sys.exit(1)
asar.close()

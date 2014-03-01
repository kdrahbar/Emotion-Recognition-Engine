import re
FILE_NAME = "AUEyeClassifier.txt"

# line.split returns each line in the file.
list_of_lines = [line.split(' ') for line in open(FILE_NAME)]
output = open("output.txt", 'w+')
check = True
for line in list_of_lines:
	check = True
	for word in line:
		if "weight" in word:
			check = False
	if check:
		for word in line:	
			if ')' in word:
				word = word.replace('(', "")
				word = word.replace(')', "")
			try:
				x = float(word)
				x = str(x)
				output.write(x)
				output.write('\n')
			except ValueError:
				pass


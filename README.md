# Tweet-generator
tweetsGenerator.c - file that generates a requested number of tweets by constructing word list according to probabilities read from a certain number of words from the given file

to compile using c enter command gcc tweetsGenerator.c -o tweetsGenerator

then when running it type ./tweetsGenerator S T "filepath" W

S representing the random seed 
T representing the number of requested tweets 
filepath representing the text file path to read from
W representing words to read from file --optional

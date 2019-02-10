#!/usr/bin/python

import random, sys
import string
from optparse import OptionParser

class shuf:
	def __init__(self, fileGiven, stdInData=None, dataFile=None, head_count=None):
		if head_count is not None:
			self.head_count = head_count
		else:
			self.head_count = None
		if fileGiven:
			f = open(dataFile, 'r')
			self.lines = f.read().splitlines()
			f.close()
		elif stdInData is not None:
			self.lines = stdInData
		else:
			self.lines = None
	def input_range(self, lo, hi):
		l = list(range(lo, hi+1))
		self.lines = [str(x) for x in l]
	def shuffle(self):
		random.shuffle(self.lines)
		i = 0
		if self.head_count is None:
			self.head_count = len(self.lines)
		while i < len(self.lines) and i < self.head_count:
			sys.stdout.write(self.lines[i] + '\n')
			i += 1
	def repeat(self):
		if len(self.lines) <= 0:
			return("no lines to repeat")
		elif self.head_count is None:
			while True:
				sys.stdout.write(random.choice(self.lines) + '\n')
		elif self.head_count == 0:
			while True:
				pass
		elif self.head_count > 0:
			for i in range(0, self.head_count):
				sys.stdout.write(random.choice(self.lines) + '\n')
		else:
			self.shuffle()
	

def main():
	version_msg = "%prog 2.0"
	usage_msg = """./%prog [OPTION]... [FILE]
  or:  ./%prog -e [OPTION]... [ARG]...
  or:  ./%prog -i LO-HI [OPTION]
	
Write a random permutation of the input lines to standard output.

With no FILE, or when FILE is -, read standard input.

Mandatory arguments to long options are mandatory for short options too."""	

	parser = OptionParser(version=version_msg,
				usage=usage_msg)	
	parser.add_option("-i", "--input-range",
			action="store", dest="lo_hi", default=None,
			help="treat each number LO through HI as an input line")
	parser.add_option("-n", "--head-count",
			action="store", dest="count", type="int", 
			help="output at most COUNT lines")
	parser.add_option("-r", "--repeat",
			action="store_true", dest="repeat", default=False,
			help="output lines can be repeated")
	options, args = parser.parse_args(sys.argv[1:])
	
	#single-option argument "-"
	stdInData = None
	if options.count is None and options.lo_hi is None and options.repeat is False:
		if args is not None and len(args) == 1 and args[0] == '-':
			args = [] 
	if len(args) < 1 and not sys.stdin.isatty():
		stdInData = sys.stdin.read().splitlines()

	#never more than two arguments
	if len(args) > 1:
		parser.error("invalid input range: '{0}'" . format(args[1]))

	#checks that there is no argument for input-range option
	if options.lo_hi is not None:
		lo_hi_input = options.lo_hi.split('-')		
		if len(lo_hi_input) != 2:
			parser.error("invalid input range: '{0}'".
				format(options.lo_hi))
		try:
			lo = int(lo_hi_input[0])
		except:
			parser.error("invalid input range: '{0}'". format(lo_hi_input[0]))
		try:
			hi = int(lo_hi_input[1])
		except:
			parser.error("invalid input range: '{0}'" . format(lo_hi_input[1]))
		if lo_hi_input[0] > lo_hi_input[1]:
			parser.error("invalid input range: '{0}'" . format(lo_hi))
		if args is not None and len(args) > 0:
			parser.error("extra operand '{0}'" . format(args[0]))						

	#checks valid argument for head-count option
	if options.count is not None and options.count < 0:
		parser.error('negative options count: {0}'. format(options.count))

	#checks for cases that yield infinite loop, or extra operand which is invalid
	if len(args) > 1:
		parser.error("extra operand '{0}'" . format(args[1]))
	elif len(args) < 1 and stdInData is None and options.count is None and options.lo_hi is None:
		while True:
			pass	#infinite loop

	try:
		sprite = None
		if options.lo_hi:
			sprite = shuf(False, None, None, options.count)
			sprite.input_range(lo, hi)
		elif len(args) > 0:
			sprite = shuf(True, None, args[0], options.count)
		elif stdInData is not None:
			sprite = shuf(False, stdInData, None, options.count)
		elif options.count > 0:
			while True:
				pass
		else:
			sprite = shuf(False, [], None, options.count)
		if options.repeat is True:
			result = sprite.repeat()
			if result:
				parser.error(result)
		else:
			sprite.shuffle()
		
	except OSError as e:
		parser.error(e)
	

if __name__ == "__main__":
        main()



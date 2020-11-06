import re
from unicodeit.replace import replace
from unicodeit.data import REPLACEMENTS, COMBININGMARKS
LATEX_COMMANDS = REPLACEMENTS + COMBININGMARKS
bannedChar = ['(',')','[',']']

def getReplace(st): 
	return replace(st)

def valid(inp):
	if (inp == "") or (inp == " ") : return False
	for c in bannedChar:
		if c in inp : return False
	return True

def getList(inp):
	if not valid(inp) : return ""
	r = re.compile(inp.replace('\\','\\\\'))
	thetuple = [tup for tup in LATEX_COMMANDS if r.match(tup[0])][:10]
	theItems = [sym+' '+com for (com,sym) in thetuple if (com != inp)]
	st = ""
	for item in theItems: st += item + ';'
	return st[:-1]
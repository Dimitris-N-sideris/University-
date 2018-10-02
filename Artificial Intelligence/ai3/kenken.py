
import time
import csp
from utils import argmin_random_tie, count, first


class Kenken(csp.CSP):

	rows =	{}				# dict of rows with the lists of each elements	
	columns = {}			# dict of columns with the lists of the elements
	kenken_cliques = {}		# (type, result, list of elements)
	positionsTOcliques = {}	# keys to the clique of each element
	values = {}				# values of each element
	mycount = 0				#number of cliques
	constraintsnumber = 0
	def __init__(self, n,opened):
   # "Construct a CSP problem. If variables is empty, it becomes domains.keys()."
    # variables = variables or list(domains.keys())
		self.size = n
		Kenken.constraintsnumber = 0
		variables = []
		domain_value = []
		domain = {}
		neighbors = {}
		for i in range(n):
			domain_value.append(i+1)			
			myrow = []
			for j in range(n):
				variables.append((i,j))
				myrow.append((i,j))
			Kenken.rows[i] = myrow 
		for j in range(n):
			mycolumn = [] 	
			for i in range(n):
				mycolumn.append((i,j))
			Kenken.columns[j] = mycolumn
		# parse input 
		for line in opened:
			mystring = line.split(']')	# split line in 2 halves
			elementsofclique = mystring[0][1:].split(',')
			sizeofclique = len(elementsofclique)//2
			TypeandResult = mystring[1].split()
			compoList = []

			for i in range(sizeofclique):	# clique elements gathering
				xaxis = int(elementsofclique[2*i][1:])
				yaxis = int(elementsofclique[2*i+1].split(")")[0])
				compoList.append((xaxis,yaxis))	# all elements that belong to this clique
			cliqueResult = int(TypeandResult[1])	# wanted result
			Kenken.kenken_cliques[Kenken.mycount] = (TypeandResult[0], cliqueResult, compoList)

			for element in compoList:
				
				if TypeandResult[0] == "''":
					domain[element] =  domain_value[cliqueResult-1: cliqueResult]
				elif TypeandResult[0] == "add" or TypeandResult[0] == "mult":
					if cliqueResult < n:
						domain[element] =  domain_value[0 : cliqueResult]
					else:
						domain[element] = domain_value
				else:
					domain[element] = domain_value
				#print(domain[element])
				Kenken.positionsTOcliques[element] = Kenken.mycount
			Kenken.mycount += 1	
		#print(Kenken.kenken_cliques)
		# same row and column neighbors
		for var in variables:	
			myNeighbors = []	
			myrow = Kenken.rows[var[0]]	# get variable's row
			for element in myrow:	# row neighbors
				if element[1] != var[1]:
					myNeighbors.append(element)
			mycolumn = Kenken.columns[var[1]]	# get variable's column
			for element in mycolumn:		#column neighbors
				if element[0] != var[0]:
					myNeighbors.append(element)
			neighbors[var] = myNeighbors	
		csp.CSP.__init__(self, variables, domain, neighbors, Kenken.constraint)

	def constraint(A,a,B,b):
		Kenken.constraintsnumber += 1
		if a == b:		#neighbors can't be same value
			return False
		myvalue = a
		myself = A
		clique = True
		for i in range(2):	
			myclique = Kenken.positionsTOcliques[myself]
			cliquedata = Kenken.kenken_cliques[myclique]		
			cliquetype = cliquedata[0]
			cliqueResult = cliquedata[1]
			cliqueNeighbors = cliquedata[2]
			mycliquevalues = []
			cliquesize = 0
			for element in cliqueNeighbors:
				cliquesize += 1
				if element == myself:
					mycliquevalues.append(myvalue)
				elif element in Kenken.values:
					mycliquevalues.append(Kenken.values[element])
			if not mycliquevalues:
				continue
			mycliquevalues.sort()
		#	print (mycliquevalues)
			assignSize = len(mycliquevalues)
			myresult = mycliquevalues.pop()
			#print (mycliquevalues)
			while mycliquevalues:
				if (cliquetype ==  "''") or (cliquetype == "add"):
					myresult +=  mycliquevalues.pop()
					#print(myresult)
				elif cliquetype == "mult":
					myresult *= mycliquevalues.pop()
				#	print(myresult)
				elif cliquetype == "sub":
					myresult -= mycliquevalues.pop()
				elif  cliquetype == "div":
					myresult //= mycliquevalues.pop()
				#	print(myresult)
			if assignSize < cliquesize:		
				# assigned value in clique less than max try to catch error
				if (cliquetype == "") or (cliquetype == "add"):
					if myresult >= cliqueResult:
						clique = False
				elif cliquetype == "mult":			
					if myresult > cliqueResult:
						clique = False
			elif  assignSize == cliquesize:	
				#if already assigned max size check if it is right
				if myresult != cliqueResult:
					clique = False
			else:
				print("error")
			myvalue = b		#check the other element too
			myself = B
		return clique

	def assign(self, var, val, assignment):
     #   "Add {var: val} to assignment; Discard the old value if any."
		assignment[var] = val
		self.nassigns += 1
		Kenken.values[var] = val
		#self.display(assignment)

	def unassign(self, var, assignment):
     ##   """Remove {var: val} from assignment.
      #  DO NOT call this if you are changing a variable to a new value;
      #  just call assign for that."""
		if var in assignment:
			del assignment[var]
			del Kenken.values[var]
		#	self.display(assignment)

	def nconflicts(self, var, val, assignment):
#    "Return the number of conflicts var=val has with other variables."
        # Subclasses may implement this more efficiently
		def conflict(var2):
			return (var2 in assignment and
				not self.constraints(var, val, var2, assignment[var2]))
		return count(conflict(v) for v in self.neighbors[var])

	def display(self, assignment):
     #   "Show a human-readable representation of the CSP."
        # Subclasses can print in a prettier way, or display with a GUI
		print('Result is :')
		if not assignment:
			print("Impossible for me")
			return
		for i in range(self.size):
			printlist = []
			for j in range(self.size):
				if (i,j) in assignment:
					printlist.append(assignment[(i,j)])
			print(printlist, '\n')
		
if __name__  == "__main__":
	sumt = 0
	for i in range(100):
		opened = open('kenken4.txt','r')
		sizestring = opened.readline()
		size = int(sizestring)
		prob = Kenken(size, opened)
		start = time.clock()
		# better run one per time didn't debug for simultaneous running
		x = csp.backtracking_search(prob) # plain BT
		#x = csp.backtracking_search(prob, csp.first_unassigned_variable, csp.unordered_domain_values, csp.forward_checking) # BT + FC
		#x = csp.backtracking_search(prob, csp.mrv, csp.unordered_domain_values, csp.no_inference)   	# BT  + MRV	(with bug)	
		#x = csp.backtracking_search(prob, csp.mrv, csp.unordered_domain_values, csp.forward_checking)		# BT + FC + MRV
		#x = csp.backtracking_search(prob,csp.first_unassigned_variable, csp.unordered_domain_values, csp.mac)	
		#x = csp.min_conflicts(prob)			# minconflicts
		print( Kenken.constraintsnumber)
		print(prob.nassigns)
		end = time.clock()
		end_time = end -start
		print(end_time)
		
		if i == 0:
			maxt = end_time
			mint = end_time
		else:
			if maxt < end_time:
				maxt = end_time
			if mint > end_time:
				mint = end_time		
		sumt += end_time
		
	prob.display(x)
	print("sum",sumt/100)
	print("max time taken: ",maxt,"\nleast time taken: ",mint )
	



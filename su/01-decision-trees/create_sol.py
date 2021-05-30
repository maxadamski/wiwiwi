import argparse
import os

def constructSolution(dir_path):
	solution = []
	for i in range(1,14):
		ds_sol = [line.strip() for line in  open(os.path.join(dir_path,str(i) + ".csv"))]
		if ds_sol[0].strip("\"") in ( "Y", "x"):
			del ds_sol[0]
		if i == 8:
			ds_sol.extend(ds_sol)
		elif i == 3:
			ds_sol.extend(ds_sol)
			ds_sol.extend(ds_sol)
		
		assert len(ds_sol) == 200
		solution.extend(ds_sol)
	return solution

parser = argparse.ArgumentParser()
parser.add_argument("dir", help="directory with solution files")
parser.add_argument("output_file", help="name of the output file")

args = parser.parse_args()

sol = constructSolution(args.dir)
with open(args.output_file, 'w') as file:
	file.write("id,Y\n")
	for i, line in enumerate(sol):
		file.write("{0},{1}\n".format(i,line))
		

import argparse
import os
import sys
import numpy as np
from time import time
from deap import creator, base, tools, algorithms
from FramsticksLib import FramsticksLib

best_fitness = 0

def genotype_within_constraint(genotype, dict_criteria_values, criterion_name, constraint_value):
	REPORT_CONSTRAINT_VIOLATIONS = False
	if constraint_value is not None:
		actual_value = dict_criteria_values[criterion_name]
		if actual_value > constraint_value:
			if REPORT_CONSTRAINT_VIOLATIONS:
				print('Genotype "%s" assigned low fitness because it violates constraint "%s": %s exceeds threshold %s' % (genotype, criterion_name, actual_value, constraint_value))
			return False
	return True


def frams_evaluate(frams_cli, individual):
	BAD_FITNESS = [-1]
	genotype = individual[0]  # individual[0] because we can't (?) have a simple str as a deap genotype/individual, only list of str.
	data = frams_cli.evaluate([genotype])
	# print("Evaluated '%s'" % genotype, 'evaluation is:', data)
	valid = True
	try:
		first_genotype_data = data[0]
		evaluation_data = first_genotype_data["evaluations"]
		data = default_evaluation_data = evaluation_data[""]
		body_recording = data["data->bodyrecording"]
		#if default_evaluation_data['vertpos'] < 0: default_evaluation_data['vertpos'] += default_evaluation_data['numparts']
		#if default_evaluation_data['vertpos'] < 0: default_evaluation_data['vertpos'] = 0

		T = len(body_recording)
		assert T > 0
		#z_idx = 2
		#z = body_recording[0][0][z_idx]
		#i = 1


		vel = data['velocity']

		jmp = max(0, 1.01 + max(body_recording[i][0][2] for i in range(T)))

		dst = 0

		j = 0
		while j < T:
			i = j
			while i < T and body_recording[i][0][2] <= 0: i += 1
			j = i
			while j < T and body_recording[j][0][2] > 0: j += 1
			if i < T and j < T:
				p_i, p_j = np.array(body_recording[i][0]), np.array(body_recording[j][0])
				dst = max(dst, np.sqrt(np.sum((p_j - p_i)**2)))


		fitness = 10*dst + jmp + vel
		#fitness = dst

		global best_fitness
		if fitness > best_fitness:
			print(f'fitness = {fitness:14.10f}; dst = {dst:.10f}; jmp = {jmp:.10f}; vel = {vel:.10f}; geno = "{genotype}"')
			best_fitness = fitness

		fitness = [fitness]
	except (KeyError, TypeError) as e:  # the evaluation may have failed for an invalid genotype (such as X[@][@] with "Don't simulate genotypes with warnings" option) or for some other reason
		valid = False
		#print('Problem "%s" so could not evaluate genotype "%s", hence assigned it low fitness: %s' % (str(e), genotype, BAD_FITNESS))
	if valid:
		default_evaluation_data['numgenocharacters'] = len(genotype)  # for consistent constraint checking below
		valid &= genotype_within_constraint(genotype, default_evaluation_data, 'numparts', parsed_args.max_numparts)
		valid &= genotype_within_constraint(genotype, default_evaluation_data, 'numjoints', parsed_args.max_numjoints)
		valid &= genotype_within_constraint(genotype, default_evaluation_data, 'numneurons', parsed_args.max_numneurons)
		valid &= genotype_within_constraint(genotype, default_evaluation_data, 'numconnections', parsed_args.max_numconnections)
		valid &= genotype_within_constraint(genotype, default_evaluation_data, 'numgenocharacters', parsed_args.max_numgenochars)
	if not valid:
		fitness = BAD_FITNESS
	return fitness


def frams_crossover(frams_cli, individual1, individual2):
	geno1 = individual1[0]  # individual[0] because we can't (?) have a simple str as a deap genotype/individual, only list of str.
	geno2 = individual2[0]  # individual[0] because we can't (?) have a simple str as a deap genotype/individual, only list of str.
	individual1[0] = frams_cli.crossOver(geno1, geno2)
	individual2[0] = frams_cli.crossOver(geno1, geno2)
	return individual1, individual2


def frams_mutate(frams_cli, individual):
	individual[0] = frams_cli.mutate([individual[0]])[0]  # individual[0] because we can't (?) have a simple str as a deap genotype/individual, only list of str.
	return individual,


def frams_getsimplest(frams_cli, genetic_format, initial_genotype):
	return initial_genotype if initial_genotype is not None else frams_cli.getSimplest(genetic_format)



def parseArguments():
	def ensureDir(string):
		if os.path.isdir(string): return string
		raise NotADirectoryError(string)
	parser = argparse.ArgumentParser(description='Run this program with "python -u %s" if you want to disable buffering of its output.' % sys.argv[0])
	parser.add_argument('-path', type=ensureDir, required=True, help='Path to Framsticks CLI without trailing slash.')
	parser.add_argument('-lib', required=False, help='Library name. If not given, "frams-objects.dll" or "frams-objects.so" is assumed depending on the platform.')
	parser.add_argument('-sim', required=False, default="eval-allcriteria.sim", help="The name of the .sim file with settings for evaluation, mutation, crossover, and similarity estimation. If not given, \"eval-allcriteria.sim\" is assumed by default. Must be compatible with the \"standard-eval\" expdef. If you want to provide more files, separate them with a semicolon ';'.")

	parser.add_argument('-genformat', required=False, help='Genetic format for the simplest initial genotype, for example 4, 9, or B. If not given, f1 is assumed.')
	parser.add_argument('-initialgenotype', required=False, help='The genotype used to seed the initial population. If given, the -genformat argument is ignored.')

	parser.add_argument('-popsize', type=int, default=50, help="Population size, default: 50.")
	parser.add_argument('-generations', type=int, default=5, help="Number of generations, default: 5.")
	parser.add_argument('-tournament', type=int, default=15, help="Tournament size, default: 5.")
	parser.add_argument('-pmut', type=float, default=0.8, help="Probability of mutation, default: 0.9")
	parser.add_argument('-pxov', type=float, default=0.2, help="Probability of crossover, default: 0.2")
	parser.add_argument('-hof_size', type=int, default=10, help="Number of genotypes in Hall of Fame. Default: 10.")
	parser.add_argument('-hof_savefile', required=False, help='If set, Hall of Fame will be saved in Framsticks file format (recommended extension *.gen).')

	parser.add_argument('-max_numparts', type=int, default=None, help="Maximum number of Parts. Default: no limit")
	parser.add_argument('-max_numjoints', type=int, default=None, help="Maximum number of Joints. Default: no limit")
	parser.add_argument('-max_numneurons', type=int, default=None, help="Maximum number of Neurons. Default: no limit")
	parser.add_argument('-max_numconnections', type=int, default=None, help="Maximum number of Neural connections. Default: no limit")
	parser.add_argument('-max_numgenochars', type=int, default=None, help="Maximum number of characters in genotype (including the format prefix, if any). Default: no limit")
	return parser.parse_args()


if __name__ == "__main__":
	# random.seed(123)  # see FramsticksLib.DETERMINISTIC below, set to True if you want full determinism
	FramsticksLib.DETERMINISTIC = False  # must be set before FramsticksLib() constructor call
	parsed_args = parseArguments()
	print("Argument values:", ", ".join(['%s=%s' % (arg, getattr(parsed_args, arg)) for arg in vars(parsed_args)]))

	framsLib = FramsticksLib(parsed_args.path, parsed_args.lib, parsed_args.sim.split(";"))

	frams_cli = framsLib
	tournament_size = parsed_args.tournament
	genetic_format = '1' if parsed_args.genformat is None else parsed_args.genformat
	initial_genotype = parsed_args.initialgenotype

	creator.create("FitnessMax", base.Fitness, weights=[1.0])
	creator.create("Individual", list, fitness=creator.FitnessMax)  # would be nice to have "str" instead of unnecessary "list of str"

	toolbox = base.Toolbox()
	toolbox.register("attr_simplest_genotype", frams_getsimplest, frams_cli, genetic_format, initial_genotype)  # "Attribute generator"
	toolbox.register("individual", tools.initRepeat, creator.Individual, toolbox.attr_simplest_genotype, 1)
	toolbox.register("population", tools.initRepeat, list, toolbox.individual)
	toolbox.register("evaluate", frams_evaluate, frams_cli)
	toolbox.register("mate", frams_crossover, frams_cli)
	toolbox.register("mutate", frams_mutate, frams_cli)
	toolbox.register("select", tools.selTournament, tournsize=tournament_size)

	pop = toolbox.population(n=parsed_args.popsize)
	hof = tools.HallOfFame(parsed_args.hof_size)
	stats = tools.Statistics(lambda ind: ind.fitness.values)
	stats.register("avg", np.mean)
	stats.register("stddev", np.std)
	stats.register("min", np.min)
	stats.register("max", np.max)

	t = time()
	pop, log = algorithms.eaSimple(population=pop, toolbox=toolbox, cxpb=parsed_args.pxov, mutpb=parsed_args.pmut, ngen=parsed_args.generations, stats=stats, halloffame=hof, verbose=True)
	t = time() - t
	print('Best individuals:')

	for ind in hof:
		print(ind.fitness, '\t-->\t', ind[0])

	if parsed_args.hof_savefile is not None:
		path = parsed_args.hof_savefile
		with open(path + '_hof.txt', 'w') as f:
			ind = hof[0]
			print(ind.fitness.values[0], file=f)
			print(ind[0], file=f)
		with open(path + '_log.txt', 'w') as f:
			print(log, file=f)
		with open(path + '_time.txt', 'w') as f:
			print(t, file=f)


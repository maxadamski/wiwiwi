import subprocess

SUB, EQV, DIS, TOP, BOT, ALL, ANY, AND, OR, NOT = [object() for _ in range(10)]
FORALL = ALL
EXISTS = ANY

class dotdict(dict):
	__getattr__ = dict.get
	__setattr__ = dict.__setitem__
	__delattr__ = dict.__delitem__

class Annot:
	__slots__ = ('data', 'lang', 'dtype')

	def __init__(self, data: str, lang=None, dtype=None):
		self.data, self.lang, self.dtype = data, lang, dtype

	def __str__(self):
		return str(self.data)

	def __repr__(self):
		result = repr(self.data)
		if self.lang is not None: result = f'{result}@{self.lang}'
		if self.dtype is not None: result = f'{result}^^{self.dtype}'
		return result

class Onto:
	def __init__(self, *, tbox: set, n_concepts=0, n_roles=0, concepts=None, roles=None, iri=None, prefixes=None, annotations=None):
		self.iri = iri
		self.tbox = tbox
		self.n_concepts = n_concepts if concepts is None else len(concepts)
		self.n_roles = n_roles if roles is None else len(roles)
		self.prefixes = {} if prefixes is None else prefixes
		self.concepts = [] if concepts is None else concepts
		self.roles = [] if roles is None else roles
		self.annotations = [] if annotations is None else annotations
		self.did_change_names()

	def did_change_names(self):
		self.concept_by_name = dotdict({name: id for id, name in enumerate(self.concepts)})
		self.role_by_name = dotdict({name: id for id, name in enumerate(self.roles)})

	def use_annotations_as_names(self, prop='rdfs:label', lang='en'):
		concepts = {x: x for x in self.concepts}
		roles = {x: x for x in self.roles}
		for p, s, o in self.annotations:
			if p == prop and isinstance(o, Annot) and o.lang == lang:
				if s in concepts: concepts[s] = str(o)
				if s in roles: roles[s] = str(o)

		self.concepts = [concepts[c] for c in self.concepts]
		self.roles = [roles[r] for r in self.roles]
		self.did_change_names()

	def use_prefix(self, prefix=':'):
		self.concepts = [c.replace(prefix, '') for c in self.concepts]
		self.roles = [r.replace(prefix, '') for r in self.roles]
		self.did_change_names()

	def __repr__(self):
		return f'<Onto {self.n_concepts} concepts, {self.n_roles} roles, {len(self.tbox)} axioms>'

	__str__ = __repr__

def parse_ofn(data, *, raw=False, silent=False, **kwargs):
	total = len(data)
	pos = 0

	def empty():
		nonlocal pos
		return pos >= len(data)

	def parse_iri():
		nonlocal pos
		pos += 1
		start = pos
		while not empty() and data[pos] != '>':
			pos += 1
		result = data[start:pos]
		pos += 1
		return result

	def parse_str():
		nonlocal pos
		pos += 1
		start = pos
		while not empty() and data[pos] != '"':
			pos += 1
			if not empty() and data[pos] == '\\':
				pos += 2

		text = data[start:pos]
		pos += 1
		lang = None
		dtype = None
		if not empty() and data[pos] == '@':
			pos += 1
			lang = parse_id()
		if not empty() and data[pos] == '^':
			pos += 2
			dtype = parse_id()
		return Annot(text, lang=lang, dtype=dtype)

	def parse_id():
		nonlocal pos
		start = pos
		while not empty() and data[pos] not in ' ()':
			pos += 1
		return data[start:pos]

	def parse_comment():
		nonlocal pos
		while not empty() and data[pos] != '\n':
			pos += 1
		pos += 1

	def parse_ws():
		nonlocal pos
		while not empty() and data[pos] in ' \t\n':
			pos += 1

	def parse_whitespace():
		nonlocal pos
		parse_ws()
		while not empty() and data[pos] == '#':
			parse_comment()
			parse_ws()

	def parse_expr():
		nonlocal pos

		parse_whitespace()
		if not empty():
			if data[pos] == '"':
				return parse_str()
			if data[pos] == '<':
				return parse_iri()

		head = parse_id()
		parse_whitespace()
		if not empty() and data[pos] != '(':
			return head

		args = []
		pos += 1
		parse_whitespace()
		while not empty() and data[pos] != ')':
			arg = parse_expr()
			if arg is not None:
				args.append(arg)
			parse_whitespace()

		pos += 1

		return tuple([head] + args)

	exprs = []
	while not empty():
		expr = parse_expr()
		if expr is not None:
			exprs.append(expr)

	result = []

	for expr in exprs:
		head = expr[0]
		if head == 'Prefix':
			prefix, iri = expr[1].split('=', 1)
			iri = iri[1:-1]
			result.append(('Prefix', prefix, iri))
		elif head == 'Ontology':
			result.append(expr)

	if raw:
		return result
	else:
		return pack_ofn(result, silent=silent)

def pack_ofn(data, *, silent=False):
	prefixes = {}
	onto = None
	for expr in data:
		head = expr[0]
		if head == 'Prefix':
			prefixes[expr[0]] = expr[1]
		elif head == 'Ontology':
			onto = expr
		else:
			raise Exception('No Ontology(...) found!')
			
	onto_iri = onto[1]
	concept_names = []
	declared_role_names = []
	role_names = set()
	labels = {}
	annot = []
	tbox = set()

	def parse_axiom(expr):
		if isinstance(expr, str):
			if expr == 'owl:Thing': return TOP
			if expr == 'owl:Nothing': return BOT
			return expr
			
		head = expr[0]
		if head == 'SubClassOf':
			return (SUB, parse_axiom(expr[1]), parse_axiom(expr[2]))
		elif head == 'DisjointClasses':
			return (DIS, *[parse_axiom(x) for x in expr[1:]])
		elif head == 'EquivalentClasses':
			return (EQV, *[parse_axiom(x) for x in expr[1:]])
		elif head == 'ObjectIntersectionOf':
			return (AND, *[parse_axiom(x) for x in expr[1:]])
		elif head == 'ObjectUnionOf':
			return (OR, *[parse_axiom(x) for x in expr[1:]])
		elif head == 'ObjectComplementOf':
			return (NOT, *[parse_axiom(x) for x in expr[1:]])
		elif head == 'ObjectSomeValuesFrom':
			result = (ANY, expr[1], parse_axiom(expr[2]))
			role_names.add(expr[1])
			return result
		elif head == 'ObjectAllValuesFrom':
			result = (ALL, expr[1], parse_axiom(expr[2]))
			role_names.add(expr[1])
			return result
		else:
			assert False

	for expr in onto[2:]:
		head = expr[0]
		if head == 'Declaration':
			decl = expr[1]
			typ = decl[0]
			if typ == 'Class':
				name = decl[1]
				if name in {'owl:Thing', 'owl:Nothing'}: continue
				concept_names.append(name)
			elif typ == 'ObjectProperty':
				declared_role_names.append(decl[1])
		
		elif head in {'SubClassOf', 'EquivalentClasses', 'DisjointClasses'}:
			try:
				axiom = parse_axiom(expr)
				tbox.add(axiom)
			except:
				if not silent: print('Unsupported class expression', expr)

		elif head == 'AnnotationAssertion':
			annot.append((expr[1], expr[2], expr[3]))
					
	if not silent:
		for role in declared_role_names:
			if role not in role_names:
				print('Dropping unused role', role)

	role_names = list(role_names)
	concept_name_to_idx = {name: i for i, name in enumerate(concept_names)}
	role_name_to_idx = {name: i for i, name in enumerate(role_names)}

	def rename(expr):
		if isinstance(expr, str):
			return concept_name_to_idx[expr]
		if not isinstance(expr, tuple):
			return expr
		head = expr[0]
		if head in {ALL, ANY}:
			return (head, role_name_to_idx[expr[1]], rename(expr[2]))
		else:
			return (head, *[rename(x) for x in expr[1:]])

	tbox = {rename(x) for x in tbox}

	return Onto(tbox=tbox, concepts=concept_names, roles=role_names, iri=onto_iri, prefixes=prefixes, annotations=annot)

def load_ofn(path, **kwargs):
	with open(path) as f:
		return parse_ofn(f.read(), **kwargs)

def load_owl(path, *, convert_script='scripts/ontoconvert', **kwargs):
	text = subprocess.Popen([convert_script, path], stdout=subprocess.PIPE).communicate()[0]
	return parse_ofn(text.decode('utf-8'), **kwargs)

from src.utils import invdict
from src.simplefact import *
from src.ontofun import *

TO_SUBSCRIPT = str.maketrans('0123456789', '₀₁₂₃₄₅₆₇₈₉')
OP_PRECEDENCE = {NOT: 50, ANY: 40, ALL: 40, AND: 30, OR: 20, SUB: 10, EQV: 10, DIS: 10}
OP_PRETTY = {NOT: '¬', AND: ' ⊓ ', OR: ' ⊔ ', SUB: ' ⊑ ', EQV: ' = ', DIS: ' != ', ANY: '∃', ALL: '∀'}
OP_SHORT = {NOT: 'not', AND: 'and', OR: 'or', SUB: 'sub', EQV: 'eqv', ANY: 'any', ALL: 'all', BOT: 'bot', TOP: 'top', DIS: 'dis'}
OP_SHORT_INV = invdict(OP_SHORT)
OP_BINARY = {AND, OR, SUB, EQV, DIS}
OP_QUANTIFIER = {ANY, ALL}
OP_UNARY = {NOT}

def expr_depth(x):
	if not isinstance(x, tuple): return 0
	op = x[0]
	if op in OP_UNARY:
		return 1 + expr_depth(x[1])
	elif op in OP_QUANTIFIER:
		return 1 + expr_depth(x[2])
	elif op in OP_BINARY:
		return 1 + max(expr_depth(x[1]), expr_depth(x[2]))
	assert False, f'bad alc expr {x}'

def to_subscript(x):
	return str(x).translate(TO_SUBSCRIPT)

def paren_if(cond, x):
	return '(' + x + ')' if cond else x

def default_show_concept(id):
	return 'C' + to_subscript(id)

def default_show_role(id):
	return 'R' + to_subscript(id)

def to_pretty(expr, *, show_concept=default_show_concept, show_role=default_show_role, onto=None):
	"""
	Returns a pretty representation of a given expression.
	"""
	if onto is not None:
		show_concept = onto.concepts.__getitem__
		show_role = onto.roles.__getitem__

	def rec(x, prec):
		if isinstance(x, tuple):
			head = x[0]
			head_prec = OP_PRECEDENCE[head]
			if head in OP_UNARY:
				result = OP_PRETTY[head] + rec(x[1], head_prec)
			elif head in OP_BINARY:
				result = OP_PRETTY[head].join(rec(xx, head_prec) for xx in x[1:])
			elif head in OP_QUANTIFIER:
				result = OP_PRETTY[head] + show_role(x[1]) + '.' + rec(x[2], head_prec)
			else:
				assert False, f'unknown operator {head}'
			return paren_if(prec > head_prec, result)
		elif x == TOP:
			return '⊤'
		elif x == BOT:
			return '⊥'
		elif isinstance(x, int):
			return show_concept(x)
		else:
			assert False, f'unknown operand {x}'

	return rec(expr, 0)

def to_prefix(expr):
	"""
	Serializes a given expression to Polish notation.

	assert x == parse_prefix(to_prefix(x))
	"""
	if isinstance(expr, tuple):
		head = expr[0]
		op = OP_SHORT[head]
		if head in OP_UNARY:
			return op + ' ' + to_prefix(expr[1])
		elif head in OP_BINARY:
			return op + ' ' + to_prefix(expr[1]) + ' ' + to_prefix(expr[2])
		elif head in OP_QUANTIFIER:
			return op + ' ' + str(expr[1]) + ' ' + to_prefix(expr[2])
		else:
			assert False, f'unknown operator {head}'
	elif expr == BOT:
		return 'bot'
	elif expr == TOP:
		return 'top'
	else:
		return str(expr)

def parse_prefix(text):
	"""
	Parses a given expression in Polish notation.

	assert x == parse_prefix(to_prefix(x))
	"""
	def do(tokens):
		token = tokens.pop()
		if token.isnumeric():
			return int(token)

		head = OP_SHORT_INV.get(token)
		if head is None:
			assert False, f'unknown token {token} ({text})'

		if head in OP_UNARY:
			return (head, do(tokens))
		elif head in OP_BINARY:
			return (head, do(tokens), do(tokens))
		elif head in OP_QUANTIFIER:
			return (head, int(tokens.pop()), do(tokens))
		return head

	return do(text.split(' ')[::-1])

def flatten_binary(expr):
	assert isinstance(expr, tuple) and expr[0] in {AND, OR}
	op = expr[0]
	args = []
	def rec(e):
		if isinstance(e, tuple) and e[0] == op:
			rec(e[1])
			rec(e[2])
		else:
			args.append(e)
	rec(expr)
	return args

def nnf(expr):
	if isinstance(expr, int):
		return expr
	if expr[0] == NOT:
		inner = expr[1]
		if inner == TOP:
			return BOT
		if inner == BOT:
			return TOP
		if isinstance(inner, tuple):
			if inner[0] == NOT:
				return nnf(inner[1])
			if inner[0] == AND:
				return nnf((OR, (NOT, inner[1]), (NOT, inner[2])))
			if inner[0] ==  OR:
				return nnf((AND, (NOT, inner[1]), (NOT, inner[2])))
			if inner[0] == ANY:
				return nnf((ALL, inner[1], (NOT, inner[2])))
			if inner[0] == ALL:
				return nnf((ANY, inner[1], (NOT, inner[2])))
		return NOT, nnf(inner)
	if expr[0] in {AND, OR}:
		return expr[0], nnf(expr[1]), nnf(expr[2])
	if expr[0] in OP_QUANTIFIER:
		return expr[0], expr[1], nnf(expr[2])
	if expr[0] in {SUB, DIS, EQV}:
		return expr[0], nnf(expr[1]), nnf(expr[2])
	assert False, f'bad expression {expr}'

def core(expr):
	if isinstance(expr, int) or expr == TOP or expr == BOT:
		return expr
	if expr[0] == NOT:
		inner = expr[1]
		if inner == TOP:
			return BOT
		if inner == BOT:
			return TOP
		if isinstance(inner, tuple):
			if inner[0] == NOT:
				return core(inner[1])
			if inner[0] == OR:
				return AND, core((NOT, inner[1])), core((NOT, inner[2]))
			if inner[0] == ALL:
				return ANY, inner[1], core((NOT, inner[2]))
		return NOT, core(inner)
	if expr[0] == OR:
		return NOT, (AND, core((NOT, expr[1])), core((NOT, expr[2])))
	if expr[0] == ALL:
		return NOT, (ANY, expr[1], core((NOT, expr[2])))
	if expr[0] == AND:
		return expr[0], core(expr[1]), core(expr[2])
	if expr[0] == ANY:
		return expr[0], expr[1], core(expr[2])
	if expr[0] == SUB:
		return expr[0], core(expr[1]), core(expr[2])
	if expr[0] == DIS:
		return SUB, (AND, core(expr[1]), core(expr[2])), BOT
	assert False, f'bad expression {expr}'

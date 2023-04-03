import torch as T
import gc
import pathlib
from time import time
from math import ceil

cuda = T.cuda.is_available()
dev = T.device('cuda' if cuda else 'cpu')

def invdict(dict):
	return {v: k for k, v in dict.items()}

def mkdirp(path):
	pathlib.Path(path).mkdir(parents=True, exist_ok=True)

def tick():
	return time()

def tock(t):
	return time() - t

def tocks(t):
	return f'{tock(t):.2f}s'

def timestr():
	from datetime import datetime
	return datetime.now().strftime('%Y%m%dT%H%M%S')
	
def append_unique(x, list, set):
	if x not in set:
		list.append(x)
		set.add(x)

def jointshuffle(rng, *arrs):
	n = len(arrs[0])
	perm = rng.permutation(n)
	result = []
	for arr in list(arrs):
		assert len(arr) == n
		result.append([arr[i] for i in perm])
	return result

class RunningMean:
	def __init__(self):
		self.numerator = self.denominator = 0
		
	def append(self, value, *, weight=1):
		self.numerator += value
		self.denominator += weight
		
	def get(self, default=float('nan')):
		if self.denominator == 0: return default
		return self.numerator / self.denominator
	
	def __str__(self):
		return 'nan' if self.denominator == 0 else str(self.get())

class Countdown:
	def __init__(self, initial):
		self.value = self.initial = initial
		
	def step(self, condition):
		# rename to step_and_check
		"""
		Returns True if countdown reached the end.
		If `condition` is true, then the countdown is reset, otherwise, the counter is decremented
		"""
		if condition:
			self.value = self.initial
		else:
			self.value -= 1
		return self.value <= 0

class dotdict(dict):
	__getattr__ = dict.get
	__setattr__ = dict.__setitem__
	__delattr__ = dict.__delitem__

def freeze(x):
	for param in x.parameters(): param.requires_grad = False

def unfreeze(x):
	for param in x.parameters(): param.requires_grad = True
		
def paramcount(x):
	return sum(T.numel(p) for p in x.parameters())

def minibatches(permutation, batch_size):
	batch_count = int(ceil(len(permutation) / batch_size))
	result = []
	for batch_idx in range(batch_count):
		result.append(permutation[batch_idx*batch_size:][:batch_size])
	return result

def fmtprog(x, y):
	max_width = len(str(y))
	return str(x).rjust(max_width, '0') + '/' + str(y)

def fmtpair(key, val):
	key = key.replace('_', ' ')
	if isinstance(val, tuple) and len(val) == 2:
		val = fmtprog(val[0]+1, val[1]) # interpret as progress (val[0]+1 out of val[1] done)
	elif isinstance(val, float):
		val = f'{val:.4f}'
	else:
		val = str(val)
	return key + ' ' + val

def printlog(*, sep=' | ', end='', **kwargs):
	print('\r' + sep.join(fmtpair(k, v) for k, v in kwargs.items()), end=end)

def dumpcuda():
	for obj in gc.get_objects():
		try:
			if (T.is_tensor(obj) or T.is_tensor(obj.data)) and (not cuda or obj.is_cuda):
				del obj
		except Exception as e:
			pass
	gc.collect()
	if cuda:
		T.cuda.empty_cache()

def save_model_to_py(input_history, *, dataset, path, last=False):
	if last:
		input_history = reversed(input_history)

	for cell in input_history:
		if cell.startswith('#!MODEL'): break
	else:
		print("WARN: Didn't find model cell!")
		return

	with open(path, 'w') as f:
		f.write(f"# dataset_path = '{dataset}'\n")
		f.write(cell)

def check_axiom_safe(reasoner, axiom):
	try:
		return int(reasoner.check_axiom(axiom))
	except:
		#print('timeout in check_axiom')
		return None

def add_axiom_safe(reasoner, axiom):
	reasoner.add_axiom(axiom)

	try:
		consistent = reasoner.is_consistent()
		timeout = False
	except Exception as e:
		print(f'error during is_consistent {e}')
		timeout = True

	if timeout or not consistent:
		reasoner.retract_last()
		return False
		
	return True

def timeout(start_time, progress, min_speed):
	elapsed = tock(start_time)
	return elapsed > min_speed and progress / elapsed < min_speed

def rprint(*args, sep=' '):
	print('\r' + sep.join(args), end='', flush=True)

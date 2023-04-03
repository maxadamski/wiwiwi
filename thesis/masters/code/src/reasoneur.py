import numpy as np
import pandas as pd
import torch as T
import torch.nn.functional as F
import torch.nn as nn
from sklearn import metrics

from src.ontofun import *
from src.syntax import *
from src.utils import *
from src.vis import *

class ReasonerPackage:
    def __init__(self, reasoner, embedding):
        self.reasoner, self.embedding = reasoner, embedding
        
    def encode(self, expr):
        return self.reasoner.encode(core(expr), self.embedding).detach().numpy()
        
    def check(self, axiom):
        return T.sigmoid(self.reasoner.classify_batch([core(axiom)], [self.embedding]))[0].item()
    
    def check_sub(self, c, d):
        return self.check((SUB, c, d))

class NNExprEncoder(nn.Module):
	def __init__(self, *, expr_size, n_concepts, n_roles):
		super().__init__()
		self.n_concepts = n_concepts
		self.n_roles = n_roles
		self.expr_size = expr_size
		self.top_concept = nn.Parameter(T.zeros(1, expr_size))
		self.concepts = nn.Parameter(T.zeros((n_concepts, expr_size)))
		self.roles = nn.ModuleList([nn.Linear(expr_size, expr_size) for _ in range(n_roles)])
		for x in [self.concepts, self.top_concept]:
			nn.init.xavier_normal_(x)
		
	@classmethod
	def from_onto(cls, onto, *args, **kwargs):
		return cls(n_concepts=onto.n_concepts, n_roles=onto.n_roles, *args, **kwargs)
	
class ContextFreeReasoner(nn.Module):
	def __init__(self, *, expr_size, hidden_size):
		super().__init__()
		self.hidden_size, self.expr_size = hidden_size, expr_size
		self.not_nn = nn.Linear(expr_size, expr_size)
		self.and_nn = nn.Linear(2*expr_size + expr_size**2, expr_size)
		self.sub_nn = nn.Sequential(
			nn.Linear(2*expr_size + expr_size**2, hidden_size),
			nn.ELU(),
			nn.Linear(hidden_size, 1),
		)
		self.bot_concept = nn.Parameter(T.zeros((1, expr_size)))
		nn.init.xavier_normal_(self.bot_concept)
			
	def encode(self, expr_, embeddings):
		def rec(expr):
			if expr is TOP:
				return T.tanh(embeddings.top_concept[0])
			if expr is BOT:
				return T.tanh(self.bot_concept[0])
			if isinstance(expr, int):
				return T.tanh(embeddings.concepts[expr])
			
			if expr[0] == SUB:
				c = rec(expr[1])
				d = rec(expr[2])
				cxd = T.outer(c, d).view(-1)
				return self.sub_nn(T.cat((c, d, cxd)))
			elif expr[0] == AND:
				c = rec(expr[1])
				d = rec(expr[2])
				cxd = T.outer(c, d).view(-1)
				return T.tanh(self.and_nn(T.cat((c, d, cxd))))
			elif expr[0] == ANY:
				c = rec(expr[2])
				r = embeddings.roles[expr[1]]
				return T.tanh(r(c))
			elif expr[0] == NOT:
				c = rec(expr[1])
				return T.tanh(self.not_nn(c))
			else:
				assert False, f'Unsupported expression {expr}. Did you convert it to core form?'
		return rec(expr_)
	
	def classify_batch(self, axioms, embeddings):
		return T.vstack([self.encode(axiom, emb) for axiom, emb in zip(axioms, embeddings)])
	
	def classify(self, axiom, emb):
		return self.classify_batch([axiom], [emb])[0].item()

def batch_stats(Y, y, **other):
	K = Y > 0.5
	roc_auc = metrics.roc_auc_score(y, Y)
	acc = metrics.accuracy_score(y, K)
	f1 = metrics.f1_score(y, K)
	prec = metrics.precision_score(y, K)
	recall = metrics.recall_score(y, K)
	return dict(acc=acc, f1=f1, prec=prec, recall=recall, roc_auc=roc_auc, **other)

def eval_batch(reasoner, encoders, X, y, onto_idx, indices=None, *, backward=False, detach=True):
	if indices is None: indices = list(range(len(X)))
	emb = [encoders[onto_idx[i]] for i in indices]
	X_ = [core(X[i]) for i in indices]
	y_ = T.tensor([y[i] for i in indices]).unsqueeze(1)
	Y_ = reasoner.classify_batch(X_, emb)
	loss = F.binary_cross_entropy_with_logits(Y_, y_, reduction='mean')
	if backward:
		loss.backward()
	Y_ = T.sigmoid(Y_)
	if detach:
		loss = loss.item()
		y_ = y_.detach().numpy()
		Y_ = Y_.detach().numpy().reshape(-1)
	return loss, y_, Y_

def train(data_tr, data_vl, reasoner_with_optim, encoders_with_optims, history_tr=None, history_vl=None, *, epoch_count=15 + 1, batch_size=32):
	idx_tr, X_tr, y_tr = data_tr
	idx_vl, X_vl, y_vl = data_vl
	reasoner, reasoner_optim = reasoner_with_optim
	encoders, optimizers = encoders_with_optims
	if history_tr is None: history_tr = []
	if history_vl is None: history_vl = []

	total_batch = 0
	for epoch_idx in range(epoch_count):
		# --- TRAINING
		mean_train_loss = RunningMean()
		batches = minibatches(T.randperm(len(X_tr)), batch_size)
		t = tick()
		for batch_idx, idxs in enumerate(batches):
			for encoder_optim in optimizers:
				encoder_optim.zero_grad()
			reasoner_optim.zero_grad()

			loss, yb, Yb = eval_batch(reasoner, encoders, X_tr, y_tr, idx_tr, idxs, backward=epoch_idx > 0)

			for encoder_optim in optimizers:
				encoder_optim.step()
			reasoner_optim.step()

			mean_train_loss.append(loss)
			history_tr.append(dict(epoch=epoch_idx, batch=total_batch, batch_loss=loss, elapsed=tock(t)))
			printlog(train_epoch=(epoch_idx, epoch_count), batch=(batch_idx, len(batches)), loss=mean_train_loss.get(), elapsed=tocks(t))
			total_batch += 1

		# --- VALIDATION
		with T.no_grad():
			val_loss, y, Y = eval_batch(reasoner, encoders, X_vl, y_vl, idx_vl)
			stats = batch_stats(Y, y)
			history_vl.append(dict(epoch=epoch_idx, batch=total_batch, val_loss=val_loss, **stats, elapsed=tock(t)))
			printlog(train_epoch=(epoch_idx, epoch_count), batch=(batch_idx, len(batches)), loss=mean_train_loss.get(), val_loss=val_loss, **stats, elapsed=tocks(t))

		print()

def test(*, test_data, test_onto, reasoner, reasoner_name, encoders, optimizers, test_hist, test_hist_by_onto, epoch_count, batch_size, freeze_reasoner=True, reasoner_optim=None):
	idx_te, X_te, y_te = test_data
	if freeze_reasoner: freeze(reasoner)
	
	for epoch_idx in range(epoch_count):
		mean_loss = RunningMean()
		batches = minibatches(T.randperm(len(X_te)), batch_size)
		t = tick()
		for batch_idx, idxs in enumerate(batches):
			for optim in optimizers:
				optim.zero_grad()
			if reasoner_optim:
				reasoner_optim.zero_grad()
			
			test_loss, y, Y = eval_batch(reasoner, encoders, X_te, y_te, idx_te, idxs, backward=epoch_idx > 0)

			for optim in optimizers:
				optim.step()
			if reasoner_optim:
				reasoner_optim.step()

			mean_loss.append(test_loss)
			test_hist.append(dict(test_epoch=epoch_idx, batch=batch_idx, batch_loss=test_loss, reasoner=reasoner_name, elapsed=tock(t)))
			printlog(test_epoch=(epoch_idx, epoch_count), batch=(batch_idx, len(batches)), test_loss=mean_loss.get(), elapsed=tocks(t))

		t_train = tock(t)
		with T.no_grad():
			test_loss, y, Y = eval_batch(reasoner, encoders, X_te, y_te, idx_te)
			stats = batch_stats(Y, y)
			printlog(test_epoch=(epoch_idx, epoch_count), batch=(batch_idx, len(batches)), test_loss=test_loss, **stats, elapsed=tocks(t))

			for onto_idx, onto in enumerate(test_onto):
				mask = np.array(idx_te) == onto_idx
				stats = batch_stats(Y[mask], y[mask], test_epoch=epoch_idx, onto=onto_idx, reasoner=reasoner_name, elapsed=t_train)
				test_hist_by_onto.append(stats)
				
		print()
		
	if freeze_reasoner: unfreeze(reasoner)

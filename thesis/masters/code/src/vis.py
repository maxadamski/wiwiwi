import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
from sklearn import metrics

sns.set_context('talk')
sns.set_style('white')
plt.rc('figure', figsize=(15, 8))

def plot_train_history(train_history, valid_history, save=False):
	fig, (ax1, ax2) = plt.subplots(nrows=2, sharex=True)
	plt.xticks(valid_history.epoch, valid_history.epoch, rotation=90)
	sns.lineplot(x='epoch', y='batch_loss', data=train_history, ci='sd', label='Training loss', ax=ax1)
	ax1.plot(valid_history.val_loss, label='Validation loss')
	ax1.legend(loc='upper right')
	ax1.set_ylabel('BCE loss')
	ax1.grid()

	ax2.plot(valid_history.roc_auc, label='Validation AUC ROC')
	ax2.set_xlim(0, valid_history.epoch.max())
	ax2.set_ylim(ymax=1)
	ax2.set_ylabel('AUC ROC')
	ax2.set_xlabel('Epoch')
	ax2.legend(loc='lower right')
	ax2.grid()

	if save: plt.savefig(save)
	plt.show()

def plot_test_history(test_history, test_history_by_onto, max_epochs=None, save=False):
	fig, (ax1, ax2) = plt.subplots(nrows=2, sharex=True)
	epochs = test_history.test_epoch.max()
	plt.xticks(range(epochs + 1), rotation=90)
	sns.lineplot(x='test_epoch', y='batch_loss', hue='reasoner', ci='sd', data=test_history, ax=ax1)
	ax1.set_ylabel('BCE loss')
	ax1.legend(loc='upper right')
	ax1.grid()

	sns.lineplot(x='test_epoch', y='roc_auc', hue='reasoner', ci=None, data=test_history_by_onto, ax=ax2)
	#sns.boxplot(x='test_epoch', y='roc_auc', hue='reasoner', data=test_history_by_onto, ax=ax2)
	sns.lineplot(x='test_epoch', y='roc_auc', hue='reasoner', units='onto', estimator=None, alpha=0.4, ls='--', lw=1, legend=False, data=test_history_by_onto, ax=ax2)
	ax2.set_ylabel('AUC ROC')
	ax2.set_xlabel('Test epoch')
	ax2.legend(loc='lower right')
	if max_epochs is None: max_epochs = epochs
	ax2.set_xlim(0, max_epochs)
	ax2.set_ylim(ymax=1)
	ax2.grid()

	if save: plt.savefig(save)
	plt.show()

def test_metrics(y, Y, thresh=0.5):
	return dict(
		acc=metrics.accuracy_score(y, Y > thresh),
		prec = metrics.precision_score(y, Y > thresh),
		recall = metrics.recall_score(y, Y > thresh),
		f1=metrics.f1_score(y, Y > thresh),
		auc_roc=metrics.roc_auc_score(y, Y),
		ap=metrics.average_precision_score(y, Y),
	)

def report(ontos, y, Y, idx, thresh=0.5, save=False):
	fig, (ax1, ax2) = plt.subplots(ncols=2)
	Y = Y.reshape(-1)
	y = np.array(y)
	
	ms = []
	for onto_idx in range(len(ontos)):
		mask = np.array(idx) == onto_idx
		y_onto, Y_onto = y[mask], Y[mask]
		fpr, tpr, _ = metrics.roc_curve(y_onto, Y_onto)
		prec, recall, _ = metrics.precision_recall_curve(y_onto, Y_onto)
		ax1.plot(fpr, tpr, alpha=0.3, lw=1, color='tab:blue')
		ax2.plot(recall, prec, alpha=0.3, lw=1, color='tab:blue')
		ms.append(test_metrics(y_onto, Y_onto, thresh))
		
	fpr, tpr, _ = metrics.roc_curve(y, Y)
	prec, recall, _ = metrics.precision_recall_curve(y, Y)
	ax1.plot(fpr, tpr, lw=3, color='tab:blue')
	ax2.plot(recall, prec, lw=3, color='tab:blue')
	ax1.set_xlabel('False Positive Rate'); ax1.set_ylabel('True Positive Rate')
	ax2.set_xlabel('Recall'); ax2.set_ylabel('Precision')

	for ax in (ax1, ax2):
		ax.set_xlim(-0.02, 1.02)
		ax.set_ylim(-0.02, 1.02)
		ax.set_aspect('equal')
		ax.grid()

	if save: plt.savefig(save.replace('%', 'curves') + '.png')
		
	disp = metrics.ConfusionMatrixDisplay.from_predictions(y, Y > thresh, normalize='all', cmap='cividis')
	if save: plt.savefig(save.replace('%', 'matrix') + '.png')
	
	df = pd.DataFrame(ms).agg(['mean', 'std']).T
	df['micro'] = pd.Series(test_metrics(y, Y, thresh))
	if save: df.round(4).to_csv(save.replace('%', 'stats') + '.csv')

	return df

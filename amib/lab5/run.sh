export FRAMS_DIR=$HOME/Desktop/amib/frams

n_proc=5
id=0
prefix=pxov$1-pmut$2
iters=10

for algo in eaSimple eaMuPlusLambda eaMuCommaLambda; do
	for iter in $(seq 1 1 $iters); do
		((id = id % n_proc)); ((id++ == 0)) && wait
		python main.py \
			-path $FRAMS_DIR \
			-sim "eval-allcriteria.sim;deterministic.sim;sample-period-2.sim;only-body.sim" \
			-opt vertpos \
			-max_numparts 30 \
			-genformat 1 \
			-popsize 100 \
			-generations 100 \
			-algo $algo \
			-pxov $1 \
			-pmut $2 \
			-hof_size 1 \
			-hof_savefile "out/$algo-$prefix-$iter" &
	done
done

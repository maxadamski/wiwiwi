export FRAMS_DIR=$HOME/Desktop/amib/frams

n_proc=5
id=0
prefix=jump-f0
iters=10

for iter in $(seq 1 1 $iters); do
	((id = id % n_proc)); ((id++ == 0)) && wait
	python main.py \
		-path $FRAMS_DIR \
		-sim "eval-allcriteria.sim;deterministic.sim;jump.sim;recording-body.sim" \
		-genformat 0 \
		-popsize 100 \
		-generations 400 \
		-hof_size 1 \
		-hof_savefile "out/$prefix-$iter" &
done

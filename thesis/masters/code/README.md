# Neural Reasoner and Concept Embeddings

## How to run

1. Install PyTorch 1.10.2 any way you like (for example with conda)
2. Install pip requirements

```
pip install -r requirements.txt
```

3. Build bindings for FaCT++

Assumes that C Python headers are installed globally or in a conda environment

```
scripts/build
```

4. Run a file

```
python -m src.generate
```

Or run a jupyter notebook


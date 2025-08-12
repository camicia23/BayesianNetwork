# Project for the "Computer Science with Laboratory" course exam, UniPi, 2024/2025

This project allows to compute marginal and conditional probabilities given a discrete Bayesian Network (specified in a BIF file where CPT lines are written in inverse lexicographical order, see ```data/```). To do so, it implements the Variable Elimination algorithm. I also made a simple Python code with the library pgmpy that computes the same thing to double-check the results.


To compile and run:

`make run`

To remove the directories obj/ and bin/:

`make clean`


To use the Python code:

```
cd python_version
python3.11 -m venv venv
source venv/bin/activate
pip install -r requirements.txt
```



## Data

Files in the `data/` folder are taken from the [Bayesian Network Repository](https://www.bnlearn.com/bnrepository/) by Marco Scrutari.

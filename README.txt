# 2017iDASHReplication

A program that trains a logistic model using Homomorphic encryption.
Testing the model is not homomorphic.

Directions to run (from this directory)
    cd build
    make
    heTraining filename.txt

filename.txt should be a csv
    - each row is a subject
    - each column is the same field
    - no holes in data
    - the first column is in {-1,1} and will be the dependent var
    - there is no checking in the csv. Reading is not robust

Output:
    - parameters of logistic model from training in ciphertext
    - final performance of model with test set + test set size
    - graph indicating convergence with test set and training set
        - performance vs epoch

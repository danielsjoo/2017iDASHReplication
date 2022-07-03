# 2017iDASHReplication

A program that trains a logistic model using Homomorphic encryption from the following paper:
https://bmcmedgenomics.biomedcentral.com/track/pdf/10.1186/s12920-018-0401-7.pdf

Requirements to run:
    1. clone this repository
    2. install the SEAL library
    3. (for now): edit SEAL hestdparms.h line 24 so that it returns 1000.

Directions to run (from root directory)
    mkdir build
    cd build
    cmake -S .. -B .
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
        
CURRENT STATUS
    - Training implementation is not finished. HE multiplication with plaintext not understood
    - Data tool to test for convergence not written

7/2
    - Training loop is finished with the exception of step 5, which is in plaintext
    - Must check for convergence. Will write this implementation first
    - Need to check for best epoch and learning rate
    - I observed that by breaking the seal library for the poly_modulus_degree=1024, we can rescale past the levels
    permitted by the coeff modulus. Mathematically, i don't know what I am doing, but the result looks fine.


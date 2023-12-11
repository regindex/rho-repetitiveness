# SLT-factorization

### Overview

Computes the size of the Suffix-Link-Tree (SLT) factorization of the text.

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208

### Install

~~~~
git clone https://github.com/nicolaprezza/SLT-factorization
cd SLT-factorization
mkdir build
cd build
cmake ..
make
~~~~

### Run

Input: a BWT in ASCII format, containing only characters A,C,G,T,N,# (the latter is the string terminator).

To compute the size of the SLT factorization given as input a BWT, run

~~~~
size-from-bwt -i bwt
~~~~
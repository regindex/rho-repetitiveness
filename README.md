# Rho repetitiveness measure

### Overview

Computes the rho repetitiveness measure (work in progress)

### Funding

This project has received funding from the European Research Council (ERC) under the European Union’s Horizon Europe research and innovation programme, project REGINDEX, grant agreement No 101039208

### Install

~~~~
git clone https://github.com/nicolaprezza/rho
cd rho
mkdir build
cd build
cmake ..
make
~~~~

### Run

Input: a BWT in ASCII format, containing only characters A,C,G,T,N,# (the latter is the string terminator).

To compute rho given as input a BWT, run

~~~~
rho -i bwt
~~~~

#!/bin/bash
python -m venv iowarp
source iowarp/bin/activate
pip3 install pyarrow pandas -q
python ../test/lambda.py $1 $2


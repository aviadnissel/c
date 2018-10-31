#!/bin/bash

for i in {0..11}; do echo $i; ../ex1 tests/test$i >& solutions/my; ~labc/www/ex1/AnalyzeProtein tests/test$i >& solutions/ptbs; diff solutions/*; rm -f solutions/* ; done

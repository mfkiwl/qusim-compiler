gatedef testgate
ry 0,-1.5707963267949
ry 1,-1.5707963267949
cnot 0,1
endgatedef
gatedef swap
cnot 0,1
cnot 1,0
cnot 0,1
endgatedef
qubit 0
qubit 1
qubit 2
cnot 0,1
testgate 0,2
swap 1,2

#!/bin/bash

#Variables
pcode="pcode.lsd12"
count=0
max=6

#Test cases
while [ $count -lt $max ]
do
	tmp="test/case$count.lsd12"
	echo '--- ' $tmp ' ---'
	./lsd12 < $tmp > $pcode
	count=$(( $count + 1 ))
done

#cat testcase/case6.lsd12 | ./lsd12 1> test.pcode 2> test.err
#echo --- case6.lsd12 --- result :
#cat test.err


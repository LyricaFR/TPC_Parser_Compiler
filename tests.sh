#!/bin/bash
good_file=`ls test/good/*.tpc`
syn_err_file=`ls test/syn-err/*.tpc`
sem_err_file=`ls test/sem-err/*.tpc`
warn_file=`ls test/warn/*.tpc`
{
echo "Good tests :"

for each in $good_file;
do
	echo -n $each : "" 
	./bin/tpcc < $each
	echo $?
done

echo ""
echo "Syn Err tests :"

for each in $syn_err_file;
do
	echo -n $each : "" 
	./bin/tpcc < $each
	echo $?
done

echo ""
echo "Sem Err tests :"

for each in $sem_err_file;
do
	echo -n $each : "" 
	./bin/tpcc < $each
	echo $?
done

echo ""
echo "Warn tests :"

for each in $warn_file;
do
	echo -n $each : "" 
	./bin/tpcc < $each
	echo $?
done

} > rapport.txt

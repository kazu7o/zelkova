for n in 1000 10000 100000 1000000 10000000 100000000
do
  date
  ./main $n sorted-$n
done

#!/shell
ls /
echo hi
mkdir d
cd d
ls
/ls
/echo bye
exit

$ ./test.sh # notice how we didn't have to run 'crash' here
(ls / contents appear)
hi
exec ls failed
.              1 35 32
..             1 1 1024
bye
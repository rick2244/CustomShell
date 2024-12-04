This will hold the answers for lab 7
1. /dev/nvme0n1p1

2. brw-rw---- 1 root disk 259, 1 Nov 21 22:37 /dev/nvme0n1p1
The difference about this one compared to most is the flag
It is directly mounted on the root

3. The file is 105 mb. It took 1.0985 seconds to write this file to the disk.

4. 255584 inodes were created 

5. 88 percent of blocks are free blocks

6. There is an empty directory inside of it since it points back to the root

7. The output of whoami.txt:70747aab603b3f2d2de530e659291ecc -

the output of ls -l: 
total 428
drwxr-xr-x  3 rjdzreke students       115 Jan 14  2023 C-project01
drwxr-xr-x  3 rjdzreke students        90 Jan 14  2023 C-project02
drwxr-xr-x  3 rjdzreke students        90 Jan 14  2023 C-project03
drwxr-xr-x  3 rjdzreke students       186 Jan 14  2023 C-project04
drwxr-xr-x  3 rjdzreke students       114 Jan 14  2023 C-project05
drwxr-xr-x  3 rjdzreke students        26 Jan 14  2023 Java-project02
-rw-r--r--  1 rjdzreke students        17 Oct  4  2022 README.md
drwxr-xr-x  5 rjdzreke students       177 Sep  1  2022 autograder
-rw-r--r--  1 rjdzreke students       170 Aug 26  2023 config
drwxr-xr-x  3 rjdzreke students        37 Oct 11  2023 cs414
-rw-r--r--  1 rjdzreke students        97 Aug 30  2022 demo1
-rwxr-xr-x  1 rjdzreke students     27168 Aug 30  2022 demo2
-rw-r--r--  1 rjdzreke students       149 Aug 30  2022 demo2.c
-rw-r--r--  1 rjdzreke students         0 Dec  2 12:21 fileA
-rw-rw-rw-  1 rjdzreke students         0 Dec  2 12:21 fileB
----------  1 rjdzreke students         0 Dec  2 12:22 fileC
drwxr-xr-x  2 rjdzreke students        10 Dec  2 12:21 fs
-rw-r--r--  1 rjdzreke students       363 Nov  9  2023 hypo.S
-rw-------  1 rjdzreke students       464 Aug 23  2023 id_ed25519_cs315_2023f
-rw-r--r--  1 rjdzreke students       105 Aug 23  2023 id_ed25519_cs315_2023f.pub
drwxr-xr-x 15 rjdzreke students       280 Nov 21  2023 inclass
drwxr-xr-x  3 rjdzreke students       124 Nov 13  2022 lab01-rick2244
drwxr-xr-x  3 rjdzreke students        82 Sep 20  2022 lab02-rick2244
drwxr-xr-x  3 rjdzreke students       124 Sep 20  2022 lab03-rick2244
drwxr-xr-x  3 rjdzreke students       122 Oct  4  2022 lab04-rick2244
drwxr-xr-x  3 rjdzreke students       174 Oct 27  2022 lab05-rick2244
drwxr-xr-x  3 rjdzreke students       249 Nov 29  2022 lab06-rick2244
drwxr-xr-x  3 rjdzreke students       120 Nov 29  2022 lab07-rick2244
drwxr-xr-x  3 rjdzreke students       158 Dec  4  2022 lab08-rick2244
-rw-r--r--  1 rjdzreke students 104857600 Dec  2 12:09 my-disk
lrwxrwxrwx  1 rjdzreke students        26 Sep 20 12:12 os -> /raid/rjdzreke/os-rick2244
drwxr-xr-x  7 rjdzreke students       335 Oct 16 10:56 os-rick2244
-rw-r--r--  1 rjdzreke students      4586 Oct 24  2023 output.json
-rw-r--r--  1 rjdzreke students       105 Nov 15  2023 output.lir
drwxr-xr-x  3 rjdzreke students       129 Jan 14  2023 project01-rick2244
drwxr-xr-x  3 rjdzreke students        70 Jan 14  2023 project02-rick2244
drwxr-xr-x  3 rjdzreke students       154 Oct  6  2022 project02-rick2244-backup
drwxr-xr-x  3 rjdzreke students        70 Jan 14  2023 project03-rick2244
drwxr-xr-x  3 rjdzreke students       164 Jan 14  2023 project04-rick2244
drwxr-xr-x  3 rjdzreke students        92 Jan 14  2023 project05-rick2244
drwxr-xr-x  2 rjdzreke students        10 Apr  6  2022 project3
drwxr-xr-x  2 rjdzreke students        10 May  2  2022 project4
-rw-------  1 rjdzreke students      2622 Jan 22  2024 public_cs326_key
-rw-r--r--  1 rjdzreke students       582 Jan 22  2024 public_cs326_key.pub
-rw-r--r--  1 rjdzreke students         0 Jan 22  2024 rjdzreke
drwx------  3 rjdzreke students        27 Aug 30  2022 snap
-rw-r--r--  1 rjdzreke students       160 Nov  7  2023 test.S
drwxr-xr-x 14 rjdzreke students       273 Sep  1  2022 test.bak
-rw-r--r--  1 rjdzreke students       320 Nov 10  2023 test.cb
-rwxr-xr-x  1 rjdzreke students      9184 Nov  7  2023 test.exe
-rw-r--r--  1 rjdzreke students       160 Nov  7  2023 test.s
-rw-r--r--  1 rjdzreke students        59 Nov 15  2023 test_simple2.cb
drwxr-xr-x 12 rjdzreke students       214 Nov 13  2022 tests
-rw-r--r--  1 rjdzreke students        29 Nov 15  2023 testsimple.cb
-rw-------  1 rjdzreke students        36 Dec  2 12:22 whoami.txt

The differences that the unmask make:
unmask 000 means not bits are masked so follwing files and 
directories will have their default mask
unmask 777 masks out all bits, which means that no permission for anyone, including reading or writing
unmask 077 sets permission to allow only the owner to read and write

8. an unmask that would set the default permissions for files to -------r-- is unmask 662

9. 4311028705 

10. The process start by checking if the path can be opened at first and if it can be stat. After that a switch case is performed on the typw of the inode, It printed out the information directly if it is a file. If it is a directory it check if the path is too long, than copies the path name. After words it loops through the directory and prints out the fiels that are in the directory.


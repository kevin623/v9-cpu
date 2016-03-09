#!/bin/sh
rm -f xc xem emhello funcall lab1
gcc -o xc -O3 -m32 -Ilinux -Iroot/lib root/bin/c.c
gcc -o xem -O3 -m32 -Ilinux -Iroot/lib root/bin/em.c -lm
#./xc -o emhello -Iroot/lib root/usr/emhello.c
#./xc -o funcall -Iroot/lib root/usr/funcall.c
./xc -o lab1 -Iroot/lib mylab/lab1/lab1.c
./xc -o lab2 -Iroot/lib mylab/lab2/lab2.c
#./xem emhello
#./xem funcall
./xem lab1
./xem lab2

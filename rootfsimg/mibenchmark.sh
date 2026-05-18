#!/bin/sh

pwd
ls
# echo 12345
# /root/benchmark/stream
# ifconfig -a
# /root/redis/redis-serve
# free -mh
# ls /dev

insmod /root/penglai/penglai.ko 
cat /proc/misc
lsmod
# major: 10 - misc dev; minor: 63 - specify the penglai dev
mknod /dev/penglai_enclave_dev c 10 63 
ls /dev
echo Penglai Enclave Start
# /root/hostc /root/prime
echo -------------------------------------------------------
# /root/host /root/sha
dmesg -n 1
cd /root
# time find . -name *asc
# ls

# sha
echo "----------------------sha--------------------------"
time ./mibench/sha /root/mibench/sha_input_small.asc | tee -a test.log
time ./hostc sha -fi /root/mibench/sha_input_small.asc | tee -a test.log

# rijndael enc
echo "-------------------rijndael enc----------------------"
time ./mibench/rijndael /root/mibench/sha_input_small.asc output_small.enc e 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321 | tee -a test.log
time ./hostc ./rijndael -fi /root/mibench/sha_input_small.asc -fo output_small.enc e 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321 | tee -a test.log

# rijndael dec
echo "-------------------rijndael dec----------------------"
time ./mibench/rijndael output_small.enc output_small.dec d 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321 | tee -a test.log
time ./hostc ./rijndael -fi output_small.enc -fo output_small.dec d 1234567890abcdeffedcba09876543211234567890abcdeffedcba0987654321 | tee -a test.log

# bitcnts
echo "---------------------bitcnts------------------------"
time ./mibench/bitcnts 20000000 | tee -a test.log
time ./hostc ./bitcnts 20000000 | tee -a test.log

./analyzer.sh test.log

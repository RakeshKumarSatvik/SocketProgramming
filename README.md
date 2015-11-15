# SocketProgramming - EE450

Problem Statement: 

In  this  project  you  will  be  simulating  a  system  similar  to  Dijkstra  to build a map of  the 
network  topology.  A  set  of  neighbor information will  be distributed among  four  servers. 
There  will  also  be  a  client  who  is  going  to  initiate  the  process  and  also  do  some 
computation  to   give  the  final  topology  output.  The  client  and  servers  are  going  to 
communicate  with  each  other  to  get  the  neighbor  informations  from  the  others  and 
combine  all  this  information together  to build the entire map of the network. The original 
map  is a connected  graph, which may contain  cycles  (or  loops). So  later, based on the 
map, the client  will  calculate the minimum spanning tree (MST) of the network and print 
it out on the terminal. 
The  servers  will  communicate  with  the  client  using  TCP  sockets.  The  client  will  
communicate with the servers using UDP sockets.  

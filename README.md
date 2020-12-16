# Distributed Key-Value Server Using CHORD protocol
****************************************************

## Guidelines
1. 2 Byte Key is used so it can support Upto 64K peer
2. .csv file is the Key-Value database of the peer and name is the HEX key assigned to the server
3. client.xml is the format in which, it is communicate with the server
4. server.xml is the response from the server
5. To start the peer compile and run `server.c` on different terminal
6. Compile and run the `client.c` and send the KEY and VALUE for Insert,Get,Update and Delete
7. For the better performance we have used 4-way set associative cache.

## for paper [a link] https://www.researchgate.net/publication/228057545_Chord_A_scalable_peer-to-peer_lookup_protocol_for_Internet_applications
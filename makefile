CC=gcc
CFLAGS=-std=c99 -Wall -pedantic-errors

packet_dissector_ipv4: packet_dissector_ipv4.c
	$(CC) $(CFLAGS) -o packet_dissector_ipv4 packet_dissector_ipv4.c

clean:
	rm -f packet_dissector_ipv4

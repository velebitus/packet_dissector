# packet_dissector
Simple IPv4 packet dissector

Before starting to solve this problem, I was aware that there was a gap in my
knowledge of the C language. So, I decided to read the book 
"Practical C Programming" by Steve Oualline. I enjoyed reading the book as it 
brought me closer to understanding what it is like to be a programmer in practice. 
The book helped me refresh my C syntax and learn a bunch of new objects. 

One thing that I already suspected and  affirmed in the book is that 
programming is not so much about the programming language you use. 
Writing the program is just the last step of the process. 
First, you need to understand the problem, do research, find the best approach 
to the solution, create documentation and then execute it by writing the program. 
The book explains other useful topics such as makefile, debugging, testing, 
style, the importance of code readability and much more. 

Of course, everything we read in a book that is a proposition on how 
to do certain things is just that - a proposition. The workflow and practices 
used are determined by the given problem and the group of people working together.
What I want to say is that in a book we can aquire a general direction but 
not an absolute direction.

Let us now dive into the problem. I had a basic idea of how a packet looks like 
and how the life of a packet progresses in the OSI model from the Application 
layer down to the Physical layer. I knew that on Layer 3, an IP header gets 
appended, and on Layer 2, an Ethernet header gets appended. Furthermore, I was 
aware that the IP header contains IP addresses, while the Ethernet header 
contains MAC addresses. Since the Ethernet header gets appended last, 
I assumed that the first bytes in the given array contained MAC addresses. 
The next step was to research my assumption.

## Research:

- Ethernet frame header format:
   
1. Destination MAC Address: 6-bytes
2. Source MAC Address: 6-bytes 
3. Type: 2-bytes that represents protocol used. 
         08 00 - IPv4
         86 dd - IPv6
4. Data: variable 46 - 1500 bytes that contains 
      encapsulated data from higher layers
5. Frame Check Sequence (FCS): 4-bytes that are used to 
      detect errors in frame
   

From the Ethernet frame, we need to extract the destination and source MAC 
addresses, which are the first 12 bytes.

                
   ![image](https://github.com/ivZalac98/packet_dissector/assets/98355693/d5dc9c33-60b2-41e8-8b58-31c302f17252)


To extract the source and destination addresses from the IPv4 header, we need 
to look at the bytes in the range [12, 20] of the IPv4 header.

The next was to find ways to test if the given packet is valid. Since we 
are dealing with the IPv4 header, we can use the following methods:

- Use the type field of the frame
- Use the version field of the IPv4 header
- Use the IPv4 header checksum to validate integrity

While writing the program, there were several things that I kept in mind:

- It needs to be portable
- It needs to be readable by another programmer
- It needs to be expandable if other data from the frame would be 
  needed in the future
- I need to learn as much as I can

After writing the program, the last step was creating the makefile.

I tested the program by creating binary files of frames, one with IP header
options and one without. The files were redirected into stdin. File with options
has the same mac addresses as file without options.

- Sources: 

1. "Practical C Programming" by Steve Oualline
2. https://www.rfc-editor.org/rfc/rfc791#section-3.2
3. https://amgedwageh.medium.com/network-packet-manual-analysis-63e25f32ea54
4. https://www.cs.miami.edu/home/burt/learning/Csc524.122/notes/networking-packet.html

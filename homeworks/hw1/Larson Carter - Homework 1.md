<!--
This is homework for preparing for work on fling.

Compile this into a pdf using

  $ pandoc README.md -o READMER.pdf --pdf-engine=xelatex

-->

# Homework 1: Networking and Basic Security

## Part 1: Networking Concepts

This project involves signal processing, networking, web technologies, 
and machine learning.   This homework focuses on Internet technologies.

On separate paper or in a file, write the answer to each of these questions.

1.1. What is a packet header?  Describe each field and its function.

- A packet header is a unique identification that provides details on the particular packets contents, the origin and the end destination of the packet.

- Version: What protocl of the IP stack we are using either IPv4 or IPv6 which shows the decimal value so either 4 or 6

- Header Length: 4 bit field that can tell us the length of the IP header in 32 bit increments. Minimum length of 20 bytes with 32 bit incremenets.

- Type of Service: This can be used for QoS on traffic shaping on Routers, Firewalls, and other networking equipment. There are 8 bits that are utilized to identify the type of service

- Total Length: this is a 16 bit field that can help identify the total IP Packet size in bytes. Minimum size is 20 bytes which means that the packet has no data, then the maximum size is 65.535 bytes, resulting in the highest value of 16 bits

- Identification: If it is fragmented packets then it will use a 16 bit identification number to identify to which IP packet it belongs.

- IP Flags: There are 3 bits that can be utilizied for identification, first bit is 0 always, the second one is the Dont fragment bit and the last is the More Fragments bit and is set on all fragmented packets except the last one.

- Fragment Offset: Is a 13 bit field that is specificing the position of the fragment in the original fragmented IP packet frame.

- TTL: Every IP packet that passes intoa  router, will get a TTL field decremented by 1 value. Once it hits 0 the router will then drop the packet and then send a ICMP(PING) message to the original sender. The TTL field has 8 bits and is used to prevent routing loops and data loss/burst.

- Protocol: 8 bit field that tells us which protocol is encapsulated in the IP packet, typically TCP vs UDP. TCP IS 6 UDP is 17.

- Header Checksums: 16 bit field that is used to store a checksum in the header. The reciever can use to fvalidate if there are any errors or corruption in the headers.

- Source/Root address: 32 bit source IP address of the sender of the packet.

- Destination Address: 32 bit destiantion IP address of the reciever of the packet.

- IP Option: Often times not utilized; however you can put source routes and specfiic routing paths in this option.

1.2. What do TCP and IP stand for?  (Looking for the words composing the
achronym.  Subsequent questions will ask for more detail)

- TCP: Transmission Control Protocol

- IP: Internet Protocol

1.3. Describe the function of each of the layers in the TCP/IP networking
stack:  Link Layer, Internet Layer, Transport Layer, Application Layer.

- Link Layer: Lowest layer in the IP stack, it is the link that a host is physically connected to.

- Internet Layer: Accepts and delivers all of the packets for the network. Resolves ARP Protocl request, ICMP stacks, and IP addresses.

- Transport Layer: Manages the exchange of data and acknowledges if data has been recieved or if data is needed to be resent. Ensure that packets can arrive in order and without error/corruption. Depends on TCP and or UDP.

- Application Layer: Last, higher level layer. Responsible for end to end communication for error free and definitive delivery of the data. There are 3 main protocls that are stored under this layer: HTTP/HTTPS, SSH, NTP. 

1.4. The OSI model breaks this into 7 layers which are useful conceptually, 
but the they do not directly associate with the design of the Internet.
Contrast and compare the TCP/IP model with the OSI 7-layer model.

- There can be a direct distinction from the OSI model to the TCP/IP Model. 

- Application, Presentation, and Session layers of the OSI Model can be commonly translated to the Application Layer on the TCP/IP Model.

- Transport Layer has a direct coorelation to the Transport lay on the TCP/IP Stack.

- Network layer is in the same situaion with a direct equivilent on the OSI to TCP/IP Stack.

- Data Link and Physical Layer in the OSI model can be commonly translated directly to the TCP/IP Stack under the Network Access Layer.

- All of the OSI Model layers can be translated into the TCP/IP stack since they serve similar functional results.
 
1.5. What are some differences between an Ethernet frame header and an
IP header?

- The ethernet header containts the source and target MAC Address. The IP header containts the source and target IP addresses.

- RFC 791 outlines clearly the layout of the IP header and how it can be utilized in the IP stack, while RFC 894 outlines how the Ethernet Headers are outlined

1.6. Why do we use both Ethernet addresses and IP addresses?

- We use them both for determining the source and the destination of the data being sent. For Ethernet we have to use MAC to determine on the local LAN. For IP headers we can use them on a WAN Scale. 

- Typically NATs will use ARP to better help control the flow of traffic across networks. This can be made possbile by using both types of frames. 

1.7. What is a Internet router?  What are some key differences between a 
router and a switch?

- A router does exactly as it sounds. It can route traffic both internall and externally.

- Internally is inter lan communications from one device to another on the same network. It can also do external network routing going from MAC to IP to WAN side.

- A router can do L2/L3 switching which means it can attempt to find the device on the LOCAL Lan for data transmission. While a switch can only check its broadcast zone then it would have to result to the router next.

1.8. In the TCP/IP model, what layer is TCP?

- Layer 4 / Transport

1.9. In the TCP/IP model, what layer is HTTP?

- Layer 5 / Application

1.10. Explain how data encapsulation works in the TCP/IP model.

- Data encapsulation works by as data is being sent, the sender host adds data to the packet header itself. 

- This can show more useful informatino to help better send the data and to ensure that the data is actaully recieved.

1.11. Explain the difference between IPv4 and IPv6. Why was IPv6 introduced?

- The main difference is IPv4 has only 32 bits while IPv6 has 128 bit. 

- The main reason IPv6 was introduced was due to the IPv4 exhaustion. We needed to find a way using more bits to where we could still identifiy uniquely, pubicly, each individual device.

- The original design of v4 did not think that the internet would expand like it did. (HARVARD, MIT) have claimed a ridiculous amount of IPv4 due to ARIN and IANA.

1.12. What is NAT (Network Address Translation) and why is it used?

- A NAT is generally publicly facing, however there can be uses where it can be privately facing as well.

- A NAT is a device or protocol that will convert IP addresses into MAC addresses. Instead of converting them it is actually doing a lookup of what IP belongs to which MAC and what MAC belongs to what IP.

- This allos for easier routing table management. It also allows for scalability and privability of each device sitting behind a NAT.

1.13. What is IPv4 address exhaustion?

- IPv4 Exhaustion is often a political thing. At least in my opinion it is. IPv4 exhaustion is due to the rapid growth of the internet and the poorly managed public IPv4 Assignments made out by RIRs and IANA.

- We only have 32 bits of numbers in all of the 4 octets in the IPv4 Scope. Once we run out of them then we are not able to add any more devices ot the network. 

- Therefore we now have IPv6, which can help phase out IPv4. However this has not happened yet - Hurricane Electric has been leading the cause on this one.

1.14. Describe what happens during the process of a TCP three-way handshake.

- It is often used to establish a reliable connection from one device to another.

- The connection is full duplex, both sides wil synchronize and then acknlowledge each other. 

- Hey you there? Yeah I'm here. Lets sync! Syncing ! Acknowledge. Transfer!

1.15. What are the differences between TCP and UDP? Give examples of 
applications that use each protocol.

- TCP Keeps the connection live ensuring that the client gets what they requested and is often synced. The UDP is connectinless which means that it does not have as much reliability and it sends the data with no error protection.

- TCP is often used in email and texting protocols.

- UDP is often used in live streaming and video chat with live feedback.

# Part 2: Domain Name System

2.1. What is the purpose of DNS (Domain Name System) and how does it work?

- DNS is often used to get lookups of a easily human readable host. Search the Domain on the TLD and then get a IP response.

- Host will span and ask the network where the DNS server is. Establishes connection. Queries a Domain name asking if it is on the network, returns result. If not on the network then it goes up a layer to see if the upstream provider has the IP host. If not then the process continues till it finds a authoratative answer. 

2.2. Explain how the DNS resolution process works, starting from when a user types a URL into their browser.

- See above. Sorta but in a web broswer.

2.3. What are the different types of DNS records, and what does each type do (e.g., A, AAAA, CNAME, MX, TXT)?

- A is a v4 ip to host

- AAAA is v6 to host

- CNAME is a alias to the host

- MX is mail server or routing.

- TXT typically host human readable info about the domain. Oftne used for human readable verifications.

2.4. What is the difference between a recursive DNS resolver and an authoritative DNS server?

- Recursive servers temporarily store info that is previously been retrieved and verified. However it may not always be avaialable in cache.

- Authoritative DNS holds the correct and proper up to date answer that can be spit out for correct queries.

2.5. Describe what happens during a DNS query and response process.

- Either Hit or Miss will occur. See above on the Top level process. 

2.6 What is a DNS zone, and how does it relate to DNS records?

- DNS zones are commonly used to host the particular records for a specific domain. 

2.7. What is DNS caching, and how does it improve performance?

- DNS Caching can improve performance by it not having to look up to a authoratative server for each answer. It can store a result for a set ammount of time. Instead of it always having to go up stream for a result.

2.8. What is TTL in the context of DNS responses?

- TTL is a field in the DNS to show how long the record is able to be cached for and how long it can take to propigatge. 

- Typically it can depend on how long it will be in the CDN or the amount of hops that it will need to endure.

2.9. Use `dig` or `nslookup` to find the A record for olemiss.edu.  What 
is/are the IP address(es)?

- 130.74.120.3

2.10. Use `dig` or `nslookup' to find the NS record for the root name servers.

- ns1.olemiss.edu or ns2.olemiss.edu

2.11 Usea 'dig' or 'nslookup' to find the NS records for the cs.olemiss.edu
name servers? 

- tim.cs.olemiss.edu

2.12. Explain the role of the root DNS servers in the DNS hierarchy.

- TLDs are the after part of the domain example: olemiss is the readable part the TLD is .edu

- It allows it to break down the Query into parts that way we are not having to scope over 1000s of TLDs.

2.13. What is a reverse DNS lookup and how is it different from a forward DNS lookup?

- Forward DNS is where you give it domain and it gives you are IP.

- Reverse DNS is where you give it a IP and it gives you a Domain.

# Part 3: Security

3.1. What is HTTPS and how is it different from HTTP?

- HTTPS is a secure version of HTTP, where all of the traffic on the end server is protected/encrypted with a SSL certificate.

3.2. Explain the role of SSL/TLS in HTTPS.

- TLS is the newly used standard of network encryption between two devices. It will then secure your data in transport to the origin server. 

- SSL is the older deprecated less secure version.

3.3. What are digital certificates, and how do they contribute to secure communications?

- A certificate is a file, typically, that is usd to identify itself to other devices, or servers that is is valid and it is a trusted devices. Typically used in data validation

3.4. Describe the concept of public-key cryptography and how it is used in secure communications.

- Public keys are used to encrypt and sign the data with two different keys. One is a private key and another is a public key. The public key anyone can use and it is visible to see.

3.5. What is a Certificate Authority (CA) and what role does it play in web security?

- A CA is a trusted validator that can issue and sign SSL/TLS certificates in a company or a organization. 

- They are also commonly placed on the internet to allow web browsers to authetnicate content sent from one device to another.

3.6. Explain the difference between symmetric and asymmetric encryption.

- Symmetric encryption is the act of only using one key to encrypt and decrypt data. Commonyl now we use Asymmetric encryption for one public and one private key to encrypt and decrypt data.

3.7. What is a VPN (Virtual Private Network) and how does it enhance security?

- A VPN can mitigate or establish a connection between a computer and a end result server. This allows you to tunnel or act as if you are directly on another network.

- It can enhance security as giving the end user the result of being on prem to the end tunneled network.

3.8. What is a firewall, and how does it protect a network?

- A firewall comes in many flavors most commonyl it is a advanced protection system that monitors, inspects and sniffs incoming and outgoing network traffic.

- It follows a set of policies to ensure that threats can be detected, mitigated, and documented. 
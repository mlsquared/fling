## Homework 5: Wireshark and Length Modulation

Methods for modulating packets.

1. Investigate whether QUIC can be used to modulate packet length. 
   Can the transport layer affect how QUIC breaks up packets?  
   Does QUIC buffers bytes until it reaches a certain size 
   datagram as with TCP (see Nagel's algorithm).   Describe
   how QUIC determines packet lenghts?   In oarticular does it
   send bytes right away?  If so then we might be able to modulate
   packet length performing separate writes to QUIC, but how would
   one do that from within a web page using Javascript?  Can the client
   know if QUIC is supported by the browser?  Can the client know if
   QUIC is being used?  Can the client select to use QUIC? 
 
2. Investigate whether WebRTC can be used to modulate packet length.  Write
   a summary of WebRTC and how it differs from TCP + TLS over 802.11 wireless
   vs. QUIC over 802.11 wireless.  How would we modify packets lengths?
   How do we it from within a web page using Javascript?

3. Install wireshark on Windows and on MacOS (whichever is available or both).
For this problem we need three computers: one computer will act as the
client and one will act as the server. For part (d) we use a third
machine which will act as a sniffer (sees network traffic).

 a. Start a simple python server on one machine and confirm you 
    can download a simple web page from it, e.g., an index.html 
    that contains some "Hello World."  Download index.html from the second machine
    using a web browser.

 b. Start wireshark on one machine and figure out how to peak in on the communication between
    your web browser on the second machine and the web server on the first machine.
    Describe what you see.  Include a snippet of traffic (just key packets)
    that illustrates what wireshark say.  Identify frame headers and payload
    where possible.   If you can get access to the packets before they are encrypted
    (which you may be able to in parts (b) and (c)) then identify the 
    transport and application parts of the packets.

 c. Repeat (b) but run wireshark from the second machine. Does it see anything
    differently?

 d. Repeat (b) but run wireshark from the third machine. Does it see anything
    differently?

4.  Creates files of slightly different lengths on the server, e.g., just
    insert blocks of 16 characters to your index.html.  Observe how this changes
    frame length and packet length where visible.




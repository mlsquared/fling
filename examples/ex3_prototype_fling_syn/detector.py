"""This is an example unsandboxed beacon detector.  It runs on the media
device and sniffs TCP traffic.

Note that this unsandboxed beacon detector (unbeacon) is incomplete. It does not
deal with encrypted wifi networks, and this script does not put the
wireless network interface in monitor mode (e.g., in Mac OS).  
When a wifi network is encrypted, the station and the access point
will share a key called a Pairwise Transient Key (PTK) which is unique
to the station-AP pair.   As such the beacon detector will NOT be able
to see IP or TCP headers in packets communicated between the station and
the AP.   If we want this unbeacon to work on an encrypted wifi network, the 
unbeacon would need to capture the 4-way handshake
when the station first joins the wifi network.  This 4-way handshake
includes enough information to compute the PTK if the unbeacon 
also knows the wifi network's Pre-Shared Key (PSK).  
"""


# This requires using scapy: 
#
#   pip install scapy

from scapy.all import sniff, TCP
import socket

detected_ports = set()


def resolve_ip(domain):
    try:
        ip_address = socket.gethostbyname(domain)
        print(f"Resolved IP address of {domain}: {ip_address}")
        return ip_address
    except socket.gaierror as e:
        print(f"Error resolving {domain}: {e}")
        return None

def detect_session_id(packet):
    if packet.haslayer(TCP) and packet[TCP].flags == 'S':  # Check for SYN flag
        port = packet[TCP].dport
        detected_ports.add(port)
        print(f"Detected port: {port}")
        if len(detected_ports) >= 3:
            session_id = sorted(detected_ports)[:3]  # Only take the first three distinct ports
            print(f"Constructed session ID: {session_id}")
            establish_session(session_id)
            detected_ports.clear()

def establish_session(session_id):
    session_id_str = '-'.join(map(str, session_id))

    print(f"Establishing communication with session ID {session_id_str}")
    
    # I assume we would do something smarter here for the relay.  We should use
    # a more standard relay protocol (TURN?).   Some thought needs to be put into
    # how a session ID should be used to establish communications between a
    # sandboxed application and a sandbox reachable service (such as fling).
    #relay_url = f'https://flingo.zapto.org/relay/{session_id_str}'
    #if response.status_code == 200:
    #    print('Session established with relay')
    #else:
    #    print('Failed to establish session with relay')

if __name__ == '__main__':
    domain = 'flingo.zapto.org'
    target_ip = resolve_ip(domain)
    if target_ip:
        print("Starting packet sniffing...")
        bpf_filter = f"tcp and dst host {target_ip} and dst portrange 1024-65535"
        sniff(filter=bpf_filter, prn=detect_session_id)




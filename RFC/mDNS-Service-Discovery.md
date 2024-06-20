# Fling - mDNS Device and Service Discovery

**Feature Name:** Fling Service Discovery

**Type:** Protocol

**Start Date:** 06-20-2024

**Author:** [Larson G Carter](https://github.com/larson-carter)

**Related components:**

1. [Link-local Multicast Name Resolution (LLMNR) - RFC 4795](https://datatracker.ietf.org/doc/html/rfc4795/)

2. [Multicast DNS - RFC 6762](https://datatracker.ietf.org/doc/html/rfc6762)

3. [DNS-Based Service Discovery - RFC 6763](https://datatracker.ietf.org/doc/html/rfc6763)

4. [ Requirements for Scalable DNS-Based Service Discovery (DNS-SD) / Multicast DNS (mDNS) Extensions - RFC 7558](https://datatracker.ietf.org/doc/html/rfc7558)

5. [DNS-Based Service Discovery (DNS-SD) Privacy and Security Requirements - RFC 8882](https://datatracker.ietf.org/doc/html/rfc8882/)

## Summary

This document discusses the mDNS-viewable service and the device discovery process for Flingable devices. It explains how to create an mDNS discoverable service, the process of identifying available mDNS services on a local network, searching for a specific service by name, and discovering the LAN IP address of the device instance.

The primary objective of this RFC is to propose a method that enables users to seamlessly join any network, regardless of varying DNS server configurations, IP ranges, subnet sizes, and other network factors, while still being able to discover Flingable devices present on that network.

## Motivation

For the Fling project, our goal is to adhere closely to the "It Just Works" philosophy, particularly when users are connecting to unfamiliar network configurations. Our aim is to enable the seamless discovery of Flingable devices on the LAN side.

Initially, we want users to effortlessly see all available Flingable devices on their local network. Following this discovery phase, our next objective is to pinpoint the root IP address of each device. This IP address serves as the crucial gateway to establishing future connections with these devices.

Transitioning from discovery to utilizing mDNS-formatted sources to obtain the IP address represents the ultimate goal. Direct communication or connection initiation through mDNS-formatted sources poses challenges, but leveraging IPv4 addresses enables straightforward communication.

## Detailed design

In the context of Fling, there are two key components:

1. Sandbox-Reachable Service (SRS): This service is designed to be visible on the network through mDNS discovery.

2. Sandboxed Service/Application (SSA): This component operates within a sandboxed environment.

For this document, our focus is on the SRS, which aims to be discoverable on the network using mDNS. The objective is to demonstrate the transition from mDNS discovery to obtaining the IPv4 address of the SRS.

In practice, we will illustrate how a non-sandboxed environment (presumably a user's device or application) can discover the SRS using mDNS protocols. Once the SRS is identified through mDNS, the subsequent step involves retrieving its IPv4 address. This IPv4 address is pivotal for establishing connections with the SRS from non-sandboxed environments or applications.

The primary goal here is to validate the concept of seamlessly transitioning from mDNS service discovery to obtaining the essential IPv4 address needed for subsequent communication.

### First we create the SRS on the network:

**Note: This part is being done on a Raspberry Pi**

1. Under the directory `/etc/avahi/services` we create a new file `fling.service`
2. Then we paste the contents into the `fling.service` file
```
<?xml version="1.0" standalone='no'?>
<!DOCTYPE service-group SYSTEM "avahi-service.dtd">
<service-group>
  <name replace-wildcards="yes">%h's Fling</name>
  <service>
    <type>_fling._tcp</type>
    <port>80</port>
  </service>
</service-group>
```

### Secondly we need to discover if our network can see Fling:

**Note: This part is being done on my Macbook**

1. In terminal run `dns-sd -B _services._dns-sd._udp`
```
Browsing for _services._dns-sd._udp
DATE: ---Thu 20 Jun 2024---
 0:47:16.149  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
 0:47:16.150  Add        3  15 .                    _udp.local.          _asquic
 0:47:16.150  Add        3  14 .                    _tcp.local.          _airplay
 0:47:16.150  Add        3  14 .                    _tcp.local.          _raop
 0:47:16.150  Add        3  14 .                    _tcp.local.          _companion-link
 0:47:16.150  Add        3   1 .                    _tcp.local.          _airplay
 0:47:16.150  Add        3   1 .                    _tcp.local.          _raop
 0:47:16.150  Add        3   1 .                    _tcp.local.          _companion-link
 0:47:16.150  Add        3  15 .                    _tcp.local.          _companion-link
 0:47:16.150  Add        3  15 .                    _udp.local.          _sleep-proxy
 0:47:16.150  Add        3  15 .                    _tcp.local.          _airplay
 0:47:16.150  Add        3  15 .                    _tcp.local.          _raop
 0:47:16.150  Add        3  15 .                    _udp.local.          _meshcop
 0:47:16.150  Add        3  15 .                    _udp.local.          _trel
 0:47:16.150  Add        3  15 .                    _tcp.local.          _googlezone
 0:47:16.150  Add        3  15 .                    _tcp.local.          _googlecast
 0:47:16.150  Add        3  15 .                    _tcp.local.          _remotepairing
 0:47:16.150  Add        2  15 .                    _tcp.local.          _apple-mobdev2
 0:47:25.131  Rmv        0  15 .                    _udp.local.          _asquic
 0:47:45.138  Add        2  15 .                    _tcp.local.          _fling
```

**Here at the bottom you can see _fling**

### Thirdly we want to find the Instance and IP of the Flingable Device

1. Now we are wanting to thind the instance host of the device, run `dns-sd -B _fling._tcp`
```
Browsing for _fling._tcp
DATE: ---Thu 20 Jun 2024---
 0:46:30.256  ...STARTING...
Timestamp     A/R    Flags  if Domain               Service Type         Instance Name
 0:47:45.138  Add        2  15 local.               _fling._tcp.         raspberrypi's Fling
```

**Here we can see the instance on "raspberrypi"**

**NOTE: this means that the resolver of mDNS is equal to raspberrypi.local**

2. Now we want to find the root IP of the Instance, run `dns-sd -G v4 raspberrypi.local`
```
DATE: ---Thu 20 Jun 2024---
 0:49:18.889  ...STARTING...
Timestamp     A/R  Flags         IF  Hostname                               Address                                      TTL
 0:49:18.890  Add  40000002      15  raspberrypi.local.                     172.16.1.96     
```

**Note: Now you can see the IP address of the device that is running the castable Fling Service is 171.16.1.96**

**Note: This can also be verified by pinging raspberrypi.local in your terminal**

## Drawbacks

Here are some potential drawbacks that should be considered for the RFC on Fling's SRS discovery process:

1. Network Configuration Variability: One significant challenge is the diverse nature of network configurations, including variations in DNS server implementations, subnet sizes, and firewall settings. These differences can impact the reliability and consistency of mDNS service discovery, potentially leading to inconsistent results across different networks.

2. Dependence on mDNS Protocol: The reliance on mDNS for service discovery may pose limitations in environments where mDNS is not supported or properly configured. Certain network setups or corporate environments may restrict or disable mDNS, thereby hindering the ability to discover SRS instances reliably.

3. Security Concerns: While mDNS itself does not inherently provide strong security mechanisms, relying on it for service discovery could expose devices to potential security risks such as spoofing or man-in-the-middle attacks. Implementing additional security measures, such as encryption and authentication, becomes crucial but adds complexity to the setup.

4. Scalability Issues: As the number of Flingable devices increases within a network, the scalability of mDNS-based discovery mechanisms might become a concern. Large-scale deployments could experience performance degradation or increased network traffic due to frequent mDNS queries and responses.

5. Compatibility Challenges: Ensuring compatibility across different platforms and operating systems for mDNS service discovery can be challenging. Variations in mDNS implementations or versions might lead to interoperability issues, affecting the ability to discover SRS instances uniformly across all supported environments.

6. Maintenance and Support: Managing and supporting mDNS-based service discovery over time requires ongoing maintenance efforts. Updates to network infrastructure, changes in network policies, or upgrades to mDNS software implementations may necessitate adjustments to ensure continued reliability and functionality.

Addressing these drawbacks requires careful consideration during the design and implementation phases of Fling's SRS discovery mechanism. Mitigation strategies could include robust error handling, alternative discovery methods for non-mDNS environments, and comprehensive security measures to protect against potential vulnerabilities.

By acknowledging these challenges upfront, the RFC aims to provide a balanced assessment of the implications and considerations involved in implementing mDNS-based SRS discovery for Flingable devices.

## Alternatives

### Alternatives Considered

**Note: Some of these are not usable in a Sandboxed Web App environment, research shows that mDNS is**

1. Static Configuration: One alternative considered was to rely on static configuration where users manually input IP addresses or hostnames of SRS instances. This approach would eliminate the dependency on mDNS but introduces potential user error and complexity, especially in dynamic network environments where IP addresses may change.

2. Centralized Discovery Server: Implementing a centralized discovery server is another alternative. Devices could register with this server upon connection to the network, and client applications would query the server for SRS instances. While this approach provides centralized control and potentially better security, it adds complexity in managing and maintaining the discovery server infrastructure.

3. UPnP (Universal Plug and Play): UPnP protocols allow devices to discover and communicate with each other on a local network. This could serve as an alternative to mDNS for discovery purposes. However, UPnP also has security concerns and may not be universally supported across all devices and platforms.

4. DNS Service Discovery (DNS-SD): DNS-SD provides a way to discover services using standard DNS queries rather than multicast DNS. This approach leverages existing DNS infrastructure but requires specific DNS records and configurations, which may not be widely implemented or supported in all network environments.

### Impacts of non implementation

1. User Experience: Without a seamless discovery mechanism like mDNS, users may experience frustration in locating and connecting to Flingable devices, particularly in unfamiliar network environments. This could degrade the "It Just Works" experience that Fling aims to achieve.

2. Compatibility: Alternative discovery methods may not offer the same level of compatibility across diverse network setups and operating systems. This could result in fragmented user experiences and reduced adoption of Flingable devices.

3. Operational Overhead: Depending on manual configurations or centralized servers introduces additional operational overhead in terms of setup, maintenance, and support. This could increase costs and complexity for both users and developers.

4. Innovation Limitations: Embracing mDNS-based discovery allows Fling to leverage a widely adopted and standardized protocol for local network service discovery. Not doing so might limit future innovation and integration with emerging technologies that rely on mDNS.

## Unresolved questions

1. Scalability: How will the proposed mDNS-based SRS discovery scale with a large number of Flingable devices on the same network? Will there be performance implications or potential bottlenecks with increased network traffic due to mDNS queries and responses?

2. Security Considerations: While mDNS itself lacks robust security features, what additional security measures will be implemented to protect against potential vulnerabilities such as spoofing or unauthorized access during the SRS discovery process?

3. Interoperability: How will the mDNS-based SRS discovery mechanism ensure interoperability across various platforms, operating systems, and network configurations? What strategies will be employed to address compatibility issues that may arise?

4. Fallback Mechanisms: In scenarios where mDNS is not supported or properly configured, what fallback mechanisms or alternative discovery methods will be implemented to ensure consistent SRS discovery across different network environments?

5. Maintenance and Updates: What provisions will be put in place to manage and update the mDNS-based SRS discovery mechanism over time? How will changes in network infrastructure or updates to mDNS software implementations be addressed to maintain functionality and reliability?

6. User Experience Optimization: How can the user experience be optimized to ensure seamless and intuitive discovery of Flingable devices through mDNS, considering varying levels of user familiarity with network configurations and technical setups?

7. Can this actaully and feasibly be done within a Sandboxed Web Application environment???

Addressing these unresolved questions is crucial to refining the design of the mDNS-based SRS discovery for Flingable devices. Each aspect requires careful consideration and potentially iterative development to ensure robustness, security, compatibility, and a positive user experience across diverse network environments.





-- Larson Carter
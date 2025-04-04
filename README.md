<!--
Compile this into a pdf using

  $ pandoc README.md -o READMER.pdf --pdf-engine=xelatex

-->


# Sassy and Fling

Sassy is a service architecture. Fling is the first service implemented using Sassy: casting
with joinng.


>  It just works...except when it shouldn't.

Fling enables one device to "fling" a URL to a TV (or other media
device) for playback.  It feels like Airplay and Chromecast, but it
does what they don't do.

> It just works...

Even if the mobile device and the network device are on different networks.

Even if the mobile device is an Android phone or an Apple phone purchased in 
the dark ages.

Even if the mobile device is any phone that at least provides web browsing
capabilities with a web browser that is reasonably compatible with 
modern browsers, or provides a sufficiently similar app development environment.

Even if the user "flinging" has no account or prior affiliation with
the receiving media device.

For flinging from a web page, it works even if the user has not installed any
application that is fling-enabled.  Fling only requires javascript.

Further, it doesn't require the user to enter a short code or scan a QR code.
And it doesn't realy on access to microphones or cameras.

> ...except when it shouldn't

Bob's neighbor cannot fling porn to Bob's 5 year old daughter watching TV.

Bob's college frenemy down the hall in their dorm cannot fling porn to
Bob's TV even though they are both in the same building using the same
wifi network.

More specifically, Fling won't work if the mobile device is not in
physical proximity to the media device.

Fling won't work if the mobile device is in a different room such that there is
an intervening wall.

Unless... the media device's owner has allowed policy exceptions to
the above requirements.


## How does it work?!?

From Joe User's perspective.  Joe walks into Bob's house.  Joe's old
iPhone 8 is not on Bob's wifi network and is using only cellular data.
Joe opens a youtube page in the phone's Safari browser.  A button
appears beneath the video, perhaps alongside the button for casting or
airplaying.  Joe pushes this button and it opens an iframe, frame, or
web page showing the video to be flung, a list of detected media
devices if more than one is nearby or the name of the device if there
is only one, and lastly a "fling" button.  Joe pushes "fling" and the
video starts playing on Bob's TV.

Joe User didn't have to join Bob's wifi network.  Joe User didn't have
to join any wifi network.  Joe User didn't have to upgrade his phone.
Joe User didn't enter any usernames, passwords, QR codes, or short
codes.

## How does it really work?

Clearly some "magic" is going on.  How did Joe's iPhone 8 communicate
with the TV when Joe isn't even on Bob's wifi network?  Short answer:
SASSY.


## SASSY

SASSY refers to Spatially-Aware Secure SYstem or maybe Spatially-Aware
Secue Services (SASSy).  It is a reimagining of Sandbox-Reachable
Services (SRS) that mostly existed to allow web sites, and mobile apps
to communciate with services from within the same network when
opeprating within the constraints of a web or mobile security sandbox.
While this did allow us to introduce the first version of casting (as
Fling) to Vizio TVs in a way that allowed it to work with existing
browsers and mobile sandboxes, it seemed more like a work-around then
an attempt to improve security.

SASSY allows services without joining.  Meaning, it is unnecessary
to join a network to access the services.  Why?  Because joinnig a
network exposes all services offered by a device to attackers on the
same network.  Joining exposes a potentially substantial attack surface.

With SASSY, the initiating device generates a beacon, an initiator
beacon, that is one-way.  The initiator beacon is like blinking
a flash light.  It could be as simple as having a device start
and stop communicating in any frequency band that can be
detected by a nearby beacon detector.   This is achievable with
a wide range of technologies and generally does not require
the beacon detector to decrypt any communications from the
beacon initiator.  Because the communication is one-way,
it exposes no receiver attack surface on the initiator device.

The beacon needs only communicate an ephemeral public key that
lasts for a single session and an Internet relay identifier.
With the assumption that the initiator beacon and beacon
detector have Itnernet access, the specified relay can handle
all further communication using the public key as a session
identifier. 

With SASSY, only the specific service(s) being offered by
the beacon detector need be exported via the relay and the
initiator devicde need only expose a client interface via the
relay.  As such the system exopses an attack surface
composed of the the mininmum number of interfaces. 

Consider the Joe and Bob example:

1) Joe's iPhone is emitting electromagnetic radiation.

2) Bob's TV is equipped with a wifi antenna (maybe two) and possible other 
hardware (although hopefully this isn't required)

3) Joe is in proximity to Bob's TV. 

4) Bob's TV is doing signal processing to pick up the presence of Bob's iPhone
based on upper harmonics from Joe's cell antenna. 

Even devices using different technology may introduce noise on nearby antennas.
This "noise" may be low energy and only suitable for low bitrate
communications.  However, it could be used to communicate a token (e.g.,
the aforementioned ephemeral public key).

Joe's cell phone has cell data enabling moderately high bitrate Internet
access.

Bob's TV has wifi enabling high bitrate Internet access.

All Joe's cell phone needs to do is to tell Bob who to talk to on the Internet
to relay information across their respective modertely high and high bitrate
channels to the Internet.  The token can do that.

The token could be a short code that is communciated to a centralized server
or a Distributed Hash Table (DHT) node.  The server or DHT node could act
as the relay for further communications between Joe's cell phone and Bob's TV.
Once communications is established via the relay, Joe's can communicate at
higher bitrate to Bob's TV in order to make API calls such as "play this URL."

When I first mention SASSY to people including security experts their
first response so far has been to object to allowing communications
between devices that are not on the same network, I submit that the
network access model is itself far from secure, especially in the
modern mobile world.  Unless network adminstrators exercise strict
security hygience, any devices connecting to a network could be
attackers.  This does not require malfeasance on behalf of the mobile
device's owner.  The device owner may be completely unaware as their
device acts a super spreader of mayhem as it moves from one network to
anohther.  This is less of a problem if those engaging in
communications choose to not join a network unless latency or
bandwidth constraints demand it and instead rely on end-to-end
encryption and minimizing attack surface.

Avoiding the requirement of joining a network to access services does
not mean that the two parties consent without information.  SASSY
services may still make decisions based on policy.  Policy decisions
may be made based on policy inputs that are inferred rather than
explicitly provided by the user.  Such policy inputs may include
inferring proximity between the two parties, or inferring relative
location or inferring the presence of intervening walls.  We submit
that policy inputs like "in the same room," "in the same apartment,"
"on the same floor," and "in the same building" are much closer to the
way people think about access control than "on the same WiFi network."
How many people are really aware of the range in which people can see
their WiFi networks.

With SASSY we intend to explore a variety of techniques for a
device build spatial awareness including triangulation and
potentially inverse scattering of WiFi and cellular EM emissions.

## Terminology

* Sandbox Reachable Service: an older architecture invented by
David Harrison and used to implement Fling 1.0.  It was deployed
in all Vizio television circa 2010.

* SASSY (a.k.a., Sassy): Spatially Aware Secure SYstem.  An
architecture that uses spatial awareness rather than or in addition to
network boundaries as policy inputs when making security-related
policy decisions including access control decisions.

* Sassy Service: a service that makes itself available to devices with
an authorized spatially defined area in proximity to the service.

* Sandboxed Application: a web page or mobile app that runs in a web
or web-like security sandbox.

* Sassy Initiator Beacon (a.k.a., Initiator Beacon, beacon): a piece
of javascript (or app code) that operates within a sandboxed
application to make its presence known to nearby unsandboxed beacon
detectors.  Because beacon is sandboxed, it can only perform
operations that available to sandboxed applications.  At the very
least, a sandboxed application can force DNS queries, download small
files, open connections to a variety of IP addresses.  The operations
performed by the beacon establish a low bitrate covert channel that
can be detected by unsandboxed beacon detectors.

* Sassy Beacon Detector (a.k.a., beacon detector): a beacon detector
runs as part of Sassy on the device offering one or more services to
Sassy clients.  A Sassy beacon detector has sufficient access to
interpret sassy beacons which presumably includes low level
(device-driver level) access to the antenna(s) on the media device.
We can upgrade the media device running the Sassy beacon detector to
increase the range of devices it can detect, though the idea is to try
to enable beacon detection using only software, machine learning, and
a wifi-antenna.

## Optimizations

The beacon initiator could send a triplet rather than a pair:

    (single use public key, relay ID, service ID)

A `service ID` would allow SASSY beacon detectors to avoid responding
if the detector is not on a device that offers the specifid Sassy
Service.


## History

### Sandbox-Reachable Services

Fling as originally deployed in all Vizio TVs is an example of
*Sandbox-Reachable Services* (SRS).  SRS is a system that allows a
sandboxed application like a web page or mobile app to communicate
with nearby network devices.

SRS has been around since 2008, when it was first deployed in Vizio
televisions, under the name *Web Videos.* *Web Videos* only
implemented fling.  Or should I say, "it only implemented Fling 1."
Fling 1 and more generally SRS 1 required the mobile device and the
media device to be on the same network.  Fling 1 provided little
benefit over Airplay or Chromecast.

Fling 1 made it onto all Vizio TVs, because AirPlay and Chromecast did
not exist when Fling was first introduced.

David Harrison holds several patents for sandbox reachable services.
<!--
Compile this into a pdf using

  $ pandoc README.md -o READMER.pdf --pdf-engine=xelatex

-->


# Fling


>  It just works except when it shouldn't.

Fling enables one device to "fling" a URL to a media device for playback.
It feels like Airplay and Chromecast, but it does what they don't do.

> It just works

Even if the mobile device and the network device are on different networks.

Even if the mobile device is an Android phone or an Apple phone purchased in 
the dark ages.

Even if the mobile device is any phone that at least provides web browsing
capabilities with a web browser that is reasonably compatible with 
modern browsers, or provides a sufficiently similar app development environment.

Even if the user "flinging" has no account or prior affiliation with the receiving
media device.

For flinging from a web page, it works even if the user has not installed any
application that is fling-enabled.  It only requires javascript.

Further, it doesn't require the user to enter a short code or scan a QR code.
And it doesn't realy on access to microphones or cameras.

> Except when it shouldn't

It won't work if the mobile device is not in physical proximity to the media
device.

It won't work if the mobile device is in a different room such that there is
an intervening wall.



## How does it work?!?

From the Joe User's perspective.  Joe walks into Bob's house.  Joe
asks if he can fling a video to Bob's TV.  Joe's old iPhone 8 is not
on Bob's wifi network and is using only cellular data.  Joe opens a
youtube page in the phone's Safari browser.  A button appears beneath
the video, perhaps alongside the button for casting or airplaying.
Joe pushes this button and it opens an iframe, frame, or web page
showing the video to be flung, a list of detected media devices if
more than one is nearby or the name of the device if there is only
one, and lastly a "fling" button.  Joe pushes "fling" and the video
starts playing on Bob's TV.

Joe User didn't have to join Bob's wifi network.  Joe User didn't have
to join any wifi network.  Joe User didn't have to upgrade his phone.
Joe User didn't enter any usernames, passwords, QR codes, or short
codes.

## How does it really work?

Clearly some "magic" is going on.  How did Joe's iPhone 8 communicate
with the TV when Joe isn't even on Bob's wifi network?  Before
answering the question, let's talk about *Sandbox Reachable Services.*

## Sandbox-Reachable Services 1

Fling is an example of *Sandbox-Reachable Services* (SRS).  SRS is a
system that allows a sandboxed application like a web page or mobile
app to communicate with nearby network devices.

SRS has been around since 2008, when it was first deployed in Vizio
televisions, under the name *Web Videos.* *Web Videos* only
implemented fling.  Or should I say, "it only implemented Fling 1."
Fling 1 and more generally SRS 1 required the mobile device and the
media device to be on the same network.  Fling 1 provided little
benefit over Airplay or Chromecast.

Fling 1 made it onto all Vizio TVs, because AirPlay and Chromecast did
not exist when Fling was first introduced.

## Sandbox-Reachable Services 2

SRS 2 and by extension Fling 2 changes the problem.  We want to fling
even when the mobile device and the media device are NOT on the same
network, but when the mobile device and the media device are in
proximity to each other or in the same room.

Consider the Joe and Bob example:

1) Joe's iPhone is emitting electromagnetic radiation.

2) Bob's TV is equipped with a wifi antenna (maybe two) and possible other 
hardware (although hopefully this isn't required)

3) Joe is in proximity to Bob's TV. 

4) Bob's TV is doing signal processing to pick up the presence of Bob's iPhone
based on upper harmonics from Joe's cell antenna. 

Even devices using different technology may introduce noise on nearby antennas.
This "noise" may be low energy and only suitable for low bitrate
communications.  However, it could be used to communicate a token.

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


## Terminology

* Sandbox Reachable Service: a network service designed to be discovered
by and to communicate with sandboxed applications.

* Sandbox Reachable Services 1: system that enables communications between
a sandboxed application and nodes on the same network.

* Sandbox Reachable Services 2: system that enables communications
between a sandboxed application and nodes running sandbox reachable
services in physical proximity to the node running the sandboxed
application.  The device running the sandboxed application and the
media device do not have to be on the same network, but they must
communicate using electromagnetic emissions that are sufficiently
similar as to induce oscillations in the antenna(s) on the

* Sandboxed Application: a web page or mobile app that run in a web or
web-like security sandbox.

* Sandboxed Initiator Beacon (a.k.a., beacon): a piece of javascript
(or app code) that operates within a sandboxed application to make its
presence known to nearby unsandboxed beacon detectors.  Because beacon
is sandboxed, it can only perform operations that available to sandboxed
applications.  At the very least, a sandboxed application can force
DNS queries, download small files, open connections to a variety of IP
addresses.  The operations performed by the beacon establish a low bitrate
covert channel that can be detected by unsandboxed beacon detectors.

* Unsandboxed Beacon Detector (a.k.a., beacon detector): a
beacon detector runs as part of sandbox-reachable services on the
device offering one or more services to sandboxed applications.
As implied by the title, the beacon detector is not subject to the limitations
of a web or web-like security sandbox.  In fact, an unsandboxed
beacon detector has low level (device-driver level) access to the
antenna(s) on the media device.  We can upgrade the media device
running the unsandboxed beacon detector to increase the range of devices
it can detect, though the idea is to try to enable beacon detection
using only software, machine learning, and a wifi-antenna.


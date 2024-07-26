# HW 4

## Part I: For Everybody

**Problem 1.1** Review Centaur by R. Nandakumar et al

Write a summary of this paper including describing the basic mechanisms used.
Look at it from a point of view of how it is applicable to our work.
I am most interested in RF-only solutions.

In respository at

  papers/2012-Nandakumar-mobicom-Centaur.pdf

https://scholar.google.com/citations?view_op=view_citation&hl=en&user=CO4txjAAAAAJ&citation_for_view=CO4txjAAAAAJ:W7OEmFMy1HYC


**Problem 1.2** Review Bai et al

Write a summary of this paper including describing the basic mechanisms used.
Look at it from a point of view of how it is applicable to our work.
I am most interested in RF-only solutions, but we should understand
what makes our system different from other techniques including those using
audio.

In repository at

  papers/2022-Bai-a-hyrbid-indooroutdoor.pdf

Yuntian Brian Bai, Lucas Holden, Allison Kealy, "A hybrid indoor/outdoor detection approach for smartphone-based seamless positioning" [...]

https://www.cambridge.org/core/services/aop-cambridge-core/content/view/BCDDFEAD36799F4275AC171F4363A706/S0373463322000194a.pdf/a-hybrid-indooroutdoor-detection-approach-for-smartphone-based-seamless-positioning.pdf


**Problem 1.3** Find one other paper and review it.

Find a related paper to our work and write a summary.
Look at it from a point of view of how it is applicable to our work.


## Part II Larson

**Problem 2.1** Characterize packet sizes with normal traffic using
wireshark.  Present a historgram of packet sizes.  Create separate
histograms for upstream and downstream directions.


**Problem 2.2** Attempt to control packet lengths using TCP_NODELAY
and socket writes.  It may not be necessary to modify the TCP/IP stack
to control packet lengths.  Use wireshark to characterize the size of
packets as a function of the size of socket writes.  Create a scatter
plot where one dimension is socket write length and the other
dimension is packet length.

**Problem 2.3** One would expect that packet length should
increase linearly with socket write length, if there are signficant
deviations, explain what might be happening.  a) what fraction of packets
follow an exactly linear relationship, b) of those that differ from this
relationship, what fraction are due to modifications to TCP header length?
IP header length?  Something else?  For those where the variation in
packet length is not explained by variation in TCP and IP headers, attempt
to explain what is going on.


**Problem 2.4** (optional) Study an alternate way to control packet length.



## Part III. Logan

**Problem 3.1** Create a Hello World using Electron that runs on the
It should play a youtube video.  Talk with Larson for help.

**Problem 3.2** Write a brief summary on how we might adopt Pandora
(or maybe drawing some elements from Spotify) to create a compelling
Jukebox demo or Video Jukebox.  The latter would use Youtube videos a
la what Larson built.  Aside: I am considering how Samba might use
such a demo to approach Pandora or Spotify to see whether they would
be interested in partnering to provide spacially-aware multi-user
recommendations for music.




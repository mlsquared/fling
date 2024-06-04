<!--
This is homework for preparing for work on fling.

Compile this into a pdf using

  $ pandoc hw1.md -o hw1.pdf --pdf-engine=xelatex

-->

# Homework 2: Web Tech

*Problem 1* Sign up for a free-tier account on AWS.  Setup an EC2
instance using an Amazon Linux 2 AMI or pick a linux distribution if
you have a preference.  Use chatgpt as much as you can to help out.
The only answer you have to give is a statement that it has been done.

*Problem 2* Log into your EC2 instance and run a python flask web
server that returns a "Hello World" web page.  Create a domain name
that points at this server.  You can obtain a free domain name from
no-ip.com.   Point the DNS *A* record to your EC2 instance.

Answer with the URL to the web server that I can click on to open the
"Hello World" web page.  This web page should reside at

     http://yourdomainname/index.html

Here yourdomainname is the domain you recevied from noip.com or some other
dynamic DNS service.

*Problem 3*  Add a hyperlink to your index.html that opens a web page
on the same server at

    http://yourdomainname/prob3.html
  
`foo.html` uses each of the following elements:

  * A title (`<title>`)
  * A heading (`<h1>`)
  * A paragraph (`<p>`)
  * An image (`<img>`)
  * A link to another webpage (`<a>`)

Cut-and-paste the URL of the web page `prob3.html` as your answer.


*Problem 4* Read about fetch.

    https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch

Add a web API call to your flask server that implements a
function that returns the time of day (e.g., "12:45").  Add the source code
below, and show a linux shell command-line call to this API using curl or wget.


*Problem 5*  Create a new web page `prob5.html` This web page should
use the `<script>` tag to load a script that queries the web API built
in problem 4.  Use fetch to make the call.   The call returns the
curren time.  Show the current time on the web page.  

  


*Problem 6* Provide a 2-3 sentence answer describing the role of
the security sandboxes provided by your web browser.


*Problem 7* What is same-origin policy? Provide 2-3 sentences.


*Problem 8* What is cross-site scripting?  Provide 2-3 sentences.


*Problem 9* Set up a second flask web server running on a different domain
name `your2nd` that offers the same time API as in problem 5.  Create a `prob9.html` served
from `yourdomainname` with URL

    http://yourdomainname/prob9.html

`prob9.html` should include a script that tries to obtain the time from `your2nd`.
THIS SHOULD FAIL!  Same origin policy should prevent the call across domains.


*Problem 10* Add CORS support to your flask server running on `your2nd` domain
in order to permit the cross-site scripting.




#
# 1. I created a domain name at no-ip.com:
#   https://my.noip.com/dynamic-dns
#
#   I used the domain name flingo.zapto.org
#
# 2. I went to the AWS EC2 console, and I started up a free tier instance.
#   I used the default AWS Linux AMI running on a micro instance.
#   I clicked the checkboxes to allow HTTP and HTTPS access to the instance.
#   I told it to allow access using my public-private key pair named
#
#        id_rsa_aws_dave_992382814940
#
#   If you create a new key pair then make sure that you download the private key.
#
#   I then clicked OK or whatever the button is at the end of the page
#   which caused EC2 to startup an instance.  EC2 started an instance
#   with i-0f353908a62767949.
#
#   I then went back to the EC2 console and I can see that the
#   instance with i-0f353908a62767949 is running.  IT has been assigned
#   the domain name
#
#     ec2-3-101-62-223.us-west-1.compute.amazonaws.com
#
# 3. Configure the private key
#
#    Put the private key in the .ssh/config on your local computer, e.g., your laptop.
#    I have a local file
#
#         ~/.ssh/id_rsa_aws_dave_992382814940.pem
#
#    In my .ssh/config I have
#
#     Host *amazonaws.com
#         User ec2-user 
#         IdentityFile /Users/dave/.ssh/id_rsa_aws_dave_992382814940.pem
#         #IdentityFile /Users/dave/EC2_keys/prod-us-west-1.pem
#         ServerAliveInterval 10
#         UserKnownHostsFile /dev/null
#         StrictHostKeyChecking no
#
# 4. I can now ssh into the created instance:
#
#     % ssh ec2-user@ec2-3-101-62-223.us-west-1.compute.amazonaws.com
#     Warning: Permanently added 'ec2-3-101-62-223.us-west-1.compute.amazonaws.com' (ED25519) to the list of known hosts.
#      ,     #_
#      ~\_  ####_        Amazon Linux 2023
#     ~~  \_#####\
#     ~~     \###|
#     ~~       \#/ ___   https://aws.amazon.com/linux/amazon-linux-2023
#      ~~       V~' '->
#       ~~~         /
#         ~~._.   _/
#            _/ _/
#          _/m/'
#   
# 5. This is a basic instance, so to run python with a flask server requires a few
#    installs.
#
#     $ sudo yum update -y
#     Dependencies resolved.
#     Nothing to do.
#     Complete!
#     $ sudo yum install python3-pip -y
#     [...]
#     $ pip3 install flask
#
# 6. Copy this file to the new instance.
#
#    On your local machine set the current working directory to the directory
#    containing hw2_example_server.py and then execute:
#
#     $ scp hw2_example_server.py ec2-user@ec2-3-101-62-223.us-west-1.compute.amazonaws.com:
#
# 7. On the new instance run the example server.
#
#     $ python hw2_example_server.py
#
# 8. If I log into the instance from another window, I can curl the server.
#     $ curl "http://172.31.31.222:5000"
#     2024-06-07 04:02:01 UTC
#
# 9. However, if I curl this from my local system, it fails.  This is because we haven't
#    enabled inbound traffic to port 5000.  To do this:
#     a. navigate to the EC2 console in a browser.
#     b. click on instances
#     c. click on the machine instance.
#     d. click on the Security tab.
#     e. within the Security tab, scroll down to security groups.
#     f. clich on the only defined security group.
#     g. scroll down to "Inbound rules"
#     h. click on "Edit inbound rules"
#     i. Click on "Add rule"
#     j. Fill in the following: Type "Custom TCP", Port range 5000, Source type
#        "Anywhere-IPv4", source 0.0.0.0/0.
#     h. Click "Save rules"
#
# 10. From a terminal window on my local computer:
#
#     % curl "http://3.101.62.223:5000"
#     2024-06-07 04:26:22 UTC
#
# Setting up DNS
#
# 1. I used noip.com to create a free domain name flingo.zapto.org.  Create
#    your own free domain.  It will be a subdomain of one of their domains
#    (e.g., zapto.org).
#
# 2. Go to
#
#     https://my.noip.com
#
# 3. click on "Hostname" in the upper-left hand portion of the web page.
# 4. click on "Create Hostname" (or a list of your created hostnames will appear
#    if have already defined one.
# 5. Once a domain name has been defined, click on "Modify" and set the IP address
#    to point to the IP address of the Amazon machine instance.
#

from flask import Flask
from datetime import datetime
import pytz

app = Flask(__name__)


@app.route('/time')
def get_current_time():
    # Set the timezone to UTC
    timezone = pytz.timezone('UTC')
    # Get the current time with the timezone
    current_time = datetime.now(timezone)
    # Format the time as a string
    formatted_time = current_time.strftime('%Y-%m-%d %H:%M:%S %Z')
    return formatted_time

if __name__ == '__main__':
    #app.run(debug=True)
    app.run(host='0.0.0.0', debug=True, port=5000)


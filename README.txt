NOTES

**YOU MUST CLOSE OUT OF TERMINAL AND OPEN NEW ONE IF READING FROM ANOTHER DIRECTORY
OR READING A FILE OF A DIFFERENT TYPE**

I legitimately have no idea why this happens. I tried to gdb the problem but came up 
with no solution.

A segfault will occur if you first read from the current directory and then read 
a file from another folder. In order to fix this, close terminal and read the file 
from that directory to begin with. I'm not sure why this happens. I get a massive 
backtrace and memory map when I segfault that I've never seen occur in the terminal 
before.

Furthermore, sometimes you need root privledges depending on the port number or you
must choose another port if you get the error that the port is already in use. 


Open two terminals
./web_server 8080 (for example)
./web_client http://localhost:8080/hello.txt

Commands should execute properly

To run again on a different directory or client (i.e. web browser) exit out of the 
server and client and run again as noted by the note above.

If you have any questions or concerns about my project or if you have trouble running
it, please don't hesitate to get in contact with me. 

Brent McCorvey

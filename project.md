# Project Requirements

## Step 1 - DONE
In this step your goal is to build an Echo Server

An Echo server listens for connections and the responds to every message sent on a connection with the same message. In other words it echos the message back to the client.

There is an echo server built into most Unix platforms (disabled by default). There is also a formal Echo protocol defined in RFC 862. We're going to implement the TCP version. The specification of which is quite simple:

TCP Based Echo Service

One echo service is defined as a connection based application on TCP. A server listens for TCP connections on TCP port 7. Once a connection is established any data received is sent back. This continues until the calling user terminates the connection.

So your challenge for this step is to create simple TCP/IP server that will listen on port 7007 (we’re going to use 7007 instead of 7 as ports below 1024 require elevated privileges on many operating systems).

When the server receives a message it should echo it back to the client. For now you only need to handle one client connection at a time.

You can test your echo server using a telnet client like so:

% telnet localhost 7007
Trying ::1...
telnet: connect to address ::1: Connection refused
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Hello Coding Challenges!
Hello Coding Challenges!
^]
telnet> quit
Connection closed.

I’ve highlighted the echoed text.

## Step 2 - DONE
Step 2
In this step your goal is to build a simple network client that can send messages to the echo server. When your server starts up it should prompt the user for some input. When the user hits enter the input should be sent to the server and the response received and printed out.

Here’s how that might look if, like me you prefix each line with a hint as to what is happening:

% chatclient
send: test
recv: test
send: quit
%

I suggest you make the client terminate when either quit or exit is entered by the user.


## Step 3 - DONE
In this step your goal is to change your client into a chat client. To do this you’ll want to allow the user to enter their name when they start the chat. Then you’ll want to send messages to the server prefixed with the users name. You can then connect to your echo server to test the client. That might look something like this:

% chatclient
name: John
Hi
John: Hi
John: Hi
exit

In the above the chat client echo’s out locally what the user entered and then echo’s out any messages received from the server. That’s why you see John: Hi twice. Don’t forget to ensure that your client can wait for both user input and a network message at the same time. If you don’t know how to do that look up select using either man or your programming language’s documentation.

## Step 4 - TBD
In this step your goal is to build the chat server. Your server should allow multiple users to connect and - you guessed it - chat! To do that your server will need to be either multithreaded or use an async framework to handle multiple concurrent client connections.

When the server receives a message from one client it will need to broadcast the message back to all the clients, except the sending client.

Once you have it working run the server and a couple of clients and talk to yourself - like I did:

% chatclient
name: John
Welcome to the chatroom!
OtherJohn: Hi!
Hey John!
John: Hey John!
How's things? What are you doing?
John: How's things? What are you doing?
OtherJohn: Writing a Coding Challenge!
Same!
John: Same!
exit
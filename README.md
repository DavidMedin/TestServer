
# What is it?
This is my most recent project.
It is two programs, a server and a client. I was initially going to make a multiplayer game where there are multiple clients connected to one server, but I 
stopped after I got basic text to be transfered between the clients and server. Here's a screenshot of 3 clients (graphical windows) and one server (top left
command line).
![clients](https://user-images.githubusercontent.com/45665232/118322235-6ff74d80-b4c4-11eb-9520-d873e012d39f.png)

# Warning
If you intend to run this, the server will want to get through the firewall, so if you don't want that, you can't run it! Maybe, I haven't tested if you click "no".

# What can you do with it?
all inputs must be surrounded by either double (") or single (') quotes.
* Server
  * SendText(message) -> will send a message to all connected clients
  * Reply(message) -> will send a message to the last client that sent something to the server
  * Quit() -> will just quit, and tell all the clients to close too
* Client
  * SendText(message) -> will send a message to the server
  * Login(name) -> will login as a certain name
  * SendTo(recipient,message) -> send text to the client who has login in as 'recipient'
  

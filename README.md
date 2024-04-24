# C-Networks-Project
# Ryan Barnwell
Project for a Networks class
Limited Functionality, sorting received packets and sending a sorted file of packet data back to the initial sender is not implemented as of 1/30/2024.  The program can be run, by first opening two instances of the terminal.  Then the user can run the client.c file in one terminal.  After the client.c file is running the user can start the server.c file in the other terminal instance.  Files to be sent should be named File00 - File10.
The goal was to randomly send packets to a receiver. The receiver would then sort the packet data using headers and put this data into a single txt file.  This sorted txt file would then be sent back to the sender.
To run the program, the client is first run and then the server is run.  They were tested being run in separate instances of the command terminal.

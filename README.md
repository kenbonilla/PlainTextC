# PlainTextC
A simple client and multi-threaded server for passing a plaintext file, written in C

 TCP client for plaintext files. Will get single file, print contents,
 save file to directory, then exit
 Capable of dynamic file size!
 default port is 8899
 port to be entered as argument client 8888
 
 Multi threaded TCP server, sends plaintext files in same directory.
 Quit program by typing 'q' enter, WARNING: do not exit with active connections
 'c' enter to see how many clients are on the server
 Port must be entered as argument ./server 8888
 default port is 8899
 Capable of handling multiple active connections.
 Capable of dynamic file size
 file name limited to 256 char

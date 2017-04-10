# Console-Windows-Sockets-Using-TCP
Simple console based Chess program to demonstrate using TCP sockets and data transfer from server to client.

Demonstration video can be found on YouTube: https://youtu.be/0c379sFdhXw

Instructions:

In order to run this example you may need to change some of the solution/project settings as described below.

1. Right-Click on the solution and go to properties
2. Change from "Single startup project" to "Multiple startup projects"
   a. Change the Action from None to Start for both the ChessServer and ChessClient projects
   b. Then change the order of startup to ChessServer first (on top) then ChessClient second (below ChessServer)
3. Then go to "Project Dependencies" from the same window and select the ChessClient project
   a. Under "Depends on" click and check off the box next to conlib
4. Now when you click Start it will start the Server then a Client however you will need to right click on the ChessClient project go down to Debug->Start new instance to create the second Client

NOTE: You may need to click Continue in Visual Studio if the window does not appear quite right, this is demonstrated in the video

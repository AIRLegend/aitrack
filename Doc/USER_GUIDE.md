# User guide

## Configuring opentrack and AiTrack

AiTrack sends data over UDP to opentrack, which in turn, sends it to your game, so both of them need to be running.

- You will need to create a new profile (under "Game Data" section, click the "Profile" drop down and create a new profile).

- Then, under "Input", select "UDP over network".

Example of Opentrack configuration:
![](../Images/OpentrackConfig.png)

Then, on AiTrack, jus click "Start tracking". The program will use the default configuration, which assumes that opentrack is on the same machine and it's listening port is 4242 (Opentrack's default). In case you want to use other config, just change it on AiTrack and save it. 
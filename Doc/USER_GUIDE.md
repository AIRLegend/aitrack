# User guide

## Configuring opentrack and AiTrack

AiTrack sends data over UDP to opentrack, which in turn, sends it to your game, so both of them need to be running.

- You will need to create a new profile (under "Game Data" section, click the "Profile" drop down and create a new profile).

- Then, under "Input", select "UDP over network".

- In order to correct some of the noise AITrack has, it's recommended to use Acella filter on Opentrack with pretty high smoothing (over 2 degrees for rotation and about 1mm for position). However, Kalman filter works also okay (adjust its settings as you like).

- Under `Options>Output` invert Pitch, Roll and Z axes.

- Under `Options>Relative translation>Relative translation` set `Mode = Enabled` and disable for X, Y and Z axes.

Example of Opentrack configuration:
![](../Images/OpentrackConfig.png)

Then, on AiTrack, jus click "Start tracking". The program will use the default configuration, which assumes that opentrack is on the same machine and it's listening port is 4242 (Opentrack's default). In case you want to use other config, just change it on AiTrack and save it. 

## Tips

Based on the testing made so far, there are some recommendations for getting the best performance.
 
-  Configure well your movement curves on Opentrack. Leave a little "dead zone" at the beginning of each curve and use the asymetric mapping feature for "Pitch".
- Position your camera at about 0.5-1 meters horizontally from your face.
    * It's better if the camera is directly in front of you, but it doesn't mattter if you have some lateral offset.
-  The camera should be, approximately,  at about your nose-level. Good positions are on top of our monitor, or at its base.
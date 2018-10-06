WarpDriveMFD Mk2 R1 - Made by Artlav, 2007-2010.  
For Orbiter 2010.  
Full add-on files are at https://www.orbithangar.com/searchid.php?ID=2297  
Some assembly required, code provided as is.  
Needs porting to newer Orbiter and Visual Studio

## About
This MFD implements the Gravimetric Field Displacement Engine (aka "the Warp Drive") for any vessel.  
It uses The Original Series scale for Star Trek warp factor.

## How to use
Open that MFD.  
Then,  
Shift+W toggles Warp Drive on and off.  
Shift+= and Shift+- regulates velocity.  
Speed limit - warp 5 or 125 times the speed of light.  
  
The main diffirence from the old WDMFD is that it uses distinctly diffirent model.  
Mk1 changed the velocity of a vessel, making a true motion, while Mk2 changes position without touching the velocity, making a fictional motion.  
In the end product this change  
fixes the incompatibility with save states,  
fixes velocity jumps,  
and removes the Calibrate button.  
All in all making the thing simplier to use.  
No need to use Shift+H to enable StarTrek steering, since it is enabled by the concept of the model.  
  
So, unlike WDMFD Mk1 the vessel's TrueSpace velocity remains unchanged, so you should synchronize velocityes with your target upon arrival (engines if your ship is big, aerobraking if not, lithobraking if you don't care about it).

## All Controls
- Shift+D or DRP returns you to truespace.
- Shift+B or REV changes the direction of the warp speed.
- Shift+G or GRV disables/enables the gravity. No idea why i put that one in it.
- Shift+W or WRP enables/disables the drive.


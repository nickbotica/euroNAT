# euroNAT
A plug-in for Euroscope that displays the live North Atlantic Tracks.

## Installation
Download the latest zip file under [Releases](https://github.com/nickbotica/euroNAT/releases).

Extract and place the euroNAT folder where you like.  
E.g. In the EuroScope folder in Documents.

Place an ISEC.txt waypoints database file in the same folder.  
An old version of ISEC.txt can be found the Documents\EuroScope\DataFiles folder.  
Otherwise purchase the FSNavigator AIRAC from Navigraph or Aerosoft and extract it from there.

![alt text](http://imgur.com/HrhawRZ.png "Folder and file structure")

To enable the euroNAT plugin, in Euroscope go to OTHER SET > Plug-ins ...  
Click on Load, browse to and open euroNAT.dll  
In the Plug-ins Dialog box, select the euroNAT from the list of Plug-ins. Next, in the box on the lower right side double-click on Standard ES radar screen so it move to the box on the left, titled "Allowed to draw on types". This allows euroNAT to draw on the tracks on the radar scope.  
Finally, click the Close button.

## Using euroNAT
By default, once you load euroNAT it will show eastbound and westbound NATs, but not the Concorde routes. You have two ways to change these (and other) settings: using the Options dialog, or using text commands.

To use the Options dialog, type .natoptions in the Message area (bottom of main EuroScope window) and press Enter. You can also type .natshow gui (both commands do the same thing). Then you can modify the options.

### Text commands:

**.natoptions** will show the Options dialog box

**.natshow gui** same as above

**.natshow east** will only show the eastbound tracks (and hide the westbound tracks)

**.natshow west** will only show the westbound tracks (and hide the eastbound tracks)

**.natshow all** will show both eastbound and westbound tracks

**.natshow none** will hide all eastbound, westbound and Concorde tracks

**.natshow conc** will toggle the Concorde tracks on and off

**.natrefresh** will download and parse new data

**.natopt linestyle dotted** will change the lines to dotted style

**.natopt linestyle solid** will change the lines to solid style

**.natopt linestyle none** will erase the lines (only NAT fixes will be shown, with no line in between)

**.natopt letters show** will show the one-letter identifier of each NAT (A, B, C...)

**.natopt letters hide** will hide the one-letter identifier of all NATs

## How it works

### Live Data
This version downloads the live data from https://pilotweb.nas.faa.gov/common/nat.html

So it'll continue to work as long as the site keeps the same URL, and doesn't change the way data is presented too much. I've added error messages that should provide enough information to narrow down any problems that arise.

### Waypoints
The way euroNAT gets the waypoint coordinates has changed from previous versions. The last version used a website that doesn't exist anymore - I want to move away from relying on external content as much as possible.

euroNAT will first check for the waypoint in waypoints.txt located in the same folder as the euroNAT.dll file. This is a small lookup file that only contains the waypoints that have been previously used in a North Atlantic Track. This helps the plugin start fast.

If the waypoint name is not found in waypoints.txt, it'll look for an ISEC.txt file in the same directory as the euroNAT.dll file.  
An old version of ISEC.txt can be found the Documents\EuroScope\DataFiles folder.  
Otherwise purchase the FSNavigator AIRAC from Navigraph or Aerosoft and extract it from there.

If euroNAT finds the waypoint in the ISEC.txt file it'll also add it to waypoints.txt for quick access next time.

## Screenshots

Westbound tracks:
![alt text](http://imgur.com/bcodsoz.png "Westbound tracks")

Eastbound tracks:
![alt text](http://imgur.com/obsWfaX.png "Eastbound tracks")

You also have the option to see the full waypoint names (55N50W) instead of short name (50W):
![alt text](http://imgur.com/SUsvK8B.png "Full waypoint names")

This screenshot shows the Options dialog for euroNAT, as well as the Concorde tracks (static, never get updated). In this case, NATs are shown as solid lines, not dashed lines (using the settings in the Options dialog):
![alt text](http://imgur.com/eFN8zvZ.png "Options dialog and Concorde tracks")


## Authors
Nov 2010 - Michael Freudeman  
Nov 2014 - Merik Nanish  
Aug 2017 - Nick Botica

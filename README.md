# euroNAT
A plug-in for Euroscope that displays the live North Atlantic Tracks.

## Installation
Download the latest zip file under [Releases](https://github.com/nickbotica/euroNAT/releases).

Extract and place the euroNAT folder where you like.
E.g. In the Euroscope folder in My Documents.

## Using euroNAT
To enable the euroNAT plugin, in Euroscope go to OTHER SET > Plug-ins ...
Click on Load, browse to and open euroNAT.dll
In the Plug-ins Dialog box, select the euroNAT from the list of Plug-ins. Next, in the box on the lower right side double-click on Standard ES radar screen so it move to the box on the left, titled "Allowed to draw on types". This allows euroNAT to draw on the tracks on the radar scope.
Finally, click the Close button.

By default, once you load euroNAT it will show eastbound and westbound NATs, but not the Concorde routes. You have two ways to change these (and other) settings: using the Options dialog, or using text commands.

To use the Options dialog, type .natoptions in the Message area (bottom of main EuroScope window) and press Enter. You can also type .natshow gui (both commands do the same thing). Then you can modify the options.

### Text commands:
**.natshow east** will only show the eastbound tracks (and hide the westbound tracks)

**.natshow west** will only show the westbound tracks (and hide the eastbound tracks)

**.natshow all** will show both eastbound and westbound tracks

**.natshow none** will hide all eastbound, westbound and Concorde tracks

**.natshow conc** will toggle the Concorde tracks on and off

**.natopt linestyle** dotted will change the lines to dotted style

**.natopt linestyle** solid will change the lines to solid style

**.natopt linestyle** none will erase the lines (only NAT fixes will be shown, with no line in between)

**.natopt letters** show will show the one-letter identifier of each NAT (A, B, C...)

**.natopt letters** hide will hide the one-letter identifier of all NATs

## Screenshots

Westbound tracks:
![alt text](http://imgur.com/bcodsoz.png "Westbound tracks")

Eastbound tracks:
![alt text](http://imgur.com/obsWfaX.png "Eastbound tracks")

You also have the option to see the full waypoint names (55N50W) instead of short name (50W):
![alt text](http://imgur.com/SUsvK8B.png "Full waypoint names")

This screenshot shows the Options dialog for euroNAT, as well as the Concorde tracks (static, never get updated). In this case, NATs are shown as solid lines, not dashed lines (using the settings in the Options dialog):
![alt text](http://imgur.com/eFN8zvZ.png "Options dialog and Concorde tracks")

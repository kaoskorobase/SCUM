Creating a GUI on Linux

On Linux you cannot use the normal SC GUI Classes, because these are created with the Cocoa system, which Linux does not support.
Instead there is a set of GUI classes called SCUM, written by Stefan Kersten.
The concepts behind SCUM are slightly different, giving you less control over the actual size of objects, but more control over the relative size of objects, allowing for easier rescaling of the objects in your window, when you resize the window, see SCUM_Layout_Management for a detailed explanation.
This Help file tries to give an overview of what is possible with the SCUM system and how to do it.

Overview:
SCUM - the basis of all other SCUM classes, containing some basic variables which are used by SCUM objects.

 SCUMObject - the basis of all SCUM classes that use system resources.  
  SCUMDesktop - class that represents the desktop and gives information and control over all toplevel gui objects.
  SCUMMenu - create a popup menu with selectable items
(not yet documented)
  SCUMView -  root class of the view hierarchy. declares basic properties for
layout and appearance.
   SCUMContainer - root class for layout elements. implementing some basic functions to get info on child objects.
    SCUMBin - container view holding a single child.
     SCUMWindow -  top level window view.
    SCUMBox - container view that lays out child views in rows or columns.
     SCUMHBox - container view that lays out child views horizontally (in columns).
     SCUMVBox - container view that lays out child views vertically (in rows).
    SCUMGrid - container view that lays out child views in a tabular manner, either columns first or rows first.
     SCUMHGrid - tabular container view that lays out child views columns first.
     SCUMVGrid - tabular container view that lays out child views rows first.
   SCUMScrollView - SCUMView with scroll capabilities, used by its child classes
    SCUMList - a scrollable list of text items
    SCUMScope - a scope view of data in a buffer on the internal server
   
   SCUMLabel - SCUMView with a text display
    SCUMTextEntry
    SCUMStringEntry
    SCUMNumberEntry

   SCUMControl
    SCUMButton - button
    SCUMToggle - toggle button
    SCUMBang - bang button, much like in certain graphical sound programming programs
    SCUMChoice - pulldown menu-like button, or combobox
    SCUMSlider - 1-dimensional slider
    SCUMPad - 2-dimensional slider
    SCUMTable - ???

 SCUMEvent - event handlers for SCUM Objects, normally you do not need to use these in your code, as they are called from SCUM Objects.
  SCUMInputEvent
  SCUMKeyEvent
  SCUMMouseEvent
  SCUMScrollWheelEvent
  SCUMContextMenuEvent

SCUMMenuBuilder

SCUMMenuItem
 SCUMMenuSeparator
 SCUMMenuLabel
  SCUMMenuOpen
  SCUMMenuAction
  SCUMMenuToggle
  SCUMMenuRadioProxy
 SCUMMenuClose
 SCUMMenuRadioGroup

SC GUI Classes, that are and can be used with SCUM:

Color - defines the color of objects
Font - defines text fonts (I should add the example in Font for SCUM!)
Point - defines a point on the Cartesian plane.



Playing a Routine that influences the GUI

Routines that play influence the GUI, have to be run on the AppClock. You can use the .play message (defined in SCUMObject) to play a routine, like this:

w.play( Routine({
});


TextAlignment:

1: Left, Top
2: Middle, Top
3: Right, Top
4: Left, Middle
5: Middle, Middle
6: Right, Middle
7: Left, Bottom
8: Middle, Bottom
9: Right, Bottom


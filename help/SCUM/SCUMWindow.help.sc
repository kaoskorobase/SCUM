// ---------------------------------------------------------------------
// SCUMWindow : SCUMBin
// ---------------------------------------------------------------------
//
// top level window view.
//
// see also: SCUMBin
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow.new;
	w.title = "Window example";
	w.bgColor = Color.rand;
	w.initialSize = Size(200, 200);
	w.show;
)

// change the minimum size the window can be resized to
w.minSize = Size(300, 300);

// release the size constraint
w.minSize = 0;

// change resizability
w.resizable = false;
w.resizable = true;

// make fullscreen (may not work properly with all window managers)
(
	w.play(Routine({
		w.screenBounds.postln;
		w.fullscreen = true;
		w.raise;
		5.wait;
		w.screenBounds.postln;
		w.fullscreen = false;
	}));
)

// close window
w.close;

// if you want to add a function it has to call when it is closed, you have to call the destruction of the window explicitly in that function as well:

w.onClose = { "now I am closed".postln; w.destroy };

// you should not use w.close instead of destroy (from the onClose function) if you want the window to disappear from the screen, as this causes the function to call itself again and again and again... causing a horrible loop.
// if you do not add the destroy command to the onClose function, then the only way to get rid of the window on the screen is calling explicitly w.destroy, or restarting the interpreter.

// ---------------------------------------------------------------------
// properties
// ---------------------------------------------------------------------
//
// <> title -- String
//
//    the string displayed in the title bar.
//
// <> initialSize -- Size
//
//    size of the receiver when it is first shown. constrained by the
//    contents' minimum size.
//
// <  screenBounds -- Rect
//
//    bounds of the receiver in screen coordinates.
//
// <> resizable -- Boolean
//
//    wether the receiver can be resized by the user.
//
// <> fullscreen -- Boolean
//
//    wether the receiver occupies the whole screen
//
// <> decorated -- Boolean
//
//    wether window manager decorations are drawn around the
//    receiver. on X11 this property usually needs to be setbefore a
//    window is shown for the first time.
//
// <> modal -- Boolean
//
//    wether the receiver is modal, i.e. grabs input events from other
//    application windows.
//
// ---------------------------------------------------------------------
// instance methods
// ---------------------------------------------------------------------
//
// close
//
//    execute the receiver's action if it is non nil, otherwise
//    destroy the receiver.
//
// updateLayout ( xFit=false, yFit=false )
//
//    recompute the receiver's layout and makes its size fit its
//    contents in one or both dimensions if one of xFit and yFit is
//    true. by default, the window is never shrunk when its layout
//    changes.
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

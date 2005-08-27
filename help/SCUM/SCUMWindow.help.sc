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

s.boot;

(
	var synths;
	w = SCUMWindow.make {
		//~title = "Window example";
		~bgColor = Color.rand;
		~minSize = Size(400, 600);
		SCUMHBox.make {
			~bgColor = Color.green;
			{ |i|
				SCUMVSlider.make { |v|
					~action = {
						[i, v.value].postln;
						synths[i].set(\freq, v.value * 50 + 20)
					};
					//~step = 0.1;
					~bgColor = Color.blue(0.6);
					~fgColor = Color.green(0.8);
					~fill = 1.0;
					~expand = 1.0;
				}
			} ! 4;
		};
	};
	synths = { Synth(\default, [\freq, 20]) } ! 4;
	w.addResources(synths, \release);
	w.show;
)
w.close;

w.decorated = false;
w.decorated = true;

w.use {
	~title = "Fucking hell";
	~minSize = Size(50, 200);
}

w = SCUMWindow.new;
w.recordPropertyChangesDuring { |w|
		w.putProperty(\title, "Window example");
		w.bgColor = Color.rand;
		w.initialSize = Size(200, 200);
	};

w.id
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

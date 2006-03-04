// ---------------------------------------------------------------------
// SCUMView : SCUMObject
// ---------------------------------------------------------------------
//
// root class of the view hierarchy. declares basic properties for
// layout and appearance.
//
// see also: SCUM, SCUMObject, SCUMContainer, SCUM_Layout_Management
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow({ |v|
		v.name = "SCUMView Help Window";
		v.title = v.name;
		v.initialSize = 400;
	});
	v = SCUMView(w, { |v|
		// set some properties
		v.name = "SCUMView Help View";
		v.minSize = Size(200, 100);
		v.bgColor = Color(0.01, 0.2, 0.4);
	});
	w.show;
)

(	// change window padding
	w.minSize = 400;
	w.play(Routine({
		forBy(0, 100, 1, { |i|
			w.xPadding = i;
			w.yPadding = i/2;
			0.01.wait
		});
	}));
)

(	// change view color
	// press cmd-. in the window to stop the clock
	v.play({ v.bgColor = Color.rand; 0.25 });
)

(	// attach a keyDownAction to the view
	// click into the view and press some keys
	v.canFocus = true;
	v.keyDownAction = { | evt |
 		evt.postln;
 		evt.accept;
	}
)

(	// add a focus change notification action
	ActionListener(v, \focus, #{ | view |
		Post << "focus " << (if (view.hasFocus, "gained", "lost")) << "\n";
	});
)

// change view visibility
v.hide;			// equivalent to v.visible = false
v.show;			// equivalent to v.visible = true

(	// add destroyed notifications
	var destroyedHandler;

	destroyedHandler = #{ | view | Post << view << " destroyed\n" };

	ActionListener(w, \destroyed, destroyedHandler);
	ActionListener(v, \destroyed, destroyedHandler);
)

// close the window either with the mouse or programatically
w.close;

// window is invalid now, release the reference
(
	[w, v].collectMsg(\isValid).postln;
	w = nil; v = nil;
)

// ---------------------------------------------------------------------
// instance variables
// ---------------------------------------------------------------------
//
// parent -- SCUMContainer
//
//    parent view
//
// keyDownAction, keyUpAction -- Function
//
//    functions executed after the view's default key handler
//
// ---------------------------------------------------------------------
// properties (view)
// ---------------------------------------------------------------------
//
// <> name -- nil/String
//
//    identifier
//
// <  bounds -- Rect
//
//    bounding rectangle in window coordinates
//
// <> fgColor -- Color
//
//    foreground color
//
// <> bgColor -- Color
//
//    background color
//
// <> visible -- Boolean
//
//    wether view is visible in parent container
//
// <> enabled -- Boolean
//
//    wether view is enabled
//
// <> canFocus -- Boolean
//
//    wether view can receive keyboard focus
//
// <  hasFocus -- Boolean
//
//    wether view currently has keyboard focus
// 
// ---------------------------------------------------------------------
// properties (layout)
// ---------------------------------------------------------------------
//
// <> alignment -- Integer
//
//    alignment of view in available space
//
// <> xExpand -- Float
// <> yExpand -- Float
//
//    weight for expansion into packing space in x/y direction
//
// <> xFill -- Float
// <> yFill -- Float
//
//    ratio for filling available space in x/y direction
//
// <> minSize -- Size
//
//    smallest size the receiver will ever have
// 
// ---------------------------------------------------------------------
// change notification keys
// ---------------------------------------------------------------------
//
// focus
//
//    the receiver has gained or lost focus.
//
// ---------------------------------------------------------------------
// class methods
// ---------------------------------------------------------------------
// 
// *new ( parent, function )
// 
//    return a new instance in container parent. function is passed to
//    SCUMObject.use for setting inital properties.
// 
// *viewClass
// 
//    return the class the primitive object is allocated for. this may
//    not be the receiver.
// 
// ---------------------------------------------------------------------
// instance methods
// ---------------------------------------------------------------------
//
// initDefaults
//
//    set default properties for the receiver.
//
//    called by the view's constructor. override in subclasses for
//    additional specialization, but don't forget to call the
//    superclass implementation.
//
// asView
//
//    convert the receiver to a view. returns the receiver.
//
// action
// action_
//
//    get/set the receiver's action. the action is called when a
//    change was made in the receiver's state by a user
//    action. exactly what triggers the action depends on the
//    particular subclass.
//
// keyDownAction
// keyDownAction_
// keyUpAction
// keyUpAction_
//
//    get/set keyboard actions. keyboard actions are passed a
//    SCUMKeyEvent and are called after the particular default key
//    handler has executed without accepting the event.
//
// show
// hide
//
//    change the receiver's visibility. invisible views aren't drawn
//    and cannot receive input events.
//
// raise
// lower
//
//    raise/lower the receiver inside its parent container. child
//    ordering influences when views are drawn and depends on the
//    container in use.
//
// layout
// layout_
//
//    convenience for getting/setting several layout properties at
//    once. see also: SCUMLayout
//
// updateLayout ( xFit=false, yFit=false )
//
//    update the receiver's window layout. see SCUMWindow.
//
// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// SCUMToggle : SCUMControl
// ---------------------------------------------------------------------
//
// button
//
// see also: SCUMControl, SCUMButton, SCUMBang, SCUMChoice
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------


// A toggle has two states: "true" and "false" between which it toggles when you click on the button. When the button-state is false, it has the foreground color, if the button is true it has the background color.
// you can define an action that is done when the button is toggled.


( 
	w = SCUMWindow.new;
	w.title = "Toggle example";
	w.initialSize = Size(300, 300);

    c = SCUMVBox( w );

    v = SCUMToggle(c, { |v|
		v.canFocus = false;
		v.expand = 1;
		v.fill = 1;
		v.bgColor = Color.white;
		v.fgColor = Color.black;
		v.action = { v.value.postln; };
	});
	w.show;
)


// ---------------------------------------------------------------------
// SCUMBang : SCUMControl
// ---------------------------------------------------------------------
//
// button
//
// see also: SCUMControl, SCUMToggle, SCUMButton, SCUMChoice
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------


// A Bang has one state, which it triggers when you push the button.
(
	w = SCUMWindow.new;
	w.title = "Bang example";
	w.initialSize = Size(300, 300);

    c = SCUMVBox( w );

    v = SCUMBang(c, { |v|
		v.expand = 1;
		v.fill = 1;
		v.bgColor = Color.white;
		v.fgColor = Color.black;
		v.action = { "BANG!".postln; };
	});
	w.show;
)

// SCUMBang has one special property: the flashTime:
v.flashTime.postln;
// default it is set to 0.1


// increase the flashtime
v.flashTime = 3;
// now bang it again to see the effect
// the effect is only visual. the action is done as soon as the button was pushed.


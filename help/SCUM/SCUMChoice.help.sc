// ---------------------------------------------------------------------
// SCUMChoice : SCUMControl
// ---------------------------------------------------------------------
//
// button
//
// see also: SCUMControl, SCUMToggle, SCUMBang, SCUMButton
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

// Choice gives a kind pulldown menu choice of states
(
	w = SCUMWindow.new;
	w.title = "Choice example";
	w.initialSize = Size(300, 50);

    c = SCUMVBox( w );

    v = SCUMChoice(c, { |v|
		v.expand = 1;
		v.fill = 1;
		v.bgColor = Color.white;
		v.fgColor = Color.black;
		v.states = [ "on" , "off", "neither"];
		v.action = LookupTable({ "now I am on".postln }, { "now I am off".postln; },{"now I am neither".postln});
	});
	w.show;
)


v.states.postln;
v.states = [ "really on" , "really off", "neither"];
// an array of strings with the names of the different items for the pulldown menu.

v.action = LookupTable({ "now I am really on".postln }, { "now I am really off".postln; },{"now I am neither on or off".postln});
// linking an action to each state



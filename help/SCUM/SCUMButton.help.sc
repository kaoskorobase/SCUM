// ---------------------------------------------------------------------
// SCUMButton : SCUMControl
// ---------------------------------------------------------------------
//
// button
//
// see also: SCUMControl, SCUMToggle, SCUMBang, SCUMChoice
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

// SCUM Button example
// a button can have a label text and any number of different states

(
	w = SCUMWindow.new;
	w.title = "Button example";
	w.initialSize = Size(300, 300);

    c = SCUMVBox( w );

    v = SCUMButton(c, { |v|
		v.canFocus = false;
		v.padding = 4;
		v.expand = 1;
		v.fill = 1;
		v.textAlignment = 5;
		v.bgColor = Color.white;
		v.fgColor = Color.black;
		v.font = Font("Helvetica-Bold", 14);
		v.states = [ ["on"] , ["off"], ["neither"]];
		v.action = LookupTable({  v.value.post; " now I am on".postln; }, {  v.value.post; " now I am off".postln; },{  v.value.post; " now I am neither".postln});
	});
	w.show;
)

states : an array of arrays with strings with the names of the different items between which is switched when clicking on the button

action : linking an action to each state

value : get the value of the current state


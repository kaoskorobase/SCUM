// ---------------------------------------------------------------------
// SCUMNumberEntry : SCUMTextEntry: SCUMLabel
// ---------------------------------------------------------------------
//
// view that displays a changeable number.
//
// see also: SCUMTextEntry, SCUMStringEntry
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(	// create some labels
	w = SCUMWindow.new;
	w.title = "NumberEntry example";

	c = SCUMNumberEntry(w, { |v|
		v.value = 3; // initial displayed value.
		v.yExpand = 1;
		v.yFill = 1;
		v.canFocus = true;
		v.action = { "value ".post; v.value.postln; };// prints the current value if the value is changed through mouse or keyboard actions.
	});
w.show;
)

// get the current value
c.value

// set the value
c.value = 100;



// other properties:

sendMouse : true or false
reacting on mouse actions

sendScroll : true or false
reaction on scrollwheel actions

precision : precision of display

c.precision = 2;
c.value = 1000;

c.defaultValue;  // cannot be set!

c.fineStep
c.coarseStep

//c.courseStep = 2

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

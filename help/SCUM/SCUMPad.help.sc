// ---------------------------------------------------------------------
// SCUMPad : SCUMControl
// ---------------------------------------------------------------------
//
// 2-dimensional slider
//
// see also: SCUMControl, SCUMSlider, SCUMTable
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

// SCUM Pad example
// a Pad is a two dimensional slider

(
	w = SCUMWindow.new;
	w.title = "Pad example";
	w.initialSize = Size(300, 300);

    c = SCUMVBox( w );

    v = SCUMPad(c, { |v|
		v.expand = 1;
		v.fill = 1;
		v.bgColor = Color.white;
		v.fgColor = Color.black;
		v.action = { v.value.postln; };
	});
	w.show;
)

// get the step size of the slider
v.step;
// 0 is a continuous slider

// set the step size of the slider
v.step = 0.1;

// step sets the step for both x and y direction, to set it separately use:

v.step = Point(0.1,0.04);

// or
v.xStep = 0.15;
v.yStep = 0.2;


//linking an action to each state
v.action

//get the value of the current state
v.value;
v.pos;
// get the x-value
v.x;
// get the y-value
v.y;

// these are also setters:
v.value = Point( 0.8, 0.1 );
v.pos = Point( 0.3, 0.3 );
v.x = 0.5;
v.y = 0.9;

// bug:
// the y-value during set is calculated from the top, instead of from the bottom!
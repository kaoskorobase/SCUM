// ---------------------------------------------------------------------
// SCUMSlider : SCUMControl
// ---------------------------------------------------------------------
//
// 1-dimensional slider
//
// see also: SCUMControl, SCUMPad, SCUMTable
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

// SCUM Slider example
// with a slider you can select one value from a range of values (default between 0 and 1)

(
	w = SCUMWindow.new;
	w.title = "Slider example";
	w.initialSize = Size(20, 300);

    c = SCUMVBox( w );

    v = SCUMSlider(c, { |v|
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

// get the size of the sliding button
v.thumbSize;

// set the size of the sliding button
v.thumbSize = 20;


// if steady is false, you can just click somewhere on the slider to jump to a new value. If steady is true, you have to click in the window and drag the slider button to a new value (you do not necessarily have to click on the button though).
v.steady;
v.steady = true;

//linking an action to each state
v.action

//get the value of the current state
v.value;



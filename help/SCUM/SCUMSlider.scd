// ---------------------------------------------------------------------
// SCUMSlider : SCUMControl
// ---------------------------------------------------------------------
//
// 1-dimensional slider
//
// see also: SCUMControl, SCUMPad, SCUMTable, SCUMWindow,
// SCUMNumberEntry
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
SCUMWindow.make {
	~title = "Slider example";
	~initialSize = Size(300, 300);
	v = SCUMHBox.make {
		// create vertical slider
		SCUMVSlider.make {
			~name = "vertical";
			~expand = 1;
			~fill = 1;
			~bgColor = Color.grey(0.3);
			~fgColor = Color.grey(0.8);
			~action = #{ |v| [\vertical, v.value].postln };
		};
		// create horizontal slider
		SCUMHSlider.make {
			~name = "horizontal";
			~expand = 1;
			~fill = 1;
			~bgColor = Color.grey(0.3);
			~fgColor = Color.grey(0.8);
			~action = #{ |v| [\horizontal, v.value].postln };
		};
	};
}.show;
)

// get the step size of the slider
// 0 is a continuous slider
v.children.collect(_.step);

// set the step size of the slider
v.children.do(_.step_(0.1));

// get the size of the sliding button
v.children.collect(_.thumbSize);

// set the size of the sliding button
v.children.do(_.thumbSize_(20));

// if steady is false, you can just click somewhere on the slider to
// jump to a new value. If steady is true, you have to click in the
// window and drag the slider button to a new value (you do not
// necessarily have to click on the button though).
v.children.collect(_.steady);
v.children.do(_.steady_(true));

// action is called when the value changes
v.children.do(_.action_(#{ |v| "slider % has value %\n".postf(v, v.value) }));

// get the current value
v.children.collect(_.value);

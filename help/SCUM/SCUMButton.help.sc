// ---------------------------------------------------------------------
// SCUMButton : SCUMControl
// ---------------------------------------------------------------------
//
// Button with multiple states.
//
// see also: SCUMToggle, SCUMBang, SCUMChoice.
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	var makeStates;
	makeStates = { | ... args |
		args.collect { |x| [x, Color.rand(0.6, 0.9), Color.rand(0.2, 0.5)] }
	};
	w = SCUMWindow.new {
		~padding = 15;
		~bgColor = Color.rand(0.2, 0.3);
		~children = {
			SCUMHBox.make {
				~homogenous = true;
				~children = {
					var font, a, b, c, d;
					font = Font("Courier", 36);
					a = SCUMButton.make {
						~font = font;
						~states = makeStates.(".", "-", "^", "-");
						~fill = 1;
						~action = { b.value = b.value + 1 };
					};
					b = SCUMButton.make {
						~font = font;
						~states = makeStates.("-", ".", "-", "^");
						~fill = 1;
						~action = { c.value = c.value + 1 };
					};
					c = SCUMButton.make {
						~font = font;
						~states = makeStates.("^", "-", ".", "-");
						~fill = 1;
						~action = { d.value = d.value + 1 };
					};
					d = SCUMButton.make {
						~font = font;
						~states = makeStates.("-", "^", "-", ".");
						~fill = 1;
						~action = { a.sched(0.1) { a.value = a.value + 1 } };
					};
				}
			}
		}
	};
	w.show;
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

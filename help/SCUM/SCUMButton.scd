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
SCUMWindow.make {
	~title = "Button example";
	~initialSize = Size(300, 300);
	SCUMVBox.make {
		SCUMButton.make {
			~canFocus = false;
			~padding = 4;
			~expand = 1;
			~fill = 1;
			~textAlignment = 5;
			~bgColor = Color.white;
			~fgColor = Color.black;
			~font = Font("Helvetica-Bold", 14);
			~states = [["on"] , ["off"], ["neither"]];
			~action = SCUMButtonAction(
				{ |v| v.value.post; " now I am on".postln; },
				{ |v| v.value.post; " now I am off".postln; },
				{ |v| v.value.post; " now I am neither".postln }
			);
		}
	}
}.show;
)

// ---------------------------------------------------------------------
// a really contrived example

(
var makeStates;
makeStates = { | ... args |
	args.collect { |x| [x, Color.rand(0.6, 0.9), Color.rand(0.2, 0.5)] }
};
w = SCUMWindow.make {
	~padding = 15;
	~bgColor = Color.rand(0.2, 0.3);
	SCUMHBox.make {
		var font = Font("Courier", 36), a, b, c, d;
		~homogenous = true;
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
};
w.show;
)

// ---------------------------------------------------------------------
// properties
// ---------------------------------------------------------------------
//
// <> states -- Array
//
//    array of arrays with strings with the names of the different
//    items between which is switched when clicking on the button and
//    optional foreground and background colors:
//
//        ["text", Color.blue, Color.green]
//
// <> action -- Function
//
//    function called for each state transition
//
// <> value -- Integer
//
//    current state index

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

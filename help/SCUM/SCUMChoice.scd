// ---------------------------------------------------------------------
// SCUMChoice : SCUMControl
// ---------------------------------------------------------------------
//
// Menu button.
//
// see also: SCUMBang, SCUMButton, SCUMToggle.
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
w = SCUMWindow.make {
	~padding = 15;
	SCUMVBox.make {
		SCUMLabel.make {
			~text = "Make your choice!";
			~font = Font("Helvetica", 20);
		};
		SCUMChoice.make {
			~font = Font("Helvetica", 18);
			~states = ["jazz", "funk", "rnb", "folk", "country"];
			~expand = 1;
			~fill = 1;
			~action = { |v|
				("you want " ++ v.states[v.value] ++ ", you got it.").postln
			}
		};
	}
};
w.show;
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

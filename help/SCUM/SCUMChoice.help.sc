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
	w = SCUMWindow.new {
		~padding = 15;
		~children = {
			SCUMVBox.make {
				~children = {
					SCUMLabel.make {
						~text = "Make your choice!";
						~font = Font("Helvetica", 22);
					};
					SCUMChoice.make {
						~font = Font("Helvetica", 18);
						~states = ["jazz", "funk", "rnb", "folk", "country"];
						~expand = 1;
						~fill = 1;
						~action = { |v|
							("you want " ++ v.states[v.value] ++ ", you got it.").postln
						};
					}
				}
			}
		}
	};
	w.show;
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

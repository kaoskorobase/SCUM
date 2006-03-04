// ---------------------------------------------------------------------
// SCUMToggle : SCUMControl
// ---------------------------------------------------------------------
//
// Toggle button.
//
// see also: SCUMBang, SCUMButton, SCUMChoice.
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
w = SCUMWindow.make {
	~padding = 15;
	SCUMToggle.make {
		~name = "toggle";
		~minSize = 30;
		~action = { |v| v.value.if("on", "off").postln };
	}
};
w.show;
)

(
w["toggle"].use {
	~fgColor = Color.yellow(0.4);
	~bgColor = Color.yellow(0.2);
}
)

w["toggle"].value_(true);				// executes action
w["toggle"] -- { |v| v.value_(false) }	// doesn't execute action

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

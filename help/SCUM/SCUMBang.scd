// ---------------------------------------------------------------------
// SCUMBang : SCUMControl
// ---------------------------------------------------------------------
//
// Oneshot button.
//
// see also: SCUMControl, SCUMButton, SCUMChoice, SCUMToggle.
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
w = SCUMWindow.new {
	~padding = 15;
	SCUMBang.make {
		var stream;
		stream = Pseq([Pseq(#[\KISS], 2), Pseq(#[\BANG], 2)], inf).asStream;
		~minSize = 40;
		~action = { stream.next.postln };
	}
};
w.show;
)

// setting flashTime in seconds (default 0.2)
w.children.first.flashTime;
w.children.first.flashTime = 0.4;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

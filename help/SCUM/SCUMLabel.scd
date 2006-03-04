// ---------------------------------------------------------------------
// SCUMLabel : SCUMView
// ---------------------------------------------------------------------
//
// view that displays a single line of text.
//
// see also: SCUMTextEntry
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
// create some labels
SCUMWindow.make {
	~title = "Label example";
	c = SCUMVBox.make {
		(8..22).do { |i|
			SCUMLabel.make {
				~text = "the brown cow jumps over the white fox";
				~font = Font("Helvetica", i);
				~yExpand = i;
				~yFill = 1;
			}
		};
	}
}.show;
)

(
// fun with textAlignment
// resize the window a bit!
c.do { | view |
	view.play {
		view.textAlignment_(1 rrand: 9);
		0.2 rrand: 0.5
	}
}
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

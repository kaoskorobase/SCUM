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

(	// create some labels
	w = SCUMWindow.new;
	w.title = "Label example";

	c = SCUMVBox(w);

	(8..22).do { |i|
		SCUMLabel(c, { |v|
			v.text = "the brown cow jumps over the white fox";
			v.font = Font("Helvetica", i);
			v.yExpand = i;
			v.yFill = 1;
		})
	};
	w.show;
)

(	// fun with textAlignment
	// resize the window a bit!
	c.do { | view |
		view.play {
			view.textAlignment_(1 rrand: 9);
			0.2 rrand: 0.5
		}
	}
)

// ---------------------------------------------------------------------
// $Id: SCUMLabel.help.sc,v 1.1 2004/07/30 16:20:14 steve Exp $
// ---------------------------------------------------------------------

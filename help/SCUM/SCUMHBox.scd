// ---------------------------------------------------------------------
// SCUMHBox : SCUMBox
// ---------------------------------------------------------------------
//
// container view that lays out child views horizontally (in columns).
//
// see also: SCUMBox, SCUMVBox
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(	// create an HBox with some labels
	w = SCUMWindow.new;
	w.title = "HBox example";

	c = SCUMHBox(w);
	c.bgColor = Color.white;

	#[\red, \green, \blue].do({ | color, i |
		SCUMLabel(c, { |v|
			v.text = "Label" + (i+1).asString;
			v.bgColor = Color.perform(color);
			v.alignment = i+1;
		});
	});
	w.show;
	// try to resize the window before proceeding
)

// child views always expand vertically. now make them expand
// horizontally with a ratio of 1:2:3 but don't fill available space
(
	c.children.do { | c, i |
		c.xExpand = i+1;
		c.xFill = 0;
	};
)

(	// randomly change alignment. press cmd-. in the window to stop the
	// clock.
	c.play({
		var i, a;
		i = c.size.rand;
		a = rrand(1, 9);
		[i, a].postln;
		c.at(i).alignment = a;
		0.5;
	})
)

// make all views fill extra x space
c.doMsg(\xFill_, 1);

// make all views fill extra y space
c.doMsg(\yFill_, 1);

// make the box homogenous
c.homogenous = true;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

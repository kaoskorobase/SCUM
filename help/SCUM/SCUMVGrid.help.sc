// ---------------------------------------------------------------------
// SCUMVGrid : SCUMGrid
// ---------------------------------------------------------------------
//
// tabular container view that lays out child views rows first.
//
// see also: SCUMGrid, SCUMHGrid
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow({ |v| v.title = "VGrid example" });
	c = SCUMVGrid(w, { |v| v.wrap = 3 });
	9.do({ |i|
		SCUMLabel(c, { |v|
			v.text = (i+1).asString;
			v.expand = i+1;
			v.fill = 1;
			v.bgColor = Color.rand;
		});
	});
	w.show;
)

// make the grid homogenous
c.homogenous = true;

// add some spacing
c.xSpacing = 5;
c.ySpacing = 10;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

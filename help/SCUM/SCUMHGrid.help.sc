// ---------------------------------------------------------------------
// SCUMHGrid : SCUMGrid
// ---------------------------------------------------------------------
//
// tabular container view that lays out child views columns first.
//
// see also: SCUMGrid, SCUMVGrid
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow({ |v| v.title = "HGrid example" });
	c = SCUMHGrid(w, { |v| v.wrap = 3 });
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
c.xSpacing = 10;
c.ySpacing = 5;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

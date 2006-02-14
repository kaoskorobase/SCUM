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
SCUMWindow.make {
	~title = "HGrid example";
	c = SCUMHGrid.make {
		~dimensions = Size(3, 4);
		(~dimensions.width * ~dimensions.height).do { |i|
			SCUMLabel.make {
				~text = (i+1).asString;
				~expand = i+1;
				~fill = 1;
				~bgColor = Color.rand;
			};
		};
	};
}.show;
)

// make the grid homogenous
c.homogenous = true;

// add some spacing
c.xSpacing = 10;
c.ySpacing = 5;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

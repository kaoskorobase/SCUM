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
SCUMWindow.make {
	~title = "VGrid example";
	c = SCUMVGrid.make {
		~dimensions = Size(4, 3);
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
c.xSpacing = 5;
c.ySpacing = 10;

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

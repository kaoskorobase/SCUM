// ---------------------------------------------------------------------
// SCUMList : SCUMScrollView
// ---------------------------------------------------------------------
//
// scrollable list of text labels.
//
// see also: SCUMScrollView
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

// PARTIALLY IMPLEMENTED

(
SCUMWindow.make {
	~title = "List example";
	~initialSize = Size(400, 400);

	SCUMList.make {
		~items = Pattern.allSubclasses.collectMsg(\asString);
		~fgColor = Color.grey(0.7);
		~bgColor = Color(0.2, 0, 0.5);
		~fgColorSel = Color.white;
		~bgColorSel = Color.grey(0.2);
		~action = #{ | view | view.value.postln };
	};
}.show;
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

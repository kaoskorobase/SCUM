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

(
	w = SCUMWindow.new;
	w.title = "List example";
	w.initialSize = Size(400, 400);

	l = SCUMList(w);
	l.items = UGen.allSubclasses.collectMsg(\asString);
	l.fgColor = Color.grey(0.7);
	l.bgColor = Color(0.2, 0, 0.5);
	l.fgColorSel = Color.white;
	l.bgColorSel = Color.grey(0.2);
	l.action = #{ | view | view.value.postln };

	w.show;
)

// ---------------------------------------------------------------------
// $Id: SCUMList.help.sc,v 1.1 2004/07/30 16:20:14 steve Exp $
// ---------------------------------------------------------------------

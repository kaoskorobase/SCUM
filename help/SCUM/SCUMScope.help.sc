// ---------------------------------------------------------------------
// SCUMScope : SCUMScrollView
// ---------------------------------------------------------------------
//
// scope view
//
// see also: SCUMScrollView
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow.new;
	w.title = "Scope example";
	w.initialSize = Size(400, 100);

	v = SCUMScope(w);
	v.fgColor = Color.red;

	w.show;
)

(
	s = Server.internal;
	s.waitForBoot {
		b = Buffer.alloc(s, 441, 1);
		v
		.addResource(b)
		.bufnum_(b.bufnum)
		.addResource(
			{
				ScopeOut.ar(Saw.ar(MouseY.kr(10, 2000), mul: MouseX.kr(0, 1)), b.bufnum);
			}.play(s)
		);
	};
)

// ---------------------------------------------------------------------
// $Id: SCUMScope.help.sc,v 1.1 2004/07/30 16:20:14 steve Exp $
// ---------------------------------------------------------------------

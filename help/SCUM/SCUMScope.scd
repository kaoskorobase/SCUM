// ---------------------------------------------------------------------
// SCUMScope : SCUMScrollView
// ---------------------------------------------------------------------
//
// scope view
//
// see also: SCUMScrollView, ScopeOut
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
	w = SCUMWindow.new;
	w.title = "Scope example";
	w.initialSize = Size(400, 200);

	v = SCUMScope(w);
	v.fgColor = Color.red;

	w.show;
)

(
	s = Server.internal;
	s.waitForBoot {
		b = Buffer.alloc(s, 441, 1);
		v
		.bufnum_(b.bufnum)
		.addResource(b)
		.addResource(
			{
				var z;
				z = Saw.ar(MouseY.kr(10, 2000), mul: MouseX.kr(0, 1));
				ScopeOut.ar(z * 4, b.bufnum);
				Out.ar(0, z * 0.4);
			}.play(s)
		);
	};
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

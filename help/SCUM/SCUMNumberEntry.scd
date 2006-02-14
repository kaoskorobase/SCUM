// ---------------------------------------------------------------------
// SCUMNumberEntry : SCUMTextEntry
// ---------------------------------------------------------------------
//
// editable number box.
//
// see also: SCUMTextEntry, SCUMStringEntry
//
// ---------------------------------------------------------------------
// examples
// ---------------------------------------------------------------------

(
var compoundSlider = { | action |
	SCUMVBox.make {
		var a, b;
		~expand = 1;
		~fill = 1;
		a = SCUMVSlider.make {
			~minSize = Size(15, 200);
			~expand = 1;
			~fill = 1;
			~action = { |v|
				b.value = v.value;
			}
		};
		b = SCUMNumberEntry.make {
			~min = 0;
			~max = 1;
			~value = a.value;
			~precision = 4;
			~minSize = "0.0000";
			~frozen = true;
			~yPadding = 1;
			~xPadding = 1;
			~action = { |v|
				a -- { a.value = v.value };
				action.value(v);
			};
		};
	};
};
SCUMWindow.make {
	SCUMHBox.make {
		{ |i| compoundSlider.value({ |v| [i, v.value].postln }) } ! 4
	};
}.show
)

// ---------------------------------------------------------------------
// $Id$
// ---------------------------------------------------------------------

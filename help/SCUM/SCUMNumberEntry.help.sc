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
var compoundSlider;
compoundSlider = {
	SCUMVBox.make {
		~expand = 1;
		~fill = 1;
		~children = {
			var a, b;
			a = SCUMVSlider.make {
				~minSize = Size(15, 200);
				~expand = 1;
				~fill = 1;
				~action = { |v| b.value_(v.value) }
			};
			b = SCUMNumberEntry.make {
				~minSize = "0.0000";
				~value = a.value;
				~precision = 4;
				~frozen = true;
				~yPadding = 1;
				~xPadding = 1;
			};
		}		
	};
};

SCUMWindow.new {
	//~initialSize = 200;
	~children = {
		SCUMHBox.make {
			~children = { compoundSlider ! 4 }
		};
		~parent.show;
	}
}
)

// ---------------------------------------------------------------------
// $Id: SCUMList.help.sc 2 2004-07-30 16:20:12Z steve $
// ---------------------------------------------------------------------

SCUMControl : SCUMView {
	/*
	*initClass {
		this.addProperty(\value, Float /* reasonable default */, 0.0);
	}
	*actionProperty { ^\value }
	*/
	
	// connecting with models
	connect { | controlValue |
		var spec;
		spec = controlValue.spec;
		this.value_(controlValue.input);
		this.action_({ controlValue.input_(this.value) });
		this.step_(spec.unmap(spec.step));
		this.addResource(
			ActionListener(
				controlValue,
				\synch,
				{ this.value_(controlValue.input) }
			)
		);
	}

	// properties
	/*
	*propertyKeys {
		^super.propertyKeys ++ [\value]
	}
	value { ^this.getProperty(\value) }
	value_ { |v| this.setActiveProperty(\value, v) }
	*/
	value { ^0 }
	value_ { }
}

// EOF
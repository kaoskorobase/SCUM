SCUMControl : SCUMView
{
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
	*propertyKeys {
		^super.propertyKeys ++ [\value]
	}
	value { ^this.getProperty(\value) }
	value_ { |v| this.setProperty(\value, v) }
	valueAction_ { |v|
		this.setPropertyAction(\value, v, {
			this.doAction;
			this.changed(\value);
		})
	}
}

// EOF
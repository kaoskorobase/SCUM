SCUMObject : SCUM {
	var dataPtr, resources;

	use { | function |
		// this.properties =
		Environment.make({
			~obj = this;
			function.value(this)
		});
	}

	// testing
	isValid { ^dataPtr.notNil }
	notValid { ^dataPtr.isNil }

	// destruction
	destroy {
		if (this.isValid) {
			this.prDestroy;
			this.prDestroyed;
		}
	}

	// properties
	*propertyKeys {
		// return _settable_ properties
		^#[]
	}
	properties {
		^this.class.propertyKeys.collectAs(
			{ | key | key -> this.perform(key) }, IdentityDictionary
		);
	}
	properties_ { | dict |
		dict.keysValuesDo { | key, value |
			this.tryPerform(key.asSetter, value);
		}
	}
	getProperty { | key, value |
		^this.prGetProperty(key, value)
	}
	setProperty { | key, value |
		// returns true if the action needs to be called
		^this.prSetProperty(key, value)
	}
	setPropertyAction { | key, value, action |
		// executes function if property changed
		if (this.setProperty(key, value) === true, action);
	}
	// backward compat
	setPropertyChanged { | key, value, action |
		this.setPropertyAction(key, value, action);
	}

	// resources
	addResource { | resource |
		resources = resources.add(resource);
	}

	// scheduling
	sched { | delta, function |
		AppClock.sched(delta, {
			if (this.isValid) {
				function.value(this)
			}
		})
	}
	play { | function |
		AppClock.play({
			if (this.isValid) {
				function.value(this)
			}
		})
	}

	// PRIVATE
	prDestroy {
		_SCUM_Object_Destroy
		^this.primitiveFailed
	}
	prDestroyed {
// 		[this.class.name, thisMethod.name, this].postln;
		// invalidate
		dataPtr = nil;
		// notify dependants
		this.changed(\destroyed);
		// release resources
		resources.do({ | resource | resource.tryPerform(\free) });
		// break links to dependants
		this.release;
	}
	prGetProperty { | key, value |
		_SCUM_Object_GetProperty
		^this.primitiveFailed
	}
	prSetProperty { | key, value |
		_SCUM_Object_SetProperty
		^this.primitiveFailed
	}
}

// EOF
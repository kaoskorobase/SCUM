SCUMObject : SCUM {
	var <id, properties, blockedDepth=0, mutedDepth=0;
	var <resources, <dependants;
	classvar propertySpecs;
	classvar propertyDefaults;
	classvar <>preferredClock;

	*initClass {
		preferredClock = AppClock;
		ActionListener(SCUM, \connected, {
			SCUM.registerObjectMethod("/changed", {Ê| obj, name ... args |
				obj.doBlocked {
					obj.tryPerform(("prSet_" ++ name).asSymbol, *args)
				}
			});
		});
	}
	*serverClass {
		^this
	}
	
	// property defaults
	*propertiesClass {
		^IdentityDictionary
	}
	*propertyDefaults {
		var defaults;
		^(propertyDefaults ?? { propertyDefaults = IdentityDictionary.new }).at(this) ?? {
			defaults = IdentityDictionary.new;
			defaults.parent_(this.superclass.tryPerform(\propertyDefaults));
			propertyDefaults.put(this, defaults);
			defaults
		}
	}
	*addProperty { | name default |
		this.propertyDefaults.put(name, default)
	}
	*addAllProperties { | dict |
		this.propertyDefaults.putAll(dict)
	}
	*propertyDefault { | name |
		^this.propertyDefaults.at(name).value
	}
	
	// model support
	// FIXME: Model should really be consistent with Object!
	changed { | what ... moreArgs |
		dependants.do({ | item |
			item.update(this, what, *moreArgs);
		});
	}
	addDependant { | dependant |
		(dependants ?? { dependants = IdentitySet.new(4) }).add(dependant)
	}
	removeDependant { | dependant |
		if (dependants.notNil) {
			dependants.remove(dependant);
		}
	}
	release {
		dependants = nil;
	}
	
	// environment support
	use { | function |
		var saveEnvir, envir, result;
		saveEnvir = currentEnvironment;
		envir = Environment.new.put(\thisObject, this);
		currentEnvironment = EnvironmentProxy(
			{ |k|
				if (this.respondsTo(k))
				{ this.perform(k) }
				{ envir.at(k) }
			},
			{ |k,v|
				if (this.respondsTo(k.asSetter))
				{ this.perform(k.asSetter, v) }
				{ envir.put(k, v) }
			}
		);
		protect {
			result = function.value(this);
		}{
			currentEnvironment = saveEnvir;
		};
		^result
	}

	// testing
	isValid { ^id.notNil }
	notValid { ^id.isNil }

	// destruction
	destroy {
		if (this.isValid) {
			this.prDestroy;
			this.prDestroyed;
		}
	}

	// messages
	newMsg { | ... args |
		^["new", this.class.serverClass.asString, id] ++ args
	}
	freeMsg {
		^["free", id]
	}
	getPropertyMsg { |Ê... keys |
		^["get", id] ++ keys
	}
	setPropertyMsg { | key value |
		var msg;
		msg = CollStream.on([]);
		msg.putAll(["set", id, key]);
		value.scumEncodeOSC(msg);
		^msg.contents
	}
	setPropertiesBundle { | dict bundle([]) |
		dict.keysValuesDo { | key value |
			bundle = bundle.add(this.setPropertyMsg(key, value));
		};
		^bundle
	}
	
	// action
	doAction { | aspect | }
	doActionUnlessMuted { | ...args |
		if (this.isMuted.not) {
			this.doAction(*args);
		}
	}
	
	// blocking and muting
	isBlocked {
		^blockedDepth > 0
	}
	doBlocked { | function |
		protect {
			blockedDepth = blockedDepth + 1;
			function.value(this);
		}{
			blockedDepth = blockedDepth - 1;
		}
	}
	isMuted {
		^mutedDepth > 0
	}
	doMuted { | function |
		protect {
			mutedDepth = mutedDepth + 1;
			function.value(this);
		}{
			mutedDepth = mutedDepth - 1;
		}
	}
	-- { | function | ^this.doMuted(function) }

	// properties
	*propertyKeys {
		// return _settable_ properties
		^#[]
	}
	properties {
		^properties ?? { properties = this.class.propertiesClass.new }
	}
	properties_ { | dict |
		dict.keysValuesDo { | k v | this.tryPerform(k.asSetter, v) }
	}
	getProperty { | name |
		^this.properties.atFail(name, { this.class.propertyDefault(name) })	}
	putProperty { | name value | // doesn't send or notify
		this.properties.put(name.asSymbol, value);
	}
	setProperty { | name value | // sends
		this.putProperty(name, value);
		this.prSetServerProperty(name, value);
	}
	setActiveProperty { | name value | // sends and notifies
		if (value != this.getProperty(name)) {
			this.setProperty(name, value);
			this.doActionUnlessMuted(name);
		}
	}
	setPropertyChanged { | name value changedFunction |
		if (value != this.perform(name)) {
			changedFunction.value(value);
			this.prSetServerProperty(name, value);
			this.doActionUnlessMuted(name);
		}
	}
	recordPropertyChangesDuring { | function |
		var saveProperties, changes;
		saveProperties = this.properties;
		properties = changes = this.class.propertiesClass.new.parent_(saveProperties);
		this.doBlocked(function);
		if (saveProperties.isEmpty && changes.isEmpty) {
			properties = nil;
		}{
			properties = saveProperties.putAll(changes);
		};
		^changes
	}
	// resources
	addResource { | resource, freeMsg=\free |
		resources = resources.add(resource -> freeMsg);
	}
	addResources { | array, freeMsg=\free |
		array.do { | elt | this.addResource(elt, freeMsg) }
	}
	freeResources {
		resources.do { | assoc | assoc.key.tryPerform(assoc.value) }
	}

	// scheduling
	sched { | delta function clock(preferredClock) |
		clock.sched(delta, {
			if (this.isValid) {
				function.value(this)
			}
		})
	}
	play { | function clock(preferredClock) |
		clock.play({
			if (this.isValid) {
				function.value(this)
			}
		})
	}

	// PRIVATE
	prInitID { | oid |
		id = oid ?? {ÊSCUM.nextObjectID };
		SCUM.putObject(id, this);
	}
	prDestroy {
		SCUM.sendMsg(*this.freeMsg);
	}
	prDestroyed {
 		// [this.class.name, thisMethod.name, this].postln;
		// invalidate
		SCUM.removeObjectAt(id);
		id = nil;
		// notify dependants
		this.changed(\destroyed);
		// release resources
		this.freeResources;
		// break links to dependants
		this.release;
	}
	prSetServerProperty { | name value |
		if (this.isBlocked.not) {
			SCUM.sendMsg(*this.setPropertyMsg(name, value));
		}
	}
	prQueryProperty { | key, type |
		// send /o_get <id> key
		// install OSCpathResponder
		// return Promise
		// when message is received, set promise value
		^nil
	}
}

// EOF
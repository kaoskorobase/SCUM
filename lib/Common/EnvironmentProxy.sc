EnvironmentProxy {
	var at, put;
	*new { | at, put |
		^super.newCopyArgs(at, put)
	}
	at { | key |
		^at.value(key)
	}
	put { | key, value |
		^put.value(key, value)
	}
}
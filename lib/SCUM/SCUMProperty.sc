SCUMPropertySpec {
	var <name, <type, default, <decoder;
	classvar defaultDecoder;
	
	*defaultDecoder {
		^defaultDecoder ?? {
			defaultDecoder = { |spec obj stream| spec.defaultDecode(obj, stream) }
		}
	}
	*new { | name type default decoder |
		^super.newCopyArgs(name, type, default, decoder ? this.defaultDecoder)
	}
	default {
		^default.value
	}
	/*
	set { | obj, value |
		if (setter.isNil) {
			obj.putProperty(name, value);
		}{
			setter.value(this, obj, value);
		}
	}
	*/
	decode { | obj stream |
		decoder.value(this, obj, stream);
	}
	defaultDecode { |obj stream|
		obj.tryPerform(name.asSetter, type.scumDecodeOSC(stream))
	}
}

+ Font {
	measure { | string |
		^this.prMeasure(string, Size.new)
	}

	// PRIVATE
	prMeasure { | string, size |
		_Font_Measure
		^this.primitiveFailed
	}
}

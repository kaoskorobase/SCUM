+ Object {
	*scumDecodeOSC { |Êstream |
		^stream.next
	}
	scumEncodeOSC { | stream |
		stream.put(this)
	}
}

+ Array {
	*scumDecodeOSC { | stream |
		var size;
		size = Integer.scumDecodeOSC(stream);
		^this.fill(size, { Object.scumDecodeOSC(stream) })
	}
	scumDecodeOSC { | stream |
		^this.collect { | class | class.scumDecodeOSC(stream) }
	}
	scumEncodeOSC { | stream |
		stream.put(this.size);
		this.do { |elt| elt.scumEncodeOSC(stream) }
	}
}

+ Boolean {
	*scumDecodeOSC { | stream |
		^super.scumDecodeOSC(stream) != 0
	}
}

+ Symbol {
	*scumDecodeOSC { | stream |
		^super.scumDecodeOSC(stream).asSymbol
	}
}

+ String {
	*scumDecodeOSC { | stream |
		^super.scumDecodeOSC(stream).asString
	}
}

+ Char {
	*scumDecodeOSC { | stream |
		^Integer.scumDecodeOSC(stream).clip(0, 255).asAscii
	}
}

+ Integer {
	*scumDecodeOSC { | stream |
		^super.scumDecodeOSC(stream).asInteger
	}
}

+ Float {
	*scumDecodeOSC { | stream |
		^super.scumDecodeOSC(stream).asFloat
	}
}

+ Color {
	*scumDecodeOSC { | stream |
		^this.new(*stream.nextN(4))
	}
	scumEncodeOSC { | stream |
		stream
			.put(this.red)
			.put(this.green)
			.put(this.blue)
			.put(this.alpha)
	}
}

+ Size {
	*scumDecodeOSC { | stream |
		^this.new(stream.next, stream.next)
	}
	scumEncodeOSC { | stream |
		stream.put(this.width).put(this.height)
	}
}

+ Point {
	*scumDecodeOSC { | stream |
		^this.new(stream.next, stream.next)
	}
	scumEncodeOSC { | stream |
		stream.put(this.x).put(this.y)
	}
}

+ Rect {
	*scumDecodeOSC { | stream |
		^this.new(*stream.nextN(4))
	}
	scumEncodeOSC { | stream |
		stream
			.put(this.left)
			.put(this.top)
			.put(this.width)
			.put(this.height)
	}
}

+ Font {
	*scumDecodeOSC { | stream |
		^this.new(String.scumDecodeOSC(stream), Float.scumDecodeOSC(stream))
	}
	scumEncodeOSC { | stream |
		stream.put(this.name).put(this.size)
	}
}
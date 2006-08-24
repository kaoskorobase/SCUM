SCUMHistory
{
	var <array, <size;

	*new { | size |
		^super.new.size_(size)
	}

	size_ { | n |
		size = n;
		array = Array(n);
	}

	isEmpty { ^array.isEmpty }

	push { | object |
		if (array.last != object) {
			if (array.size < size) {
				array.add(object);
			}{
				array = array.rotate(-1);
				array.put(array.size-1, object);
			}
		}
	}

	at { | i | ^array.wrapAt(array.size-1-i) }
	last { ^this.at(0) }

	printOn { | stream |
		super.printOn(stream);
		stream << $(;
		array.printOn(stream);
		stream << $);
	}
}

+ Object {
	asSize { ^Size(this) }
}

+ Size {
	asSize { ^this }
}

+ Point {
	asSize { ^Size(x, y) }
}

+ Rect {
	size { ^Size(width, height) }
	asSize { ^this.size }
}

// EOF
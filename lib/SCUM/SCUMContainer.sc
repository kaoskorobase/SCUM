SCUMContainer : SCUMView {
	var children, named;

	*initClass {
		this.propertyDefaults.putAll((
			xPadding: 0.0,
			yPadding: 0.0
		));
	}
	initObject { | argParent initFunction serverArgs |
		var continuation;
		super.initObject(argParent, initFunction, serverArgs);
		continuation = children;
		children = List.new;
		named = Dictionary.new;
		Environment.new.put(\parent, this).use(continuation);
	}

	size { ^children.size }
	at { | key |
		^key.isNumber.if(children, named).at(key)
	}
	children_ { | continuation |
		if (children.isNil) {
			children = continuation;
		}
	}
	children {
		^children.copy
	}
	do { | function | children.do(function) }
	doMsg { | selector ... args | children.performList(\doMsg, selector, args) }

	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\xPadding, \yPadding];
	}
	xPadding { ^this.getProperty(\xPadding) }
	xPadding_ { |v| this.setProperty(\xPadding, v) }
	yPadding { ^this.getProperty(\yPadding) }
	yPadding_ { |v| this.setProperty(\yPadding, v) }
	padding { ^this.prGetPointProperty(\xPadding, \yPadding) }
	padding_ { |v| this.prSetPointProperty(\xPadding, \yPadding, v.asPoint) }

	// PRIVATE
	prDestroyed {
		var list;
		list = children.copy;
		children.clear;
		list.do { arg child; child.prDestroyed };
		super.prDestroyed;
	}
	prAddChild { | child, name |
		if (children.includes(child).not) {
			children = children.add(child);
		};
		if (name.notNil) { named.put(name, child) }
	}
	prRemoveChild { | child, name |
		children.remove(child);
		if (name.notNil) { named.removeAt(name) }
	}
	prRaiseChild { | child |
		var i;
		i = children.indexOf(child);
		if (i.notNil && { i > 0 }) {
			children.swap(i, i-1);
		}
	}
	prLowerChild { | child |
		var i;
		i = children.indexOf(child);
		if (i.notNil && { i < (children.size-1) }) {
			children.swap(i, i+1);
		}
	}
}

SCUMBin : SCUMContainer {
}

SCUMBox : SCUMContainer {
	// properties
	*propertyKeys {
		^super.propertyKeys ++ #[\homogenous, \spacing];
	}
	homogenous { ^this.getProperty(\homogenous) }
	homogenous_ { |v| this.setProperty(\homogenous, v) }
	spacing { ^this.getProperty(\spacing) }
	spacing_ { |v| this.setProperty(\spacing, v) }
}

SCUMHBox : SCUMBox {
}

SCUMVBox : SCUMBox {
}

SCUMGrid : SCUMContainer {
	// accessing
	at { | key |
		^if (key.isKindOf(Point)) { this.atRowCol(key.y, key.x) }{ super.at(key) }
	}
	atRowCol { | row, col |
		^this.subclassResponsibility(thisMethod)
	}
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\homogenous, \spacing, \wrap]
	}
	homogenous { ^this.getProperty(\homogenous) }
	homogenous_ { |v| this.setProperty(\homogenous, v) }
	xSpacing { ^this.getProperty(\xSpacing) }
	xSpacing_ { |v| this.setProperty(\xSpacing, v) }
	ySpacing { ^this.getProperty(\ySpacing) }
	ySpacing_ { |v| this.setProperty(\ySpacing, v) }
	spacing { ^this.prGetPointProperty(\xSpacing, \ySpacing) }
	spacing_ { |v| this.prSetPointProperty(\xSpacing, \ySpacing, v.asPoint) }
// 	wrap { ^this.getProperty(\wrap) }
// 	wrap_ { |v| this.setProperty(\wrap, v) }
	dimensions { ^this.getProperty(\dimensions, Size.new) }
	dimensions_ { |v| this.setProperty(\dimensions, v.asSize) }
}

SCUMHGrid : SCUMGrid {
	atRowCol { | row, col |
		^children.at(row * this.wrap + col)
	}
}

SCUMVGrid : SCUMGrid {
	atRowCol { | row, col |
		^children.at(col * this.wrap + row)
	}
}

// actually not a container
SCUMScrollView : SCUMView {
	// properties
	*propertyKeys {
		^super.propertyKeys ++ [\hThumb, \vThumb, \thumbSize];
	}
	x { ^this.getProperty(\x) }
	x_ { |v| this.setProperty(\x, v) }
	y { ^this.getProperty(\y) }
	y_ { |v| this.setProperty(\y, v) }
	scroll { ^this.prGetPointProperty(\x, \y) }
	scroll_ { |v| this.prSetPointProperty(\x, \y, v.asPoint) }
	thumbSize { ^this.getProperty(\thumbSize) }
	thumbSize_ { |v| this.setProperty(\thumbSize, v) }
	hThumb { ^this.getProperty(\hThumb) }
	hThumb_ { |v| this.setProperty(\hThumb, v) }
	vThumb { ^this.getProperty(\vThumb) }
	vThumb_ { |v| this.setProperty(\vThumb, v) }
}

// EOF
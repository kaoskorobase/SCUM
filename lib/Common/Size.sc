Size {
	var <>width, <>height;

	*new { | width=0, height=nil |
		^super.new.width_(width.abs).height_((height ? width).abs)
	}

	printOn { arg stream;
		stream << this.class.name << "(" <<* [width, height] << ")";
	}
}

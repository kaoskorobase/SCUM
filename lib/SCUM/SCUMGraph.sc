SCUMScope : SCUMScrollView {
	initDefaults {
		super.initDefaults;
		this.vThumb = false;
	}

	// properties
	xZoom { ^this.getProperty(\xZoom) }
	xZoom_ { |v| this.setProperty(\xZoom, v) }
	yZoom { ^this.getProperty(\yZoom) }
	yZoom_ { |v| this.setProperty(\yZoom, v) }
	zoom { ^this.prGetPointProperty(\xZoom, \yZoom) }
	zoom_ { |v| this.prSetPointProperty(\xZoom, \yZoom, v.asPoint) }
	bufnum { ^this.getProperty(\bufnum) }
	bufnum_ { |v| this.setProperty(\bufnum, v) }
	style { ^this.getProperty(\style) }
	style_ { |v| this.setProperty(\style, v) }	
}

// EOF
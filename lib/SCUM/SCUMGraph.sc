SCUMGLView : SCUMView {
}

SCUMScope : SCUMGLView {
	var sndBuf, shm;
	classvar kSndBufHeaderSize = 16, <>maxBufSize = 1024;
	
	initObject { | argParent initFunction serverArgs |
		sndBuf = SndBuf(0);
		shm = SharedMemory.open(this.makeIPCName, kSndBufHeaderSize + (maxBufSize * 4));
		super.initObject(argParent, initFunction, [shm.name] ++ serverArgs);
		ActionListener(this, \destroyed, {
			CmdPeriod.remove(this);
			shm.free;
			sndBuf.free;
		});
		CmdPeriod.add(this);
		sndBuf.update(false);
		this.cmdPeriod;
	}

	// properties
	xZoom { ^this.getProperty(\xZoom) }
	xZoom_ { |v| this.setProperty(\xZoom, v) }
	yZoom { ^this.getProperty(\yZoom) }
	yZoom_ { |v| this.setProperty(\yZoom, v) }
	zoom { ^this.prGetPointProperty(\xZoom, \yZoom) }
	zoom_ { |v| this.prSetPointProperty(\xZoom, \yZoom, v.asPoint) }
	bufnum { ^sndBuf.bufnum }
	bufnum_ { |v| sndBuf.bufnum_(v).update(false) }
	style { ^this.getProperty(\style) }
	style_ { |v| this.setProperty(\style, v) }

	// cmd period	
	cmdPeriod {
		this.play({ try { this.prUpdate }; 0.05 });
	}

	// PRIVATE
	prUpdate {
		if (sndBuf.update(true)) {
			shm.doLocked {
				shm.doWithStream { | stream |
					stream
						.putDouble(sndBuf.sampleRate)
						.putUInt32(sndBuf.numChannels)
						.putUInt32(sndBuf.numSamples)
						.putAll(sndBuf.data);
				};
			}
		}
	}
}

// EOF
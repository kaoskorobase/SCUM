SCUMScope : SCUMScrollView {
	var sndBuf, shm, updater;
	classvar kSndBufHeaderSize = 16, <>maxBufSize = 1024;
	
	initView {
		var shmName;
		super.initView;
		this.vThumb = false;
		sndBuf = SndBuf(0);
		// TODO: pass this as constructor arg!
		shmName = "/SCUM:" ++ SCUM.addr.port ++ ":Scope:" ++ id;
		shm = SharedMemory.open(shmName, kSndBufHeaderSize + (maxBufSize * 4));
		updater = Routine.run {
			loop {
				try { this.prUpdate } { |e| e.reportError };
				0.1.wait;
			}
		};
		ActionListener(this, \destroyed, {
			updater.stop;
			sndBuf.free;
			shm.free;
		});
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
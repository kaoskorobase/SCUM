SCUM {
	// constants
	classvar <modShift		= 1;
	classvar <modControl	= 2;
	classvar <modCommand	= 4;
	classvar <modKeypad		= 8;

	classvar <keyBackSpace	= 16r0008;
	classvar <keyTab		= 16r0009;
	classvar <keyLF			= 16r000A;
	classvar <keyCR			= 16r000D;
	classvar <keyEscape		= 16r001B;
	classvar <keySpace		= 16r0020;
	classvar <keyDelete		= 16rF728;

	classvar <keyUp			= 16rF700;
	classvar <keyDown		= 16rF701;
	classvar <keyLeft		= 16rF702;
	classvar <keyRight		= 16rF703;
	classvar <keyPageUp		= 16rF72C;
	classvar <keyPageDown	= 16rF72D;
	classvar <keyHome		= 16rF729;
	classvar <keyBegin		= 16rF72A;
	classvar <keyEnd		= 16rF72B;
	
	classvar <keyF1			= 16rF704;
	classvar <keyF2			= 16rF705;
	classvar <keyF3			= 16rF706;
	classvar <keyF4			= 16rF707;
	classvar <keyF5			= 16rF708;
	classvar <keyF6			= 16rF709;
	classvar <keyF7			= 16rF70A;
	classvar <keyF8			= 16rF70B;
	classvar <keyF9			= 16rF70C;
	classvar <keyF10		= 16rF70D;
	classvar <keyF11		= 16rF70E;
	classvar <keyF12		= 16rF70F;
	
	// default modifier (platform dependent)
	classvar <>modDefault;
	
	classvar <>addr, <desktop, <objectTable, objectIDStream, resources;
	classvar <>serverProgram = "scum";
	classvar <>serverAddress;
	
	*initClass {
		objectTable = IdentityDictionary.new;
		objectIDStream = {: x, x <- (0..inf), x != 0 };
		serverAddress = NetAddr("127.0.0.1", 57130);
	}

	// messages (might go to window)	
	*message { |obj|
		("SCUM: " ++ obj.asString).postln
	}
	message { | obj |
		this.class.message(obj);
	}
	// connection
	*isConnected {
		^addr.notNil and: { addr.isConnected }
	}
	/*
	*connectRendezvous { | completionFunc |
		var booted = false;
		if (addr.isNil or: {Êaddr.isConnected.not }) {
			Routine.run {
				10.do { |i|
					#addr = (defaultServerAddress ?? { OSCService.knownAddresses("SCUM", \tcp) }).asArray;
					if (addr.isNil) {
						if (booted.not) {
							this.message("booting server");
							booted = true;
							this.prBoot;
						}
					}{
						try {
							addr.connect { this.prOnDisconnect(addr) };
							try {
								this.prOnConnect;
								completionFunc.value;
							}{ |e|
								this.message(e.errorString);
							};
							nil.alwaysYield;
						}{
							addr = nil;
						}
					};
					0.5.wait
				};
				this.message("couldn't connect to server");
			}
		}
	}
	*/
	*connect { | completionFunc |
		var theAddr;
		if (this.isConnected.not) {
			theAddr = serverAddress;
			Routine.run {
				this.message("booting server");
				this.prBoot(theAddr);
				0.5.wait;
				10.do { |i|
					try {
						theAddr.connect {
							addr = nil;
							this.prOnDisconnect(theAddr)
						};
						addr = theAddr;
						try {
							this.prOnConnect(theAddr);
							completionFunc.value;
						}{ |e|
							this.message(e.errorString);
						};
						nil.alwaysYield;
					};
					0.5.wait
				};
				this.message("couldn't connect to server");
			}
		}
	}
	*disconnect {
		if (addr.notNil) {
			addr.disconnect;
		}
	}
	*kill {
		"killall -HUP scum".systemCmd
	}
	*do { | function |
		if (this.isConnected, function, { this.connect(function) })
	}
	// OSC messages + bundles
	*sendMsg { | ... msg |
		addr.sendMsg(*msg)
	}
	*sendBundle { | time ... args |
		addr.sendBundle(time, *args)
	}
	*makeBundle { | time, function, bundle |
		addr = BundleNetAddr.copyFrom(addr, bundle);
		protect {
			function.value;
			bundle = addr.bundle;
		}{
			addr = addr.saveAddr;
		};
		if (time != false) {
			this.sendBundle(time, *bundle)
		};
		^bundle
	}
	*dumpOSC { |flag(true)|
		this.sendMsg("/dumpOSC", flag)
	}
	// object ids
	*nextObjectID {
		^objectIDStream.next.asInteger
	}
	*putObject { | id, obj |
		objectTable.put(id, obj)
	}
	*removeObjectAt { | id |
		objectTable.removeAt(id)
	}
	*objectAt { | id, failFunction |
		^objectTable.atFail(id, failFunction)
	}
	*objectDo { | id, function ... args |
		^function.value(this.objectAt(id, { ^nil }), *args)
	}
	*objectPerform { | id, selector ... args |
		^this.objectAt(id, { ^nil }).performList(selector, args)
	}
	*registerObjectMethod { | path, function |
		^OSCresponder(addr, path, { | time, resp, msg |
			var path, id, args;
			#path, id ... args = msg;
			try {
				this.objectDo(id, function, *args);
			}{ | e |
				this.message("Error in object method: " ++ e.errorString);
			};
		}).add
	}
	// PRIVATE
	*prBoot { | addr |
		var args = "";
		if (serverProgram.notNil) {
			if (addr.notNil) {
				args = addr.ip + addr.port;
			};
			"% % &".format(serverProgram, args).systemCmd;
		}
	}
	*prOnConnect { | addr |
		this.message("connected");
		desktop = SCUMDesktop.new;
		OSCresponder(addr, "/message", { | time, resp, msg |
			this.message(msg[1])
		}).add;
		OSCresponder(addr, "/error", { | time, resp, msg |
			var who, where, str;
			#who, where, str = msg;
			this.message("ERROR in %%::%%: %%".format(who, where, str))
		}).add;
		this.changed(\connected);
	}
	*prOnDisconnect { | addr |
		this.message("disconnected");
		if (desktop.notNil) {
			desktop.prDestroyed;
			desktop = nil;
		};
		OSCresponder.removeAddr(addr);
		this.changed(\disconnected);
	}
}

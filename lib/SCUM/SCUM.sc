SCUM {
	// constants
	classvar <modShift, <modControl, <modCommand, <modKeypad;
	classvar <>modDefault;
	
	classvar <keyTab, <keyDelete, <keyBackSpace, <keySpace;
	classvar <keyLF, <keyCR, <keyEscape;
	classvar <keyUp, <keyDown, <keyLeft, <keyRight, <keyPageUp, <keyPageDown;
	classvar <keyHome, <keyBegin, <keyEnd;
	classvar <keyF1, <keyF2, <keyF3, <keyF4, <keyF5, <keyF6;
	classvar <keyF7, <keyF8, <keyF9, <keyF10, <keyF11, <keyF12;

	classvar <borderNone, <borderFlat, <borderRaised, <borderSunken;

	classvar <addr, <desktop, <objectTable, objectIDStream, resources;
	classvar <>defaultServerAddress;
	classvar <>serverProgramPath = "/usr/local/bin/scum";
	
	*initClass {
		modShift			= (1 << 0);
		modControl		= (1 << 1);
		modCommand		= (1 << 2);
		modKeypad			= (1 << 3);

		keyBackSpace		= 16r0008;
		keyTab			= 16r0009;
		keyLF			= 16r000A;
		keyCR			= 16r000D;
		keyEscape			= 16r001B;
		keySpace			= 16r0020;
// 		keyDelete			= 16r007F;
		keyDelete			= 16rF728;

		keyUp			= 16rF700;
		keyDown			= 16rF701;
		keyLeft			= 16rF702;
		keyRight			= 16rF703;
		keyPageUp			= 16rF72C;
		keyPageDown		= 16rF72D;
		keyHome			= 16rF729;
		keyBegin			= 16rF72A;
		keyEnd			= 16rF72B;

		keyF1			= 16rF704;
		keyF2			= 16rF705;
		keyF3			= 16rF706;
		keyF4			= 16rF707;
		keyF5			= 16rF708;
		keyF6			= 16rF709;
		keyF7			= 16rF70A;
		keyF8			= 16rF70B;
		keyF9			= 16rF70C;
		keyF10			= 16rF70D;
		keyF11			= 16rF70E;
		keyF12			= 16rF70F;
		
		objectTable = IdentityDictionary.new;
		objectIDStream = {: x, x <- (0..inf), x != 0 };
	}

	// messages (might go to window)	
	*message { |obj|
		("SCUM: " ++ obj.asString).postln
	}
	message { | obj |
		this.class.message(obj);
	}
	// connection
	*connect { | completionFunc |
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
	*disconnect {
		if (addr.notNil) {
			addr.disconnect;
		}
	}
	*kill {
		"killall -HUP scum".systemCmd
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
	*prBoot {
		if (serverProgramPath.notNil) {
			(serverProgramPath ++ "&").systemCmd;
		}
	}
	*prOnConnect {
		this.message("connected");
		desktop = SCUMDesktop.new;
		OSCresponder(addr, "/message", { | time, resp, msg |
			this.message(msg[1])
		}).add;
		OSCresponder(addr, "/error", { | time, resp, msg |
			this.message("ERROR in " ++ msg[1].asString ++ ": " ++ msg[2].asString)
		}).add;
		this.changed(\connected);
	}
	*prOnDisconnect {
		this.message("disconnected");
		if (desktop.notNil) {
			desktop.prDestroyed;
			desktop = nil;
		};
		OSCresponder.removeAddr(addr);
		this.changed(\disconnected);
	}
}

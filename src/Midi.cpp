#include "Midi.h"
#include <sstream>
void* MidiIn::midiIn = NULL;

using namespace std;
static void	MyReadProc(const MIDIPacketList *pktlist, void *refCon, void *connRefCon) {
	
	MIDIPacket *packet = (MIDIPacket *)pktlist->packet;
	for (unsigned int j = 0; j < pktlist->numPackets; ++j) {
		//printf("Message type: %d %x\n", j, packet->data[0] & 0xF0);
		int type = packet->data[0] & 0xF0;
		//printf("Strue? %d\n",type);
		if(packet->length==3 && (type == 144 || type == 128)) { // note on

			((MidiIn*)MidiIn::midiIn)->midiReceived(packet->data[1], type==144?packet->data[2]:0);
			//printf("Note: %d\n", packet->data[1]);
			
			
		}
		packet = MIDIPacketNext(packet);
	}
	
	/*MIDIPacket *packet = (MIDIPacket *)pktlist->packet;	// remove const (!)
	for (unsigned int j = 0; j < pktlist->numPackets; ++j) {
		for (int i = 0; i < packet->length; ++i) {
							printf("%02X ", packet->data[i]);
			
			// rechannelize status bytes
			//if (packet->data[i] >= 0x80 && packet->data[i] < 0xF0)
			//	packet->data[i] = (packet->data[i] & 0xF0) | gChannel;
		}
		
		//			printf("\n");
		packet = MIDIPacketNext(packet);
	}*/
	MIDIReceived((MIDIEndpointRef)refCon, pktlist);
	//printf("Midi received\n");
}



// This function was submitted by Douglas Casey Tucker and apparently
// derived largely from PortMidi.
CFStringRef EndpointName( MIDIEndpointRef endpoint, bool isExternal )
{
	CFMutableStringRef result = CFStringCreateMutable( NULL, 0 );
	CFStringRef str;
	
	// Begin with the endpoint's name.
	str = NULL;
	MIDIObjectGetStringProperty( endpoint, kMIDIPropertyName, &str );
	if ( str != NULL ) {
		CFStringAppend( result, str );
		CFRelease( str );
	}
	
	MIDIEntityRef entity = NULL;
	MIDIEndpointGetEntity( endpoint, &entity );
	if ( entity == NULL )
		// probably virtual
		return result;
	
	if ( CFStringGetLength( result ) == 0 ) {
		// endpoint name has zero length -- try the entity
		str = NULL;
		MIDIObjectGetStringProperty( entity, kMIDIPropertyName, &str );
		if ( str != NULL ) {
			CFStringAppend( result, str );
			CFRelease( str );
		}
	}
	// now consider the device's name
	MIDIDeviceRef device = NULL;
	MIDIEntityGetDevice( entity, &device );
	if ( device == NULL )
		return result;
	
	str = NULL;
	MIDIObjectGetStringProperty( device, kMIDIPropertyName, &str );
	if ( CFStringGetLength( result ) == 0 ) {
		CFRelease( result );
		return str;
	}
	if ( str != NULL ) {
		// if an external device has only one entity, throw away
		// the endpoint name and just use the device name
		if ( isExternal && MIDIDeviceGetNumberOfEntities( device ) < 2 ) {
			CFRelease( result );
			return str;
		} else {
			if ( CFStringGetLength( str ) == 0 ) {
				CFRelease( str );
				return result;
			}
			// does the entity name already start with the device name?
			// (some drivers do this though they shouldn't)
			// if so, do not prepend
			if ( CFStringCompareWithOptions( result, /* endpoint name */
											str /* device name */,
											CFRangeMake(0, CFStringGetLength( str ) ), 0 ) != kCFCompareEqualTo ) {
				// prepend the device name to the entity name
				if ( CFStringGetLength( result ) > 0 )
					CFStringInsert( result, 0, CFSTR(" ") );
				CFStringInsert( result, 0, str );
			}
			CFRelease( str );
		}
	}
	return result;
}


// This function was submitted by Douglas Casey Tucker and apparently
// derived largely from PortMidi.
static CFStringRef ConnectedEndpointName( MIDIEndpointRef endpoint )
{
	CFMutableStringRef result = CFStringCreateMutable( NULL, 0 );
	CFStringRef str;
	OSStatus err;
	int i;
	
	// Does the endpoint have connections?
	CFDataRef connections = NULL;
	int nConnected = 0;
	bool anyStrings = false;
	err = MIDIObjectGetDataProperty( endpoint, kMIDIPropertyConnectionUniqueID, &connections );
	if ( connections != NULL ) {
		// It has connections, follow them
		// Concatenate the names of all connected devices
		nConnected = CFDataGetLength( connections ) / sizeof(MIDIUniqueID);
		if ( nConnected ) {
			const SInt32 *pid = (const SInt32 *)(CFDataGetBytePtr(connections));
			for ( i=0; i<nConnected; ++i, ++pid ) {
				MIDIUniqueID id = EndianS32_BtoN( *pid );
				MIDIObjectRef connObject;
				MIDIObjectType connObjectType;
				err = MIDIObjectFindByUniqueID( id, &connObject, &connObjectType );
				if ( err == noErr ) {
					if ( connObjectType == kMIDIObjectType_ExternalSource  ||
						connObjectType == kMIDIObjectType_ExternalDestination ) {
						// Connected to an external device's endpoint (10.3 and later).
						str = EndpointName( (MIDIEndpointRef)(connObject), true );
					} else {
						// Connected to an external device (10.2) (or something else, catch-
						str = NULL;
						MIDIObjectGetStringProperty( connObject, kMIDIPropertyName, &str );
					}
					if ( str != NULL ) {
						if ( anyStrings )
							CFStringAppend( result, CFSTR(", ") );
						else anyStrings = true;
						CFStringAppend( result, str );
						CFRelease( str );
					}
				}
			}
		}
		CFRelease( connections );
	}
	if ( anyStrings )
		return result;
	
	// Here, either the endpoint had no connections, or we failed to obtain names 
	return EndpointName( endpoint, false );
}

std::string getPortName( unsigned int portNumber )
{
	CFStringRef nameRef;
	MIDIEndpointRef portRef;
	std::ostringstream ost;
	char name[128];
	
	if ( portNumber >= MIDIGetNumberOfSources() ) {
		ost << "RtMidiIn::getPortName: the 'portNumber' argument (" << portNumber << ") is invalid.";
		//errorString_ = ost.str();
		//error( RtError::INVALID_PARAMETER );
	}
	portRef = MIDIGetSource( portNumber );
	
	nameRef = ConnectedEndpointName(portRef);
	//MIDIObjectGetStringProperty( portRef, kMIDIPropertyName, &nameRef );
	// modified by D. Casey Tucker 2009-03-10
	
	CFStringGetCString( nameRef, name, sizeof(name), 0);
	CFRelease( nameRef );
	std::string stringName = name;
	return stringName;
}




MidiIn::MidiIn() {

	midiIn = this;

	

	MIDIClientCreate(CFSTR("MIDI Echo"), NULL, NULL, &client);
	MIDIInputPortCreate(client, CFSTR("Input port"), MyReadProc, NULL, &inPort);
	// open connections from all sources
	int n = MIDIGetNumberOfSources();
	printf("%d sources\n", n);
	for (int i = 0; i < n; ++i) {
		//cout << getPortName(i) << endl;
		MIDIEndpointRef src = MIDIGetSource(i);
		MIDIPortConnectSource(inPort, src, NULL);
		return;
	}
}

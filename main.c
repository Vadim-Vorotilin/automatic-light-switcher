#define MAX_ZONE_DEPENDENCIES_COUNT 5
#define NO_DEPENDENCY -1
#define RELAY_ON LOW
#define RELAY_OFF HIGH

const int ZonesCount = 1;	// 2;

struct Zone {
	int moveSensorPinIn;
	int lumonositySensorPinIn;
	int relayPinOut;
	int dependencies[MAX_ZONE_DEPENDENCIES_COUNT];
	
	bool isOn;
	bool isUpdated;
	
	Zone(int moveSensorPinIn,
		 int lumonositySensorPinIn,
		 int relayPinOut,
		 int dependency0 = NO_DEPENDENCY,
		 int dependency1 = NO_DEPENDENCY,
		 int dependency2 = NO_DEPENDENCY,
		 int dependency3 = NO_DEPENDENCY,
		 int dependency4 = NO_DEPENDENCY)
		: moveSensorPinIn(moveSensorPinIn),
		lumonositySensorPinIn(lumonositySensorPinIn),
		relayPinOut(relayPinOut),
		isOn(false),
		isUpdated(false) {
			dependencies[0] = dependency0;
			dependencies[1] = dependency1;
			dependencies[2] = dependency2;
			dependencies[3] = dependency3;
			dependencies[4] = dependency4;
		}
};

Zone zones[ZonesCount] = { 
	Zone(2, 3, 10),		//Zone(2, 3, 10, 1)	// 0. Doors
	//Zone(4, 5, 11, dependencies1)			// 1. Corridor
};

void setup() {
  Serial.begin(9600);
  
  for (int i = 0; i != ZonesCount; i++) {
	  pinMode(zones[i].moveSensorPinIn, INPUT);
	  pinMode(zones[i].lumonositySensorPinIn, INPUT);
	
	  pinMode(zones[i].relayPinOut, OUTPUT);
	  digitalWrite(zones[i].relayPinOut, RELAY_OFF);
  }
}

void loop() {
	Serial.println(String(ZonesCount));

	ResetZones();

	for (int i = 0; i != ZonesCount; i++) {
		if (!zones[i].isUpdated)
			UpdateZone(i);
	}

	for (int i = 0; i != ZonesCount; i++) {
		digitalWrite(zones[i].relayPinOut, zones[i].isOn ? RELAY_ON : RELAY_OFF);
	}
	
	delay(1);
}

void ResetZones() {
	for (int i = 0; i != ZonesCount; i++) {
		zones[i].isUpdated = false;
	}
}

void UpdateZone(int zone) {
	Serial.println("Updating zone: " + String(zone));
	
	for (int i = 0; i != MAX_ZONE_DEPENDENCIES_COUNT; i++) {
		if (zones[zone].dependencies[i] == NO_DEPENDENCY)
			break;
		
		if (!zones[zones[zone].dependencies[i]].isUpdated) {
			Serial.println("Need to update zone: " + String(zones[zone].dependencies[i]));
			UpdateZone(zones[zone].dependencies[i]);
		}
	}

	int movement = digitalRead(zones[zone].moveSensorPinIn);
	int luminosity = digitalRead(zones[zone].lumonositySensorPinIn);
	
	TableData("Movement " + String(zone), movement, "Zones");
	TableData("Luminosity " + String(zone), luminosity, "Zones");
	
	zones[zone].isOn = movement == HIGH && (luminosity == HIGH || zones[zone].isOn);
	
	for (int i = 0; i != MAX_ZONE_DEPENDENCIES_COUNT; i++) {
		if (zones[zone].dependencies[i] == NO_DEPENDENCY)
			break;

		zones[zone].isOn |= zones[zones[zone].dependencies[i]].isOn;
	}
	
	zones[zone].isUpdated = true;
	
	TableData("Zone " + String(zone), zones[zone].isOn, "Zones");
}

void TimePlot(float data, String seriesName, String channelName) {
  Serial.print("{TIMEPLOT:");
  Serial.print(channelName);
  Serial.print("|data|");
  Serial.print(seriesName);
  Serial.print("|T|");
  Serial.print(data, 3);
  Serial.println("}");
}

void TableData(String Name, float Value, String Description) {
  Serial.print("{TABLE");
  Serial.print("|SET|");
  Serial.print(Name);
  Serial.print("|");
  Serial.print(Value);
  Serial.print("|");
  Serial.print(Description);
  Serial.println("}");
}
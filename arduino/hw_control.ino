// Include the libraries we need
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
#define MAX_DEVICE_COUNT 20

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

// arrays to hold device addresses
DeviceAddress thermometers[MAX_DEVICE_COUNT];
int deviceCount;

void setup(void)
{
  // start serial port
  Serial.begin(9600);
  // Serial.println("Dallas Temperature IC Control Library Demo");

  // Start up the library
  sensors.begin();

  // locate devices on the bus  
  deviceCount = sensors.getDeviceCount();
  Serial.print("START Locating devices... ");
  Serial.print("Found ");
  Serial.print(deviceCount, DEC);
  Serial.println(" devices.");
  
  // report parasite power requirements
  Serial.print("START Parasite power is: ");
  if (sensors.isParasitePowerMode()) Serial.println("ON");
  else Serial.println("OFF");

  // Search for devices on the bus and assign based on an index. Ideally,
  // you would do this to initially discover addresses on the bus and then
  // use those addresses and manually assign them (see above) once you know
  // the devices on your bus (and assuming they don't change).
  //
  // method 1: by index


  // method 2: search()
  // search() looks for the next device. Returns 1 if a new address has been
  // returned. A zero might mean that the bus is shorted, there are no devices,
  // or you have already retrieved all of them. It might be a good idea to
  // check the CRC to make sure you didn't get garbage. The order is
  // deterministic. You will always get the same devices in the same order
  //
  // Must be called before search()
  //oneWire.reset_search();
  // assigns the first address found to insideThermometer
  //if (!oneWire.search(insideThermometer)) Serial.println("Unable to find address for insideThermometer");
  // assigns the seconds address found to outsideThermometer
  //if (!oneWire.search(outsideThermometer)) Serial.println("Unable to find address for outsideThermometer");

  deviceCount = 0;
  int i;
  for(i = 0; i < MAX_DEVICE_COUNT; i++)
  {
    if (!oneWire.search(thermometers[i]))
    {
      break;
    }
    Serial.print("START Device ");
    printAddress(thermometers[i]);
    Serial.println(" found");
    deviceCount++;
  }

  // show the addresses we found on the bus
  // Serial.print("Device 0 Address: ");
  
  // Serial.println();

  // Serial.print("Device 1 Address: ");
  // printAddress(outsideThermometer);
  // Serial.println();

  // set the resolution to 9 bit per device
  // sensors.setResolution(insideThermometer, TEMPERATURE_PRECISION);
  // sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);

  for (int i = 0; i < deviceCount; i++)
  {
    sensors.setResolution(thermometers[i], TEMPERATURE_PRECISION);
  }

  //Serial.print("Device 0 Resolution: ");
  //Serial.print(sensors.getResolution(insideThermometer), DEC);
  //Serial.println();

  //Serial.print("Device 1 Resolution: ");
  //Serial.print(sensors.getResolution(outsideThermometer), DEC);
  //Serial.println();

  for (int i = 0; i < deviceCount; i++)
  {
    Serial.print("START Device ");
    printAddress(thermometers[i]);    
    Serial.print(" ");
    printResolution(thermometers[i]);
  }
}

// function to print a device address
void printAddress(DeviceAddress deviceAddress)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    // zero pad the address if necessary
    if (deviceAddress[i] < 16) Serial.print("0");
    Serial.print(deviceAddress[i], HEX);
  }
}

// function to print the temperature for a device
void printTemperature(DeviceAddress deviceAddress)
{
  sensors.requestTemperatures();
  float tempC = sensors.getTempC(deviceAddress);
  if(tempC == DEVICE_DISCONNECTED_C) 
  {
    Serial.print("ERROR");
    return;
  }
  // Serial.print("Temp C: ");
  Serial.print(tempC);
  // Serial.print(" Temp F: ");
  // Serial.print(DallasTemperature::toFahrenheit(tempC));
}

// function to print a device's resolution
void printResolution(DeviceAddress deviceAddress)
{
  Serial.print("Resolution: ");
  Serial.print(sensors.getResolution(deviceAddress));
  Serial.println();
}

// main function to print information about a device
void printData(DeviceAddress deviceAddress)
{
  Serial.print("Device Address: ");
  printAddress(deviceAddress);
  Serial.print(" ");
  printTemperature(deviceAddress);
  Serial.println();
}

void report_sensors()
{
  for (int i = 0; i < deviceCount; i++)
  {
    Serial.print("TEMP ");
    printAddress(thermometers[i]);    
    Serial.print(" ");
    printTemperature(thermometers[i]);
    Serial.println();
  }  
  Serial.println("END");
}

void set_output(int port, int value)
{
  Serial.println(port);
  Serial.println(value);
  Serial.println("OK");
}

/*
   Main function, calls the temperatures in a loop.
*/
void loop(void)
{
  Serial.println("READY");
  while (!Serial.available());
  String command = Serial.readString();
  command.trim();
  if (command == "READ_SENSORS")
  {
    report_sensors();
  }
  else if (command.startsWith("SET "))
  {
    String args = command.substring(4);
    Serial.println(args);
    int spacePos = args.indexOf(" ");
    int port = args.substring(0, spacePos).toInt();
    int value = args.substring(spacePos).toInt();
    set_output(port, value);
  }
}

/*
 * NodeManager
 */
 
#ifndef NodeManager_h
#define NodeManager_h

#include <Arduino.h>

/***********************************
   Sensors types
*/
// Generic analog sensor, return a pin's analog value or its percentage
#define SENSOR_ANALOG_INPUT 0
// LDR sensor, return the light level of an attached light resistor in percentage
#define SENSOR_LDR 1
// Thermistor sensor, return the temperature based on the attached thermistor
#define SENSOR_THERMISTOR 2
// Generic digital sensor, return a pin's digital value
#define SENSOR_DIGITAL_INPUT 3
// Generic digital output sensor, allows setting the digital output of a pin to the requested value
#define SENSOR_DIGITAL_OUTPUT 4
// Relay sensor, allows activating the relay
#define SENSOR_RELAY 5
// Latching Relay sensor, allows activating the relay with a pulse
#define SENSOR_LATCHING_RELAY 6
// DHT11/DHT22 sensors, return temperature/humidity based on the attached DHT sensor
#define SENSOR_DHT11 7
#define SENSOR_DHT22 8
// SHT21 sensor, return temperature/humidity based on the attached SHT21 sensor
#define SENSOR_SHT21 9
// Generic switch, wake up the board when a pin changes status
#define SENSOR_SWITCH 10
// Door sensor, wake up the board and report when an attached magnetic sensor has been opened/closed
#define SENSOR_DOOR 11
// Motion sensor, wake up the board and report when an attached PIR has triggered
#define SENSOR_MOTION 12
// DS18B20 sensor, return the temperature based on the attached sensor
#define SENSOR_DS18B20 13

/***********************************
   Constants
*/

// define sleep mode
#define IDLE 0
#define SLEEP 1
#define WAIT 2

// define time unit
#define SECONDS 0
#define MINUTES 1
#define HOURS 2
#define DAYS 3

// define value type
#define TYPE_INTEGER 0
#define TYPE_FLOAT 1
#define TYPE_STRING 2

// define interrupt pins
#define INTERRUPT_PIN_1 3
#define INTERRUPT_PIN_2 2

// define eeprom addresses
#define EEPROM_LAST_ID 4
#define EEPROM_SLEEP_SAVED 0
#define EEPROM_SLEEP_MODE 1
#define EEPROM_SLEEP_TIME_MAJOR 2
#define EEPROM_SLEEP_TIME_MINOR 3
#define EEPROM_SLEEP_UNIT 4

// define NodeManager version
#define VERSION 1.0

/***********************************
   Configuration settings
*/
// default configuration settings

// if enabled, will load the sleep manager library. Sleep mode and sleep interval have to be configured to make the board sleeping/waiting
#define SLEEP_MANAGER 1
// if enabled, enable the capability to power on sensors with the arduino's pins to save battery while sleeping
#define POWER_MANAGER 1
// if enabled, will load the battery manager library to allow the battery level to be reported automatically or on demand
#define BATTERY_MANAGER 1
// if enabled, allow modifying the configuration remotely by interacting with the configuration child id
#define REMOTE_CONFIGURATION 1
// if enabled, persist the configuration settings on EEPROM
#define PERSIST 0

// if enabled, enable debug messages on serial port
#define DEBUG 1

// if enabled, send a SLEEPING and AWAKE service messages just before entering and just after leaving a sleep cycle
#define SERVICE_MESSAGES 1
// if enabled, a battery sensor will be created at BATTERY_CHILD_ID and will report vcc voltage together with the battery level percentage
#define BATTERY_SENSOR 1

// the child id used to allow remote configuration
#define CONFIGURATION_CHILD_ID 200
// the child id used to report the battery voltage to the controller
#define BATTERY_CHILD_ID 201

// Enable this module to use one of the following sensors: SENSOR_ANALOG_INPUT, SENSOR_LDR, SENSOR_THERMISTOR
#define MODULE_ANALOG_INPUT 1
// Enable this module to use one of the following sensors: SENSOR_DIGITAL_INPUT
#define MODULE_DIGITAL_INPUT 1
// Enable this module to use one of the following sensors: SENSOR_DIGITAL_OUTPUT, SENSOR_RELAY, SENSOR_LATCHING_RELAY
#define MODULE_DIGITAL_OUTPUT 1
// Enable this module to use one of the following sensors: SENSOR_SHT21
#define MODULE_SHT21 0
// Enable this module to use one of the following sensors: SENSOR_DHT11, SENSOR_DHT22
#define MODULE_DHT 0
// Enable this module to use one of the following sensors: SENSOR_SWITCH, SENSOR_DOOR, SENSOR_MOTION
#define MODULE_SWITCH 0
// Enable this module to use one of the following sensors: SENSOR_DS18B20
#define MODULE_DS18B20 0

// include user defined configuration difrectives
#include "config.h"

/***********************************
  Libraries
*/

// include MySensors libraries
#include <core/MySensorsCore.h>
#include <core/MyHwAVR.h>

// include third party libraries
#if MODULE_DHT == 1
  #include <DHT.h>
#endif
#if MODULE_SHT21 == 1
  #include <Wire.h>
  #include <Sodaq_SHT2x.h>
#endif
#if MODULE_SHT21 == 1
  #include <Wire.h>
  #include <Sodaq_SHT2x.h>
#endif
#if MODULE_DS18B20 == 1
  #include <OneWire.h>
  #include <DallasTemperature.h>
#endif


/**************************************
   Classes
*/

/*
   PowerManager
*/

class PowerManager {
  public:
    PowerManager() {};
    // to save battery the sensor can be optionally connected to two pins which will act as vcc and ground and activated on demand
    void setPowerPins(int ground_pin, int vcc_pin, long wait = 0);
    void powerOn();
    void powerOff();
  private:
    int _vcc_pin = -1;
    int _ground_pin = -1;
    long _wait = 0;
    bool _hasPowerManager();
};


/***************************************
   Sensor: generic sensor class
*/
class Sensor {
  public:
    Sensor(int child_id, int pin);
    // where the sensor is attached to (default: not set)
    void setPin(int value);
    int getPin();
    // child_id of this sensor (default: not set)
    void setChildId(int value);
    int getChildId();
    // presentation of this sensor (default: S_CUSTOM)
    void setPresentation(int value);
    int getPresentation();
    // type of this sensor (default: V_CUSTOM)
    void setType(int value);
    int getType();
    // when queried, send the message multiple times (default: 1)
    void setRetries(int value);
    // For some sensors, the measurement can be queried multiple times and an average is returned (default: 1)
    void setSamples(int value);
    // If more then one sample has to be taken, set the interval in milliseconds between measurements (default: 0)
    void setSamplesInterval(int value);
    // if true will report the measure only if different then the previous one (default: false)
    void setTackLastValue(bool value);
    // if track last value is enabled, force to send an update after the configured number of cycles (default: -1)
    void setForceUpdate(int value);
    // the value type of this sensor (default: TYPE_INTEGER)
    void setValueType(int value);
    // for float values, set the float precision (default: 2)
    void setFloatPrecision(int value);
    #if POWER_MANAGER == 1
      // to save battery the sensor can be optionally connected to two pins which will act as vcc and ground and activated on demand
      void setPowerPins(int ground_pin, int vcc_pin, long wait = 0);
      void powerOn();
      void powerOff();
    #endif
    // define what to do at each stage of the sketch
    virtual void before();
    virtual void presentation();
    virtual void loop(const MyMessage & message);
    virtual void receive(const MyMessage & message);
    // abstract functions, subclasses need to implement
    virtual void onBefore() = 0;
    virtual void onLoop() = 0;
    virtual void onReceive(const MyMessage & message) = 0;
  protected:
    MyMessage _msg;
    int _pin = -1;
    int _child_id;
    int _presentation = S_CUSTOM;
    int _type = V_CUSTOM;
    int _retries = 1;
    int _samples = 1;
    int _samples_interval = 0;
    bool _track_last_value = false;
    int _cycles = 0;
    int _force_update = -1;
    void _send(MyMessage & msg);
    #if POWER_MANAGER  == 1
      PowerManager _powerManager;
    #endif
    int _value_type = TYPE_INTEGER;
    int _float_precision = 2;
    int _value_int = -1;
    float _value_float = -1;
    char * _value_string = "";
    int _last_value_int = -1;
    float _last_value_float = -1;
    char * _last_value_string = "";
};

/*
   SensorAnalogInput: read the analog input of a configured pin
*/
class SensorAnalogInput: public Sensor {
  public:
    SensorAnalogInput(int child_id, int pin);
    // the analog reference to use (default: not set, can be either INTERNAL or DEFAULT)
    void setReference(int value);
    // reverse the value or the percentage (e.g. 70% -> 30%) (default: false)
    void setReverse(bool value);
    // when true returns the value as a percentage (default: true)
    void setOutputPercentage(bool value);
    // minimum value for calculating the percentage (default: 0)
    void setRangeMin(int value);
    // maximum value for calculating the percentage (default: 1024)
    void setRangeMax(int value);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    int _reference = -1;
    bool _reverse = false;
    bool _output_percentage = true;
    int _range_min = 0;
    int _range_max = 1024;
    int _getPercentage(int value);
    int _getAnalogRead();
};

/*
   SensorLDR: return the percentage of light from a Light dependent resistor
*/
class SensorLDR: public SensorAnalogInput {
  public:
    SensorLDR(int child_id, int pin);
};

/*
   SensorThermistor: read the temperature from a thermistor
*/
class SensorThermistor: public Sensor {
  public:
    SensorThermistor(int child_id, int pin);
    // resistance at 25 degrees C (default: 10000)
    void setNominalResistor(int value);
    // temperature for nominal resistance (default: 25)
    void setNominalTemperature(int value);
    // The beta coefficient of the thermistor (default: 3950)
    void setBCoefficient(int value);
    // the value of the resistor in series with the thermistor (default: 10000)
    void setSeriesResistor(int value);
    // set a temperature offset
    void setOffset(float value);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    int _nominal_resistor = 10000;
    int _nominal_temperature = 25;
    int _b_coefficient = 3950;
    int _series_resistor = 10000;
    float _offset = 0;
};

/*
   SensorDigitalInput: read the digital input of the configured pin
*/
class SensorDigitalInput: public Sensor {
  public:
    SensorDigitalInput(int child_id, int pin);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
};

/*
   SensorDigitalOutput: control a digital output of the configured pin
*/
class SensorDigitalOutput: public Sensor {
  public:
    SensorDigitalOutput(int child_id, int pin);
    // set how to initialize the output (default: LOW)
    void setInitialValue(int value);
    // if greater than 0, send a pulse of the given duration in ms and then restore the output back to the original value (default: 0)
    void setPulseWidth(int value);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    int _initial_value = LOW;
    int _pulse_width = 0;
};


/*
   SensorRelay
*/
class SensorRelay: public SensorDigitalOutput {
  public:
    SensorRelay(int child_id, int pin);
};

/*
   SensorLatchingRelay
*/
class SensorLatchingRelay: public SensorRelay {
  public:
    SensorLatchingRelay(int child_id, int pin);
};

/*
   SensorDHT
*/
#if MODULE_DHT == 1
class SensorDHT: public Sensor {
  public:
    SensorDHT(int child_id, int pin, DHT* dht, int sensor_type, int dht_type);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    DHT* _dht;
    int _dht_type = DHT11;
    float _offset = 0;
    int _sensor_type = 0;
};
#endif

/*
   SensorSHT21
*/
#if MODULE_SHT21 == 1
class SensorSHT21: public Sensor {
  public:
    SensorSHT21(int child_id, int sensor_type);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    float _offset = 0;
    int _sensor_type = 0;
};
#endif

/*
 * SensorSwitch
 */
class SensorSwitch: public Sensor {
  public:
    SensorSwitch(int child_id, int pin);
    // set the interrupt mode. Can be CHANGE, RISING, FALLING (default: CHANGE)
    void setMode(int value);
    int getMode();
    // milliseconds to wait before reading the input (default: 0)
    void setDebounce(int value);
    // time to wait in milliseconds after a change is detected to allow the signal to be restored to its normal value (default: 0)
    void setTriggerTime(int value);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    int _debounce = 0;
    int _trigger_time = 0;
    int _mode = CHANGE;
};

/*
 * SensorDoor
 */
class SensorDoor: public SensorSwitch {
  public:
    SensorDoor(int child_id, int pin);
};

/*
 * SensorMotion
 */
class SensorMotion: public SensorSwitch {
  public:
    SensorMotion(int child_id, int pin);
};

/*
   SensorDs18b20
*/
#if MODULE_DS18B20 == 1
class SensorDs18b20: public Sensor {
  public:
    SensorDs18b20(int child_id, int pin, DallasTemperature* sensors, int index);
    // define what to do at each stage of the sketch
    void onBefore();
    void onLoop();
    void onReceive(const MyMessage & message);
  protected:
    float _offset = 0;
    int _index;
    DallasTemperature* _sensors;
};
#endif


/***************************************
   NodeManager: manages all the aspects of the node
*/
class NodeManager {
  public:
    NodeManager();
    // the pin to connect to the RST pin to reboot the board (default: 4)
    void setRebootPin(int value);
    #if BATTERY_MANAGER == 1
      // the expected vcc when the batter is fully discharged, used to calculate the percentage (default: 2.7)
      void setBatteryMin(float value);
      // the expected vcc when the batter is fully charged, used to calculate the percentage (default: 3.3)
      void setBatteryMax(float value);
      // how frequently (in hours) to report the battery level to the controller. When reset the battery is always reported (default: 1)
      void setBatteryReportCycles(int value);
    #endif
    #if SLEEP_MANAGER == 1
      // define if the board has to sleep every time entering loop (default: IDLE). It can be IDLE (no sleep), SLEEP (sleep at every cycle), WAIT (wait at every cycle
      void setSleepMode(int value);
      // define for how long the board will sleep (default: 0)
      void setSleepTime(int value);
      // define the unit of SLEEP_TIME. It can be SECONDS, MINUTES, HOURS or DAYS (default: MINUTES)
      void setSleep(int value1, int value2, int value3);
      void setSleepUnit(int value);
      // if enabled, when waking up from the interrupt, the board stops sleeping. Disable it when attaching e.g. a motion sensor (default: true)
      void setSleepInterruptPin(int value);
    #endif
    // configure the interrupt pin and mode. Mode can be CHANGE, RISING, FALLING (default: MODE_NOT_DEFINED)
    void setInterrupt(int pin, int mode, int pull = -1);
    // register a built-in sensor
    int registerSensor(int sensor_type, int pin = -1, int child_id = -1);
    // register a custom sensor
    int registerSensor(Sensor* sensor);
    // return a sensor by its index
    Sensor* get(int sensor_index);
    #if POWER_MANAGER == 1
      // to save battery the sensor can be optionally connected to two pins which will act as vcc and ground and activated on demand
      void setPowerPins(int ground_pin, int vcc_pin, long wait = 10);
      void powerOn();
      void powerOff();
    #endif

    // hook into the main sketch functions
    void before();
    void presentation();
    void loop();
    void receive(const MyMessage & msg);
  private:
    #if SLEEP_MANAGER == 1
      int _sleep_mode = IDLE;
      int _sleep_time = 0;
      int _sleep_unit = MINUTES;
      int _sleep_interrupt_pin = -1;
    #endif
    #if BATTERY_MANAGER == 1
      float _battery_min = 2.6;
      float _battery_max = 3.3;
      int _battery_report_cycles = 10;
      int _cycles = 0;
      float _getVcc();
    #endif

    #if POWER_MANAGER == 1
      // to optionally controller power pins
      PowerManager _powerManager;
    #endif
    MyMessage _msg;
    int _interrupt_1_mode = MODE_NOT_DEFINED;
    int _interrupt_2_mode = MODE_NOT_DEFINED;
    int _interrupt_1_pull = -1;
    int _interrupt_2_pull = -1;
    int _reboot_pin = -1;
    Sensor* _sensors[255] = {0};
    void _process(const char * message);
    void _sleep();
    int _getAvailableChildId();
    int _getInterruptInitialValue(int mode);
    bool _startup = true;
};

#endif
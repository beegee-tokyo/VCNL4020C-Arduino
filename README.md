# VNCL4020C-Arduino
----
Arduino library for the Vishay VNCL4020C    

## Content     
- General info
- VNCL4020C technical info
- API

## General info
The VCNL4020C is a fully integrated biosensor and ambient light sensor. Fully integrated means that the infrared emitter is included in the package. It has 16 bit resolution. It includes a signal processing IC and features standard I2C communication interface. It features an interrupt function.    

**Applications**
- Wearables
- Health monitoring
- Pulse oximetry

**Optical Biosensor Function**
- Built-in infrared emitter and broader sensitivity
photodiode allows to also work with green and red LEDs
- 16 bit effective resolution ensures excellent cross talk
immunity
- Programmable LED drive current from 10 mA to 200 mA in
10 mA steps
- Excellent ambient light suppression through signal
modulation

**Ambient Light Function**
- Built-in ambient light photo-pin-diode with
close-to-human-eye sensitivity
- 16 bit dynamic range from 0.25 lx to 16 klx
- 100 Hz and 120 Hz flicker noise rejection

## VNCL4020C technical info    
[Vishay product page](https://www.vishay.com/ppg?84350)    
[VNCL4020C data sheet](https://www.vishay.com/docs/84350/vcnl4020c.pdf)    

## API    
The library gives access to all registers of the VNCL4020C. For heart rate calculation the     
_**Optical Heart Rate Detection (PBA Algorithm)**_    
from _**Nathan Seidle (SparkFun Electronics)**_    
is included. The sources for the heart rate calculation are available on [Github](https://github.com/sparkfun/SparkFun_MAX3010x_Sensor_Library/tree/master/src) and are licensed under MIT.

The library supports the interrupt signal of the VNCL4020C chip. But if the interrupt signal is not connected, the sensor can as well be polled for measurement results    

Both single measurement and automatic periodic measurements by the sensor are supported.    

### Class declaration    
To make the library compatible with more Arduino microcontrollers, the class declaration takes a pointer to the I2C class to be used. This way it is easier to use it e.g. with a nRF52 chip.

#### Declaration example for ESP32 (or e.g. Arduino Uno)    
```CPP
VCNL4020C ppg1(&Wire, VCNL4020C_ADDR);
```    

#### Declaration example for nRF52 using a custom I2C assignment
```CPP
TwoWire i2cWire1 = TwoWire(NRF_TWIM0, NRF_TWIS0, SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, SDA1, SCL1);
VCNL4020C ppg1(&i2cWire1, VCNL4020C_ADDR);
```    

### Initialization of the sensor
```CPP
VCNL4020C(TwoWire *i2c, int addr = VCNL4020C_ADDR);
```    
The initialization checks if the sensor is connected and presets it with some default settings.    
- LED current set to 100mA (middle of range)    
- Conversion mode set to single measurement mode    
- Interrupts disabled    
- Threshold disabled    
- Bio sensor data rate set to 125 measurements/s    
- Bio sensor modulation adjustment set to Vishay default    
- Ambient light sensor data set to 10 samples/s    
- Ambient light automatic offset compensation off    
- Ambient light averaging off    

The function returns FALSE if sensor is not found or not responding, TRUE if success    
```CPP
if (!ppg1.initSensorDefault())
{
	Serial.println("Sensor initialization failed!");
}
```    
### Start a single measurement
```CPP
bool startSingle(bool bio = true, bool als = false);
```    
Start a single measurement on either the Bio sensor or the ambient light sensor or both.
Selection of the sensors are done with the parameters.    

The function returns FALSE if the communication with the sensor failed.    

### Start continous measurment
```CPP
bool startContinuous(bool bio = true, bool als = false);
```    
Start continuous measurement on either the Bio sensor or the ambient light sensor or both.
Selection of the sensors are done with the parameters.    

The function returns FALSE if the communication with the sensor failed.    

### Stop continous measurment
```CPP
bool stopContinuous(void);
```    
Stops continuous measurement. The measurement is stopped after the current measurement cycle of the sensor is finished.    

The function returns FALSE if the communication with the sensor failed.    

### Check if Bio sensor data is available (polling)    
```CPP
bool bioDataReady(void);
```    
This function checks if the sensors command register indicates that Bio sensor data is available.    
It should be only used if the sensor's interrupt line is not connected.    

### Check if ambient light sensor data is available (polling)    
```CPP
bool alsDataReady(void);
```    
This function checks if the sensors command register indicates that ambient light sensor data is available.    
It should be only used if the sensor's interrupt line is not connected.    

### Setup interrupt control
```CPP
bool setIntControl(bool bioEna, bool alsEna, bool thresEna, uint8_t thresSel, uint8_t thresCount);
```
If the interrupt signal is connected interrupts for different events can be enabled. This command sets as well whether if the treshhold function of the sensor should be applied to the Bio sensor or the ambient light sensor and how often the measurement has to be above or below the threshold before an interrupt is issued.     
- bioEna	Enable (TRUE) or disable (FALSE) bio sensor data ready interrupt
- alsEna	Enable (TRUE) or disable (FALSE) ambient light sensor data ready interrupt
- thresEna	Enable (TRUE) or disable (FALSE) threshold exceed interrupt
- thresSel	Select measurement method where threshold should be applied
			0 = apply to bio sensor measurements
			1 = apply to ambient light sensor measurements
- thresCount	Set number of measurements needed above/below the threshold before interrupt is set    
				Valid values:    

|      |      |    
| :---- | :---- |
| INT_CNT_EXC_1 | 1 count |
| INT_CNT_EXC_2 | 2 counts |
| INT_CNT_EXC_4 | 4 counts |
| INT_CNT_EXC_8 | 8 counts |
| INT_CNT_EXC_16 | 16 counts |
| INT_CNT_EXC_32 | 32 counts |
| INT_CNT_EXC_64 | 64 counts |
| INT_CNT_EXC_128 | 128 counts |

### Initialize interrupt callback function and GPIO for interrupt line
```CPP
void setInterruptCb(void (*sensorInt)(), int intPin);
```    
Enable interrupt controlled measurements.
**sensorInt** is a pointer to the callback function that will be called when the interrupt line goes low.
**intPin** is the GPIO to that the interrupt line is connected.    
_**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**_
_**Interrupt controlled measurement works only if both callback function and GPIO pin are defined**_
_**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**_

### Check if a Bio data is ready (interrupt controlled)
```CPP
bool checkBioInt(void);
```    
Returns true if the Bio sensor data available interrupt is set.     
This call clears as well the interrupt.     

### Check if an ambient light data is ready (interrupt controlled)
```CPP
bool checkAlsInt(void);
```    
Returns true if the ambient light sensor data available interrupt is set.     
This call clears as well the interrupt.     

### Check if threshold low hit (interrupt controlled)
```CPP
bool checkTreshLowInt(void);
```    
Returns true if the treshhold low interrupt is set.     
This call clears as well the interrupt.     

### Check if threshold high hit (interrupt controlled)
```CPP
bool checkTreshHighInt(void);
```    
Returns true if the treshhold high interrupt is set.     
This call clears as well the interrupt.     

### Check programmatically if an interrupt was issued (interrupt controlled)    
```CPP
bool checkInterrupts(uint8_t *intStatus);
```
Returns the content of the sensors interrupt status register.     
|      |      |    
| :---- | :---- |
| Bit 0 | 1 indicates a treshold low interrupt occured |
| Bit 1 | 1 indicates a treshold high interrupt occured |
| Bit 2 | 1 indicates a ambient light sensor data available interrupt occured |
| Bit 3 | 1 indicates a Bio sensor data available interrupt occured |   

### Read Bio sensor data
```CPP
uint16_t getBioValue(void);
```    
Reads the content of the Bio data register. Returns bio value as 16 bit value or 0xFFFF if no data available    

### Read ambient light sensor data
```CPP
uint16_t getAlsValue(void);
```    
Reads the content of the ambient light sensor data register. Returns bio value as 16 bit value or 0xFFFF if no data available    

### Set Bio sensor data rate for continuous measurement mode
```CPP
bool setBioDataRate(uint8_t dataRate);
```
Set the Bio sensor data rate to be used when continuous measurement mode is used.    
Valid values:
|	|	|
| :---- | :---- |
| BIO_SENS_RATE_1_95 | 1.95 measurements/s (DEFAULT) | 
| BIO_SENS_RATE_3_9 | 3.90625 measurements/s | 
| BIO_SENS_RATE_7_8 | 7.8125 measurements/s | 
| BIO_SENS_RATE_16_3 | 16.625 measurements/s | 
| BIO_SENS_RATE_31_3 | 31.25 measurements/s | 
| BIO_SENS_RATE_62_5 | 62.5 measurements/s | 
| BIO_SENS_RATE_125 | 125 measurements/s | 
| BIO_SENS_RATE_250 | 250 measurements/s | 

### Set LED current
```CPP
bool setLedCurrent(uint8_t ledCurrent);
```    
Sets the LED current value. Allowed value 0 to 20, LED current is value * 10mA    

### Set ambient light sensor parameters
```CPP
bool setAlsParam(uint8_t dataRate, uint8_t avgConv, bool offsetComp = true);
```
Set Ambient light sensor parameters    
**dataRate**	Sensor data rate, allowed values    
|	|	|
| :---- | :---- |
| AMB_SENS_RATE_1 | 1 samples/s |
| AMB_SENS_RATE_2 | 2 samples/s (DEFAULT) |
| AMB_SENS_RATE_3 | 3 samples/s |
| AMB_SENS_RATE_4 | 4 samples/s |
| AMB_SENS_RATE_5 | 5 samples/s |
| AMB_SENS_RATE_6 | 6 samples/s |
| AMB_SENS_RATE_8 | 8 samples/s |
| AMB_SENS_RATE_10 | 10 samples/s |
**avgConv**	Set number of conversion used for averaging the result, allowed values    
|	|	|
| :---- | :---- |
| AVG_CONV_1 | Average over 1 conversion |
| AVG_CONV_2 | Average over 2 conversions |
| AVG_CONV_4 | Average over 4 conversions |
| AVG_CONV_8 | Average over 8 conversions |
| AVG_CONV_16 | Average over 16 conversions |
| AVG_CONV_32 | Average over 32 conversions |
| AVG_CONV_64 | Average over 64 conversions |
| AVG_CONV_128 | Average over 128 conversions |
**offsetComp**	Enable(TRUE) or disable(FALSE) automatic offset compensation    

### Set lower treshold value
```CPP
bool setThresholdLow(uint16_t threshold);
```    
Sets lower treshold.    

### Set upper treshold value
```CPP
bool setThresholdHigh(uint16_t threshold);
```    
Sets upper treshold.    

### Set bio sensor modulation    
_**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**_    
_**The settings for best performance are provided by Vishay.**_    
_**With first samples this is evaluated to:**_    
_**Delay time = 0; dead time = 1 and BS frequency = 00.**_    
_**With that register #15 should be programmed with 1 (= default value).**_    
_**!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!**_     
```CPP
bool setBioSensMod(uint8_t bioSensMod);
```
**bioSensMod**
|	|	|	|
| :---- | :---- | :---- |
| Bit 7 -- Bit 5 | Bit 4  -- Bit 3 | Bit 2 -- Bit 0| 
| Modulation delay time | Biosensor frequency | Modulation dead time| 
- Modulation delay time R/W bits. 
	- Setting a delay time between LED signal and detectors input signal evaluation. This function is for compensation of delays from LED and photo diode. Also in respect to the possibility for setting different proximity signal frequency. Correct adjustment is optimizing measurement signal level. (DEFAULT = 0)
- Biosensor frequency R/W bits. 
	- Setting the biosensor test signal frequency. The biosensor measurement is using a square signal as measurement signal. Four different values are possible:    

|	|	|
| :---- | :---- |
| 00 | 390.625 kHz (DEFAULT)|
| 01 | 781.25 kHz|
| 10 | 1.5625 MHz|
| 11 | 3.125 MHz|    
- Modulation dead time R/W bits.
	- Setting a dead time in evaluation of LED signal at the slopes of the signal. (DEFAULT = 1). This function is for reducing of possible disturbance effects. This function is reducing signal level and should be used carefully.

### Read sensor IDs
```CPP
bool getIds(uint8_t *prodID, uint8_t *revID);
```     
Writes product ID and revision ID into the parameters    
Returns FALSE if the communication fails    

### Read command register    
```CPP
bool getCmdReg(uint8_t *cmdVal);
```
Writes the content of the command register into the parameters     
Returns FALSE if the communication fails

### Read bio sensor data rate
```CPP
bool getBioDataRate(uint8_t *cmdVal);
```     
Writes the selected Bio sensor data rate into the parameters    
Returns FALSE if the communication fails    

### Read ambient light sensor parameters
```CPP
bool getAlsParam(uint8_t *alsParam);
```     
Writes the selected ambient light sensor settings into the parameters    
Returns FALSE if the communication fails    

### Read interrupt control register    
```CPP
bool getIntControl(uint8_t *intCntrl);
```
Writes the content of the interrupt control register into the parameters     
Returns FALSE if the communication fails

### Read treshold values    
```CPP
bool getThresholds(uint16_t * thresholdHigh, uint16_t * thresholdLow);
```
Writes the content of the lower and upper threshold register into the parameters     
Returns FALSE if the communication fails

### Read Bio sensor modulation settings    
```CPP
bool getBioSensMod(uint8_t * modSetting);
```
Writes the content of the bio sensor modulation register into the parameters     
Returns FALSE if the communication fails


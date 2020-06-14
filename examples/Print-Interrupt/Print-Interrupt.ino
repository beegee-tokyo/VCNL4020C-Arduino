#include <Arduino.h>

#include <vcnl4020c.h>
#include <heartRate.h>

#ifdef NRF52_SERIES
#define SDA1 18 // I2C 1 SDA
#define SCL1 16 // I2C 1 SCL

TwoWire i2cWire1 = TwoWire(NRF_TWIM0, NRF_TWIS0, SPIM0_SPIS0_TWIM0_TWIS0_SPI0_TWI0_IRQn, SDA1, SCL1);

VCNL4020C ppg1(&i2cWire1, VCNL4020C_ADDR);
#else
VCNL4020C ppg1(&Wire, VCNL4020C_ADDR);
#endif

void ppg1IntHandler(void);
int vcnlIntPin = 15;

uint8_t regVal1;
uint8_t regVal2;
uint16_t regValL1;
uint16_t regValL2;

bool ppgHasData = false;

HEART_RATE hr;
int beatsPerMinute = 0;
uint16_t bioVal;

#ifndef time_t
#define time_t uint32_t
#endif

void setup()
{
	Serial.begin(115200);

	Serial.println("VCNL4020C test");
	// Initialize sensor
	if (!ppg1.initSensorDefault())
	{
		Serial.println("Sensor initialization failed!");
	}
	else
	{
		Serial.println("Initialization success");
	}

	// Show default values
	Serial.println("+++++++++++++++++++++++++++++++++++");
	Serial.println("Default values after initialization");
	Serial.println("+++++++++++++++++++++++++++++++++++");
	// Get DevID and RevID
	if (!ppg1.getIds(&regVal1, &regVal2))
	{
		Serial.println("Reading ID register failed!");
	}
	else
	{
		Serial.println("Got DevID " + String(regVal1) + ", RevID " + String(regVal2));
	}

	// Get current command register settings
	if (!ppg1.getCmdReg(&regVal1))
	{
		Serial.println("Reading command register failed!");
	}
	else
	{
		Serial.println("Self timed measures " + String(((regVal1 & SELF_TIMED_EN) == SELF_TIMED_EN ? "enabled" : "disabled")));
		Serial.println("Continuous bio measurement " + String(((regVal1 & PER_BIO_MEAS_EN) == PER_BIO_MEAS_EN ? "active" : "inactive")));
		Serial.println("Continuous als measurement " + String(((regVal1 & PER_ALS_MEAS_EN) == PER_ALS_MEAS_EN ? "active" : "inactive")));
		Serial.println("Single bio measurement " + String(((regVal1 & START_BIO_MES) == START_BIO_MES ? "active" : "inactive")));
		Serial.println("Single als measurement " + String(((regVal1 & START_ALS_MES) == START_ALS_MES ? "active" : "inactive")));
	}
	// Get current bio sensor data rate
	if (!ppg1.getBioDataRate(&regVal1))
	{
		Serial.println("Reading bio data rate failed!");
	}
	else
	{
		String sensRates[] = {"1.95", "3.90625", "7.8125", "16.625", "31.25", "62.5", "125", "250"};
		if (regVal1 > 7)
		{
			Serial.println("Got invalid bio sensor rate");
		}
		else
		{
			Serial.println("Bio sensor data rate is " + sensRates[regVal1] + "measures/s");
		}
	}
	// Get LED current setting
	regVal1 = ppg1.getLedCurrent();
	if (regVal1 > 20)
	{
		Serial.println("Got invalid LED current");
	}
	else
	{
		Serial.println("LED current is " + String(regVal1 * 10) + " mA");
	}

	// Get ambient light sensor settings
	if (!ppg1.getAlsParam(&regVal1))
	{
		Serial.println("Reading ALS settings failed");
	}
	else
	{
		uint8_t alsRates[] = {1, 2, 3, 4, 5, 6, 8, 10};
		regVal2 = (regVal1 & 0b0111000) >> 4;
		Serial.println("ALS data rate " + String(alsRates[regVal2]) + " samples/s");
		Serial.println("Auto offset compensation is " + String(((regVal1 & AUTO_COMP_ENA) == AUTO_COMP_ENA ? "enabled" : "disabled")));
		regVal2 = regVal1 & 0b00000011;
		Serial.println("Averaging count is " + String(pow(2, regVal2)));
	}

	Serial.println("+++++++++++++++++++++++++++++++++++");
	Serial.println("Do a single measurment");
	Serial.println("+++++++++++++++++++++++++++++++++++");

	// Start a single Bio and ALS measurement and poll result from command register
	if (!ppg1.startSingle(true, true))
	{
		Serial.println("Starting Bio & ALS single measurement failed");
	}
	else
	{
		time_t startTimeout = millis();
		bool waitTimeOut = true;
		uint8_t bothReady = 0;
		while ((millis() - startTimeout) < 5000) // Stop polling after 5 seconds
		{
			if (ppg1.bioDataReady())
			{
				bothReady++;
			}
			if (ppg1.alsDataReady())
			{
				bothReady++;
			}
			if (bothReady >= 2)
			{
				Serial.println("Got both Bio and ALS data ready");
				waitTimeOut = false;
				break;
			}
		}

		if (waitTimeOut)
		{
			Serial.println("Polling Bio and ALS values timed out");
		}
		else
		{
			regValL1 = ppg1.getBioValue();
			regValL2 = ppg1.getAlsValue();
			if (regValL1 == 0xFFFF)
			{
				Serial.println("Measured bio value is invalid");
			}
			else
			{
				Serial.println("Got Bio value " + regValL1);
			}
			if (regValL2 == 0xFFFF)
			{
				Serial.println("Measured ALS value is invalid");
			}
			else
			{
				Serial.println("Got ALS value " + regValL2);
			}
		}
	}

	// Set interrupt pin and callback function
	ppg1.setInterruptCb(ppg1IntHandler, vcnlIntPin);

	// Set bio sensor data rate
	ppg1.setBioDataRate(BIO_SENS_RATE_250);
	// Set ALS data rate
	ppg1.setAlsParam(AMB_SENS_RATE_10, AVG_CONV_1, true);

	// Set LED current
	ppg1.setLedCurrent(3);

	Serial.println("+++++++++++++++++++++++++++++++++++");
	Serial.println("Started continuous measurements of both Bio sensor and Ambient Light sensor");
	Serial.println("+++++++++++++++++++++++++++++++++++");
	// Start continuous measurement
	ppg1.startContinuous(true, true);
}

void loop()
{
	if (ppgHasData)
	{
		if (ppg1.checkBioInt())
		{
			uint16_t bioVal = ppg1.getBioValue();
			if (hr.checkForBeat(bioVal))
			{
				beatsPerMinute = hr.getLastHR();
			}
			Serial.println("Bio value " + String(bioVal) + " Heartrate " + String(beatsPerMinute));
		}
		if (ppg1.checkAlsInt())
		{
			Serial.println("ALS value " + String(ppg1.getAlsValue()));
		}

		// Attempt to auto-adjust LED current
		uint8_t current;
		if (bioVal < 40000)
		{
			// Value is too low, raise the LED current
			current = ppg1.getLedCurrent();
			if (current < 20)
			{
				current++;
				ppg1.setLedCurrent(current);
			}
		}
		if (bioVal > 60000)
		{
			// Value is too high, lower the LED current
			current = ppg1.getLedCurrent();
			if (current > 0)
			{
				current--;
				ppg1.setLedCurrent(current);
			}
		}
		ppgHasData = false;
	}
}

void ppg1IntHandler(void)
{
	ppgHasData = true;
}

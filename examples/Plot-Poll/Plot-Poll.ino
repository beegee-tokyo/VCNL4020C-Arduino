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

uint8_t regVal1;
uint8_t regVal2;
uint16_t regValL1;
uint16_t regValL2;

bool ppgHasData = false;

HEART_RATE hr;
int beatsPerMinute = 0;
uint16_t bioVal;

void setup()
{
	Serial.begin(115200);

	// Initialize sensor
	if (!ppg1.initSensorDefault())
	{
		Serial.println("Sensor initialization failed!");
	}

	// Set bio sensor data rate
	ppg1.setBioDataRate(BIO_SENS_RATE_250);
	// Set ALS data rate
	ppg1.setAlsParam(AMB_SENS_RATE_10, AVG_CONV_1, true);

	// Set LED current
	ppg1.setLedCurrent(3);

	// Start continuous measurement with Bio sensor only
	ppg1.startContinuous(true, false);
}

void loop()
{
	if (ppg1.bioDataReady())
	{
		bioVal = ppg1.getBioValue();
		Serial.printf("%d\n", bioVal);

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
	}
}

// Arduino library for the Vishay VCNL4020C
// High Resolution Digital Biosensor for Wearable Applications With I²C Interface
// APPLICATIONS
// - Wearables
// - Health monitoring
// - Pulse oximetry

#include "vcnl4020c.h"

VCNL4020C::VCNL4020C(TwoWire *i2c, int addr)
{
	_i2c = i2c;
	_addr = addr;
}

VCNL4020C::~VCNL4020C()
{
}

bool VCNL4020C::initSensorDefault(void)
{
	uint8_t checkID = 0;
	uint8_t checkRev = 0;
	// Initialize I2C
	_i2c->begin();
	// Set I2C to 800 kHz
	_i2c->setClock(800000);
	// Read device ID and revision ID
	if (!getIds(&checkID, &checkRev))
	{
		return false;
	}
	// Check if device ID and revision are as expected
	if ((checkID != 2) && (checkRev != 1))
	{
#ifdef NRF52_SERIES
		_i2c->end();
#endif
		return false;
	}
	// Set default values
	regValue = 0;
	if (!writeRegs(CMD_REG, &regValue, 1))
	{
		return false;
	}
	delay(10);

	if (!setBioDataRate(BIO_SENS_RATE_125))
	{
		return false;
	}
	if (!setLedCurrent(10))
	{
		return false;
	}
	if (!setAlsParam(AMB_SENS_RATE_10, AVG_CONV_1, false))
	{
		return false;
	}
	if (!setIntControl(false, false, false, 0, INT_CNT_EXC_1))
	{
		return false;
	}
	if (!setThresholdLow(0))
	{
		return false;
	}
	if (!setThresholdHigh(0))
	{
		return false;
	}
	if (!setBioSensMod(BIO_SETTINGS_VISHAY))
	{
		return false;
	}
	return true;
}

bool VCNL4020C::getCmdReg(uint8_t *cmdVal)
{
	return readRegs(CMD_REG, cmdVal, 1);
}

bool VCNL4020C::alsDataReady(void)
{
	if (readRegs(CMD_REG, &regValue, 1))
	{
		if ((regValue & ALS_DATA_READY) == ALS_DATA_READY)
		{
			return true;
		}
	}
	return false;
}

bool VCNL4020C::bioDataReady(void)
{
	if (readRegs(CMD_REG, &regValue, 1))
	{
		if ((regValue & BIO_DATA_READY) == BIO_DATA_READY)
		{
			return true;
		}
	}
	return false;
}

bool VCNL4020C::startSingle(bool bio, bool als)
{
	// Prepare interrupt control register
	regValue = 0;

	// Check if interrupt callback function is set and interrupt GPIO is defined
	if ((_sensorInt != NULL) && (_intPin != -1))
	{
		pinMode(_intPin, INPUT_PULLUP);
		attachInterrupt(_intPin, _sensorInt, FALLING);

		// Enable the interrupts
		if (bio)
		{
			regValue |= INT_BS_RDY_ENA;
			_intMeasurementBio = true;
		}
		if (als)
		{
			regValue |= INT_ALS_RDY_ENA;
			_intMeasurementALS = true;
		}
		if ((_lowThresh != 0) && (_highThresh != 0))
		{
			regValue |= INT_THRES_ENA;
			_intThreshold = true;
		}

		// Start single measurement
		return writeRegs(INT_CONTR, &regValue, 1);
	}

	// Prepare command register
	regValue = 0;
	// Enable measurements
	if (bio)
	{
		regValue |= START_BIO_MES;
	}
	if (als)
	{
		regValue |= START_ALS_MES;
	}
	regValue &= SELF_TIMED_DIS;

	// Start the measurement
	return writeRegs(CMD_REG, &regValue, 1);
}

bool VCNL4020C::startContinuous(bool bio, bool als)
{
	// Prepare interrupt control register
	regValue = 0;

	// Check if interrupt callback function is set and interrupt GPIO is defined
	if ((_sensorInt != NULL) && (_intPin != -1))
	{
		pinMode(_intPin, INPUT_PULLUP);
		attachInterrupt(_intPin, _sensorInt, FALLING);

		// Enable the interrupts
		if (bio)
		{
			regValue |= INT_BS_RDY_ENA;
			_intMeasurementBio = true;
		}
		if (als)
		{
			regValue |= INT_ALS_RDY_ENA;
			_intMeasurementALS = true;
		}
		if ((_lowThresh != 0) && (_highThresh != 0))
		{
			regValue |= INT_THRES_ENA;
			_intThreshold = true;
		}
		if (!writeRegs(INT_CONTR, &regValue, 1))
		{
			return false;
		}
	}

	// Prepare command register
	regValue = 0;
	// Enable measurements
	if (bio)
	{
		regValue |= PER_BIO_MEAS_EN;
	}
	if (als)
	{
		regValue |= PER_ALS_MEAS_EN;
	}
	regValue |= SELF_TIMED_EN;

	// Start the measurement
	return writeRegs(CMD_REG, &regValue, 1);
}

bool VCNL4020C::stopContinuous(void)
{
	// Prepare interrupt control register
	regValue = 0;

	// Check if interrupt callback function is set and interrupt GPIO is defined
	if ((_sensorInt != NULL) && (_intPin != -1))
	{
		detachInterrupt(_intPin);
		if (!writeRegs(INT_STATUS, &regValue, 1))
		{
			return false;
		}
	}

	_intMeasurementBio = false;
	_intMeasurementALS = false;
	_intThreshold = false;

	// Prepare command register
	regValue = 0;
	// Stop the measurement
	return writeRegs(CMD_REG, &regValue, 1);
}

bool VCNL4020C::getIds(uint8_t *prodID, uint8_t *revID)
{
	if (!readRegs(PROD_ID, &regValue, 1))
	{
		return false;
	}
	*prodID = (uint8_t)(regValue >> 4);
	*revID = regValue & 0b00001111;
	return true;
}

bool VCNL4020C::setBioDataRate(uint8_t dataRate)
{
	if (dataRate > BIO_SENS_RATE_250)
	{
		dataRate = BIO_SENS_RATE_250;
	}
	return writeRegs(BIO_SENS_RATE, &dataRate, 1);
}

bool VCNL4020C::getBioDataRate(uint8_t *dataRate)
{
	return readRegs(BIO_SENS_RATE, dataRate, 1);
}

bool VCNL4020C::setLedCurrent(uint8_t ledCurrent)
{
	if (ledCurrent > 20)
	{
		ledCurrent = 20;
	}
	return writeRegs(LED_CURRENT, &ledCurrent, 1);
}

uint8_t VCNL4020C::getLedCurrent(void)
{
	readRegs(LED_CURRENT, &regValue, 1);
	regValue = regValue & 0b00111111;
	return regValue;
}

bool VCNL4020C::setAlsParam(uint8_t dataRate, uint8_t avgConv, bool offsetComp)
{
	regValue = 0;
	if (dataRate > AMB_SENS_RATE_10)
	{
		dataRate = AMB_SENS_RATE_10;
	}
	if (avgConv > AVG_CONV_128)
	{
		avgConv = AVG_CONV_128;
	}

	if (offsetComp)
	{
		regValue |= AUTO_COMP_ENA;
	}
	regValue |= dataRate;
	regValue |= avgConv;

	/// \todo how to handle Continuous Conversion Mode

	return writeRegs(AMBIENT_LIGHT_PARAM, &regValue, 1);
}

bool VCNL4020C::getAlsParam(uint8_t *alsParam)
{
	return readRegs(AMBIENT_LIGHT_PARAM, alsParam, 1);
}

uint16_t VCNL4020C::getAlsValue(void)
{
	uint8_t valHi = 0;
	uint8_t valLo = 0;

	if (!readRegs(AMB_RESULT_L, &valLo, 1))
	{
		return 0xFFFF;
	}
	if (!readRegs(AMB_RESULT_H, &valHi, 1))
	{
		return 0xFFFF;
	}
	return (uint16_t)((valHi) << 8) + valLo;
}

uint16_t VCNL4020C::getBioValue(void)
{
	uint8_t valHi = 0;
	uint8_t valLo = 0;

	if (!readRegs(BIO_RESULT_L, &valLo, 1))
	{
		return 0xFFFF;
	}
	if (!readRegs(BIO_RESULT_H, &valHi, 1))
	{
		return 0xFFFF;
	}
	return (uint16_t)((valHi) << 8) + valLo;
}

bool VCNL4020C::setIntControl(bool bioEna, bool alsEna, bool thresEna, uint8_t thresSel, uint8_t thresCount)
{
	regValue = 0;
	if (bioEna)
	{
		regValue |= INT_BS_RDY_ENA;
	}
	if (alsEna)
	{
		regValue |= INT_ALS_RDY_ENA;
	}
	if (thresEna)
	{
		regValue |= INT_THRES_ENA;
	}
	if (thresSel == INT_THRES_BIO)
	{
		regValue |= INT_THRES_BIO;
	}
	else
	{
		regValue |= INT_THRES_ALS;
	}
	if (thresCount > INT_CNT_EXC_128)
	{
		thresCount = INT_CNT_EXC_128;
	}
	regValue |= thresCount;
	return writeRegs(INT_CONTR, &regValue, 1);
}

bool VCNL4020C::getIntControl(uint8_t *intCntrl)
{
	return readRegs(INT_CONTR, intCntrl, 1);
}

bool VCNL4020C::setThresholdLow(uint16_t threshold)
{
	regValue = threshold;
	if (!writeRegs(THRES_LOW_VAL_L, &regValue, 1))
	{
		return false;
	}
	regValue = threshold >> 8;
	return writeRegs(THRES_LOW_VAL_H, &regValue, 1);
}

bool VCNL4020C::setThresholdHigh(uint16_t threshold)
{
	regValue = threshold;
	if (!writeRegs(THRES_HIGH_VAL_L, &regValue, 1))
	{
		return false;
	}
	regValue = threshold >> 8;
	return writeRegs(THRES_HIGH_VAL_H, &regValue, 1);
}

bool VCNL4020C::getThresholds(uint16_t *thresholdHigh, uint16_t *thresholdLow)
{
	uint8_t lowByte = 0;
	uint8_t highByte = 0;
	if (!readRegs(THRES_LOW_VAL_L, &lowByte, 1))
	{
		return false;
	}
	if (!readRegs(THRES_LOW_VAL_H, &highByte, 1))
	{
		return false;
	}
	*thresholdLow = ((uint16_t)(highByte) << 8) + lowByte;
	if (!readRegs(THRES_HIGH_VAL_L, &lowByte, 1))
	{
		return false;
	}
	if (!readRegs(THRES_HIGH_VAL_H, &highByte, 1))
	{
		return false;
	}
	*thresholdHigh = ((uint16_t)(highByte) << 8) + lowByte;
	return true;
}

bool VCNL4020C::checkInterrupts(uint8_t *intStatus)
{
	if (!readRegs(INT_STATUS, intStatus, 1))
	{
		return false;
	}
	return true;
}

bool VCNL4020C::checkBioInt(void)
{
	if (!checkInterrupts(&regValue))
	{
		return false;
	}

	if ((regValue & INT_BIO_RDY) == INT_BIO_RDY)
	{
		regValue = INT_BIO_RDY;
		writeRegs(INT_STATUS, &regValue, 1);
		if (_intMeasurementBio)
		{
			getIntControl(&regValue);
			regValue |= INT_BIO_RDY;
			writeRegs(INT_CONTR, &regValue, 1);
		}
		return true;
	}
	return false;
}

bool VCNL4020C::checkAlsInt(void)
{
	if (!checkInterrupts(&regValue))
	{
		return false;
	}
	if ((regValue & INT_ALS_RDY) == INT_ALS_RDY)
	{
		regValue = INT_ALS_RDY;
		writeRegs(INT_STATUS, &regValue, 1);
		if (_intMeasurementALS)
		{
			getIntControl(&regValue);
			regValue |= INT_ALS_RDY;
			writeRegs(INT_CONTR, &regValue, 1);
		}
		return true;
	}
	return false;
}

bool VCNL4020C::checkThreshLowInt(void)
{
	if (!checkInterrupts(&regValue))
	{
		return false;
	}
	if ((regValue & INT_TH_LOW_RDY) == INT_TH_LOW_RDY)
	{
		regValue = INT_TH_LOW_RDY;
		writeRegs(INT_STATUS, &regValue, 1);
		if (_intThreshold)
		{
			getIntControl(&regValue);
			regValue |= INT_TH_LOW_RDY;
			writeRegs(INT_CONTR, &regValue, 1);
		}
		return true;
	}
	return false;
}

bool VCNL4020C::checkThreshHighInt(void)
{
	if (!checkInterrupts(&regValue))
	{
		return false;
	}
	if ((regValue & INT_TH_HIGH_RDY) == INT_TH_HIGH_RDY)
	{
		regValue = INT_TH_HIGH_RDY;
		writeRegs(INT_STATUS, &regValue, 1);
		if (_intThreshold)
		{
			getIntControl(&regValue);
			regValue |= INT_TH_HIGH_RDY;
			writeRegs(INT_CONTR, &regValue, 1);
		}
		return true;
	}
	return false;
}

bool VCNL4020C::setBioSensMod(uint8_t bioSensMod)
{
	return writeRegs(BIO_SETTINGS, &regValue, 1);
}

void VCNL4020C::setInterruptCb(void (*sensorInt)(), int intPin)
{
	_sensorInt = sensorInt;
	_intPin = intPin;
}

bool VCNL4020C::getBioSensMod(uint8_t *modSetting)
{
	return readRegs(BIO_SETTINGS, modSetting, 1);
}

bool VCNL4020C::writeRegs(int reg_addr, uint8_t *data, int len)
{
	_i2c->beginTransmission(_addr);
	if (_i2c->write(reg_addr) == 0)
	{
		_i2c->endTransmission();
		return false;
	}
	for (unsigned char i = 0; i < len; i++)
	{
		if (_i2c->write(data[i]) == 0)
		{
			_i2c->endTransmission();
			return false;
		}
	}
	_i2c->endTransmission();
	return true;
}

bool VCNL4020C::readRegs(int reg_addr, uint8_t *data, int len)
{
	_i2c->beginTransmission(_addr);
	if (_i2c->write(reg_addr) == 0)
	{
		_i2c->endTransmission();
		return false;
	}
	if (_i2c->endTransmission(false) != 0)
	{
		return false;
	}
	if (_i2c->requestFrom(_addr, len) == 0)
	{
		return false;
	}
	for (unsigned char i = 0; i < len; i++)
	{
		data[i] = _i2c->read();
	}

	return true;
}

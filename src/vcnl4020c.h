#ifndef VCNL4020C_H
#define VCNL4020C_H

// Arduino library for the Vishay VCNL4020C
// High Resolution Digital Biosensor for Wearable Applications With I²C Interface
// APPLICATIONS
// - Wearables
// - Health monitoring
// - Pulse oximetry

#define VCNL4020C_ADDR 0x13 // ??? Datasheet said 0x26 ???

/**
 * Command register
 * Bit 7		Bit 6			Bit 5			Bit 4
 * config_lock	als_data_rdy	bs_data_rdy		als_od
 * Bit 3		Bit 2			Bit 1			Bit 0
 * bs_od		als_en			bs_en			selftimed_en
 * config_lock Read only bit. Value = 1
 * als_data_rdy Read only bit. Value = 1 when ambient light measurement data is available
 * 		in the result registers. This bit will be reset when one of the corresponding
 * 		result registers (reg #5, reg #6) is read.
 * bs_data_rdy Read only bit. Value = 1 when biosensor measurement data is available
 * 		in the result registers. This bit will be reset when one of the corresponding
 * 		result registers (reg #7, reg #8) is read.
 * als_od R/W bit. Starts a single on-demand measurement for ambient light. 
 * 		If averaging is enabled, starts a sequence of readings and stores the averaged
 * 		result. Result is available at the end of conversion for reading in the 
 * 		registers #5(HB) and #6(LB).
 * bs_od R/W bit. Starts a single on-demand measurement for biosensor. Result is 
 * 		available at the end of conversion for reading in the registers #7(HB) and #8(LB).
 * als_en R/W bit. Enables periodic als measurement
 * bs_en R/W bit. Enables periodic biosensor measurement selftimed_en R/W bit. 
 * 		Enables state machine and LP oscillator for self timed measurements; no measurement
 * 		is performed until the corresponding bit is set
 */
#define CMD_REG 0x80

#define SELF_TIMED_EN 0b00000001	/*<-- Enable self timed measurements */
#define SELF_TIMED_DIS 0b11111110	/*<-- Disable self timed measurements */
#define PER_BIO_MEAS_EN 0b00000010	/*<-- Enable bio sensor measurements */
#define PER_BIO_MEAS_DIS 0b11111101 /*<-- Disable bio sensor measurements */
#define PER_ALS_MEAS_EN 0b00000100	/*<-- Enable ambient light measurement */
#define PER_ALS_MEAS_DIS 0b11111011 /*<-- Disable ambient light measurement */
#define START_BIO_MES 0b00001000	/*<-- Start bio sensor measurement */
#define STOP_BIO_MES 0b11110111		/*<-- Stop bio sensor measurement */
#define START_ALS_MES 0b00010000	/*<-- Start ambient light measurement */
#define STOP_ALS_MES 0b11101111		/*<-- Stop ambient light measurement */
#define BIO_DATA_READY 0b00100000	/*<-- Bio sensor data available */
#define ALS_DATA_READY 0b01000000	/*<-- Ambient light sensor data available */
#define CONFIG_LOCK 0b10000000		/*<-- Fixed value 1 */

/**
 * Product ID Revision Register
 * Bit 7 -- Bit 4			Bit 3 -- Bit 0
 * Product ID, Value = 2	Revision ID, Value = 1
 */
#define PROD_ID 0x81

/**
 * Rate of Biosensor Measurement Register
 * Bit 2 -- Bit 0
 * Rate of biosensor measurement
 */
#define BIO_SENS_RATE 0x82

#define BIO_SENS_RATE_1_95 0b00000000 /*<-- 1.95 measurements/s (DEFAULT) */
#define BIO_SENS_RATE_3_9 0b00000001  /*<-- 3.90625 measurements/s */
#define BIO_SENS_RATE_7_8 0b00000010  /*<-- 7.8125 measurements/s */
#define BIO_SENS_RATE_16_3 0b00000011 /*<-- 16.625 measurements/s */
#define BIO_SENS_RATE_31_3 0b00000100 /*<-- 31.25 measurements/s */
#define BIO_SENS_RATE_62_5 0b00000101 /*<-- 62.5 measurements/s */
#define BIO_SENS_RATE_125 0b00000110  /*<-- 125 measurements/s */
#define BIO_SENS_RATE_250 0b00000111  /*<-- 250 measurements/s */

/**
 * LED Current Setting fro Biosensor Mode
 * Value adjustable from 0mA to 200mA in 10mA steps
 * Bit 7 -- Bit 6		Bit 5 -- Bit 0
 * Fuse Prog ID			LED current value
 * Fuse prog ID Read only bits. Information about fuse program revision
 * 		used for initial setup/calibration of the device.
 * LED current value R/W bits. LED current = Value (dec.) x 10 mA.
 * 		Valid Range = 0 to 20d. e.g. 0 = 0 mA, 1 = 10 mA, …., 20 = 200 mA (2 = 20 mA = DEFAULT)
 * 		LED Current is limited to 200 mA for values higher as 20d.
 */
#define LED_CURRENT 0x83

#define FUSE_MASK 0b11000000
#define CURRENT_MASK 0b00111111

/**
 * Ambient Light Parameter Register
 * Bit 7						Bit 6 -- Bit 4
 * Cont. conv mode				Ambient light measurement rate
 * Bit 3						Bit 2 -- Bit 0
 * Auto offset compensation		Averaging function
 * Cont. conversion mode R/W bit. Continuous conversion mode.
 *		Enable = 1; Disable = 0 = DEFAULT
 *		This function can be used for performing faster ambient light measurements.
 *		This mode should only be used with ambient light on-demand measurements.
 *		Do not use with self-timed mode. Please refer to the application information
 *		chapter 3.3 for details about this function.
 * Ambient light measurement rate R/W bits. Ambient light measurement rate
 *		000 - 1 samples/s
 *		001 - 2 samples/s = DEFAULT
 *		010 - 3 samples/s
 *		011 - 4 samples/s
 *		100 - 5 samples/s
 *		101 - 6 samples/s
 *		110 - 8 samples/s
 *		111 - 10 samples/s
 * Auto offset compensation R/W bit. Automatic offset compensation.
 *		Enable = 1 = DEFAULT; Disable = 0
 *		In order to compensate a technology, package or temperature related drift of the 
 *		ambient light values there is a built in automatic offset compensation function.
 *		With active auto offset compensation the offset value is measured before each 
 *		ambient light measurement and subtracted automatically from actual reading.
 * Averaging function R/W bits. Averaging function.
 *		Bit values sets the number of single conversions done during one measurement cycle. 
 *		Result is the average value of all conversions.
 *		Number of conversions = 2^decimal_value e.g. 0 = 1 conv., 1 = 2 conv, 
 *		2 = 4 conv., ….7 = 128 conv.
 *		DEFAULT = 32 conv. (bit 2 to bit 0: 101)
 */
#define AMBIENT_LIGHT_PARAM 0x84

#define CONT_CONV_ENA 0b10000000	/*<-- Enable continuous conversion mode */
#define CONT_CONV_DIS 0b01111111	/*<-- Disable continuous conversion mode */
#define AMB_SENS_RATE_1 0b00000000	/*<-- 1 samples/s */
#define AMB_SENS_RATE_2 0b00010000	/*<-- 2 samples/s (DEFAULT) */
#define AMB_SENS_RATE_3 0b00100000	/*<-- 3 samples/s */
#define AMB_SENS_RATE_4 0b00110000	/*<-- 4 samples/s */
#define AMB_SENS_RATE_5 0b01000000	/*<-- 5 samples/s */
#define AMB_SENS_RATE_6 0b01010000	/*<-- 6 samples/s */
#define AMB_SENS_RATE_8 0b01100000	/*<-- 8 samples/s */
#define AMB_SENS_RATE_10 0b01110000 /*<-- 10 samples/s */
#define AUTO_COMP_ENA 0b00001000	/*<-- Enable automatic offset compensation */
#define AUTO_COMP_DIS 0b11110111	/*<-- Disable automatic offset compensation */
#define AVG_CONV_1 0b00000000		/*<-- Average over 1 conversion */
#define AVG_CONV_2 0b00000001		/*<-- Average over 2 conversions */
#define AVG_CONV_4 0b00000010		/*<-- Average over 4 conversions */
#define AVG_CONV_8 0b00000011		/*<-- Average over 8 conversions */
#define AVG_CONV_16 0b00000100		/*<-- Average over 16 conversions */
#define AVG_CONV_32 0b00000101		/*<-- Average over 32 conversions */
#define AVG_CONV_64 0b00000110		/*<-- Average over 64 conversions */
#define AVG_CONV_128 0b00000111		/*<-- Average over 128 conversions */

/**
 * Ambient light result register
 * High byte
 */
#define AMB_RESULT_H 0x85

/**
 * Ambient light result register
 * Low byte
 */
#define AMB_RESULT_L 0x86

/**
 * Bio sensor result register
 * High byte
 */
#define BIO_RESULT_H 0x87

/**
 * Bio sensor result register
 * Low byte
 */
#define BIO_RESULT_L 0x88

/**
 * Interrupt control register
 * Bit 7 -- Bit 5				Bit 4
 * Int count exceed				unused
 * Bit 3			Bit 2				Bit 1			Bit 0
 * INT_BS_READY_EN	INT_ALS_READY_EN	INT_THRES_EN	INT_THRES_SEL
 * Int count exceed R/W bits. 
 *		These bits contain the number of consecutive measurements needed above/below the threshold
 *		000 - 1 count = DEFAULT
 *		001 - 2 count
 *		010 - 4 count
 *		011 - 8 count
 *		100 -16 count
 *		101 - 32 count
 *		110 - 64 count
 *		111 - 128 count
 * INT_BS_ready_EN R/W bit.
 *		Enables interrupt generation at biosensor data ready
 * INT_ALS_ ready_EN R/W bit. 
 *		Enables interrupt generation at ambient data ready
 * INT_THRES_EN R/W bit. 
 *		Enables interrupt generation when high or low threshold is exceeded
 * INT_THRES_SEL R/W bit. 
 *		If 0: thresholds are applied to biosensor measurements
 *		If 1: thresholds are applied to als measurements
 */
#define INT_CONTR 0x89

#define INT_CNT_EXC_1 0b00000000   /*<-- 1 count (DEFAULT) */
#define INT_CNT_EXC_2 0b00100000   /*<-- 2 counts */
#define INT_CNT_EXC_4 0b01000000   /*<-- 4 counts */
#define INT_CNT_EXC_8 0b01100000   /*<-- 8 counts */
#define INT_CNT_EXC_16 0b10000000  /*<-- 16 counts */
#define INT_CNT_EXC_32 0b10100000  /*<-- 32 counts */
#define INT_CNT_EXC_64 0b11000000  /*<-- 64 counts */
#define INT_CNT_EXC_128 0b11100000 /*<-- 128 counts */
#define INT_BS_RDY_ENA 0b00001000  /*<-- Enable Bio sensor data ready interrupt */
#define INT_BS_RDY_DIS 0b11110111  /*<-- Enable Bio sensor data ready interrupt */
#define INT_ALS_RDY_ENA 0b00000100 /*<-- Enable Ambient light sensor data ready interrupt */
#define INT_ALS_RDY_DIS 0b11111011 /*<-- Enable Ambient light sensor data ready interrupt */
#define INT_THRES_ENA 0b00000010   /*<-- Enable Ambient light sensor data ready interrupt */
#define INT_THRES_DIS 0b11111101   /*<-- Enable Ambient light sensor data ready interrupt */
#define INT_THRES_BIO 0b00000000   /*<-- Threshold applied to Bio sensor data */
#define INT_THRES_ALS 0b00000001   /*<-- Threshold applied to Ambient light sensor data */

/**
 * Low threshold register
 * High byte
 */
#define THRES_LOW_VAL_H 0x8A

/**
 * Low threshold register
 * Low byte
 */
#define THRES_LOW_VAL_L 0x8B

/**
 * High threshold register
 * High byte
 */
#define THRES_HIGH_VAL_H 0x8C

/**
 * High threshold register
 * Low byte
 */
#define THRES_HIGH_VAL_L 0x8D

/**
 * Interrupt Status Register
 * Bit 3			Bit 2			Bit 1		Bit 0
 * int_bs_ready		int_als_ready	int_th_low	int_th_hi
 * int_bs_ready R/W bit. 
 *		Indicates a generated interrupt for biosensor
 * int_als_ready R/W bit. 
 *		Indicates a generated interrupt for als
 * int_th_low R/W bit. 
 *		Indicates a low threshold exceed
 * int_th_hi R/W bit.
 *		Indicates a high threshold exceed
 * Once an interrupt is generated the corresponding status bit goes to 1
 * and stays there unless it is cleared by writing a 1 in the corresponding
 * bit. The int pad will be pulled down while at least one of the status bit is 1.
 */
#define INT_STATUS 0x8E

#define INT_BIO_RDY 0b00001000	   /*<-- Bio sensor data ready / clear mask */
#define INT_ALS_RDY 0b00000100	   /*<-- Ambient light sensor data ready / clear mask */
#define INT_TH_LOW_RDY 0b00000010  /*<-- Low threshold exceed / clear mask */
#define INT_TH_HIGH_RDY 0b00000001 /*<-- High threshold exceed / clear mask */

/** Biosensor Modulator Timing Adjustment
 * Bit 7 -- Bit 5 			Bit 4  -- Bit 3 		Bit 2 -- Bit 0
 * Modulation delay time	Biosensor frequency		Modulation dead time
 * Modulation delay time R/W bits. 
 *		Setting a delay time between LED signal and detectors input signal evaluation.
 *		This function is for compensation of delays from LED and photo diode.
 *		Also in respect to the possibility for setting different proximity signal frequency.
 *		Correct adjustment is optimizing measurement signal level. (DEFAULT = 0)
 * Biosensor frequency R/W bits. 
 *		Setting the biosensor test signal frequency
 *		The biosensor measurement is using a square signal as measurement signal.
 *		Four different values are possible:
 *		00 = 390.625 kHz (DEFAULT)
 *		01 = 781.25 kHz
 *		10 = 1.5625 MHz
 *		11 = 3.125 MHz
 * Modulation dead time R/W bits.
 *		Setting a dead time in evaluation of LED signal at the slopes of the signal. (DEFAULT = 1)
 *		This function is for reducing of possible disturbance effects.
 *		This function is reducing signal level and should be used carefully.
 *
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 * The settings for best performance will be provided by Vishay. 
 * With first samples this is evaluated to:
 * Delay time = 0; dead time = 1 and BS frequency = 00.
 * With that register #15 should be programmed with 1 (= default value).
 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 */
#define BIO_SETTINGS 0x8F

#define BIO_SETTINGS_VISHAY 0b00000001 /*<-- Vishay provided settings for best performance */

#include <Arduino.h>
#include <Wire.h>

class VCNL4020C
{
public:
	/**
	 * VCNL4020C constructor
	 * @param i2c
	 * 		Pointer to I2C class
	 * @param addr
	 * 		Chip address
	 */
	VCNL4020C(TwoWire *i2c, int addr = VCNL4020C_ADDR);

	/**
	 * VCNL4020 destructor
	 */
	~VCNL4020C();

	/**
	 * Initialize the sensor with default values
	 * - LED current set to 100mA (middle of range)
	 * - Conversion mode set to single measurement mode
	 * - Interrupts disabled
	 * - Threshold disabled
	 * - Bio sensor data rate set to 125 measurements/s
	 * - Bio sensor modulation adjustment set to Vishay default
	 * - Ambient light sensor data set to 10 samples/s
	 * - Ambient light automatic offset compensation off
	 * - Ambient light averaging off
	 * @return result of initialization
	 * 		FALSE if sensor is not found or not responding, TRUE if success
	 */
	bool initSensorDefault(void);
	/**
	 * Read command register
	 * @param cmdVal
	 * 		Pointer to uint8_t variable with the content of the command register
	 * @return result of request
	 */
	bool getCmdReg(uint8_t *cmdVal);
	/** 
	 * Check command register if Ambient light sensor data is available
	 */
	bool alsDataReady(void);
	/** 
	 * Check command register if Bio sensor data is available
	 */
	bool bioDataReady(void);
	/**
	 * Start a single measurement
	 * @param bio
	 * 		Start single bio sensor measurement
	 * @param als
	 * 		Start single ambient light sensor measurement
	 * @return result of request
	 */
	bool startSingle(bool bio = true, bool als = false);
	/**
	 * Start continuous measurement
	 * @param bio
	 * 		Start continuous bio sensor measurement
	 * @param als
	 * 		Start continuous ambient light sensor measurement
	 * @return result of request
	 */
	bool startContinuous(bool bio = true, bool als = false);
	/**
	 * Stop continuous measurement
	 * @return result of request
	 */
	bool stopContinuous(void);
	/**
	 * Get product ID and revision ID. Writes the ID's into the 
	 * variables provided as parameters
	 * @param prodID
	 * 		Address of uint8_t variable
	 * @param revID
	 * 		Address of uint8_t variable
	 * @return result of request
	 */
	bool getIds(uint8_t *prodID, uint8_t *revID);
	/**
	 * Set bio sensor data rate
	 * @param dataRate
	 * 		Sensor data rate, allowed values
	 * 		BIO_SENS_RATE_1_95	1.95 measurements/s (DEFAULT)
	 * 		BIO_SENS_RATE_3_9	3.90625 measurements/s
	 * 		BIO_SENS_RATE_7_8	7.8125 measurements/s
	 * 		BIO_SENS_RATE_16_3	16.625 measurements/s
	 * 		BIO_SENS_RATE_31_3	31.25 measurements/s
	 * 		BIO_SENS_RATE_62_5	62.5 measurements/s
	 * 		BIO_SENS_RATE_125	125 measurements/s
	 * 		BIO_SENS_RATE_250	250 measurements/s
	 * @return result of request
	 */
	bool setBioDataRate(uint8_t dataRate);
	/**
	 * get current Bio data rate
	 * @param dataRate
	 * 		Pointer to uint8_t variable with the current data rate
	 * @return result of request
	 */
	bool getBioDataRate(uint8_t *cmdVal);
	/**
	 * Set LED current 
	 * @param ledCurrent
	 *		LED current, allowed value 0 to 20, LED current is value * 10mA
	 * @return result of request
	 */
	bool setLedCurrent(uint8_t ledCurrent);
	/**
	 * Read LED current setting
	 * @return LED current
	 */
	uint8_t getLedCurrent(void);
	/**
	 * Set Ambient light sensor parameters
	 * @param dataRate
	 * 		Sensor data rate, allowed values
	 * 		AMB_SENS_RATE_1		1 samples/s
	 * 		AMB_SENS_RATE_2		2 samples/s (DEFAULT)
	 * 		AMB_SENS_RATE_3		3 samples/s
	 * 		AMB_SENS_RATE_4		4 samples/s
	 * 		AMB_SENS_RATE_5		5 samples/s
	 * 		AMB_SENS_RATE_6		6 samples/s
	 * 		AMB_SENS_RATE_8		8 samples/s
	 * 		AMB_SENS_RATE_10	10 samples/s
	 * @param avgConv
	 * 		Set number of conversion used for averaging the result, allowed values
	 * 		AVG_CONV_1 		Average over 1 conversion
	 * 		AVG_CONV_2		Average over 2 conversions
	 * 		AVG_CONV_4		Average over 4 conversions
	 * 		AVG_CONV_8		Average over 8 conversions
	 * 		AVG_CONV_16		Average over 16 conversions
	 * 		AVG_CONV_32		Average over 32 conversions
	 * 		AVG_CONV_64		Average over 64 conversions
	 * 		AVG_CONV_128	Average over 128 conversions
	 * @param offsetComp
	 * 		Enable(TRUE) or disable(FALSE) automatic offset compensation
	 * @return result of request
	 */
	bool setAlsParam(uint8_t dataRate, uint8_t avgConv, bool offsetComp = true);
	/**
	 * Read Ambient light sensor parameters
	 * @param alsParam
	 * 		Pointer to uint8_t variable with the LED current setting
	 * @return result of request
	 */
	bool getAlsParam(uint8_t *alsParam);
	/**
	 * Get ambient light sensor result
	 * @return als value as 16 bit value or 0xFFFF if no data available
	 */
	uint16_t getAlsValue(void);
	/**
	 * Get bio sensor result
	 * @return bio value as 16 bit value or 0xFFFF if no data available
	 */
	uint16_t getBioValue(void);
	/**
	 * Set interrupt control register
	 * @param bioEna
	 * 		Enable (TRUE) or disable (FALSE) bio sensor data ready interrupt
	 * @param alsEna
	 * 		Enable (TRUE) or disable (FALSE) ambient light sensor data ready interrupt
	 * @param thresEna
	 * 		Enable (TRUE) or disable (FALSE) threshold exceed interrupt
	 * @param thresSel
	 * 		Select measurement method where threshold should be applied
	 * 		0 = apply to bio sensor measurements
	 * 		1 = apply to ambient light sensor measurements
	 * @param thresCount
	 * 		Set number of measurements needed above/below the threshold before interrupt is set
	 * 		Valid values:
	 * 		INT_CNT_EXC_1		1 count
	 * 		INT_CNT_EXC_2		2 counts
	 * 		INT_CNT_EXC_4		4 counts
	 * 		INT_CNT_EXC_8		8 counts
	 * 		INT_CNT_EXC_16		16 counts
	 * 		INT_CNT_EXC_32		32 counts
	 * 		INT_CNT_EXC_64		64 counts
	 * 		INT_CNT_EXC_128		128 counts
	 * @return result of request
	 */
	bool setIntControl(bool bioEna, bool alsEna, bool thresEna, uint8_t thresSel, uint8_t thresCount);
	/**
	 * Read interrupt control register
	 * @param intCntrl
	 * 		Pointer to uint8_t variable with the LED current setting
	 * @return result of request
	 */
	bool getIntControl(uint8_t *intCntrl);
	/**
	 * Set low threshold value
	 * @param threshold
	 * 		Threshold value
	 * @return result of request
	 */
	bool setThresholdLow(uint16_t threshold);
	/**
	 * Set high threshold value
	 * @param threshold
	 * 		Threshold value
	 * @return result of request
	 */
	bool setThresholdHigh(uint16_t threshold);
	/**
	 * Get threshold values
	 * @param thresholdHigh
	 * 		Pointer to uint16_t variable with the threshold high value
	 * @param thresholdLow
	 * 		Pointer to uint16_t variable with the threshold low value
	 * @return result of request
	 */
	bool getThresholds(uint16_t * thresholdHigh, uint16_t * thresholdLow);
	/**
	 * Check if any interrupt is set
	 * @param intStatus
	 * 		Address of uint8_t variable with interrupt status
	 * @return result of request
	 */
	bool checkInterrupts(uint8_t *intStatus);
	/**
	 * Check if bio sensor interrupt is set
	 * Calling this function clears the interrupt bit
	 * @return result TRUE if bio sensor interrupt is set or FALSE if no interrupt is set or request failed
	 */
	bool checkBioInt(void);
	/**
	 * Check if ambient light sensor interrupt is set
	 * Calling this function clears the interrupt bit
	 * @return result TRUE if ambient light sensor interrupt is set or FALSE if no interrupt is set or request failed
	 */
	bool checkAlsInt(void);
	/**
	 * Check if threshold low exceed interrupt is set
	 * Calling this function clears the interrupt bit
	 * @return result TRUE if threshold low exceed interrupt is set or FALSE if no interrupt is set or request failed
	 */
	bool checkThreshLowInt(void);
	/**
	 * Check if threshold high exceed interrupt is set
	 * Calling this function clears the interrupt bit
	 * @return result TRUE if threshold high interrupt is set or FALSE if no interrupt is set or request failed
	 */
	bool checkThreshHighInt(void);
	/**
	 * Set bio sensor modulation
	 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 * The settings for best performance will be provided by Vishay. 
	 * With first samples this is evaluated to:
	 * Delay time = 0; dead time = 1 and BS frequency = 00.
	 * With that register #15 should be programmed with 1 (= default value).
	 * !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	 * @param bioSensMod
	 * Bit 7 -- Bit 5 			Bit 4  -- Bit 3 		Bit 2 -- Bit 0
	 * Modulation delay time	Biosensor frequency		Modulation dead time
 	 * Modulation delay time R/W bits. 
	 *		Setting a delay time between LED signal and detectors input signal evaluation.
	 *		This function is for compensation of delays from LED and photo diode.
	 *		Also in respect to the possibility for setting different proximity signal frequency.
	 *		Correct adjustment is optimizing measurement signal level. (DEFAULT = 0)
	 * Biosensor frequency R/W bits. 
	 *		Setting the biosensor test signal frequency
	 *		The biosensor measurement is using a square signal as measurement signal.
	 *		Four different values are possible:
	 *		00 = 390.625 kHz (DEFAULT)
	 *		01 = 781.25 kHz
	 *		10 = 1.5625 MHz
	 *		11 = 3.125 MHz
	 * Modulation dead time R/W bits.
	 *		Setting a dead time in evaluation of LED signal at the slopes of the signal. (DEFAULT = 1)
	 *		This function is for reducing of possible disturbance effects.
	 *		This function is reducing signal level and should be used carefully.
	 * @return result of request
	 */
	bool setBioSensMod(uint8_t bioSensMod);
	/**
	 * Get current Biosensor modulation
	 * @param modSetting
	 * 		Pointer to uint8_t variable with the current bio sensor modulation setting
	 *  
	 * @return result of request
	 */
	bool getBioSensMod(uint8_t * modSetting);
	/**
	 * Set user interrupt callback function
	 * @param sensorInt
	 * 			Pointer to user interrupt function
	 * @param intPin
	 * 			GPIO connected to the sensors interrupt pin
	 */
	void setInterruptCb(void (*sensorInt)(), int intPin);

private:
	TwoWire *_i2c;
	int _addr = VCNL4020C_ADDR;

	uint8_t _cmdReg = 0b10000000;
	uint8_t _bioSensRate = BIO_SENS_RATE_125;
	uint8_t _ledCurrent = 10;
	uint8_t _alsParam = CONT_CONV_ENA || AMB_SENS_RATE_10;
	uint8_t _intCtrl = 0;
	uint16_t _lowThresh = 0;
	uint16_t _highThresh = 0;
	uint8_t _bioSensMod = 1;

	uint8_t regValue = 0;

	bool _intMeasurementBio = false;
	bool _intMeasurementALS = false;
	bool _intThreshold = false;
	
	bool readRegs(int reg_addr, uint8_t *data, int len);
	bool writeRegs(int reg_addr, uint8_t *data, int len);

	/**
	 * Interrupt callback routine
	 */
	void (*_sensorInt)() = NULL;

	int _intPin = -1;
};
#endif

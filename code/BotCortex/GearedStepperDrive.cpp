#include "Arduino.h"
#include "GearedStepperDrive.h"
#include "BotMemory.h"
#include "utilities.h"

// function called by AccelStepper when a forward step impulse happens.
// sends an impulse to the stepper driver board.
void forwardstep(void* obj) {
	GearedStepperDrive* driver = (GearedStepperDrive*)obj;
	driver->direction(true);
	driver->performStep();
}

// same but backwards
void backwardstep(void* obj) {
	GearedStepperDrive* driver = (GearedStepperDrive*)obj;
	driver->direction(false);
	driver->performStep();
}

void GearedStepperDrive::setup(	StepperConfig* pConfigData, ActuatorConfiguration* pActuatorConfig, StepperSetupData* pSetupData, RotaryEncoder* pEncoder) {
	movement.setNull();
	actuatorConfig = pActuatorConfig;
	configData = pConfigData;
	setupData = pSetupData;
	encoder =  pEncoder;

	if (memory.persMem.logSetup) {
		logger->print(F("   setup stepper"));
		logger->print(F(" pin(EN,DIR,CLK)=("));
		logPin(getPinEnable());
		logger->print(",");
		logPin(getPinDirection());
		logger->print(",");
		logPin(getPinClock());
		logger->print(") M=(");
		logger->print("),");
		logger->print(configData->microSteps);

		logger->print(F(") 1:"));
		logger->println(getGearReduction(),1);

		logger->print(F("   "));
		pSetupData->print();

	}
	pinMode(getPinClock(), OUTPUT);
	pinMode(getPinDirection(), OUTPUT);
	pinMode(getPinEnable(), OUTPUT);
	direction(!currentDirection); // force setting the PIN by setting the other direction than the current one

	// no movement currently
	movement.setNull();
	anglePerMicroStep =  getAnglePerMicroStep();
	frequency = sampleFrequency();
	stepsPerDegree = 1.0/setupData->degreePerStep;
	accel.setup(this, forwardstep, backwardstep);
	accel.setMaxSpeed(getMaxStepsPerSecond());
	accel.setAcceleration(getMaxStepAccPerSecond());

	if (memory.persMem.logSetup) {
		logger->print(F("   "));
		pConfigData->print();
		logger->print(F("   "));
	}

	logger->print(F("microsteps="));
	logger->print(getMicroSteps());

	logger->print(F(" getMaxAcc="));
	logger->print(getMaxAcc());

	logger->print(F(" getMaxRPM="));
	logger->print(getMaxRpm());

	logger->print(F(" maxAcc="));
	logger->print(getMaxStepAccPerSecond());
	logger->print(F(" maxSpeed="));
	logger->print(getMaxStepsPerSecond());
	logger->println();
}

void GearedStepperDrive::changeAngle(float pAngleChange,uint32_t pAngleTargetDuration) {
	// this methods works even when no current Angle has been measured
	uint32_t now = millis();
	movement.set(getCurrentAngle(), getCurrentAngle()+pAngleChange, now, pAngleTargetDuration);
}

void GearedStepperDrive::setAngle(float pAngle,uint32_t pAngleTargetDuration) {

	// setting an absolute angle is allowed only if the encoder has called setMeasuredAngle already
	// if we do not yet know the current angle, we cannot set the angle and return
	if (currentAngleAvailable) {

		// limit angle
		pAngle = constrain(pAngle, configData->minAngle,configData->maxAngle);

		// set actuator angle (which is not the motor angle)
		uint32_t now = millis();
		movement.set(movement.getCurrentAngle(now), pAngle, now, pAngleTargetDuration);
	}
}

void GearedStepperDrive::performStep() {
	uint8_t clockPIN = getPinClock();
	// This LOW to HIGH change is what creates the step
	digitalWrite(clockPIN, HIGH);
	delayMicroseconds(PIBOT_PULSE_WIDTH_US);
	digitalWrite(clockPIN, LOW);

	
	// adapt last motor position according the step, if motor is enabled
	if (enabled) { 
		bool direction = currentDirection;	// currently selected direction
		float diffAngle = anglePerMicroStep;
		if (direction != setupData->direction)
			diffAngle = -diffAngle;
		currentAngle += diffAngle;
	}
}


void GearedStepperDrive::direction(bool forward) {
	bool toBeDirection = forward;

	if ((toBeDirection != currentDirection))
	{
		if (!setupData->direction)
			forward=!forward;

		digitalWrite(getPinDirection(), forward?LOW:HIGH);
		currentDirection = toBeDirection;
	}
}

void GearedStepperDrive::enable() {
	enableDriver(true);
	integral = 0.0;
}

bool GearedStepperDrive::isEnabled() {
	return enabled;
}

void GearedStepperDrive::disable() {
	enableDriver(false);
}

void GearedStepperDrive::enableDriver(bool ok) {
	enabled = ok; // do this first to switch off the stepper loop, otherwise ticks happens

	// set the clock to low to avoid switch-on-tick due to low/high impulse
	digitalWrite(getPinClock(), LOW);
	delayMicroseconds(100);
	digitalWrite(getPinEnable(), ok?HIGH:LOW);
}

// called very often to execute one stepper step. Dont do complex operations here.
// Parameter is ignored, no need for time here
void GearedStepperDrive::loop(uint32_t) {
	loop();
}

// called very often to execute one stepper step. Dont do complex operations here.
void GearedStepperDrive::loop() {
	if (accel.runSpeed())
		accel.computeNewSpeed();
}

// returns angle that has been measured lately
float GearedStepperDrive::getCurrentAngle() {
	return currentAngle;
}

void GearedStepperDrive::setCurrentAngle(float angle) {
	currentAngle = angle;
}

void GearedStepperDrive::setMeasuredAngle(float pMeasuredActuatorAngle, uint32_t now) { 
	currentAngle = pMeasuredActuatorAngle;
	if (!currentAngleAvailable) {
		lastToBeAngle = pMeasuredActuatorAngle;
		currentAngleAvailable = true;
	}

	if (!movement.isNull()) {
		// compute steps resulting from trajectorys speed and the
		// error when comparing the to-be position with the measured position
		float dT = sampleTime();

		float toBeAngle = 			movement.getCurrentAngle(now);
		float nextToBeAngle = 		movement.getCurrentAngle(now+dT);

		float anglePerSample = 		toBeAngle-lastToBeAngle;
		float nextAnglePerSample = 	nextToBeAngle - toBeAngle;

		float currStepsPerSample = getMicroStepsByAngle(anglePerSample);
		float nextStepsPerSample = getMicroStepsByAngle(nextAnglePerSample);
		float stepErrorPerSample = getMicroStepsByAngle(toBeAngle  - currentAngle);		// current error, i.e. to-be-angle compared with encoder's angle

		// the step error is going through a PI-controller and added to the to-be speed (=stepsPerSample)
		float maxAcc = getMaxStepAccPerSecond();

		float Pout = configData->kP * stepErrorPerSample;
		integral += stepErrorPerSample * dT;
		float Iout = configData->kI * integral;
		float PIDoutput = Pout + Iout;
		float accelerationPerSample = PIDoutput;

		float distanceToNextSample = accelerationPerSample + currStepsPerSample;

		// compute the acceleration used in this sample
		float sampleAcc = (currStepsPerSample-nextStepsPerSample + stepErrorPerSample)*frequency;

		// arbitrary definition: deceleration is as double as high as acceleration
		if (((distanceToNextSample > 0) && (sampleAcc > 0)) ||
			((distanceToNextSample < 0) && (sampleAcc < 0))) {
			sampleAcc = constrain(sampleAcc*2, -maxAcc, maxAcc);
		} else {
			sampleAcc = constrain(sampleAcc, -maxAcc, maxAcc);
		}

		accel.setAcceleration(fabs(sampleAcc));
		accel.move(distanceToNextSample);

		/*
		if ((configData->id == 4) && memory.persMem.logStepper) {
			logger->print("t=");
			logger->print(millis());
			logger->print(" a=");
			logger->print(toBeAngle);
			logger->print(" last=");
			logger->print(lastToBeAngle);
			logger->print(" curr=");
			logger->print(currentAngle);
			logger->print(" rpm=");
			logger->print(getRPMByAnglePerSample(anglePerSample));
			logger->print(" ms=");
			logger->println(configData->microSteps);
			logger->print(" serror=");
			logger->print(stepErrorPerSample);
			logger->print(" o=");
			logger->print(PIDoutput);
			logger->print(" acc=");
			logger->print(sampleAcc);
			logger->print(" av=");
			logger->println(accel.speed());
		}
		*/
		lastToBeAngle = toBeAngle;
	}
}





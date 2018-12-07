/******************************************************************************
	Description:	Implements the CCANPosition control class.

	Classes:		CCANPosition

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
******************************************************************************/
#include "CANPosition.h"
#include "WPILib.h"
///////////////////////////////////////////////////////////////////////////////


/******************************************************************************
	Description:	CCANPosition Constructor.

	Arguments:		int nDeviceID - CAN Bus Device ID

	Derived From:	Nothing
******************************************************************************/
CCANPosition::CCANPosition(int nDeviceID)
{
	m_nDeviceID = nDeviceID;
	// Create the object pointers.
	m_pMotor						= new WPI_TalonSRX(nDeviceID);
	m_pTimer						= new Timer();

	// Initialize member variables.
	m_nCurrentState					= eIdle;
	m_bReady						= true;
	m_bFwdLimitSwitchNormallyOpen	= true;
	m_bRevLimitSwitchNormallyOpen	= true;
	m_bHomingComplete				= false;
	m_dSetpoint						= 0.000;
	m_nPulsesPerRev					= nDefaultCANPositionPulsesPerRev;
	m_dRevsPerUnit					= dDefaultCANPositionRevsPerUnit;
	m_dFwdHomeSpeed					= dDefaultCANPositionFwdHomeSpeed;
	m_dRevHomeSpeed					= dDefaultCANPositionRevHomeSpeed;
	m_dTolerance					= dDefaultCANPositionTolerance;
	m_dLowerSoftLimit				= dDefaultCANPositionLowerSoftLimit;
	m_dUpperSoftLimit				= dDefaultCANPositionUpperSoftLimit;
	m_dIZone						= dDefaultCANPositionIZone;
	m_dMaxHomingTime				= dDefaultCANPositionMaxHomingTime;
	m_dMaxFindingTime				= dDefaultCANPositionMaxFindingTime;
	m_dHomingStartTime				= 0.000;
	m_dFindingStartTime				= 0.000;

	// Set up the feedback device for a quadrature encoder.
	m_pMotor->ConfigSelectedFeedbackSensor(FeedbackDevice::QuadEncoder, nCANPositionPIDLoopIdx, nCANPositionTimeoutMs);
	// Reset the encoder count to zero.
	ResetEncoderPosition();
	// Set the encoder and motor as both positive.
	SetMotorInverted(false);
	SetSensorInverted(false);
	// Set up the nominal motor output for both directions.
	SetNominalOutputVoltage(0.000, 0.000);
	// Set the peak (maximum) motor output for both directions.
	SetPeakOutputPercent(1.000, -1.000);
	// Set the tolerance.
	SetTolerance(m_dTolerance);
	// Set the PID and feed forward values.
	SetPIDValues(dDefaultCANPositionProportional, dDefaultCANPositionIntegral, dDefaultCANPositionDerivative, dDefaultCANPositionFeedForward);
	// Stop the motor.
	Stop();
	// Set the neutral mode to brake.
	m_pMotor->SetNeutralMode(NeutralMode::Brake);
	// Disable both forward and reverse limit switches.
	m_pMotor->ConfigForwardLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, LimitSwitchNormal::LimitSwitchNormal_Disabled, nCANPositionTimeoutMs);
	m_pMotor->ConfigReverseLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, LimitSwitchNormal::LimitSwitchNormal_Disabled, nCANPositionTimeoutMs);
	// Set acceleration (seconds from neutral to full output).
	SetOpenLoopRampRate(dDefaultCANPositionVoltageRampRate);
	SetClosedLoopRampRate(dDefaultCANPositionVoltageRampRate);
	// Clear the sticky faults in memory.
	ClearStickyFaults();
	// Set the Integral Zone. Accumulated integral is reset to zero when the error exceeds this value.
	SetAccumIZone(m_dIZone);
	// Clear the sticky faults in memory.
	ClearStickyFaults();

	// Start the timer.
	m_pTimer->Start();
}

/******************************************************************************
	Description:	CCANPosition Destructor.

	Arguments:		None

	Derived From:	Nothing
******************************************************************************/
CCANPosition::~CCANPosition()
{
	// Delete our object pointers.
	delete	m_pMotor;
	delete	m_pTimer;

	// Set the objects to NULL.
	m_pMotor	= NULL;
	m_pTimer	= NULL;
}

/******************************************************************************
	Description:	Tick - main method that does functionality.
					Called each time through robot main loop to update state.

	Arguments:	 	None

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::Tick()
{
	// State machine
	switch(m_nCurrentState)
	{
		case eIdle :
			// Stop the motor.
			m_pMotor->Set(ControlMode::PercentOutput, 0.000);
			m_bReady = true;
			break;

		case eHomingReverse :
			// If the state is eHomingReverse, the motor will move toward
			// the home switch, and then turn off and go to eHomingForward.
			m_bReady = false;

			// Check to see if the home limit is pressed or if we have exceeded the maximum homing time.
			if ((IsRevLimitSwitchPressed()) ||
				((m_dMaxHomingTime > 0.000) && (m_pTimer->Get() > (m_dHomingStartTime + m_dMaxHomingTime))))
			{
				// At the home limit switch, turn off the motor.
				m_pMotor->Set(ControlMode::PercentOutput, 0.00);
				// Set the state to eHomingForward.
				m_nCurrentState = eHomingForward;
			}
			else
			{
				// Not yet at the home limit switch, keep moving.
				m_pMotor->Set(ControlMode::PercentOutput, m_dRevHomeSpeed);
			}
			break;

		case eHomingForward :
			// If the state is eHomingForward, the motor will slowly
			// move (forward) off the limit switch. Once the switch releases,
			// the motor will stop and the encoder will be reset.
			m_bReady = false;

			// Check to see we are off the home limit switch or the homing timeout has been reached.
			if (!IsRevLimitSwitchPressed())
			{
				// Stop the motor and change the control mode to position.
				m_pMotor->Set(ControlMode::Position, 0.000);
				// Reset the encoder to zero.
				m_pMotor->SetSelectedSensorPosition(0, nCANPositionPIDLoopIdx, nCANPositionTimeoutMs);
				// Set flag that homing is complete.
				m_bHomingComplete = true;
				// Set the state to eIdle.
				m_nCurrentState = eIdle;
			}
			else
			{
				// Still on the home limit switch, keep moving.
				m_pMotor->Set(ControlMode::PercentOutput, m_dFwdHomeSpeed);
			}
			break;

		case eFinding :
			// If the state is eFinding, the motor will continue until
			// the PID reaches the target or until the limit switch in
			// the direction of travel is pressed. The state then becomes idle.
			m_bReady = false;
			// Check to see if position is within tolerance or limit switch
			// is activated in direction of travel.
			if (IsAtSetpoint() ||
			   (((GetSetpoint() > GetActual()) && IsFwdLimitSwitchPressed()) ||
			    ((GetSetpoint() < GetActual()) && IsRevLimitSwitchPressed()) ||
				((m_dMaxFindingTime > 0.000) && (m_pTimer->Get() > (m_dFindingStartTime + m_dMaxFindingTime)))))
			{
				// Stop the motor and set the current state to eIdle.
				Stop();
			}
			break;

		case eManualForward :
			if (!IsFwdLimitSwitchPressed())
			{
				// Manually raise the arm.
				m_pMotor->Set(ControlMode::PercentOutput, 0.500);
				m_bReady = false;
			}
			else
			{
				// Change the arm lift state to eIdle.
				SetState(eIdle);
				m_bReady = true;
			}
			break;

		case eManualReverse :
			if (!IsRevLimitSwitchPressed())
			{
				// Manually lower the arm.
				m_pMotor->Set(ControlMode::PercentOutput, -0.500);
				m_bReady = false;
			}
			else
			{
				// Change the arm lift state to eIdle.
				SetState(eIdle);
				m_bReady = true;
			}
			break;

		default :
			break;
	}
}

/******************************************************************************
	Description:	SetSetpoint - Sets the position for the motor.

	Arguments:	 	dPosition - The position to move to in desired units.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetSetpoint(double dPosition)
{
	// Clamp the new setpoint within soft limits.
	if (dPosition > m_dUpperSoftLimit)
	{
		dPosition = m_dUpperSoftLimit;
	}
	else
	{
		if (dPosition < m_dLowerSoftLimit)
		{
			dPosition = m_dLowerSoftLimit;
		}
	}

	// Set the setpoint member variable.
	m_dSetpoint = dPosition;

	// Get current time.
	m_dFindingStartTime = m_pTimer->Get();

	// Set the motor to the desired position.
	m_pMotor->Set(ControlMode::Position, (dPosition * m_dRevsPerUnit * m_nPulsesPerRev));

	printf("CCANPosition::SetSetpoint - Setpoint = %7.3f\n", dPosition);
	printf("CCANPosition::SetSetpoint - Revs Per Unit = %7.3f\n", m_dRevsPerUnit);

	// Set the state to eFinding.
	m_nCurrentState = eFinding;
}

/******************************************************************************
	Description:	GetSetpoint - Returns the current setpoint of the motor's
					PID in desired units of measure.

	Arguments:	 	None

	Returns: 		The setpoint of the motor's PID in desired units of measure.
******************************************************************************/
double CCANPosition::GetSetpoint()
{
	return m_dSetpoint;
}

/******************************************************************************
	Description:	StartHoming - Initializes the homing sequence.

	Arguments:	 	None

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::StartHoming()
{
	// Stop the motor and set the control mode for percent output.
	m_pMotor->Set(ControlMode::PercentOutput, 0.000);

	// Get the homing start time.
	m_dHomingStartTime = m_pTimer->Get();

	// Set flag that homing is not complete.
	m_bHomingComplete = false;

	// Set the current state to eHomingReverse.
	m_nCurrentState = eHomingReverse;
}

/******************************************************************************
	Description:	Stop - Stop the motor.

	Arguments:	 	None

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::Stop()
{
	// Stop the motor.
	m_pMotor->Set(ControlMode::PercentOutput, 0.000);

	// Set the current state to eIdle.
	m_nCurrentState = eIdle;
}

/******************************************************************************
	Description:	SetTolerance - Sets the tolerance of the PID in desired
					units of measure.

	Arguments:	 	dValue - Tolerance in the desired units.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetTolerance(double dValue)
{
	// Set the member variable.
	m_dTolerance = dValue;

	// Set the allowed error for the PID. This is in quadrature pulses.
	m_pMotor->ConfigAllowableClosedloopError(nCANPositionSlotIdx, (m_dTolerance * m_dRevsPerUnit * m_nPulsesPerRev), nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	GetTolerance - Returns the tolerance in the desired units.

	Arguments:	 	None

	Returns: 		dValue - Tolerance in the desired units.
******************************************************************************/
double CCANPosition::GetTolerance()
{
	return m_dTolerance;
}

/******************************************************************************
	Description:	SetSoftLimits - Sets soft limits for minimum and maximum travel.

	Arguments:	 	dMinValue - Minimum travel distance.
					dMaxValue - Maximum travel distance.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetSoftLimits(double dMinValue, double dMaxValue)
{
	// Set the member variables.
	m_dLowerSoftLimit	= dMinValue;
	m_dUpperSoftLimit	= dMaxValue;
}

/******************************************************************************
	Description:	ConfigLimitSwitches - Sets up the limit switches as
					normally open or normally closed.

	Arguments:	 	bool bFwdLimit - True if normally open, false if normally closed.
					bool bRevLimit - True if normally open, false if normally closed.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::ConfigLimitSwitches(bool bFwdLimit, bool bRevLimit)
{
	// Set the member variables.
	m_bFwdLimitSwitchNormallyOpen = bFwdLimit;
	m_bRevLimitSwitchNormallyOpen = bRevLimit;

	m_pMotor->ConfigForwardLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, (bFwdLimit ? LimitSwitchNormal::LimitSwitchNormal_NormallyOpen : LimitSwitchNormal::LimitSwitchNormal_NormallyClosed), nCANPositionTimeoutMs);
	m_pMotor->ConfigReverseLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, (bRevLimit ? LimitSwitchNormal::LimitSwitchNormal_NormallyOpen : LimitSwitchNormal::LimitSwitchNormal_NormallyClosed), nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetAccumIZone - sets the IZone for the accumulated integral.

	Arguments:	 	double dIZone - The accumulated integral is reset to zero
					when the error exceeds this value. This value is in the
					units of measure.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetAccumIZone(double dIZone)
{
	// Set the member variable.
	m_dIZone = dIZone;

	// Set the Integral Zone. Accumulated integral is reset to zero when the error exceeds this value.
	m_pMotor->Config_IntegralZone(nCANPositionSlotIdx, (m_dIZone * m_dRevsPerUnit * m_nPulsesPerRev), nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	IsFwdLimitSwitchPressed - Returns true if forward limit
					switch is pressed, false otherwise.

	Arguments:	 	None

	Returns: 		bool - True if pressed, false otherwise
******************************************************************************/
bool CCANPosition::IsFwdLimitSwitchPressed()
{
	// Must first read the sensor collection to access the limit switch state.
	SensorCollection Collection = m_pMotor->GetSensorCollection();

	return ((m_bFwdLimitSwitchNormallyOpen && Collection.IsFwdLimitSwitchClosed()) ||
		   (!m_bFwdLimitSwitchNormallyOpen && !Collection.IsFwdLimitSwitchClosed()));
}

/******************************************************************************
	Description:	IsRevLimitSwitchPressed - Returns true if reverse limit
					switch is pressed, false otherwise.

	Arguments:	 	None

	Returns: 		bool - True if pressed, false otherwise
******************************************************************************/
bool CCANPosition::IsRevLimitSwitchPressed()
{
	// Must first read the sensor collection to access the limit switch state.
	SensorCollection Collection = m_pMotor->GetSensorCollection();

	return ((m_bRevLimitSwitchNormallyOpen && Collection.IsRevLimitSwitchClosed()) ||
		   (!m_bRevLimitSwitchNormallyOpen && !Collection.IsRevLimitSwitchClosed()));
}

/******************************************************************************
	Description:	IsAtSetpoint - Returns whether or not the motor has reached
					the desired setpoint.

	Arguments:	 	None

	Returns: 		bool - True if at setpoint, false otherwise.
******************************************************************************/
bool CCANPosition::IsAtSetpoint()
{
	return (((fabs(GetSetpoint() - GetActual())) < m_dTolerance) && (fabs(m_pMotor->GetMotorOutputVoltage()) < 1.000));
}

/******************************************************************************
	Description:	ResetEncoderPosition - Sets the encoder position to zero.

	Arguments:	 	None

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::ResetEncoderPosition()
{
	// Reset the encoder count to zero.
	m_pMotor->SetSelectedSensorPosition(0, nCANPositionPIDLoopIdx, nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetPeakOutputPercent - Sets the maximum output for the
					motors. This is in PercentOutput (-1, to 1).

	Arguments:	 	double dMaxFwdOutput - The maximum forward output.
					double dMaxRevOutput - The maximum reverse output.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetPeakOutputPercent(double dMaxFwdOutput, double dMaxRevOutput)
{
	m_pMotor->ConfigPeakOutputForward(dMaxFwdOutput, nCANPositionTimeoutMs);
	m_pMotor->ConfigPeakOutputReverse(dMaxRevOutput, nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetNominalOutputVoltage - Sets the nominal output for the
					motors. This is in PercentOutput (-1, to 1).

	Arguments:	 	double dNominalFwdOutput - The nominal forward output.
					double dNominalRevOutput - The nominal reverse output.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetNominalOutputVoltage(double dNominalFwdOutput, double dNominalRevOutput)
{
	m_pMotor->ConfigNominalOutputForward(dNominalFwdOutput, nCANPositionTimeoutMs);
	m_pMotor->ConfigNominalOutputReverse(dNominalRevOutput, nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetOpenLoopRampRate - Sets the acceleration for open loop.

	Arguments:	 	double dOpenLoopRampRate - Acceleration in seconds from
					off to full output.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetOpenLoopRampRate(double dOpenLoopRampRate)
{
	m_pMotor->ConfigOpenloopRamp(dOpenLoopRampRate, nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetClosedLoopRampRate - Sets the acceleration for closed loop.

	Arguments:	 	double dClosedLoopRampRate - Acceleration in seconds from
					off to full output.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetClosedLoopRampRate(double dClosedLoopRampRate)
{
	m_pMotor->ConfigClosedloopRamp(dClosedLoopRampRate, nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetMotorNeutralMode - Sets the stop mode to brake or coast.

	Arguments:	 	int nMode - Mode, 1 is coast, 2 is brake.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetMotorNeutralMode(int nMode)
{
	m_pMotor->SetNeutralMode((nMode == 1) ? NeutralMode::Coast : NeutralMode::Brake);
}

/******************************************************************************
	Description:	GetCurrentPositionInUnits - Returns the current position in units.

	Arguments:	 	None

	Returns: 		double - Position of the motor.
******************************************************************************/
double CCANPosition::GetActual()
{
	return (m_pMotor->GetSelectedSensorPosition(nCANPositionPIDLoopIdx) / m_dRevsPerUnit / m_nPulsesPerRev);
}

/******************************************************************************
	Description:	SetHomeSpeeds - Sets the home speeds for the state machine.

	Arguments:	 	double dFwdSpeed - Speed for homing forward, coming off of
					home switch.
					double dRevSpeed - Speed for homing backward, moving towards
					home switch.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetHomeSpeeds(double dFwdSpeed, double dRevSpeed)
{
	m_dFwdHomeSpeed = dFwdSpeed;
	m_dRevHomeSpeed = dRevSpeed;
}

/******************************************************************************
	Description:	SetPulsesPerRev - Sets the pulses per revolution for the PID
					controller.

	Arguments:	 	int nPPR - Encoder pulses per revolution.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetPulsesPerRev(int nPPR)
{
	m_nPulsesPerRev = nPPR;
}

/******************************************************************************
	Description:	SetRevsPerUnit - Sets the revolutions per unit of measure.

	Arguments:	 	double dRPU - Revolutions per unit of measure.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetRevsPerUnit(double dRPU)
{
	m_dRevsPerUnit = dRPU;
}

/******************************************************************************
	Description:	SetPIDValues - Sets the PID and Feed Forward gain values.

	Arguments:	 	double dProportional 	- Proportion Gain
					double dIntegral		- Integral Gain
					double dDerivative		- Derivative Gain
					double dFeedForward		- Feed Forward Gain

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetPIDValues(double dProportional, double dIntegral, double dDerivative, double dFeedForward)
{
	m_pMotor->Config_kP(nCANPositionPIDLoopIdx, dProportional, 	nCANPositionTimeoutMs);
	m_pMotor->Config_kI(nCANPositionPIDLoopIdx, dIntegral, 		nCANPositionTimeoutMs);
	m_pMotor->Config_kD(nCANPositionPIDLoopIdx, dDerivative, 	nCANPositionTimeoutMs);
	m_pMotor->Config_kF(nCANPositionPIDLoopIdx, dFeedForward, 	nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetMotorInverted - Inverts the motor output.

	Arguments:	 	bool bInverted - True to invert motor output.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetMotorInverted(bool bInverted)
{
	m_pMotor->SetInverted(bInverted);
}

/******************************************************************************
	Description:	SetSensorInverted - Inverts the sensor input.

	Arguments:	 	bool bInverted - True to invert sensor input.

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::SetSensorInverted(bool bInverted)
{
	m_pMotor->SetSensorPhase(bInverted);
}

/******************************************************************************
	Description:	ClearStickyFaults - Clears the controller's sticky faults.

	Arguments:	 	None

	Returns: 		Nothing
******************************************************************************/
void CCANPosition::ClearStickyFaults()
{
	m_pMotor->ClearStickyFaults(nCANPositionTimeoutMs);
}
///////////////////////////////////////////////////////////////////////////////

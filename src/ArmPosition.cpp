/****************************************************************************
	Description:	Implements the CArmPosition control class.

	Classes:		CArmPosition

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#include "WPILib.h"
#include "ArmPosition.h"
#include "IOMap.h"
#include "CANPosition.h"
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CArmPosition Constructor.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
CArmPosition::CArmPosition()
{
	// Create the object pointers.
	m_pArmMotor 	= new CCANPosition(nArmPositionMotor);
	m_pArmBrake		= new Solenoid(nArmBrakeSolenoid);
}

/****************************************************************************
	Description:	CArmPosition Destructor.

	Arguments:		None

	Derived From:	None
****************************************************************************/
CArmPosition::~CArmPosition()
{
	// Delete our object pointers.
	delete	m_pArmMotor;
	delete	m_pArmBrake;

	// Set object pointers to NULL.
	m_pArmMotor		= NULL;
	m_pArmBrake		= NULL;
}

/****************************************************************************
	Description:	Initialize Arm Position parameters.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CArmPosition::Init()
{
	// Set the pulses per rev.
	m_pArmMotor->SetPulsesPerRev(1024);
	// Set the Revs/Unit.
	m_pArmMotor->SetRevsPerUnit(dArmPositionEncoderRevsPerDegree);
	// Invert the motor output.
	m_pArmMotor->SetMotorInverted(true);
	// Set the encoder's sensor phase.
	m_pArmMotor->SetSensorInverted(true);
	// Set the PID Terms.
	m_pArmMotor->SetPIDValues(dArmPositionProportional, dArmPositionIntegral, dArmPositionDerivative);
	// Set the I Zone.
	m_pArmMotor->SetAccumIZone(dArmPositionAccumIZone);
	// Set the soft limits.
	m_pArmMotor->SetSoftLimits(dArmPositionMinDistance, dArmPositionMaxDistance);
	// Set the position tolerance.
	m_pArmMotor->SetTolerance(dArmPositionTolerance);
	// Set the maximum forward and reverse percent output.
	m_pArmMotor->SetPeakOutputPercent(dArmPositionMaxSpeed, dArmPositionMinSpeed);
	// Set the nominal forward and reverse percent output.
	m_pArmMotor->SetNominalOutputVoltage(0.000, 0.000);
	// Set the open loop ramp rate.
	m_pArmMotor->SetOpenLoopRampRate(dArmPositionOpenLoopRampRate);
	// Set the closed loop ramp rate.
	m_pArmMotor->SetClosedLoopRampRate(dArmPositionClosedLoopRampRate);
	// Set motor neutral mode to brake.
	m_pArmMotor->SetMotorNeutralMode(2);
	// Set limit switches to Normally Closed
	m_pArmMotor->ConfigLimitSwitches(false, false);
	// Set the homing speeds in Percent Output.
	m_pArmMotor->SetHomeSpeeds(dArmPositionHomingSpeedForward, dArmPositionHomingSpeedReverse);
	// Set the homing timeout.
	m_pArmMotor->SetMaxHomingTime(dArmPositionMaxHomingTimeout);
	// Set the finding timeout.
	m_pArmMotor->SetMaxFindingTime(dArmPositionMaxFindingTimeout);
	// Stop the motor.
	m_pArmMotor->Stop();
	// Reset encoder counts to zero.
	m_pArmMotor->ResetEncoderPosition();
	// Clear any sticky faults.
	m_pArmMotor->ClearStickyFaults();

	// Engage brake.
	m_pArmBrake->Set(false);
}

/****************************************************************************
	Description:	Tick - main method that does functionality.
					Called each time through robot main loop to update state.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CArmPosition::Tick()
{
	// Call the CANPosition ticks.
	m_pArmMotor->Tick();

	// Get ArmPosition State.
	State nArmState = m_pArmMotor->GetState();

	// Engage/Disengage brake based on ArmPosition state.
	if ((nArmState == eHomingReverse) ||
		(nArmState == eHomingForward) ||
		(nArmState == eManualForward) ||
		(nArmState == eManualReverse) ||
		(nArmState == eFinding))
	{
		// Disengage brake.
		m_pArmBrake->Set(true);
	}
	else
	{
		// Engage brake.
		m_pArmBrake->Set(false);
	}

	// Check to see if the motor has reached setpoint.
	if ((nArmState == eFinding) && m_pArmMotor->IsReady())
	{
		// Stop the motor.
		m_pArmMotor->Stop();
	}

	// Put arm information on SmartDashboard for Auto/Teleop.
	SmartDashboard::PutBoolean("Arm IsReady", 	m_pArmMotor->IsReady());
	SmartDashboard::PutNumber("Arm Setpoint", 	m_pArmMotor->GetSetpoint());
	SmartDashboard::PutNumber("Arm Actual", 	m_pArmMotor->GetActual());
}

/****************************************************************************
	Description:	SetSetpoint - Sets the desired positional setpoint for
					the arm position motor.

	Arguments: 		double dSetpoint - Setpoint in degree for motor.

	Returns: 		Nothing
****************************************************************************/
void CArmPosition::SetSetpoint(double dSetpoint)
{
	m_pArmMotor->SetSetpoint(dSetpoint);
}

/****************************************************************************
	Description:	JogArm - Moves the arm up or down.

	Arguments: 		bool bArmUp - Chooses whether arm goes up or down.

	Returns: 		Nothing
****************************************************************************/
void CArmPosition::JogArm(bool bArmUp)
{
	if (m_pArmMotor->GetState() == eIdle)
	{
		// Get the current arm position.
		double dArmPosition = GetActual();
		if (bArmUp)
		{
			// If the current position is greater than or equal to 90° then up is in the reverse direction.
			m_pArmMotor->SetState((dArmPosition >= 90.000) ? eManualReverse : eManualForward);
		}
		else
		{
			// If the current position is greater than or equal to 90° then up is in the reverse direction.
			m_pArmMotor->SetState((dArmPosition >= 90.000) ? eManualForward : eManualReverse);
		}
	}
}

/****************************************************************************
	Description:	StopArm - Stops the arm motor.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CArmPosition::StopArm()
{
	m_pArmMotor->SetState(eIdle);
}

///////////////////////////////////////////////////////////////////////////////


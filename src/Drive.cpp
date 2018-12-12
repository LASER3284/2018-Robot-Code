/****************************************************************************
	Description:	Implements the CDrive control class.

	Classes:		CDrive

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#include "WPILib.h"
#include "Drive.h"
#include "IOMap.h"
#include "CANPosition.h"
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CDrive Constructor.

	Arguments:		Joystick* pDriveController

	Derived From:	Nothing
****************************************************************************/
CDrive::CDrive(Joystick* pDriveController)
{
	// Store a copy of the drive controller.
	m_pDriveController					= pDriveController;

	// Create the object pointers.
	m_pLeftDrive 						= new CCANPosition(nLeftDriveMotor1);
	m_pRightDrive						= new CCANPosition(nRightDriveMotor1);
	m_pLeftDriveMotor1					= m_pLeftDrive->GetMotorPointer();			// Do not instantiate this motor, it is done in the CCanPosition class.
	m_pLeftDriveMotor2					= new WPI_TalonSRX(nLeftDriveMotor2);
	m_pLeftDriveMotor3					= new WPI_TalonSRX(nLeftDriveMotor3);
	m_pRightDriveMotor1					= m_pRightDrive->GetMotorPointer();			// Do not instantiate this motor, it is done in the CCanPosition class.
	m_pRightDriveMotor2					= new WPI_TalonSRX(nRightDriveMotor2);
	m_pRightDriveMotor3					= new WPI_TalonSRX(nRightDriveMotor3);
	m_pShiftSolenoid					= new Solenoid(nDriveShiftSolenoid);
	m_pRobotDrive						= new DifferentialDrive(*m_pLeftDriveMotor1, *m_pRightDriveMotor1);

	// Initialize member variables.
	m_dYAxis							= 0.000;
	m_dXAxis							= 0.000;
	m_bJoystickControl					= false;
	m_bDriveControllerLeftAnalogPressed	= false;
}

/****************************************************************************
	Description:	CDrive Destructor.

	Arguments:		None

	Derived From:	None
****************************************************************************/
CDrive::~CDrive()
{
	// Delete our object pointers.
	delete	m_pLeftDrive;
	delete	m_pRightDrive;
	delete	m_pLeftDriveMotor2;
	delete	m_pLeftDriveMotor3;
	delete	m_pRightDriveMotor2;
	delete	m_pRightDriveMotor3;
	delete	m_pShiftSolenoid;
	delete	m_pRobotDrive;

	// Set object pointers to NULL.
	m_pLeftDrive		= NULL;
	m_pRightDrive		= NULL;
	m_pLeftDriveMotor1	= NULL;
	m_pLeftDriveMotor2	= NULL;
	m_pLeftDriveMotor3	= NULL;
	m_pRightDriveMotor1	= NULL;
	m_pRightDriveMotor2	= NULL;
	m_pRightDriveMotor3	= NULL;
	m_pShiftSolenoid	= NULL;
	m_pRobotDrive		= NULL;
}

/****************************************************************************
	Description:	Initialize drive parameters.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CDrive::Init()
{
	// Set the pulses per rev.
	m_pLeftDrive->SetPulsesPerRev(1024);
	m_pRightDrive->SetPulsesPerRev(1024);
	// Set the Revs/Unit for 6 inch drive wheels.
	m_pLeftDrive->SetRevsPerUnit(dDriveEncoderRevsPerInch);
	m_pRightDrive->SetRevsPerUnit(dDriveEncoderRevsPerInch);
	// Invert the motor outputs.
	m_pLeftDrive->SetMotorInverted(true);
	m_pLeftDriveMotor2->SetInverted(false);
	m_pLeftDriveMotor3->SetInverted(true);
	m_pRightDrive->SetMotorInverted(true);
	m_pRightDriveMotor2->SetInverted(false);
	m_pRightDriveMotor3->SetInverted(true);
	// Set the second and third motors on each drive to follow the first motor.
	m_pLeftDriveMotor2->Follow(*m_pLeftDriveMotor1);
	m_pLeftDriveMotor3->Follow(*m_pLeftDriveMotor1);
	m_pRightDriveMotor2->Follow(*m_pRightDriveMotor1);
	m_pRightDriveMotor3->Follow(*m_pRightDriveMotor1);
	// Set the encoder's sensor phase.
	m_pLeftDrive->SetSensorInverted(true);
	m_pRightDrive->SetSensorInverted(true);
	// Set the PID Terms.
	m_pLeftDrive->SetPIDValues(dDriveProportional, dDriveIntegral, dDriveDerivative);
	m_pRightDrive->SetPIDValues(dDriveProportional, dDriveIntegral, dDriveDerivative);
	// Set the I Zone.
	m_pLeftDrive->SetAccumIZone(dDriveAccumIZone);
	m_pRightDrive->SetAccumIZone(dDriveAccumIZone);
	// Set the soft limits.
	m_pLeftDrive->SetSoftLimits(dDriveMinDistance, dDriveMaxDistance);
	m_pRightDrive->SetSoftLimits(dDriveMinDistance, dDriveMaxDistance);
	// Set the position tolerance.
	m_pLeftDrive->SetTolerance(dDriveTolerance);
	m_pRightDrive->SetTolerance(dDriveTolerance);
	// Set the maximum forward and reverse percent output.
	m_pLeftDrive->SetPeakOutputPercent(dDriveMaxSpeed, dDriveMinSpeed);
	m_pRightDrive->SetPeakOutputPercent(dDriveMaxSpeed, dDriveMinSpeed);
	// Set the nominal forward and reverse percent output.
	m_pLeftDrive->SetNominalOutputVoltage(0.000, 0.000);
	m_pRightDrive->SetNominalOutputVoltage(0.000, 0.000);
	// Set the open loop ramp rate.
	m_pLeftDrive->SetOpenLoopRampRate(dDriveOpenLoopRampRate);
	m_pRightDrive->SetOpenLoopRampRate(dDriveOpenLoopRampRate);
	// Set the closed loop ramp rate.
	m_pLeftDrive->SetClosedLoopRampRate(dDriveClosedLoopRampRate);
	m_pRightDrive->SetClosedLoopRampRate(dDriveClosedLoopRampRate);
	// Set motor neutral mode to brake.
	m_pLeftDriveMotor1->SetNeutralMode(NeutralMode::Brake);
	m_pLeftDriveMotor2->SetNeutralMode(NeutralMode::Brake);
	m_pLeftDriveMotor3->SetNeutralMode(NeutralMode::Brake);
	m_pRightDriveMotor1->SetNeutralMode(NeutralMode::Brake);
	m_pRightDriveMotor2->SetNeutralMode(NeutralMode::Brake);
	m_pRightDriveMotor3->SetNeutralMode(NeutralMode::Brake);
	// Stop the motor.
	m_pLeftDrive->Stop();
	m_pRightDrive->Stop();
	// Reset encoder counts to zero.
	m_pLeftDrive->ResetEncoderPosition();
	m_pRightDrive->ResetEncoderPosition();
	// Clear any sticky faults.
	m_pLeftDrive->ClearStickyFaults();
	m_pRightDrive->ClearStickyFaults();

	// Shift to high gear.
	m_pShiftSolenoid->Set(false);
}

/****************************************************************************
	Description:	Tick - main method that does functionality.
					Called each time through robot main loop to update state.

	Arguments: 		bool bSquareDrive  	- True for more sensitivity at lower speeds.

	Returns: 		Nothing
****************************************************************************/
void CDrive::Tick(bool bSquareDrive)
{
	if (m_bJoystickControl)
	{
		// Check to see if the Trigger Button was pressed. (Toggles the drive shift solenoid from low to high gear).
		if (m_pDriveController->GetRawButton(eLeftAnalog) && !m_bDriveControllerLeftAnalogPressed)
		{
			m_bDriveControllerLeftAnalogPressed = true;
			// Shift gears.
			m_pShiftSolenoid->Set(!m_pShiftSolenoid->Get());
		}
		else
		{
			if (!m_pDriveController->GetRawButton(eLeftAnalog))
			{
				m_bDriveControllerLeftAnalogPressed = false;
			}
		}

		// Get the Y axis value from the Left Stick.
		m_dYAxis = m_pDriveController->GetRawAxis(1);
		// Check if the Y Axis value is outside of the deadzone.
		if (fabs(m_dYAxis) <= 0.250)
		{
			// Y Axis is inside the deadzone, set it to zero.
			m_dYAxis = 0.000;
		}

		// Get the X axis value from the joystick.
		m_dXAxis = m_pDriveController->GetRawAxis(4);
		// Check if the X Axis value is outside of the deadzone.
		if (fabs(m_dXAxis) > 0.200)
		{
			if (m_pShiftSolenoid->Get())
			{
				// Valid X Axis value, multiply by the throttle.
				m_dXAxis = (m_dXAxis / (SmartDashboard::GetNumber("Z Axis Divisor", 2.000)));
			}
		}
		else
		{
			// X Axis is inside the deadzone, set it to zero.
			m_dXAxis = 0.000;
		}

		// Drive the robot.
		m_pRobotDrive->CurvatureDrive(m_dYAxis, -m_dXAxis, m_pShiftSolenoid->Get());
	}
	else
	{
		// Call the CANPosition ticks.
		m_pLeftDrive->Tick();
		m_pRightDrive->Tick();

		// Check to see if either motor has reached setpoint.
		if (m_pLeftDrive->IsReady() || m_pRightDrive->IsReady())
		{
			// Stop all drive motors.
			m_pLeftDrive->Stop();
			m_pRightDrive->Stop();
		}
	}

	// Put drive information on SmartDashboard for Auto/Teleop.
	SmartDashboard::PutBoolean("JoyStick Control",		m_bJoystickControl);
	SmartDashboard::PutNumber("Left Drive State",		m_pLeftDrive->GetState());
	SmartDashboard::PutNumber("Right Drive State",		m_pRightDrive->GetState());
	SmartDashboard::PutBoolean("Left Drive IsReady", 	IsLeftDriveReady());
	SmartDashboard::PutBoolean("Right Drive IsReady", 	IsRightDriveReady());
	SmartDashboard::PutBoolean("High Gear", 			IsDriveInHighGear());
	SmartDashboard::PutNumber("Left Drive Setpoint", 	GetLeftSetpoint());
	SmartDashboard::PutNumber("Right Drive Setpoint", 	GetRightSetpoint());
	SmartDashboard::PutNumber("Left Drive Actual", 		GetLeftActual());
	SmartDashboard::PutNumber("Right Drive Actual", 	GetRightActual());
}

/****************************************************************************
	Description:	VisionTick - Method to move to tracked object.
					Called each time through Autonomous loop.

	Arguments: 		dTolerance, dProportional

	Returns: 		Nothing
****************************************************************************/

void CDrive::VisionTick()
{

	const int nHorizontalRes		= SmartDashboard::GetNumber("VisionHorizontalRes", 176);
	// Constants.
	const double dCameraCenter 		=  nHorizontalRes/2;
	// Declare variables.
	double dTolerance				=  (SmartDashboard::GetNumber("VisionTolerance", 15.0));
	double dTurningTolerance		=  (SmartDashboard::GetNumber("VisionTurningTolerance", 10));
	double dProportional			=  (SmartDashboard::GetNumber("VisionProportional", 1.500));
	double dSpeedForward 		 	=  (SmartDashboard::GetNumber("VisionSpeedForward", 0.600));
	double dSpeedTurning			=  (SmartDashboard::GetNumber("VisionSpeedTurning", 0.420));
	double dInsideTurnDivisor  		=  (SmartDashboard::GetNumber("VisionInsideTurnDivisor", 2.000));
	double dOutsideTurnDivisor 		=  (SmartDashboard::GetNumber("VisionOutsideTurnDivisor", 1.200));
	double dDetectSize;
	double dxCenter;
	double dSetpointLeft;
	double dSetpointRight;
	double dErrorLeft;
	double dErrorRight;
	double dGainLeft;
	double dGainRight;
	double dSpeedLeft  = 0.00;
	double dSpeedRight = 0.00;
	// Calculate Center Point on X Axis.
	dxCenter	 	= ((SmartDashboard::GetNumber("VisionObjectCenter", dCameraCenter)));
	// Calculate Setpoints with Tolerance
	dSetpointLeft 	= (dCameraCenter -  dTolerance);
	dSetpointRight  = (dCameraCenter +  dTolerance);
	// Calculate Error.
	dErrorLeft		= (dSetpointLeft  -   dxCenter);
	dErrorRight		= (dxCenter - dSetpointRight);
	// Calculate Proportional Gain.
	dGainLeft 		= (dProportional *  dErrorLeft);
	dGainRight 		= (dProportional * dErrorRight);

	// Read Size from SmartDashboard.
	dDetectSize 	= SmartDashboard::GetNumber("VisionObjectSize", 0);

	// The object is close, stop motors.
	if (dDetectSize >= 30)
	{
		dSpeedLeft  = 0.000;
		dSpeedRight = 0.000;
	}
	else
	{
		// Due to the camera being mounted in reverse, all logic is inverted.
		// Object is Centered, move forward until Lock size.
		if (dxCenter > dSetpointLeft && dxCenter < dSetpointRight)
		{
			dSpeedLeft = dSpeedForward;
			dSpeedRight = dSpeedForward;
		}
		// Object is offset to the left, turn.
		if (dxCenter < dSetpointLeft)
		{
			if (!((dSetpointLeft - dxCenter) >= dTurningTolerance))
			{
				dSpeedLeft   = ((dGainLeft / dSetpointLeft) / dOutsideTurnDivisor) + dSpeedTurning;
				dSpeedRight  = (((dGainLeft / dSetpointLeft) / dInsideTurnDivisor) + dSpeedTurning) * -1;
			}
			else
			{
				dSpeedLeft   = ((dGainLeft / dSetpointLeft) / dOutsideTurnDivisor);
				dSpeedRight  = (((dGainLeft / dSetpointLeft) / dInsideTurnDivisor) * -1);
			}
		}
		// Object is offset to the right, turn.
		if (dxCenter > dSetpointRight)
		{
			if (!((dxCenter - dSetpointRight) >= dTurningTolerance))
			{
				dSpeedLeft  = ((dGainRight / dSetpointRight) / dInsideTurnDivisor) + dSpeedTurning;
				dSpeedRight = (((dGainRight / dSetpointRight) / dOutsideTurnDivisor) + dSpeedTurning) * -1;
			}
			else
			{
				dSpeedLeft  = ((dGainRight / dSetpointRight) / dInsideTurnDivisor);
				dSpeedRight = (((dGainRight / dSetpointRight) / dOutsideTurnDivisor) * -1);
			}
		}
	}
	m_pShiftSolenoid->Set(true);
	// Set motor speeds.
	m_pLeftDriveMotor1->Set(ControlMode::PercentOutput,  dSpeedLeft);
	m_pLeftDriveMotor2->Set(ControlMode::PercentOutput,  dSpeedLeft);
	m_pLeftDriveMotor3->Set(ControlMode::PercentOutput,  dSpeedLeft);
	m_pRightDriveMotor1->Set(ControlMode::PercentOutput, dSpeedRight);
	m_pRightDriveMotor2->Set(ControlMode::PercentOutput, dSpeedRight);
	m_pRightDriveMotor3->Set(ControlMode::PercentOutput, dSpeedRight);
}

/****************************************************************************
	Description:	Stop the drive, both left and right side.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CDrive::Stop()
{
	// Stop all drive motors.
	m_pLeftDrive->Stop();
	m_pRightDrive->Stop();
}

/****************************************************************************
	Description:	Toggles the solenoids to change from low to high gear.

	Arguments: 		bLowGear - True to shift to high gear.

	Returns: 		Nothing
****************************************************************************/
void CDrive::Shift(bool bLowGear)
{
	m_pShiftSolenoid->Set(bLowGear);
}

/****************************************************************************
	Description:	IsReady - Returns true if all drive motors are ready.

	Arguments: 		None

	Returns: 		bool - True if all drive motors ready, false otherwise.
****************************************************************************/
bool CDrive::IsReady()
{
	return (m_pLeftDrive->IsReady() || m_pRightDrive->IsReady());
}

/****************************************************************************
	Description:	SetJoystickControl - Sets the desired joystick control.

	Arguments: 		bool bJoystickControl - True if joystick control enabled,
					false otherwise.

	Returns: 		Nothing
****************************************************************************/
void CDrive::SetJoystickControl(bool bJoystickControl)
{
	m_bJoystickControl = bJoystickControl;

	if (m_bJoystickControl)
	{
		// For joystick control, set the CAN Talons to Percent Output.
		m_pLeftDriveMotor1->Set(ControlMode::PercentOutput, 0.000);
		m_pRightDriveMotor1->Set(ControlMode::PercentOutput, 0.000);
	}
	else
	{
		// For PID control, set the CAN Talons to Position.
		m_pLeftDriveMotor1->Set(ControlMode::Position, 0.000);
		m_pLeftDriveMotor1->Set(ControlMode::Position, 0.000);
	}

	// Reset the drive encoders.
	m_pLeftDrive->ResetEncoderPosition();
	m_pRightDrive->ResetEncoderPosition();
}

/****************************************************************************
	Description:	SetSetpoint - Sets the desired positional setpoints for
					both the left and right drive motors.

	Arguments: 		double dLeftSetpoint - Setpoint in inches for left motor.
					double dRightSetpoint - Setpoint in inches for right motor.

	Returns: 		Nothing
****************************************************************************/
void CDrive::SetSetpoint(double dLeftSetpoint, double dRightSetpoint)
{
	m_pLeftDrive->SetSetpoint(dLeftSetpoint);
	m_pRightDrive->SetSetpoint(dRightSetpoint);
}

/****************************************************************************
	Description:	SetMotorExpiration - Sets the motor safety expiration
					timeout.

	Arguments: 		double dTimeout - Expiration timeout

	Returns: 		Nothing
****************************************************************************/
void CDrive::SetMotorExpiration(double dTimeout)
{
	m_pRobotDrive->SetExpiration(dTimeout);
}

/****************************************************************************
	Description:	SetMotorSafety - Sets the motor safety enabled for the
					drive motors.

	Arguments: 		bool bEnabled - True to set MotorSafetyEnabled

	Returns: 		Nothing
****************************************************************************/
void CDrive::SetMotorSafety(bool bEnabled)
{
	m_pRobotDrive->SetSafetyEnabled(bEnabled);
}

/******************************************************************************
	Description:	SetPIDValues - Sets the PID and Feed Forward gain values
					for the left drive motors.

	Arguments:	 	double dProportional 	- Proportion Gain
					double dIntegral		- Integral Gain
					double dDerivative		- Derivative Gain
					double dFeedForward		- Feed Forward Gain

	Returns: 		Nothing
******************************************************************************/
void CDrive::SetLeftPIDValues(double dProportional, double dIntegral, double dDerivative, double dFeedForward)
{
	m_pLeftDrive->SetPIDValues(nCANPositionPIDLoopIdx, dProportional, 	nCANPositionTimeoutMs);
	m_pLeftDrive->SetPIDValues(nCANPositionPIDLoopIdx, dIntegral, 		nCANPositionTimeoutMs);
	m_pLeftDrive->SetPIDValues(nCANPositionPIDLoopIdx, dDerivative, 	nCANPositionTimeoutMs);
	m_pLeftDrive->SetPIDValues(nCANPositionPIDLoopIdx, dFeedForward, 	nCANPositionTimeoutMs);
}

/******************************************************************************
	Description:	SetPIDValues - Sets the PID and Feed Forward gain values
					for the right drive motors.

	Arguments:	 	double dProportional 	- Proportion Gain
					double dIntegral		- Integral Gain
					double dDerivative		- Derivative Gain
					double dFeedForward		- Feed Forward Gain

	Returns: 		Nothing
******************************************************************************/
void CDrive::SetRightPIDValues(double dProportional, double dIntegral, double dDerivative, double dFeedForward)
{
	m_pRightDrive->SetPIDValues(nCANPositionPIDLoopIdx, dProportional, 	nCANPositionTimeoutMs);
	m_pRightDrive->SetPIDValues(nCANPositionPIDLoopIdx, dIntegral, 		nCANPositionTimeoutMs);
	m_pRightDrive->SetPIDValues(nCANPositionPIDLoopIdx, dDerivative, 	nCANPositionTimeoutMs);
	m_pRightDrive->SetPIDValues(nCANPositionPIDLoopIdx, dFeedForward, 	nCANPositionTimeoutMs);
}
///////////////////////////////////////////////////////////////////////////////

/****************************************************************************
	Description:	Implements the CGripper control class.

	Classes:		CGripper

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#include "WPILib.h"
#include "Gripper.h"
#include "IOMap.h"
#include <ctre/Phoenix.h>
/////////////////////////////////////////////////////////////////////////////

/****************************************************************************
	Description:	CGripper Constructor.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
CGripper::CGripper(Joystick* pDriveController)
{
	// Store a copy of the drive controller.
	m_pDriveController					= pDriveController;

	// Create the object pointers.
	m_pPlungerSolenoid			= new Solenoid(nPlungerSolenoid);
	m_pPlungerVacuumSolenoid	= new Solenoid(nPlungerVacuumSolenoid);
	m_pLeftClawSolenoid			= new Solenoid(nLeftClawSolenoid);
	m_pRightClawSolenoid		= new Solenoid(nRightClawSolenoid);
	m_pLeftIntakeMotor			= new WPI_TalonSRX(nLeftIntakeMotor);
	m_pRightIntakeMotor			= new WPI_TalonSRX(nRightIntakeMotor);
	m_pPowerCubeSensor			= new DigitalInput(nPowerCubeSensor);
	m_pTimer					= new Timer();

	// Initialize member variables.
	m_nGripperState				= eGripperIdle;
	m_dDelayStartTime			= 0.000;
	m_dEjectThrottle			= 1.000;

	// Start the timer.
	m_pTimer->Start();
}

/****************************************************************************
	Description:	CGripper Destructor.

	Arguments:		None

	Derived From:	None
****************************************************************************/
CGripper::~CGripper()
{
	// Delete our object pointers.
	delete	m_pPlungerSolenoid;
	delete  m_pPlungerVacuumSolenoid;
	delete	m_pLeftClawSolenoid;
	delete	m_pRightClawSolenoid;
	delete	m_pLeftIntakeMotor;
	delete	m_pRightIntakeMotor;
	delete	m_pPowerCubeSensor;
	delete	m_pTimer;

	// Set the objects to NULL.
	m_pPlungerSolenoid		 = NULL;
	m_pPlungerVacuumSolenoid = NULL;
	m_pLeftClawSolenoid 	 = NULL;
	m_pRightClawSolenoid 	 = NULL;
	m_pLeftIntakeMotor		 = NULL;
	m_pRightIntakeMotor		 = NULL;
	m_pPowerCubeSensor		 = NULL;
	m_pTimer				 = NULL;
}

/****************************************************************************
	Description:	Initialize Gripper parameters.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CGripper::Init()
{
	// Retract both plungers.
	m_pPlungerSolenoid->Set(false);
	// Disable vacuum.
	m_pPlungerVacuumSolenoid->Set(false);
	// Extend both gripper solenoids (gripper arms are closed).
	m_pLeftClawSolenoid->Set(false);
	m_pRightClawSolenoid->Set(false);
	// Set both intake motors to brake (To hold power cube secure).
	m_pLeftIntakeMotor->SetNeutralMode(NeutralMode::Brake);
	m_pRightIntakeMotor->SetNeutralMode(NeutralMode::Brake);
	// Invert Right intake motor.
	m_pLeftIntakeMotor->SetInverted(false);
	m_pRightIntakeMotor->SetInverted(true);
	// Stop both intake motors.
	m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
	m_pRightIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
	// Clear motor sticky faults.
	m_pLeftIntakeMotor->ClearStickyFaults(0.000);
	m_pRightIntakeMotor->ClearStickyFaults(0.000);
	// Disable forward/reverse limit switches.
	m_pLeftIntakeMotor->ConfigForwardLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, LimitSwitchNormal::LimitSwitchNormal_Disabled, 0.000);
	m_pRightIntakeMotor->ConfigForwardLimitSwitchSource(LimitSwitchSource::LimitSwitchSource_FeedbackConnector, LimitSwitchNormal::LimitSwitchNormal_Disabled, 0.000);

	// Put the gripper delay for changing from the SmartDashboard.
	SmartDashboard::PutNumber("Intake Read Sensor Delay", 			dDefaultIntakeReadSensorDelay);
	SmartDashboard::PutNumber("Intake Close Claw Delay",			dDefaultIntakeCloseClawDelay);
	SmartDashboard::PutNumber("Intake Stop Motor Delay", 			dDefaultIntakeStopMotorDelay);
	SmartDashboard::PutNumber("Floor Eject Stop Motor Delay", 		dDefaultFloorEjectStopMotorDelay);
	SmartDashboard::PutNumber("Scale Eject Plunger Delay", 			dDefaultScaleEjectPlungerDelay);
	SmartDashboard::PutNumber("Scale Eject Plunger Return Delay", 	dDefaultScaleEjectPlungerReturnDelay);
	SmartDashboard::PutNumber("Scale Eject Close Claw Delay", 		dDefaultScaleEjectCloseClawDelay);
}

/****************************************************************************
	Description:	Tick - Called each time through robot main loop.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CGripper::Tick()
{
	// Call the Gripper State machine.
	GripperStateMachine();

	// Put gripper information on SmartDashboard for Auto/Teleop.
	SmartDashboard::PutBoolean("Cube Sensor", m_pPowerCubeSensor->Get());
}

/****************************************************************************
	GripperStateMachine - This is the state machine used for the gripper class.

	Arguments: 			None

	Returns: 			Nothing
****************************************************************************/
void CGripper::GripperStateMachine()
{
	double 	dIntakeReadSensorDelay;
	double	dIntakeCloseClawDelay;
	double	dIntakeStopMotorDelay;
	double	dFloorEjectStopMotorDelay;
	double	dScaleEjectPlungerDelay;
	double  dScaleEjectPlungerReturnDelay;
	double  dScaleEjectCloseClawDelay;

	// State Machine.
	switch (m_nGripperState)
	{
		/********************************************************************
			eGripperIdle - Idle, Motors Off, Plungers Retracted, Claw Closed.
		********************************************************************/
		case eGripperIdle :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "eGripperIdle");

			// Turn intake motors off.
			m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
			m_pRightIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
			// Retract Plunger solenoid.
			m_pPlungerSolenoid->Set(false);
			// Disable vacuum.
			m_pPlungerVacuumSolenoid->Set(false);
			// Close the claw.
			m_pRightClawSolenoid->Set(false);
			m_pLeftClawSolenoid->Set(false);
			break;

		/********************************************************************
			eGripperOpenIntake1 - Motors On, Plungers Retracted, Claw Open.
		********************************************************************/
		case eGripperOpenIntake1A :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Open Intake - Start");

			// Turn intake motors on.
			m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, dDefaultIntakeMotorSpeed);
			m_pRightIntakeMotor->Set(ControlMode::PercentOutput, dDefaultIntakeMotorSpeed);
			// Retract Plunger solenoid.
			m_pPlungerSolenoid->Set(false);
			// Disable vacuum.
			m_pPlungerVacuumSolenoid->Set(false);
			// Open the claw.
			m_pRightClawSolenoid->Set(true);
			m_pLeftClawSolenoid->Set(true);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eGripperOpenIntake1B state.
			m_nGripperState = eGripperOpenIntake1B;
			break;

		case eGripperOpenIntake1B :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Open Intake - Waiting for cube.");

			// Get the delay to open the claw before reading the cube sensor from the dashboard.
			dIntakeReadSensorDelay = SmartDashboard::GetNumber("Intake Read Sensor Delay", dDefaultIntakeReadSensorDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dIntakeReadSensorDelay))
			{
				// Start checking the cube sensor.
				if (m_pPowerCubeSensor->Get())
				{
					// Get current time.
					m_dDelayStartTime = m_pTimer->Get();
					// Move to eGripperOpenIntake1C state.
					m_nGripperState = eGripperOpenIntake1C;
				}
			}
			break;

		case eGripperOpenIntake1C :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Open Intake - Delay Closing Claw.");

			// Get the delay to close claw from the dashboard.
			dIntakeCloseClawDelay = SmartDashboard::GetNumber("Intake Close Claw Delay", dDefaultIntakeCloseClawDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dIntakeCloseClawDelay))
			{
				// Get current time.
				m_dDelayStartTime = m_pTimer->Get();
				// Close the claw.
				m_pRightClawSolenoid->Set(false);
				m_pLeftClawSolenoid->Set(false);
				// Move to eGripperOpenIntake1D state.
				m_nGripperState = eGripperOpenIntake1D;
			}
			break;

		case eGripperOpenIntake1D :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Open Intake - Delay Stopping Motors");

			// Get the delay to turn off the intake motors from the dashboard.
			dIntakeStopMotorDelay = SmartDashboard::GetNumber("Intake Stop Motor Delay", dDefaultIntakeStopMotorDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dIntakeStopMotorDelay))
			{
				// Move to Gripper Idle state.
				m_nGripperState = eGripperIdle;
			}
			break;

		/********************************************************************
			eGripperClosedIntake1 - Motors On, Plungers Retracted, Claw Closed.
		********************************************************************/
		case eGripperClosedIntake1A :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Closed Intake - Start");

			// Turn intake motors on.
			m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, dDefaultIntakeMotorSpeed);
			m_pRightIntakeMotor->Set(ControlMode::PercentOutput, dDefaultIntakeMotorSpeed);
			// Retract Plunger solenoid.
			m_pPlungerSolenoid->Set(false);
			// Disable vacuum.
			m_pPlungerVacuumSolenoid->Set(false);
			// Close the claw.
			m_pRightClawSolenoid->Set(false);
			m_pLeftClawSolenoid->Set(false);
			// Move to eGripperClosedIntake1B state.
			m_nGripperState = eGripperClosedIntake1B;
			break;

		case eGripperClosedIntake1B :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Closed Intake - Waiting for cube.");

			// Start checking the cube sensor.
			if (m_pPowerCubeSensor->Get())
			{
				// Get current time.
				m_dDelayStartTime = m_pTimer->Get();
				// Move to eGripperClosedIntake1C state.
				m_nGripperState = eGripperClosedIntake1C;
			}
			break;

		case eGripperClosedIntake1C :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Closed Intake - Delay Stopping Intake Motors.");

			// Get the delay to turn off the intake motors from the dashboard.
			dIntakeStopMotorDelay = SmartDashboard::GetNumber("Intake Stop Motor Delay", dDefaultIntakeStopMotorDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dIntakeStopMotorDelay))
			{
				// Move to Gripper Idle state.
				m_nGripperState = eGripperIdle;
			}
			break;

		/********************************************************************
			eGripperFloorEject1A - Motors On, Plungers Retracted, Claw Closed.
		********************************************************************/
		case eGripperFloorEject1A :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Floor Eject - Start");

			// Get the Drive POV. Scale eject output to 95% if DPAD up.
			if (m_pDriveController->GetPOV() == 0)
			{
				m_dEjectThrottle = 0.95;
			}
			else
			{
				// Scale eject to 25% power if DPAD down.
				if (m_pDriveController->GetPOV() == 180)
				{
					m_dEjectThrottle = 0.25;
				}
				// Scale eject to 50% power if DPAD center.
				else
				{
					m_dEjectThrottle = 0.50;
				}
			}

			SmartDashboard::PutNumber("Gripper Throttle", m_dEjectThrottle);
			// Turn intake motors on to eject. Scale the speed with the throttle on the drive controller.
			m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, (m_dEjectThrottle * dDefaultEjectMotorSpeed));
			m_pRightIntakeMotor->Set(ControlMode::PercentOutput, (m_dEjectThrottle * dDefaultEjectMotorSpeed));
			// Retract Plunger solenoid.
			m_pPlungerSolenoid->Set(false);
			// Disable vacuum.
			m_pPlungerVacuumSolenoid->Set(false);
			// Close the claw.
			m_pRightClawSolenoid->Set(false);
			m_pLeftClawSolenoid->Set(false);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eGripperFloorEject1B state.
			m_nGripperState = eGripperFloorEject1B;
			break;

		case eGripperFloorEject1B :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Floor Eject - Delay Stopping Motors");

			// Get the delay to turn off the intake motors from the dashboard.
			dFloorEjectStopMotorDelay = SmartDashboard::GetNumber("Floor Eject Stop Motor Delay", dDefaultFloorEjectStopMotorDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dFloorEjectStopMotorDelay))
			{
				// Move to Gripper Idle state.
				m_nGripperState = eGripperIdle;
			}
			break;

		/********************************************************************
			eGripperFloorEject2A - Motors off, Plungers Retracted, Claw open.
		********************************************************************/
		case eGripperFloorEject2A :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Floor Eject - Start");

			// Open the claw.
			m_pRightClawSolenoid->Set(true);
			m_pLeftClawSolenoid->Set(true);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eGripperFloorEject1B state.
			m_nGripperState = eGripperFloorEject1B;
			break;

		case eGripperFloorEject2B :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Floor Eject - Delay Closing Claw");

			// Get the delay to close claw from the dashboard.
			dFloorEjectStopMotorDelay = SmartDashboard::GetNumber("Floor Eject Stop Motor Delay", dDefaultFloorEjectStopMotorDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dFloorEjectStopMotorDelay))
			{
				// Move to Gripper Idle state.
				m_nGripperState = eGripperIdle;
			}
			break;

		/********************************************************************
			eGripperEject2 - Motors Off, Claw Open, Delay Plunger.
		********************************************************************/
		case eGripperScaleEject1A :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Scale Eject - Start");

			// Turn intake motors off.
			m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
			m_pRightIntakeMotor->Set(ControlMode::PercentOutput, 0.000);
			// Disable vacuum.
			m_pPlungerVacuumSolenoid->Set(false);
			// Open the claw.
			m_pRightClawSolenoid->Set(true);
			m_pLeftClawSolenoid->Set(true);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eGripperScaleEject1B state.
			m_nGripperState = eGripperScaleEject1B;
			break;

		case eGripperScaleEject1B :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Scale Eject - Plunger Delay");

			// Get the plunger delay from the dashboard.
			dScaleEjectPlungerDelay = SmartDashboard::GetNumber("Scale Eject Plunger Delay", dDefaultScaleEjectPlungerDelay);

			if (m_pTimer->Get() > (m_dDelayStartTime + dScaleEjectPlungerDelay))
			{
				// Extend Plunger solenoid.
				m_pPlungerSolenoid->Set(true);
				// Get current time.
				m_dDelayStartTime = m_pTimer->Get();
				// Move to eGripperScaleEject1C state.
				m_nGripperState = eGripperScaleEject1C;
			}
			break;

		case eGripperScaleEject1C :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Scale Eject - Plunger Return Delay");

			// Get the plunger return delay from the dashboard.
			dScaleEjectPlungerReturnDelay = SmartDashboard::GetNumber("Scale Eject Plunger Return Delay", dDefaultScaleEjectPlungerReturnDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dScaleEjectPlungerReturnDelay))
			{
				// Retract Plunger solenoid.
				m_pPlungerSolenoid->Set(false);
				// Enable vacuum.
				m_pPlungerVacuumSolenoid->Set(true);
				// Get current time.
				m_dDelayStartTime = m_pTimer->Get();
				// Move to eGripperScaleEject1D state.
				m_nGripperState = eGripperScaleEject1D;
			}
			break;

		case eGripperScaleEject1D :
			// Print out gripper state.
			SmartDashboard::PutString("Gripper State", "Scale Eject - Delay Closing Claw.");

			// Get the delay to close claw from the dashboard.
			dScaleEjectCloseClawDelay = SmartDashboard::GetNumber("Scale Eject Close Claw Delay", dDefaultScaleEjectCloseClawDelay);
			if (m_pTimer->Get() > (m_dDelayStartTime + dScaleEjectCloseClawDelay))
			{
				// Move to eGripperIdle state.
				m_nGripperState = eGripperIdle;
			}
			break;

	}
}

/****************************************************************************
	Description:	SetGripperState - Sets the gripper's state machine state.

	Arguments: 		int nGripperState

	Returns: 		Nothing
****************************************************************************/
void CGripper::SetGripperState(int nGripperState)
{
	m_nGripperState = (GripperStates)nGripperState;
}

/****************************************************************************
	Description:	EnableVacuum - Used in TeleOp to extend or retracts the
					vacuum solenoids to enable/disable.

	Arguments: 		bool bSetVacuumSolenoid

	Returns: 		Nothing
****************************************************************************/
void CGripper::SetVacuumSolenoid(bool bSetVacuumSolenoid)
{
	m_pPlungerVacuumSolenoid->Set(bSetVacuumSolenoid);
}

/****************************************************************************
	Description:	GetCubeSensor - Used in Test Mode to get status of the
					Cube sensor.

	Arguments: 		None

	Returns: 		bool bIsCubePresent
****************************************************************************/
bool CGripper::TestGetCubeSensor()
{
	return m_pPowerCubeSensor->Get();
}

/****************************************************************************
	Description:	ExtendPlunger - Used in Test Mode to extend the plunger
					solenoid. Note: Turning off the solenoid does not retract
					the solenoid. The solenoid is retracted using a bungee
					cord.

	Arguments: 		bool bExtend

	Returns: 		Nothing
****************************************************************************/
void CGripper::TestExtendPlunger(bool bExtend)
{
	m_pPlungerSolenoid->Set(bExtend ? true : false);
}

/****************************************************************************
	Description:	OpenClaw - Used in Test Mode to extend or retracts the
					claw solenoids to open/close.

	Arguments: 		bool bClawOpen

	Returns: 		Nothing
****************************************************************************/
void CGripper::TestOpenClaw(bool bClawOpen)
{
	m_pLeftClawSolenoid->Set(bClawOpen ? true : false);
	m_pRightClawSolenoid->Set(bClawOpen ? true : false);
}

/****************************************************************************
	Description:	TestLeftMotor - Used in Test Mode to turn the intake
					motors on/off.

	Arguments: 		double dSetpoint - Motor Setpoint (-1.000 to 1.000);

	Returns: 		Nothing
****************************************************************************/
void CGripper::TestLeftMotor(double dSetpoint)
{
	m_pLeftIntakeMotor->Set(ControlMode::PercentOutput, dSetpoint);
}

/****************************************************************************
	Description:	TestRightMotor - Used in test mode to turn the motors on/off.

	Arguments: 		double dSetpoint - Motor Setpoint (-1.000 to 1.000);

	Returns: 		Nothing
****************************************************************************/
void CGripper::TestRightMotor(double dSetpoint)
{
	m_pRightIntakeMotor->Set(ControlMode::PercentOutput, dSetpoint);
}

/****************************************************************************
	Description:	EnableVacuum - Used in Test Mode to extend or retracts the
					vacuum solenoids to enable/disable.

	Arguments: 		bool bEnableVacuum

	Returns: 		Nothing
****************************************************************************/
void CGripper::TestEnableVacuum(bool bEnableVacuum)
{
	m_pPlungerVacuumSolenoid->Set(bEnableVacuum ? true : false);
}
///////////////////////////////////////////////////////////////////////////////


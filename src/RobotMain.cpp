/******************************************************************************
	Description:	2018 PowerUp Robot Control Software.

	Classes:		CRobotMain

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER Robotics.
******************************************************************************/
#include "WPILib.h"
#include <ctre/Phoenix.h>
#include "RobotMain.h"
#include "IOMap.h"
#include "Drive.h"
#include "ArmPosition.h"
#include "Gripper.h"
#include "Ramp.h"
#include "Vision.h"
///////////////////////////////////////////////////////////////////////////////


/******************************************************************************
	Description:	CRobotMain Constructor.

	Arguments:		None

	Derived From:	TimedRobot
******************************************************************************/
CRobotMain::CRobotMain() : TimedRobot()
{
	// Create the object pointers.
	m_pDriveController			= new Joystick(1);
	m_pAuxController			= new Joystick(2);	
	m_pCompressor				= new Compressor();
	m_pDrive					= new CDrive(m_pDriveController);
	m_pArmPosition				= new CArmPosition();
	m_pGripper					= new CGripper(m_pDriveController);
	m_pLeftRamp					= new CRamp(nLeftRampMotor, nLeftRampHingeSensor, true);
	m_pRightRamp				= new CRamp(nRightRampMotor, nRightRampHingeSensor, false);
	m_pVision					= new CVision();
	m_pTimer					= new Timer();
	m_pAutonomousChooser		= new SendableChooser<std::string>();
	m_pLiveWindow				= LiveWindow::GetInstance();

	// Initialize Variables
	m_bDriveControllerBackPressed			= false;
	m_bDriveControllerStartPressed			= false;
	m_bDriveControllerAButtonPressed		= false;
	m_bDriveControllerBButtonPressed		= false;
	m_bDriveControllerXButtonPressed		= false;
	m_bDriveControllerYButtonPressed		= false;
	m_bDriveControllerLeftTriggerPressed	= false;
	m_bDriveControllerRightTriggerPressed	= false;
	m_bDriveControllerLeftBumperPressed		= false;
	m_bDriveControllerRightBumperPressed	= false;
	m_bAuxControllerBackPressed				= false;
	m_bAuxControllerStartPressed			= false;
	m_bAuxControllerButtonAPressed			= false;
	m_bAuxControllerButtonBPressed			= false;
	m_bAuxControllerButtonXPressed			= false;
	m_bAuxControllerButtonYPressed			= false;
	m_bAuxControllerButtonLBPressed			= false;
	m_bAuxControllerButtonRBPressed			= false;
	m_bAuxControllerLeftAnalogPressed		= false;
	m_bAuxControllerRightAnalogPressed		= false;
	m_bAuxControllerPOVLeftPressed			= false;
	m_bAuxControllerPOVRightPressed			= false;
	m_bAuxControllerPOVUpPressed			= false;
	m_bAuxControllerPOVDownPressed			= false;
	m_bScaleLeftHot							= false;
	m_bSwitchLeftHot						= false;
	m_bDoVision								= false;
	m_dDelayStartTime						= 0.000;
	m_strGameSpecificMessage				= "NULL";
	m_nAutonomousState						= eAutonomousIdle;
	m_nTeleOpState							= eTeleOpIdle;

	// Start the timer.
	m_pTimer->Start();

	// Set motor expiration.
	m_pDrive->SetMotorExpiration(0.100);
}

/******************************************************************************
	Description:	CRobotMain Destructor.

	Arguments:		None

	Derived From:	SimpleRobot
******************************************************************************/
CRobotMain::~CRobotMain()
{
	// Delete our object pointers.
	delete m_pDriveController;
	delete m_pAuxController;
	delete m_pCompressor;
	delete m_pDrive;
	delete m_pArmPosition;
	delete m_pGripper;
	delete m_pVision;
	delete m_pTimer;

	m_pDriveController 	= NULL;
	m_pAuxController	= NULL;
	m_pCompressor		= NULL;
	m_pDrive			= NULL;
	m_pArmPosition		= NULL;
	m_pGripper			= NULL;
	m_pVision			= NULL;
	m_pTimer			= NULL;
}

/******************************************************************************
	RobotInit:	Robot-wide initialization code which is called only once upon
				a power on reset.

	Arguments: 	None

	Returns: 	Nothing
******************************************************************************/
void CRobotMain::RobotInit()
{
	// Add the Z Axis Divisor information to the dashboard.
	SmartDashboard::PutNumber("Z Axis Divisor", 3.000);

	// Add the autonomous chooser to the smart dashboard.
	SmartDashboard::PutData("Autonomous Mode", m_pAutonomousChooser);
	SmartDashboard::PutBoolean("Do magic?", false);
	SmartDashboard::PutNumber("Left Scale Speed",	   1.00);
	SmartDashboard::PutNumber("Right Scale Speed",	   1.00);
	SmartDashboard::PutNumber("CenterLeft 1 Left", 	 -18.00);		// Center Auton, Left Switch Hot, First Move, Left Drive
	SmartDashboard::PutNumber("CenterLeft 1 Right",	  18.00);		// Center Auton, Left Switch Hot, First Move, Right Drive
	SmartDashboard::PutNumber("CenterLeft 2 Left",    12.20);		// Center Auton, Left Switch Hot, Turn, Left Drive
	SmartDashboard::PutNumber("CenterLeft 2 Right",   12.20);		// Center Auton, Left Switch Hot, Turn, Right Drive
	SmartDashboard::PutNumber("CenterLeft 3 Left",   -92.00);		// Center Auton, Left Switch Hot, Second Move, Left Drive
	SmartDashboard::PutNumber("CenterLeft 3 Right",   92.00);		// Center Auton, Left Switch Hot, Second Move, Right Drive

	SmartDashboard::PutNumber("CenterRight 1 Left",  -30.00);		// Center Auton, Right Switch Hot, First Move, Left Drive
	SmartDashboard::PutNumber("CenterRight 1 Right",  30.00);		// Center Auton, Right Switch Hot, First Move, Right Drive
	SmartDashboard::PutNumber("CenterRight 2 Left",  -12.50);		// Center Auton, Right Switch Hot, Turn, Left Drive
	SmartDashboard::PutNumber("CenterRight 2 Right", -12.50);		// Center Auton, Right Switch Hot, Turn, Right Drive
	SmartDashboard::PutNumber("CenterRight 3 Left",  -66.00);		// Center Auton, Right Switch Hot, Second Move, Left Drive
	SmartDashboard::PutNumber("CenterRight 3 Right",  66.00);		// Center Auton, Right Switch Hot, Second Move, Right Drive

	// Add the vision values to the smart dashboard.
	SmartDashboard::PutNumber("VisionCameraCenter", 88.000);
	SmartDashboard::PutNumber("VisionTolerance", 15.0);
	SmartDashboard::PutNumber("VisionTurningTolerance", 10);
	SmartDashboard::PutNumber("VisionProportional", 1.500);
	SmartDashboard::PutNumber("VisionSpeedForward", 0.600);
	SmartDashboard::PutNumber("VisionSpeedTurning", 0.420);
	SmartDashboard::PutNumber("VisionInsideTurnDivisor", 2.000);
	SmartDashboard::PutNumber("VisionOutsideTurnDivisor", 1.200);

	// Set up the autonomous chooser for the smart dashboard.
	m_pAutonomousChooser->AddDefault(m_strAutonomousIdle, 			m_strAutonomousIdle);
	m_pAutonomousChooser->AddObject(m_strAutonomousVgetblock, 		m_strAutonomousVgetblock);
	m_pAutonomousChooser->AddObject(m_strAutonomousCrossBaseline, 	m_strAutonomousCrossBaseline);
	m_pAutonomousChooser->AddObject(m_strAutonomousLeft,			m_strAutonomousLeft);
	m_pAutonomousChooser->AddObject(m_strAutonomousCenter,			m_strAutonomousCenter);
	m_pAutonomousChooser->AddObject(m_strAutonomousRight,			m_strAutonomousRight);

	// Initialize object parameters.
	m_pDrive->Init();
	m_pArmPosition->Init();
	m_pGripper->Init();
	m_pLeftRamp->Init();
	m_pRightRamp->Init();
	m_pVision->Init();
}

/******************************************************************************
	RobotPeriodic:  Called based on the TimedRobot's period any time the program
					is running.

	Arguments: 		None

	Returns: 		Nothing
******************************************************************************/
void CRobotMain::RobotPeriodic()
{
	// Get the state of the Switches and Scale (in R/L string form, 3 characters).
	m_strGameSpecificMessage = DriverStation::GetInstance().GetGameSpecificMessage();

	// Call the Vision tick (update camera).
	m_pVision->Tick();
}

/******************************************************************************
	DisabledInit:	Called one time, each time the robot enters the Disabled
					state.

	Arguments: 		None

	Returns: 		Nothing
******************************************************************************/
void CRobotMain::DisabledInit()
{
}

/******************************************************************************
	DisabledPeriodic:	Called based on the TimedRobot's period any time the
						robot is disabled.

	Arguments: 			None

	Returns: 			Nothing
******************************************************************************/
void CRobotMain::DisabledPeriodic()
{
}

/******************************************************************************
	AutonomousInit:	Called one time, each time the robot enters the autonomous
					state.

	Arguments: 	None

	Returns: 	Nothing
******************************************************************************/
void CRobotMain::AutonomousInit()
{
	// Set the Drive to position control, turn off joystick control.
	m_pDrive->SetJoystickControl(false);

	// Initialize drive parameters.
	m_pDrive->Init();

	// Initialize arm parameters.
	m_pArmPosition->Init();

	// Home the arm.
	m_pArmPosition->StartHoming();

	// Shift to high gear for Autonomous.
	m_pDrive->Shift(false);

	// MotorSafety improves safety when motors are updated in loops
	// but is disabled here because motor updates are not looped in
	// this autonomous mode.
	m_pDrive->SetMotorSafety(false);

	// Read the alliance from the driver station.
	m_strAllianceSelected = (SmartDashboard::GetBoolean("IsRedAlliance", false) ? "Red Alliance" : "Blue Alliance");

	// Read the selected mode from the driver station.
	m_strAutonomousSelected = m_pAutonomousChooser->GetSelected();

	// Read whether or not to do Vision for autonomous.
	m_bDoVision = SmartDashboard::GetBoolean("Do magic?", false);

	// Check the selected mode from SmartDashboard
	if (m_strAutonomousSelected == "Autonomous Idle")
	{
		m_nAutonomousState = eAutonomousIdle;
	}

	if (m_strAutonomousSelected == "Get Block")
	{
		m_nAutonomousState = eAutonomousVgetblock1;
	}

	if (m_strAutonomousSelected == "Cross Baseline")
	{
		m_nAutonomousState = eAutonomousCrossBaseline1;
	}

	if (m_strAutonomousSelected == "Auton Left")
	{
		m_nAutonomousState = eAutonomousLeft1;
	}

	if (m_strAutonomousSelected == "Auton Center")
	{
		m_nAutonomousState = eAutonomousCenter1;
	}

	if (m_strAutonomousSelected == "Auton Right")
	{
		m_nAutonomousState = eAutonomousRight1;
	}

	// Get the Game Specific Message and interpret it.
	if (m_strGameSpecificMessage.length() == 3)
	{
		m_bSwitchLeftHot = ((m_strGameSpecificMessage[0] == 'L') ? true : false);
		m_bScaleLeftHot  = ((m_strGameSpecificMessage[1] == 'L') ? true : false);
	}
}

/******************************************************************************
	AutonomousPeriodic:	Called based on the TimedRobot's period any time the
						robot is in autonomous mode and enabled.

	Arguments: 			None

	Returns: 			Nothing
******************************************************************************/
void CRobotMain::AutonomousPeriodic()
{
	// Call the autonomous state machine.
	AutonomousStateMachine();

	// Call the drive tick.
	m_pDrive->Tick(false);

	// Call the arm tick.
	m_pArmPosition->Tick();

	// Call the gripper tick.
	m_pGripper->Tick();
}

/******************************************************************************
	AutonomousStartMove:	Called when a new move in the AutonomousStateMachine
							is started. Sets normal values.
	
	Arguments: 				double dTimeout, double dTolerance,
							double dLeftOutputPercent, double dRightOutputPercent,
							double dRampRate, double dIZone
	
	Returns: 				Nothing
******************************************************************************/
void CRobotMain::AutonomousStartMove(double dTimeout, double dTolerance = dDriveTolerance,
									 double dLeftOutputPercent = 0.5, double dRightOutputPercent = 0.5,
									 double dRampRate = dDriveClosedLoopRampRate,
									 double dIZone = dDriveAccumIZone)
{
	// TODO: Investigate Setting PID like this, and why it doesn't work

	// Reset encoder counts to zero.
	m_pDrive->ResetLeftEncoder();
	m_pDrive->ResetRightEncoder();

	// Set the maximum time to reach setpoint.
	m_pDrive->SetLeftMaxFindingTime(dTimeout);
	m_pDrive->SetRightMaxFindingTime(dTimeout);

	// Set the position tolerance.
	m_pDrive->SetLeftTolerance(dTolerance);
	m_pDrive->SetRightTolerance(dTolerance);

	// Set the maximum forward and reverse percent output.
	m_pDrive->SetLeftPeakOutputPercent(dLeftOutputPercent, dLeftOutputPercent * -1);
	m_pDrive->SetRightPeakOutputPercent(dRightOutputPercent, dRightOutputPercent * -1);

	// Set the closed loop ramp rate.
	m_pDrive->SetLeftClosedLoopRampRate(dRampRate);
	m_pDrive->SetRightClosedLoopRampRate(dRampRate);

	// Set the I Zone.
	m_pDrive->SetLeftAccumIZone(dIZone);
	m_pDrive->SetRightAccumIZone(dIZone);
}

/******************************************************************************
	AutonomousStateMachine:	Called based on the TimedRobot's period any time the
							robot is in autonomous mode and enabled.

	Arguments: 				None

	Returns: 				Nothing
******************************************************************************/
void CRobotMain::AutonomousStateMachine()
{
	// State Machine.
	switch (m_nAutonomousState)
	{
		/********************************************************************
			Auton Idle - Do Nothing
		********************************************************************/
		case eAutonomousIdle :
			SmartDashboard::PutString("Robot State", "Autonomous Idle");
			break;

		/********************************************************************
			Auton Get Block
		********************************************************************/

		case eAutonomousVgetblock1 :
			SmartDashboard::PutString("Robot State", "Autonomous Vgetblock1");
			// Activate Gripper.
			//m_pGripper->SetGripperState(m_pGripper->eGripperClosedIntake1A);
			// Move to AutonomousVgetblock2.
			m_nAutonomousState = eAutonomousVgetblock2;
			break;

		case eAutonomousVgetblock2 :
			SmartDashboard::PutString("Robot State", "Autonomous Vgetblock2");
			// Use Vision to find a cube.
			///SmartDashboard::PutNumber("LockObjectThresh", 236);
			SmartDashboard::PutBoolean("VisionFreeMode", true);
			m_pDrive->VisionTick();
			// Wait until Gripper successfully grabs cube.
			//if (m_pGripper->GetGripperState() == 0)
			//{
				// Move to Arm to Switch position.
				//m_pArmPosition->SetSetpoint(dArmPositionScalePosition);
				// Move to AutonomousVgetblock3.
				//m_nAutonomousState = eAutonomousVgetblock3;
			//}
			break;

		case eAutonomousVgetblock3 :
			SmartDashboard::PutString("Robot State", "Autonomous Vgetblock3");
			// Wait for arm to get to position before tracking vision.
			if (m_pArmPosition->IsReady())
			{
				m_nAutonomousState = eAutonomousVgetblock4;
			}
			break;

		case eAutonomousVgetblock4 :
			SmartDashboard::PutString("Robot State", "Autonomous Vgetblock4");
			// Go find tape
			///SmartDashboard::PutNumber("LockObjectThresh", 40);
			SmartDashboard::PutBoolean("VisionFreeMode", false);
			m_pDrive->VisionTick();
			if (SmartDashboard::GetNumber("VisionDistanceNumber", 10) >= SmartDashboard::GetNumber("LockObjectThresh", 40))
			{
				m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);
				m_nAutonomousState = eAutonomousIdle;
			}
			break;

		/********************************************************************
			Auton Cross Baseline - Drive forward to cross the baseline.
		********************************************************************/
		// Move forward 95 inches.
		case eAutonomousCrossBaseline1 :
			SmartDashboard::PutString("Robot State", "Autonomous CrossBaseline1");

			// Wait for the arm to finish homing.
			if (m_pArmPosition->IsHomingComplete() && m_pArmPosition->IsReady())
			{
				// Set the gripper to Idle (Close the claw and stop motors).
				m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
				// Set Arm Setpoint to Switch Eject position.
				m_pArmPosition->SetTolerance(5.000);
				m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
				// Move to eAutonomousCrossBaseline2.
				m_nAutonomousState = eAutonomousCrossBaseline2;
			}
			break;

		case eAutonomousCrossBaseline2 :
			SmartDashboard::PutString("Robot State", "Autonomous CrossBaseline2");

			AutonomousStartMove(3.0);

			// Set the drive setpoint to start moving.
			m_pDrive->SetSetpoint(-104.000, 104.000);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousIdle.
			m_nAutonomousState = eAutonomousIdle;
			break;

	/*******************************************************************
		Auton Left - Read the Game Data. If the Scale is ours, drive and
		eject the cube in the Scale. If the Switch is ours, drive to the
		switch and eject the cube. If neither the Scale or Switch are
		ours, drive to the Scale and hold the cube.
	********************************************************************/
		case eAutonomousLeft1 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 1");

			AutonomousStartMove(0.0, 2.0);

			// Check if the Scale or Switch is hot on the left side
			if (m_bScaleLeftHot)
			{
				// Drive to Scale position.
				m_pDrive->SetSetpoint(-300.000, 300.000); // -305, 305
			}
			else
			{
				if (m_bSwitchLeftHot)
				{
					// Drive to Switch position.
					m_pDrive->SetSetpoint(-146.500, 146.500);
				}
				else
				{
					// Neither the Scale or Switch is hot on the left. Drive to the row between the Scale and Switch.
					m_pDrive->SetSetpoint(-216.000, 216.000);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousLeft2.
			m_nAutonomousState = eAutonomousLeft2;
			break;

		case eAutonomousLeft2 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 2");

			// Wait for the move to complete and the arm to finish homing.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Move to eAutonomousLeft3.
				m_nAutonomousState = eAutonomousLeft3;
			}
			break;

		case eAutonomousLeft3 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 3");

			AutonomousStartMove(2.0, 4.0);

			// Turn the robot.
			if (m_bScaleLeftHot)
			{
				// Turn towards the Scale.
				m_pDrive->SetSetpoint(-17.000, -17.000);
				// Set Arm Setpoint to Scale Eject position.
				m_pArmPosition->SetTolerance(5.000);
				m_pArmPosition->SetSetpoint(dArmPositionScalePosition);
			}
			else
			{
				if (m_bSwitchLeftHot)
				{
					// Turn towards the Switch.
					m_pDrive->SetSetpoint(-26.000, -26.000);
					// Set Arm Setpoint to Switch Eject position.
					m_pArmPosition->SetTolerance(5.000);
					m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
				}
				else
				{
					// Turn to go between the Scale and the Switch.
					m_pDrive->SetSetpoint(-26.000, -26.000);
					// Set Arm Setpoint to Switch Eject position.
					m_pArmPosition->SetTolerance(5.000);
					m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousLeft4.
			m_nAutonomousState = eAutonomousLeft4;
			break;

		case eAutonomousLeft4 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 4");

			// Wait for the move to complete and the arm to finish moving to the Switch Eject position.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Move to eAutonomousLeft5.
				m_nAutonomousState = eAutonomousLeft5;
			}
			break;

		case eAutonomousLeft5 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 5");

			AutonomousStartMove(1.5, 2.0);

			// Check if the Scale or Switch is hot on the left side
			if (m_bScaleLeftHot)
			{
				// Drive to Scale position. (back up to the wall)
				m_pDrive->SetSetpoint(-28.000, -28.000);
			}
			else
			{
				if (m_bSwitchLeftHot)
				{
					// Drive to Switch position.
					m_pDrive->SetSetpoint(-19.000, 19.000);
				}
				else
				{
					// Neither the Scale or Switch is hot on the left. Drive  between the Scale and Switch.
					m_pDrive->SetSetpoint(-150.000, 150.000);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousLeft6.
			m_nAutonomousState = eAutonomousLeft6;
			break;

		case eAutonomousLeft6 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 6");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				if (m_bScaleLeftHot)
				{
					// Move to eAutonomousLeft7.
					m_nAutonomousState = eAutonomousLeft7;
				}
				else
				{
					if (m_bSwitchLeftHot)
					{
						// Eject the cube into the Switch.
						m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);
						// Move to eAutonomousIdle.
						m_nAutonomousState = eAutonomousIdle;
					}
					else
					{
						// Move to eAutonomousIdle
						m_nAutonomousState = eAutonomousIdle;
					}
				}
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
			}
			break;

		case eAutonomousLeft7 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 7");

			AutonomousStartMove(1.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Eject the cube into the Scale.
			m_pDrive->SetSetpoint(24.000, -24.000); //RLC added // 6 6 was pretty good // 8 8 was ok

			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eAutonomousLeft8.
			m_nAutonomousState = eAutonomousLeft8;
			break;

		case eAutonomousLeft8 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 8");

			// Wait for Encoder count to hit 14 to fire.
			if ((m_pDrive->GetLeftActual() < -14.000) && (m_pDrive->GetRightActual() > 14.000))
			{
				// FIRE!
				m_pGripper->SetGripperState(m_pGripper->eGripperScaleEject1A);
				// Move to eAutonomousLeft9
				m_nAutonomousState = eAutonomousLeft9;
			}
			break;

		case eAutonomousLeft9 :
			SmartDashboard::PutString("Robot State", "Autonomous Left 9");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				// Move to eAutonomousIdle.
				m_nAutonomousState = eAutonomousIdle;
			}
			break;

		/********************************************************************
			Auton Center - Read the Game Data. Drive forward, turn to the
			hot side of the Switch. Drive at an angle to the switch and eject
			the cube.
		********************************************************************/
		case eAutonomousCenter1 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 1");

			AutonomousStartMove(2.0, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Drive to forward.
			if (m_bSwitchLeftHot)
			{
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterLeft 1 Left", -18.00), SmartDashboard::GetNumber("CenterLeft 1 Right", 18.00));
///				m_pDrive->SetSetpoint(-18.000, 18.000);
			}
			else
			{
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterRight 1 Left", -30.00), SmartDashboard::GetNumber("CenterRight 1 Right", 30.00));
///				m_pDrive->SetSetpoint(-32.000, 32.000);
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousCenter2.
			m_nAutonomousState = eAutonomousCenter2;
			break;

		case eAutonomousCenter2 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 2");

			// Wait for the move to complete and the arm to finish homing.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Stop the drive before giving the next move command.
				m_pDrive->Stop();

				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);

				// Move to eAutonomousCenter3.
				m_nAutonomousState = eAutonomousCenter3;
			}
			break;

		case eAutonomousCenter3 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 3");

			AutonomousStartMove(0.5, 4.0);

			// Turn towards the hot side of the Switch.
			if (m_bSwitchLeftHot)
			{
				// Turn left.
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterLeft 2 Left", 11.40), SmartDashboard::GetNumber("CenterLeft 2 Right", 11.40));
///				m_pDrive->SetSetpoint(12.000, 12.000);
			}
			else
			{
				// Turn right.
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterRight 2 Left", -11.75), SmartDashboard::GetNumber("CenterRight 2 Right", -11.75));
///				m_pDrive->SetSetpoint((-1.000 * 12.000), (-1.000 * 12.000));
			}
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to Switch Eject position.
			m_pArmPosition->SetTolerance(5.000);
			m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);

			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousCenter4.
			m_nAutonomousState = eAutonomousCenter4;
			break;

		case eAutonomousCenter4 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 4");

			// Wait for the move to complete and the arm to finish moving to the Switch Eject position.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 2.000)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Stop the drive before giving the next move command.
				m_pDrive->Stop();

				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);

				// Move to eAutonomousCenter5.
				m_nAutonomousState = eAutonomousCenter5;
			}
			break;

		case eAutonomousCenter5 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 5");

			AutonomousStartMove(2.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Drive to forward.
			if (m_bSwitchLeftHot)
			{
				// Add a few inches to the outside turn wheels to help square up to the switch.
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterLeft 3 Left", -92.000), SmartDashboard::GetNumber("CenterLeft 3 Right", 92.000));
///				m_pDrive->SetSetpoint(-87.000, 87.000);
			}
			else
			{
				// Add a few inches to the outside turn wheels to help square up to the switch.
				m_pDrive->SetSetpoint(SmartDashboard::GetNumber("CenterRight 3 Left", -65.000), SmartDashboard::GetNumber("CenterRight 3 Right", 65.000));
///				m_pDrive->SetSetpoint(-85.000, 85.000);
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousCenter6.
			m_nAutonomousState = eAutonomousCenter6;
			break;

		case eAutonomousCenter6 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 6");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				// Eject the cube.
				m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);

				// Stop the drive.
				m_pDrive->Stop();

				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);

				if (m_bDoVision)
				{
					// Move to eAutonomousCenter7.
					m_nAutonomousState = eAutonomousCenter7;
				}
				else
				{
					// Move to eAutonomousIdle.
					m_nAutonomousState = eAutonomousIdle;
				}
			}
			break;

		case eAutonomousCenter7 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 7");

			AutonomousStartMove(2.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Reverse until safe turning point, turn to find cubes.
			m_pDrive->SetSetpoint(40.000, -40.000);
			// Move arm to back pickup position.
			m_pArmPosition->SetSetpoint(dArmPositionBackPickup);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousCenter8
			m_nAutonomousState = eAutonomousCenter8;
			break;

		case eAutonomousCenter8 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 8");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Move to eAutonomousCenter9.
				m_nAutonomousState = eAutonomousCenter9;
			}
			break;

		case eAutonomousCenter9 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 9");

			AutonomousStartMove(1.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Turn to find cubes.
		    if (m_bSwitchLeftHot)
		    {
		    	// Turn
		    	m_pDrive->SetSetpoint(24.000, 24.000);
		    }
		    else
		    {
		    	// Turn
		    	m_pDrive->SetSetpoint(-24.000, -24.000);
		    }
		    // Move to eAutonomousCenter10.
			m_nAutonomousState = eAutonomousCenter10;
			break;

		case eAutonomousCenter10 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 10");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				// Stop the drive before giving the next move command.
				m_pDrive->Stop();

				// Start claw intake.
				m_pGripper->SetGripperState(m_pGripper->eGripperOpenIntake1A);
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Move to eAutonomousCenter11.
				m_nAutonomousState = eAutonomousCenter11;
			}
			break;

		case eAutonomousCenter11 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 11");

			// Use Vision to find a cube.
			SmartDashboard::PutNumber("LockObjectThresh", 999);
			SmartDashboard::PutBoolean("VisionFreeMode", true);
			// Wait until Gripper successfully grabs cube.
			if (m_pGripper->GetGripperState() == 0)
			{
				// Call m_pDrive->Init() because it stops Vision?
				m_pDrive->Init();
				// Move to eAutonomousCenter12.
				m_nAutonomousState = eAutonomousCenter12;
			}
			else
			{
				// Cycle through the VisionTick until Gripper grabs a cube.
				m_pDrive->VisionTick();
			}
			break;

		case eAutonomousCenter12 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 12");

			AutonomousStartMove(2.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Reverse until safe turning point, turn to find reflective tape on Switch.
			m_pDrive->SetSetpoint(-36.000, 36.000);
		    // Raise arm to Switch position.
		    m_pArmPosition->SetSetpoint(dArmPositionSwitchBackPosition);
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousCenter13
			m_nAutonomousState = eAutonomousCenter13;
			break;

		case eAutonomousCenter13 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 13");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Reset Vision Distance Number to forget about the reflection of the arm.
				SmartDashboard::PutNumber("VisionDistanceNumber", 0);
			    // Start watching for tape (in case we overshoot the turn).
				SmartDashboard::PutBoolean("VisionFreeMode", false);
				SmartDashboard::PutNumber("LockObjectThresh", 999);
				// Move to eAutonomousCenter14.
				m_nAutonomousState = eAutonomousCenter14;
			}
			break;

		case eAutonomousCenter14 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 14");

			AutonomousStartMove(1.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Turn to find tape.
		    if (m_bSwitchLeftHot)
		    {
		    	// Turn Left
		    	m_pDrive->SetSetpoint(17.000, 17.000);
		    }
		    else
		    {
		    	// Turn Right
		    	m_pDrive->SetSetpoint(-17.000, -17.000);
		    }

		    // Move to eAutonomousCenter15.
		    m_nAutonomousState = eAutonomousCenter15;
		    break;

		case eAutonomousCenter15 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 15");

			// Wait for the move to complete or if it finds the tape.
			if ((m_pTimer->Get() > m_dDelayStartTime + 0.500) && (m_pDrive->IsReady() || (SmartDashboard::GetNumber("VisionPositionNumber", 0.000) >= 80.000 && m_pArmPosition->IsReady())))
			{
				// Stop the drive before giving the next move command.
				m_pDrive->Stop();

			    // Set VisionDistanceNumber to 0, or else it sees the arm.
			    SmartDashboard::PutNumber("VisionDistanceNumber", 0);

				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Move to eAutonomousCenter16.
				m_nAutonomousState = eAutonomousCenter16;
			}
			break;

		case eAutonomousCenter16 :
			SmartDashboard::PutString("Robot State", "Autonomous Center 16");

			// Wait until Vision gets to the Switch.
			if (SmartDashboard::GetNumber("VisionDistanceNumber", 0.00) >= 32.00)
			{
				// Eject cube.
				m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);
				// Call m_pDrive->Init() because it stops Vision?
				m_pDrive->Init();
				// Move to eAutonomousIdle.
				m_nAutonomousState = eAutonomousIdle;
			}
			else
			{
				// Cycle through the VisionTick
				m_pDrive->VisionTick();
			}
			break;

		/********************************************************************
			Auton Right - Read the Game Data. If the Scale is ours, drive and
			eject the cube in the Scale. If the Switch is ours, drive to the
			switch and eject the cube. If neither the Scale or Switch are
			ours, drive to the Scale and hold the cube.
		********************************************************************/
		case eAutonomousRight1 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 1");

			AutonomousStartMove(0.0, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Check if the Scale or Switch is hot on the right side
			if (!m_bScaleLeftHot)
			{
				// Drive to Scale position.
				m_pDrive->SetSetpoint(-300.000, 300.000);
			}
			else
			{
				if (!m_bSwitchLeftHot)
				{
					// Drive to Switch position.
					m_pDrive->SetSetpoint(-146.500, 146.500);
				}
				else
				{
					// Neither the Scale or Switch is hot on the right. Drive to the row between the Scale and Switch.
					m_pDrive->SetSetpoint(-210.000, 210.000);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousRight2.
			m_nAutonomousState = eAutonomousRight2;
			break;

		case eAutonomousRight2 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 2");

			// Wait for the move to complete and the arm to finish homing.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 1.000)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Move to eAutonomousRight3.
				m_nAutonomousState = eAutonomousRight3;
			}
			break;

		case eAutonomousRight3 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 3");

			AutonomousStartMove(2.0, 4.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Turn the robot.
			if (!m_bScaleLeftHot)
			{
				// Turn towards the Scale.
				m_pDrive->SetSetpoint(20.000, 20.000);
				// Set Arm Setpoint to Scale Eject position.
				m_pArmPosition->SetTolerance(5.000);
				m_pArmPosition->SetSetpoint(dArmPositionScalePosition);
			}
			else
			{
				if (!m_bSwitchLeftHot)
				{
					// Turn towards the Switch.
					m_pDrive->SetSetpoint(19.000, 19.000);
					// Set Arm Setpoint to Switch Eject position.
					m_pArmPosition->SetTolerance(5.000);
					m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
				}
				else
				{
					// Turn to go between the Scale and the Switch.
					m_pDrive->SetSetpoint(20.000, 20.000);  // 21, 21
					// Set Arm Setpoint to Switch Eject position.
					m_pArmPosition->SetTolerance(5.000);
					m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousRight4.
			m_nAutonomousState = eAutonomousRight4;
			break;

		case eAutonomousRight4 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 4");

			// Wait for the move to complete and the arm to finish moving to the Switch Eject position.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady() && m_pArmPosition->IsReady())
			{
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
				// Move to eAutonomousRight5.
				m_nAutonomousState = eAutonomousRight5;
			}
			break;

		case eAutonomousRight5 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 5");

			AutonomousStartMove(1.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Check if the Scale or Switch is hot on the right side
			if (!m_bScaleLeftHot)
			{
				// Drive to Scale position. (back up to the wall)
				m_pDrive->SetSetpoint(28.000, -28.000); //RLC playing 5 -5 // 10 10 was pretty good // 12 12 ouch
			}
			else
			{
				if (!m_bSwitchLeftHot)
				{
					// Drive to Switch position.
					m_pDrive->SetSetpoint(-19.000, 19.000);
				}
				else
				{
					// Neither the Scale or Switch is hot on the right. Drive  between the Scale and Switch.
					m_pDrive->SetSetpoint(-150.000, 150.000);
				}
			}
			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();

			// Move to eAutonomousRight6.
			m_nAutonomousState = eAutonomousRight6;
			break;

		case eAutonomousRight6 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 6");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				if (!m_bScaleLeftHot)
				{
					// Move to eAutonomousRight7.
					m_nAutonomousState = eAutonomousRight7;
				}
				else
				{
					if (!m_bSwitchLeftHot)
					{
						// Eject the cube into the Switch.
						m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);
						// Move to eAutonomousIdle.
						m_nAutonomousState = eAutonomousIdle;
					}
					else
					{
						// Move to eAutonomousIdle.
						m_nAutonomousState = eAutonomousIdle;
					}
				}
				// Reset the maximum time to reach setpoint back to zero.
				m_pDrive->SetLeftMaxFindingTime(0.000);
				m_pDrive->SetRightMaxFindingTime(0.000);
			}
			break;

		case eAutonomousRight7 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 7");

			AutonomousStartMove(1.5, 2.0, (0.500 * SmartDashboard::GetNumber("Left Scale Speed", 1.00)),
										  (0.500 * SmartDashboard::GetNumber("Right Scale Speed", 1.00)));

			// Eject the cube into the Scale.
			m_pDrive->SetSetpoint(-24.000, 24.000); //RLC added // 6 6 was pretty good // 8 8 was ok

			// Get current time.
			m_dDelayStartTime = m_pTimer->Get();
			// Move to eAutonomousRight8.
			m_nAutonomousState = eAutonomousRight8;
			break;

		case eAutonomousRight8 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 8");

			// Wait for Encoder count to hit 14 to fire.
			if ((m_pDrive->GetLeftActual() < -7.000) && (m_pDrive->GetRightActual() > 7.000))
			{
				// FIRE!!!
				m_pGripper->SetGripperState(m_pGripper->eGripperScaleEject1A);
				// Move to eAutonomousRight9
				m_nAutonomousState = eAutonomousRight9;
			}
			break;

		case eAutonomousRight9 :
			SmartDashboard::PutString("Robot State", "Autonomous Right 9");

			// Wait for the move to complete.
			if ((m_pTimer->Get() > (m_dDelayStartTime + 0.500)) && m_pDrive->IsReady())
			{
				// Move to eAutonomousIdle.
				m_nAutonomousState = eAutonomousIdle;
			}
	}
}

/******************************************************************************
	TeleopInit:	Called one time, each time the robot enters the teleop state.

	Arguments: 	None

	Returns: 	Nothing
******************************************************************************/
void CRobotMain::TeleopInit()
{
	// Start the compressor.
	m_pCompressor->Start();

	// Initialize drive parameters.
	m_pDrive->Init();

	// Set the Drive to joystick control.
	m_pDrive->SetJoystickControl(true);

	// MotorSafety improves safety when motors are updated in loops.
	m_pDrive->SetMotorSafety(false);

	// Make sure we are in Cube tracking mode.
	SmartDashboard::PutBoolean("VisionFreeMode", true);
}

/******************************************************************************
	TeleopPeriodic:	Called based on the TimedRobot's period any time the
					robot is in teleop mode and enabled.

	Arguments: 		None

	Returns: 		Nothing
******************************************************************************/
void CRobotMain::TeleopPeriodic()
{
	/********************************************************************
		Drive Controller - Call vision tick if button is held down.
	********************************************************************/
	static bool bGripperSet = false;

	// Check to see if Left Trigger was pressed.
	if (m_pDriveController->GetRawAxis(2) >= 0.5) // Note that this should be looped.
	{
///		if (bGripperSet == false)
///		{
///			// Open claw for intake.
///			m_pGripper->SetGripperState(m_pGripper->eGripperOpenIntake1A);
///			bGripperSet = true;
///		}
		m_bDriveControllerLeftTriggerPressed = true;

		// Call VisionTick.
		m_pDrive->VisionTick();

/// 	// Check if the claw has a cube and move the arm to Switch Position.
///		if (m_pGripper->eGripperIdle)
///		{
///			m_nTeleOpState = eTeleOpArmSwitchPosition1;
///		}
	}
	if (m_pDriveController->GetRawAxis(2) < 0.5 && m_bDriveControllerLeftTriggerPressed)
	{
		// Reset to Joystick control to avoid "sluggish motor syndrome."
		m_pDrive->Init();
		// The gripper should no longer be set, set it to false.
		bGripperSet = false;
		m_bDriveControllerLeftTriggerPressed = false;
	}

	/********************************************************************
		Drive Controller - Toggle between front and back cameras.
	********************************************************************/
	// Check to see if Button 3 was pressed.
	if (m_pDriveController->GetRawButton(eButtonA) && !m_bDriveControllerAButtonPressed)
	{
		m_bDriveControllerAButtonPressed = true;

		// Toggle the camera.
		m_pVision->SwitchCamera();
	}
	else
	{
		if (!m_pDriveController->GetRawButton(eButtonA))
		{
			m_bDriveControllerAButtonPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Cube Eject
	********************************************************************/
	// Check to see if Right Trigger was pressed
	if (m_pDriveController->GetRawAxis(3) >= 0.5 && !m_bDriveControllerRightTriggerPressed)
	{
		m_bDriveControllerRightTriggerPressed = true;

		// Get current Arm Position.
		double dArmPosition = m_pArmPosition->GetActual();
		if ((dArmPosition > (dArmPositionScalePosition - (dArmPositionTolerance + 15.000))) &&
			(dArmPosition < (dArmPositionScalePosition + (dArmPositionTolerance + 15.000))))
		{
			// Eject Power Cube to Scale (Uses Plunger).
			m_nTeleOpState = eTeleOpCubeScaleEject1;
		}
		else
		{
			// Eject Power Cube to Switch (Uses Rollers).
			m_nTeleOpState = eTeleOpCubeFloorEject1;
		}
	}
	else
	{
		if (m_pDriveController->GetRawAxis(3) < 0.5)
		{
			m_bDriveControllerRightTriggerPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Cube Closed Intake.
	********************************************************************/
	// Check to see if Left Bumper was pressed.
	if (m_pDriveController->GetRawButton(eLeftBumper) && !m_bDriveControllerLeftBumperPressed)
	{
		m_bDriveControllerLeftBumperPressed = true;
		m_nTeleOpState = eTeleOpCubeClosedIntake;
	}
	else
	{
		if (m_pDriveController->GetRawButton(eLeftBumper))
		{
			m_bDriveControllerLeftBumperPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Cube Open Intake.
	********************************************************************/
	// Check to see if Right Bumper was pressed
	if (m_pDriveController->GetRawButton(eRightBumper) && !m_bDriveControllerRightBumperPressed)
	{
		m_bDriveControllerRightBumperPressed = true;
		m_nTeleOpState = eTeleOpCubeOpenIntake;
	}
	else
	{
		if (m_pDriveController->GetRawButton(eRightBumper))
		{
			m_bDriveControllerRightBumperPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Move Arm to Cube Shoot in Down Position
	********************************************************************/
	if (m_pDriveController->GetRawButton(eButtonY) && !m_bDriveControllerYButtonPressed)
	{
		m_bDriveControllerYButtonPressed = true;
		m_nTeleOpState = eTeleOpArmScaleLowPosition1;
	}
	else
	{
		if (m_pDriveController->GetRawButton(eButtonY))
		{
			m_bDriveControllerYButtonPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Home the Arm
	********************************************************************/
	if ((m_pAuxController->GetPOV() == 270) && !m_bAuxControllerPOVLeftPressed)
	{
		m_bAuxControllerPOVLeftPressed = true;
		m_nTeleOpState = eTeleOpArmHoming1;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 270))
		{
			m_bAuxControllerPOVLeftPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Front Floor Pickup Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eButtonA) && !m_bAuxControllerButtonAPressed)
	{
		m_bAuxControllerButtonAPressed = true;
		m_nTeleOpState = eTeleOpArmFrontFloorPosition1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eButtonA))
		{
			m_bAuxControllerButtonAPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Back Floor Pickup Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eButtonB) && !m_bAuxControllerButtonBPressed)
	{
		m_bAuxControllerButtonBPressed = true;
		m_nTeleOpState = eTeleOpArmBackFloorPosition1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eButtonB))
		{
			m_bAuxControllerButtonBPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Switch Eject Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eButtonX) && !m_bAuxControllerButtonXPressed)
	{
		m_bAuxControllerButtonXPressed = true;
		m_nTeleOpState = eTeleOpArmSwitchPosition1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eButtonX))
		{
			m_bAuxControllerButtonXPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Front High Cube Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eRightBumper) && !m_bAuxControllerButtonRBPressed)
	{
		m_bAuxControllerButtonRBPressed = true;
		m_nTeleOpState = eTeleOpArmHighCubeFront1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eRightBumper))
		{
			m_bAuxControllerButtonRBPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Back High Cube Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eLeftBumper) && !m_bAuxControllerButtonLBPressed)
	{
		m_bAuxControllerButtonLBPressed = true;
		m_nTeleOpState = eTeleOpArmHighCubeBack1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eLeftBumper))
		{
			m_bAuxControllerButtonLBPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Move Arm to Scale Eject Position
	********************************************************************/
	if (m_pAuxController->GetRawButton(eButtonY) && !m_bAuxControllerButtonYPressed)
	{
		m_bAuxControllerButtonYPressed = true;
		m_nTeleOpState = eTeleOpArmScalePosition1;
	}
	else
	{
		if (m_pAuxController->GetRawButton(eButtonY))
		{
			m_bAuxControllerButtonYPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Reset Claw to Idle
	********************************************************************/
	if ((m_pAuxController->GetPOV() == 90) && !m_bAuxControllerPOVRightPressed)
	{
		m_bAuxControllerPOVRightPressed = true;
		m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 90))
		{
			m_bAuxControllerPOVRightPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Manually Move Arm Up
	********************************************************************/
	// Check to see if POV Up was pressed
	if ((m_pAuxController->GetPOV() == 0) && !m_bAuxControllerPOVUpPressed)
	{
		m_pArmPosition->JogArm(true);
		m_bAuxControllerPOVUpPressed = true;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 0) && m_bAuxControllerPOVUpPressed)
		{
			m_pArmPosition->Stop();
			m_bAuxControllerPOVUpPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Manually Move Arm Down
	********************************************************************/
	// Check to see if POV Down was pressed
	if ((m_pAuxController->GetPOV() == 180) && !m_bAuxControllerPOVDownPressed)
	{
		m_pArmPosition->JogArm(false);
		m_bAuxControllerPOVDownPressed = true;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 180) && m_bAuxControllerPOVDownPressed)
		{
			m_pArmPosition->Stop();
			m_bAuxControllerPOVDownPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Deploy Left Ramp.
	********************************************************************/
	// Check to see if Button 5 was pressed, and if the Left Trigger is greater than 0.9.
	if (m_pDriveController->GetRawButton(eBack) && (m_pAuxController->GetRawAxis(2) > 0.900) && !m_bDriveControllerBackPressed)
	{
		m_bDriveControllerBackPressed = true;
		m_pLeftRamp->DeployRamp();
	}
	else
	{
		if (!m_pDriveController->GetRawButton(eBack))
		{
			m_bDriveControllerBackPressed = false;
		}
	}

	/********************************************************************
		Drive Controller - Deploy Right Ramp.
	********************************************************************/
	// Check to see if Button 6 was pressed, and if the Right Trigger is greater than 0.9.
	if (m_pDriveController->GetRawButton(eStart) && (m_pAuxController->GetRawAxis(3) > 0.900) && !m_bDriveControllerStartPressed)
	{
		m_bDriveControllerStartPressed = true;
		m_pRightRamp->DeployRamp();
	}
	else
	{
		if (!m_pDriveController->GetRawButton(eStart))
		{
			m_bDriveControllerStartPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Start Left Ramp Motor.
	********************************************************************/
	// Check to see if the Back Button was pressed.
	if (m_pAuxController->GetRawButton(eBack) && !m_bAuxControllerBackPressed)
	{
		m_bAuxControllerBackPressed = true;
		m_pLeftRamp->StartRampMotor();
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eBack))
		{
			m_bAuxControllerBackPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Start Right Ramp Motor.
	********************************************************************/
	// Check to see if the Start Button was pressed.
	if (m_pAuxController->GetRawButton(eStart) && !m_bAuxControllerStartPressed)
	{
		m_bAuxControllerStartPressed = true;
		m_pRightRamp->StartRampMotor();
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eStart))
		{
			m_bAuxControllerStartPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Stop Left Ramp Motor.
	********************************************************************/
	// Check to see if the Left Analog stick was pressed.
	if (m_pAuxController->GetRawButton(eLeftAnalog) && !m_bAuxControllerLeftAnalogPressed)
	{
		m_bAuxControllerLeftAnalogPressed = true;
		m_pLeftRamp->StopRampMotor();
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eLeftAnalog))
		{
			m_bAuxControllerLeftAnalogPressed = false;
		}
	}

	/********************************************************************
		Aux Controller - Stop Right Ramp Motor.
	********************************************************************/
	// Check to see if the Right Analog stick was pressed.
	if (m_pAuxController->GetRawButton(eRightAnalog) && !m_bAuxControllerRightAnalogPressed)
	{
		m_bAuxControllerRightAnalogPressed = true;
		m_pRightRamp->StopRampMotor();
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eRightAnalog))
		{
			m_bAuxControllerRightAnalogPressed = false;
		}
	}

	// Call the Teleop State Machine.
	TeleOpStateMachine();

	// Call the drive tick.
	m_pDrive->Tick(false);

	// Call the Arm Position tick.
	m_pArmPosition->Tick();

	// Call the Gripper tick.
	m_pGripper->Tick();

	// Call both Ramp ticks.
	m_pLeftRamp->Tick();
	m_pRightRamp->Tick();
}

/******************************************************************************
	TeleOpStateMachine - This is the main state machine used during the
						 Operator Control period.

	Arguments: 	None

	Returns: 	Nothing
******************************************************************************/
void CRobotMain::TeleOpStateMachine()
{
	// TeleOp State Machine.
	switch (m_nTeleOpState)
	{
		case eTeleOpIdle :
			SmartDashboard::PutString("Robot State", "TeleOp Idle");
			break;

		case eTeleOpArmHoming1 :
			SmartDashboard::PutString("Robot State", "Arm Homing 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Start homing the arm.
			m_pArmPosition->StartHoming();
			// Move to eTeleOpArmHoming2.
			m_nTeleOpState = eTeleOpArmHoming2;
			break;

		case eTeleOpArmHoming2 :
			SmartDashboard::PutString("Robot State", "Arm Homing 2");
			// Wait for arm to finish homing.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmFrontFloorPosition1 :
			SmartDashboard::PutString("Robot State", "Arm FrontFloorPickup 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to 0 degrees.
			m_pArmPosition->SetSetpoint(dArmPositionFrontPickup);
			m_nTeleOpState = eTeleOpArmFrontFloorPosition2;
			break;

		case eTeleOpArmFrontFloorPosition2 :
			SmartDashboard::PutString("Robot State", "Arm FrontFloorPickup 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmBackFloorPosition1 :
			SmartDashboard::PutString("Robot State", "Arm BackFloorPickup 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to 180 degrees.
			m_pArmPosition->SetSetpoint(dArmPositionBackPickup);
			m_nTeleOpState = eTeleOpArmBackFloorPosition2;
			break;

		case eTeleOpArmBackFloorPosition2 :
			SmartDashboard::PutString("Robot State", "Arm BackFloorPickup 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmSwitchPosition1 :
			SmartDashboard::PutString("Robot State", "Arm SwitchPosition 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to Switch Eject position.
			m_pArmPosition->SetSetpoint(dArmPositionSwitchPosition);
			m_nTeleOpState = eTeleOpArmSwitchPosition2;
			break;

		case eTeleOpArmSwitchPosition2 :
			SmartDashboard::PutString("Robot State", "Arm SwitchPosition 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmHighCubeFront1 :
			SmartDashboard::PutString("Robot State", "Arm HighCubeFront 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to Front High Cube Pickup position.
			m_pArmPosition->SetSetpoint(dArmPositionHighCubeFrontPosition);
			m_nTeleOpState = eTeleOpArmSwitchPosition2;
			break;

		case eTeleOpArmHighCubeFront2 :
			SmartDashboard::PutString("Robot State", "Arm HighCubeFront 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmHighCubeBack1 :
			SmartDashboard::PutString("Robot State", "Arm HighCubeBack 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to Back High Cube Pickup position.
			m_pArmPosition->SetSetpoint(dArmPositionHighCubeBackPosition);
			m_nTeleOpState = eTeleOpArmSwitchPosition2;
			break;

		case eTeleOpArmHighCubeBack2 :
			SmartDashboard::PutString("Robot State", "Arm HighCubeBack 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmScaleLowPosition1 :
			SmartDashboard::PutString("Robot State", "Arm ScaleShootCubeDown 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			// Set Arm Setpoint to Upsidedown Cube Shooting position.
			m_pArmPosition->SetSetpoint(dArmPositionShootCubeDownPosition);
			m_nTeleOpState = eTeleOpArmScaleLowPosition2;
			break;

		case eTeleOpArmScaleLowPosition2:
			SmartDashboard::PutString("Robot State", "Arm ScaleShootCubeDown 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpArmScalePosition1 :
			SmartDashboard::PutString("Robot State", "Arm ScalePosition 1");
			// Set the gripper to Idle (Close the claw and stop motors).
			m_pGripper->SetGripperState(m_pGripper->eGripperIdle);
			m_pArmPosition->SetSetpoint(dArmPositionScalePosition);
			m_nTeleOpState = eTeleOpArmScalePosition2;
			break;

		case eTeleOpArmScalePosition2 :
			SmartDashboard::PutString("Robot State", "Arm ScalePosition 2");
			// Wait for arm to finish moving.
			if (m_pArmPosition->IsReady())
			{
				// Finished, move to eTeleOpIdle.
				m_nTeleOpState = eTeleOpIdle;
			}
			break;

		case eTeleOpCubeOpenIntake :
			SmartDashboard::PutString("Robot State", "Arm Open Intake");
			m_pGripper->SetGripperState(m_pGripper->eGripperOpenIntake1A);
			// Finished, move to eTeleOpIdle.
			m_nTeleOpState = eTeleOpIdle;
			break;

		case eTeleOpCubeClosedIntake :
			SmartDashboard::PutString("Robot State", "Arm Closed Intake");
			m_pGripper->SetGripperState(m_pGripper->eGripperClosedIntake1A);
			// Finished, move to eTeleOpIdle.
			m_nTeleOpState = eTeleOpIdle;
			break;

		case eTeleOpCubeFloorEject1 :
			SmartDashboard::PutString("Robot State", "Arm Eject 1");
			m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject1A);
			// Finished, move to eTeleOpIdle.
			m_nTeleOpState = eTeleOpIdle;
			break;

		case eTeleOpCubeFloorEject2 :
			SmartDashboard::PutString("Robot State", "Arm Drop 1");
			m_pGripper->SetGripperState(m_pGripper->eGripperFloorEject2A);
			// Finished, move to eTeleOpIdle.
			m_nTeleOpState = eTeleOpIdle;
			break;

		case eTeleOpCubeScaleEject1 :
			SmartDashboard::PutString("Robot State", "Arm Eject 2");
			m_pGripper->SetGripperState(m_pGripper->eGripperScaleEject1A);
			// Finished, move to eTeleOpIdle.
			m_nTeleOpState = eTeleOpIdle;
			break;
	}
}

/******************************************************************************
	TestInit:	Called one time, each time the robot enters the test state.

	Arguments: 	None

	Returns: 	Nothing
******************************************************************************/
void CRobotMain::TestInit()
{
    Scheduler::GetInstance()->SetEnabled(true);
    LiveWindow::GetInstance()->SetEnabled(true);

	// Start the compressor.
	m_pCompressor->Start();

    // Initialize drive parameters.
	m_pDrive->Init();

	// Set the Drive to joystick control.
	m_pDrive->SetJoystickControl(true);

	// MotorSafety improves safety when motors are updated in loops.
	m_pDrive->SetMotorSafety(false);
}

/******************************************************************************
	TestPeriodic:	Called based on the TimedRobot's period any time the
					robot is in test mode and enabled.

	Arguments: 		None

	Returns: 		Nothing
******************************************************************************/
void CRobotMain::TestPeriodic()
{
	/********************************************************************
		Home Arm Up (POV Left Button).
	********************************************************************/
	// Check to see if POV Up was pressed
	if ((m_pAuxController->GetPOV() == 270) && !m_bAuxControllerPOVLeftPressed)
	{
		m_pArmPosition->StartHoming();
		m_bAuxControllerPOVLeftPressed = true;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 270) && m_bAuxControllerPOVLeftPressed)
		{
			m_bAuxControllerPOVLeftPressed = false;
		}
	}

	/********************************************************************
		Jog Arm Up (POV Up Button).
	********************************************************************/
	// Check to see if POV Up was pressed
	if ((m_pAuxController->GetPOV() == 0) && !m_bAuxControllerPOVUpPressed)
	{
		m_pArmPosition->JogArm(true);
		m_bAuxControllerPOVUpPressed = true;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 0) && m_bAuxControllerPOVUpPressed)
		{
			m_pArmPosition->Stop();
			m_bAuxControllerPOVUpPressed = false;
		}
	}

	/********************************************************************
		Jog Arm Down (POV Down Button).
	********************************************************************/
	// Check to see if POV Down was pressed
	if ((m_pAuxController->GetPOV() == 180) && !m_bAuxControllerPOVDownPressed)
	{
		m_pArmPosition->JogArm(false);
		m_bAuxControllerPOVDownPressed = true;
	}
	else
	{
		if (!(m_pAuxController->GetPOV() == 180) && m_bAuxControllerPOVDownPressed)
		{
			m_pArmPosition->Stop();
			m_bAuxControllerPOVDownPressed = false;
		}
	}

	/********************************************************************
		Test Intake Motors (Reverse - Left Trigger).
						   (Forward - Right Trigger)n .
	********************************************************************/
	// Check if the Left Trigger value is outside of the dead zone.
	if ((fabs(m_pAuxController->GetRawAxis(2)) > 0.250) && fabs(m_pAuxController->GetRawAxis(3)) < 0.250)
	{
		m_pGripper->TestLeftMotor(m_pAuxController->GetRawAxis(2) * -1.000);
		m_pGripper->TestRightMotor(m_pAuxController->GetRawAxis(2) * -1.000);
	}
	else
	{
		// Check if the Right Trigger value is outside of the dead zone.
		if ((fabs(m_pAuxController->GetRawAxis(3)) > 0.250) && fabs(m_pAuxController->GetRawAxis(2)) < 0.250)
		{
			m_pGripper->TestLeftMotor(m_pAuxController->GetRawAxis(3));
			m_pGripper->TestRightMotor(m_pAuxController->GetRawAxis(3));
		}
		else
		{
			m_pGripper->TestLeftMotor(0.000);
			m_pGripper->TestRightMotor(0.000);
		}
	}

	/********************************************************************
		Test Plunger Solenoid (Left Bumper).
	********************************************************************/
	if (m_pAuxController->GetRawButton(eLeftBumper) && !m_bAuxControllerButtonLBPressed && m_pAuxController->GetRawButton(eRightBumper))
	{
		m_bAuxControllerButtonLBPressed = true;
		m_pGripper->TestExtendPlunger(true);
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eLeftBumper) && m_bAuxControllerButtonLBPressed)
		{
			m_bAuxControllerButtonLBPressed = false;
			m_pGripper->TestExtendPlunger(false);
		}
	}

	/********************************************************************
		Test Claw Solenoids (Right Bumper).
	********************************************************************/
	if (m_pAuxController->GetRawButton(eRightBumper) && !m_bAuxControllerButtonRBPressed)
	{
		m_bAuxControllerButtonRBPressed = true;
		m_pGripper->TestOpenClaw(true);
	}
	else
	{
		if (!m_pAuxController->GetRawButton(eRightBumper) && m_bAuxControllerButtonRBPressed)
		{
			m_bAuxControllerButtonRBPressed = false;
			m_pGripper->TestOpenClaw(false);
		}
	}

	/********************************************************************
		Test Left Ramp Motor (Start Button + Left 'Y' Axis (1)).
	********************************************************************/
	if (m_pAuxController->GetRawButton(eStart) && (fabs(m_pAuxController->GetRawAxis(1)) > 0.250))
	{
		m_pLeftRamp->m_pRampMotor->Set(ControlMode::PercentOutput, m_pAuxController->GetRawAxis(1));
	}
	else
	{
		// Stop the Left Ramp Motor.
		m_pLeftRamp->m_pRampMotor->Set(ControlMode::PercentOutput, 0.000);
	}

	/********************************************************************
		Test Right Ramp Motor (Back Button + Right 'Y' Axis (5)).
	********************************************************************/
	if (m_pAuxController->GetRawButton(eBack) && (fabs(m_pAuxController->GetRawAxis(5)) > 0.250))
	{
		m_pRightRamp->m_pRampMotor->Set(ControlMode::PercentOutput, (m_pAuxController->GetRawAxis(5) * -1.000));
	}
	else
	{
		// Stop the Right Ramp Motor.
		m_pRightRamp->m_pRampMotor->Set(ControlMode::PercentOutput, 0.000);
	}

	m_pArmPosition->Tick();

	/********************************************************************
	    Put test information on SmartDashboard.
	********************************************************************/
	// Drive Information.
	SmartDashboard::PutNumber("Left Drive Actual", 	m_pDrive->GetLeftActual());
	SmartDashboard::PutNumber("Right Drive Actual", m_pDrive->GetRightActual());

	// Arm Information.
	SmartDashboard::PutBoolean("Arm Home Switch",	m_pArmPosition->IsHomeSwitchPressed());
	SmartDashboard::PutBoolean("Arm Fwd Switch",	m_pArmPosition->IsForwardSwitchPressed());

	// Gripper Information.
	SmartDashboard::PutBoolean("Cube Sensor", 		m_pGripper->TestGetCubeSensor());

	// Ramp Information.
	SmartDashboard::PutBoolean("Left Ramp Sensor", 	m_pLeftRamp->GetRampSensor());
	SmartDashboard::PutBoolean("Right Ramp Sensor", m_pRightRamp->GetRampSensor());

	SmartDashboard::UpdateValues();
}
///////////////////////////////////////////////////////////////////////////////
START_ROBOT_CLASS(CRobotMain);

/******************************************************************************
	Description:	2018 PowerUp Robot Control Software.

	Classes:		CRobotMain

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER Robotics.
******************************************************************************/
#ifndef RobotMain_h
#define RobotMain_h
#pragma once

#include <LiveWindow/LiveWindow.h>

// Forward Class References.
class CDrive;
class CArmPosition;
class CGripper;
class CRamp;
class CVision;
////////////////////////////////////////////////////////////////////////////////


/******************************************************************************
	Description:	CRobotMain control class.

	Arguments:		None

	Derived From:	TimedRobot
******************************************************************************/
class CRobotMain : public TimedRobot
{
public:
	CRobotMain();
	~CRobotMain();

private:
	void RobotInit();
	void RobotPeriodic();
	void DisabledInit();
	void DisabledPeriodic();
	void AutonomousInit();
	void AutonomousPeriodic();
	void AutonomousStartMove(double dTimeout, double dTolerance, double dLeftOutputPercent, double dRightOutputPercent, double dRampRate, double dIZone);
	void AutonomousStateMachine();
	void TeleopInit();
	void TeleopPeriodic();
	void TeleOpStateMachine();
	void TestInit();
	void TestPeriodic();

	// Object pointers
	Joystick*						m_pDriveController;			// Pointer to the drive joystick
	Joystick*						m_pAuxController;			// Pointer to the second joystick
	Compressor*						m_pCompressor;
	CDrive*							m_pDrive;
	CArmPosition*					m_pArmPosition;
	CGripper*						m_pGripper;
	CRamp*							m_pLeftRamp;
	CRamp*							m_pRightRamp;
	CVision*						m_pVision;
	Timer*							m_pTimer;
	SendableChooser<std::string>*	m_pAutonomousChooser;
	LiveWindow*						m_pLiveWindow;

	// Variables
	bool			m_bDriveControllerStartPressed;
	bool			m_bDriveControllerBackPressed;
	bool			m_bDriveControllerAButtonPressed;
	bool			m_bDriveControllerBButtonPressed;
	bool			m_bDriveControllerXButtonPressed;
	bool			m_bDriveControllerYButtonPressed;
	bool			m_bDriveControllerLeftTriggerPressed;
	bool			m_bDriveControllerRightTriggerPressed;
	bool			m_bDriveControllerLeftBumperPressed;
	bool			m_bDriveControllerRightBumperPressed;
	bool			m_bAuxControllerBackPressed;
	bool			m_bAuxControllerStartPressed;
	bool			m_bAuxControllerButtonAPressed;
	bool			m_bAuxControllerButtonBPressed;
	bool			m_bAuxControllerButtonXPressed;
	bool			m_bAuxControllerButtonYPressed;
	bool			m_bAuxControllerButtonLBPressed;
	bool			m_bAuxControllerButtonRBPressed;
	bool			m_bAuxControllerLeftAnalogPressed;
	bool			m_bAuxControllerRightAnalogPressed;
	bool			m_bAuxControllerPOVLeftPressed;
	bool			m_bAuxControllerPOVRightPressed;
	bool			m_bAuxControllerPOVUpPressed;
	bool			m_bAuxControllerPOVDownPressed;
	bool			m_bScaleLeftHot;
	bool			m_bSwitchLeftHot;
	bool			m_bDoVision;
	int				m_nAutonomousState;
	int				m_nTeleOpState;
	double			m_dDelayStartTime;
	std::string		m_strGameSpecificMessage;
	std::string		m_strAllianceSelected;
	std::string		m_strAutonomousSelected;

	// Constants.
	const std::string	m_strRedAlliance					= "Red Alliance";
	const std::string	m_strBlueAlliance					= "Blue Alliance";
	const std::string	m_strAutonomousIdle					= "Idle";
	const std::string	m_strAutonomousVgetblock			= "Get Block";
	const std::string	m_strAutonomousCrossBaseline		= "Cross Baseline";
	const std::string	m_strAutonomousLeft					= "Auton Left";
	const std::string	m_strAutonomousCenter				= "Auton Center";
	const std::string	m_strAutonomousRight				= "Auton Right";

	// Autonomous State Machine
	enum AutonomousStates
	{
		eAutonomousIdle,

		eAutonomousVgetblock1,
		eAutonomousVgetblock2,
		eAutonomousVgetblock3,
		eAutonomousVgetblock4,

		eAutonomousCrossBaseline1,
		eAutonomousCrossBaseline2,

		eAutonomousLeft1,
		eAutonomousLeft2,
		eAutonomousLeft3,
		eAutonomousLeft4,
		eAutonomousLeft5,
		eAutonomousLeft6,
		eAutonomousLeft7,
		eAutonomousLeft8,
		eAutonomousLeft9,

		eAutonomousCenter1,
		eAutonomousCenter2,
		eAutonomousCenter3,
		eAutonomousCenter4,
		eAutonomousCenter5,
		eAutonomousCenter6,
		eAutonomousCenter7,
		eAutonomousCenter8,
		eAutonomousCenter9,
		eAutonomousCenter10,
		eAutonomousCenter11,
		eAutonomousCenter12,
		eAutonomousCenter13,
		eAutonomousCenter14,
		eAutonomousCenter15,
		eAutonomousCenter16,

		eAutonomousRight1,
		eAutonomousRight2,
		eAutonomousRight3,
		eAutonomousRight4,
		eAutonomousRight5,
		eAutonomousRight6,
		eAutonomousRight7,
		eAutonomousRight8,
		eAutonomousRight9
	};

	// TeleOp State Machine
	enum TeleOpStates
	{
		eTeleOpIdle,
		eTeleOpArmHoming1,
		eTeleOpArmHoming2,
		eTeleOpArmFrontFloorPosition1,
		eTeleOpArmFrontFloorPosition2,
		eTeleOpArmBackFloorPosition1,
		eTeleOpArmBackFloorPosition2,
		eTeleOpArmSwitchPosition1,
		eTeleOpArmSwitchPosition2,
		eTeleOpArmHighCubeFront1,
		eTeleOpArmHighCubeFront2,
		eTeleOpArmHighCubeBack1,
		eTeleOpArmHighCubeBack2,
		eTeleOpArmScaleLowPosition1,
		eTeleOpArmScaleLowPosition2,
		eTeleOpArmScalePosition1,
		eTeleOpArmScalePosition2,
		eTeleOpCubeOpenIntake,
		eTeleOpCubeClosedIntake,
		eTeleOpCubeFloorEject1,
		eTeleOpCubeFloorEject2,
		eTeleOpCubeScaleEject1
	};
};
///////////////////////////////////////////////////////////////////////////////
#endif

/****************************************************************************
	Description:	Defines the CGripper control class.

	Classes:		CGripper

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef Gripper_h
#define Gripper_h
#pragma once

#include "WPILib.h"
#include <ctre/Phoenix.h>

// Default Constants set for gripper class.
const double dDefaultIntakeMotorSpeed				=  0.800;
const double dDefaultEjectMotorSpeed				= -1.000;
const double dDefaultIntakeReadSensorDelay			=  0.120;
const double dDefaultIntakeCloseClawDelay			=  0.220;
const double dDefaultIntakeStopMotorDelay			=  0.200;
const double dDefaultFloorEjectStopMotorDelay		=  0.500;
const double dDefaultScaleEjectPlungerDelay			=  0.015;
const double dDefaultScaleEjectPlungerReturnDelay	=  0.250;
const double dDefaultScaleEjectCloseClawDelay		=  1.500;
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CGripper control class.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
class CGripper
{
public:
	CGripper(Joystick* pDriveController);
	~CGripper();

	void	Init();
	void 	Tick();
	void	GripperStateMachine();
	void	SetGripperState(int nGripperState);
	int 	GetGripperState()						{ return m_nGripperState; };
	bool	TestGetCubeSensor();
	void	TestExtendPlunger(bool bExtended);
	void	TestOpenClaw(bool bClawOpen);
	void	TestLeftMotor(double dSetpoint);
	void	TestRightMotor(double dSetpoint);
	void	TestEnableVacuum(bool bEnableVacuum);
	void	SetVacuumSolenoid(bool bSetVacuumSolenoid);

	// Gripper state machine states.
	enum GripperStates
	{
		eGripperIdle,						// Idle.
		eGripperOpenIntake1A,				// Open Intake Start - Motors on, Claw Open
		eGripperOpenIntake1B,				// Open Intake - Waiting for cube
		eGripperOpenIntake1C,				// Open Intake - Delay closing claw
		eGripperOpenIntake1D,				// Open Intake - Delay stopping intake motors
		eGripperClosedIntake1A,				// Closed Intake Start - Motors on, Claw closed
		eGripperClosedIntake1B,				// Closed Intake - Waiting for cube
		eGripperClosedIntake1C,				// Closed Intake - Delay stopping intake motors
		eGripperFloorEject1A,				// Floor Eject Start - Motors on, No Plungers, Claw closed.
		eGripperFloorEject1B,				// Floor Eject - Delay stopping intake motors
		eGripperFloorEject2A,
		eGripperFloorEject2B,
		eGripperScaleEject1A,				// Scale Eject Start - Motors off, Claw open, Plunger delayed
		eGripperScaleEject1B,				// Scale Eject - Delay plunger
		eGripperScaleEject1C,				// Scale Eject - Delay plunger return
		eGripperScaleEject1D				// Scale Eject - Delay closing claw
	};

private:
	// Object pointers.
	Joystick*			m_pDriveController;
	WPI_TalonSRX*		m_pLeftIntakeMotor;
	WPI_TalonSRX*		m_pRightIntakeMotor;
	Solenoid*			m_pPlungerSolenoid;
	Solenoid*			m_pPlungerVacuumSolenoid;
	Solenoid*			m_pLeftClawSolenoid;
	Solenoid*			m_pRightClawSolenoid;
	DigitalInput*		m_pPowerCubeSensor;
	Timer*				m_pTimer;

	// Member variables.
	double				m_dDelayStartTime;
	GripperStates		m_nGripperState;
	double				m_dEjectThrottle;
};
/////////////////////////////////////////////////////////////////////////////
#endif

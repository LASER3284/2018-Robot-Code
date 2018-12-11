/****************************************************************************
	Description:	Defines the CDrive control class.

	Classes:		CDrive

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef Drive_h
#define Drive_h
#pragma once

#include "CANPosition.h"

// Drive Constants.
const double dDriveEncoderRevsPerInch	= (1.000 / (6.250 * 3.1415));
const double dDriveMaxSpeed				=    1.000;		// Maximum drive output speed.
const double dDriveMinSpeed				=   -1.000;		// Minimum drive output speed.
const double dDriveMaxDistance			=  325.000;		// Maximum drive distance.
const double dDriveMinDistance			= -325.000;		// Minimum drive distance.
const double dDriveTolerance			=    5.000;		// Drive position tolerance.
const double dDriveProportional			=	 0.500;		// Drive Proportional.
const double dDriveIntegral				=	 0.000;		// Drive Integral.
const double dDriveDerivative			=	 0.000;		// Drive Derivative.
const double dDriveAccumIZone			=	 5.000;		// Drive Accumulated Integral Zone.
const double dDriveOpenLoopRampRate		=	 0.500;		// Open Loop Ramp Rate.
const double dDriveClosedLoopRampRate	=	 0.500;		// Closed Loop Ramp Rate.
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CDrive control class.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
class CDrive
{
public:
	CDrive(Joystick* pDriveController);
	~CDrive();

	void	Init();
	void 	Tick(bool squareDrive = false);
	void	VisionTick();
	void	Stop();
	void 	Shift(bool bLowGear);
	bool	IsReady();
	void	SetJoystickControl(bool bJoystickControl);
	void	SetSetpoint(double dLeftSetpoint, double dRightSetpoint);
	void	SetMotorExpiration(double dTimeout);
	void	SetMotorSafety(bool bEnabled);
	void	SetLeftPIDValues(double dProportional, double dIntegral, double dDerivative, double dFeedForward = 0.000);
	void	SetRightPIDValues(double dProportional, double dIntegral, double dDerivative, double dFeedForward = 0.000);

	bool	IsDriveInHighGear()														{	return !m_pShiftSolenoid->Get();										};
	bool	IsLeftDriveReady()														{	return m_pLeftDrive->IsReady();										};
	bool	IsRightDriveReady()														{	return m_pRightDrive->IsReady();									};
	double	GetLeftSetpoint()														{	return m_pLeftDrive->GetSetpoint();									};
	double	GetRightSetpoint()														{	return m_pRightDrive->GetSetpoint();								};
	double	GetLeftActual()															{	return m_pLeftDrive->GetActual();									};
	double	GetRightActual()														{	return m_pRightDrive->GetActual();									};
	double	GetLeftEncoderCount()													{	return m_pLeftDrive->GetRawEncoderCounts();							};
	double	GetRightEncoderCount()													{	return m_pRightDrive->GetRawEncoderCounts();						};
	void	SetLeftPeakOutputPercent(double dMaxFwdOutput, double dMaxRevOutput)	{	m_pLeftDrive->SetPeakOutputPercent(dMaxFwdOutput, dMaxRevOutput);	};
	void	SetRightPeakOutputPercent(double dMaxFwdOutput, double dMaxRevOutput)	{	m_pRightDrive->SetPeakOutputPercent(dMaxFwdOutput, dMaxRevOutput);	};
	void	SetLeftAccumIZone(double dAccumIZone)									{	m_pLeftDrive->SetAccumIZone(dAccumIZone);							};
	void	SetRightAccumIZone(double dAccumIZone)									{	m_pRightDrive->SetAccumIZone(dAccumIZone);							};
	void	SetLeftTolerance(double dTolerance)										{	m_pLeftDrive->SetTolerance(dTolerance);								};
	void	SetRightTolerance(double dTolerance)									{	m_pRightDrive->SetTolerance(dTolerance);							};
	void	SetLeftClosedLoopRampRate(double dClosedLoopRampRate)					{	m_pLeftDrive->SetClosedLoopRampRate(dClosedLoopRampRate);			};
	void	SetRightClosedLoopRampRate(double dClosedLoopRampRate)					{	m_pRightDrive->SetClosedLoopRampRate(dClosedLoopRampRate);			};
	void	ResetLeftEncoder()														{	m_pLeftDrive->ResetEncoderPosition();								};
	void	ResetRightEncoder()														{	m_pRightDrive->ResetEncoderPosition();								};
	void	ClearLeftStickyFaults()													{	m_pLeftDrive->ClearStickyFaults();									};
	void	ClearRightStickyFaults()												{	m_pRightDrive->ClearStickyFaults();									};
	void	SetLeftMaxFindingTime(double dFindingTime)								{	m_pLeftDrive->SetMaxFindingTime(dFindingTime);						};
	void	SetRightMaxFindingTime(double dFindingTime)								{	m_pRightDrive->SetMaxFindingTime(dFindingTime);						};

private:
	// Object pointers.
	Joystick*			m_pDriveController;
	DifferentialDrive*	m_pRobotDrive;
	CCANPosition*		m_pLeftDrive;
	CCANPosition*		m_pRightDrive;
	WPI_TalonSRX*		m_pLeftDriveMotor1;
	WPI_TalonSRX*		m_pLeftDriveMotor2;
	WPI_TalonSRX*		m_pLeftDriveMotor3;
	WPI_TalonSRX*		m_pRightDriveMotor1;
	WPI_TalonSRX*		m_pRightDriveMotor2;
	WPI_TalonSRX*		m_pRightDriveMotor3;
	Solenoid*			m_pShiftSolenoid;

	// Member variables.
	double 				m_dYAxis;
	double 				m_dXAxis;
	bool				m_bJoystickControl;
	bool				m_bDriveControllerLeftAnalogPressed;
};
/////////////////////////////////////////////////////////////////////////////
#endif

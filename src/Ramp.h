/****************************************************************************
	Description:	Defines the CRamp control class.

	Classes:		CRamp

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef Ramp_h
#define Ramp_h
#pragma once

#include "WPILib.h"
#include <ctre/Phoenix.h>

// Default Constants set for Ramp class.
const double dDefaultDeploySpeed		= 0.500;
const double dDefaultRampMotorSpeed 	= 0.500;
const double dDefaultDeployRampDelay	= 0.500;
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CRamp control class.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
class CRamp
{
public:
	CRamp(int nMotorNum, int nDigitalInput, bool bLeftRamp);
	~CRamp();

	void	Init();
	void 	Tick();
	void	DeployRamp();
	void	StartRampMotor()		{	m_bRampMotorOn = true;	};
	void	StopRampMotor();
	bool	GetRampSensor();

	WPI_TalonSRX*	m_pRampMotor;

private:
	// Object pointers.
	DigitalInput*	m_pHingeSensor;
	Timer*			m_pTimer;

	// Member variables.
	double			m_dDelayStartTime;
	bool			m_bLeftRamp;
	bool			m_bRampMotorOn;
	bool			m_bDeployingRamp;
	bool			m_bRampDeployed;
};
/////////////////////////////////////////////////////////////////////////////
#endif

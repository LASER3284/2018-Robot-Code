/****************************************************************************
	Description:	Defines the CArmPosition control class.

	Classes:		CArmPosition

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef ArmPosition_h
#define ArmPosition_h
#pragma once

#include "CANPosition.h"

// Arm Position Constants.
const double dArmPositionEncoderRevsPerDegree	= ((1.000 / 16) * (72 / 1.000) * (1.000 / 360));	// ((1 enc rev / 16 teeth)(72 teeth / 1 arm rev)(1 arm rev / 360°))
const double dArmPositionHomingSpeedReverse		=	-0.500;		// Homing speed reverse (towards home switch).
const double dArmPositionHomingSpeedForward		=	 0.150;		// Homing speed forward (off of home switch)
const double dArmPositionMaxHomingTimeout		=	 6.000;		// Maximum time to home.
const double dArmPositionMaxFindingTimeout		=	 6.000;		// Maximum time to find position.
const double dArmPositionMaxSpeed				=    1.000;		// Maximum arm position output speed.
const double dArmPositionMinSpeed				=   -1.000;		// Minimum arm position output speed.
const double dArmPositionMaxDistance			=  180.000;		// Maximum arm position distance (180°).
const double dArmPositionMinDistance			= 	-1.000;		// Minimum arm position distance (0°).
const double dArmPositionTolerance				=    2.000;		// Arm Position tolerance in degrees.
const double dArmPositionProportional			=	 1.800;		// Arm Position Proportional. (2.000)   /// 2.250
const double dArmPositionIntegral				=	 0.000;		// Arm Position Integral.
const double dArmPositionDerivative				=	 0.000;		// Arm Position Derivative.
const double dArmPositionAccumIZone				=	15.000;		// Arm Position Accumulated Integral Zone.
const double dArmPositionOpenLoopRampRate		=	 0.500;		// Open Loop Ramp Rate.
const double dArmPositionClosedLoopRampRate		=	 0.500;		// Closed Loop Ramp Rate.
const double dArmPositionFrontPickup			=	-1.000;		// Arm position for Front Pickup in degrees.
const double dArmPositionBackPickup				=  180.000;		// Arm position for Back Pickup in degrees.
const double dArmPositionSwitchPosition			=	52.000;		// Arm position for Switch Eject in degrees.
const double dArmPositionSwitchBackPosition		=  105.000;		// Arm position for Switch Back Eject in degrees.
const double dArmPositionHighCubeFrontPosition	=	21.000;		// Arm position for stacked cubes in front.
const double dArmPositionHighCubeBackPosition   =  149.000;		// Arm position for stacked cubes in back.
const double dArmPositionShootCubeDownPosition  =   62.500;		// Arm position for shooting cube in Down orientation.
const double dArmPositionScalePosition			=	75.000;		// Arm position for Scale Eject in degrees.
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CArmPosition control class.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
class CArmPosition
{
public:
	CArmPosition();
	~CArmPosition();

	void	Init();
	void 	Tick();
	void	SetSetpoint(double dSetpoint);
	void	JogArm(bool bArmUp);
	void	StopArm();

	bool	IsReady()															{	return m_pArmMotor->IsReady();					};
	bool	IsHomingComplete()													{	return m_pArmMotor->IsHomingComplete();			};
	double	GetSetpoint()														{	return m_pArmMotor->GetSetpoint();				};
	double	GetActual()															{	return m_pArmMotor->GetActual();				};
	double	GetEncoderCount()													{	return m_pArmMotor->GetRawEncoderCounts();		};
	void	SetTolerance(double dTolerance)										{	m_pArmMotor->SetTolerance(dTolerance);			};
	void	StartHoming()														{	m_pArmMotor->StartHoming();						};
	void	Stop()																{	m_pArmMotor->Stop();							};
	bool	IsHomeSwitchPressed()												{	return m_pArmMotor->IsRevLimitSwitchPressed();	};
	bool	IsForwardSwitchPressed()											{	return m_pArmMotor->IsFwdLimitSwitchPressed();	};

private:
	// Object pointers.
	CCANPosition*		m_pArmMotor;
	Solenoid*			m_pArmBrake;

	// Member variables.

};
/////////////////////////////////////////////////////////////////////////////
#endif

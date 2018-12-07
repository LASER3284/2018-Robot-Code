/****************************************************************************
	Description:	Defines the 2018 PowerUp Robot I/O map.

	Classes:		None

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef IOMap_h
#define IOMap_h
#pragma once
/////////////////////////////////////////////////////////////////////////////

// Solenoid Channels.
const int nDriveShiftSolenoid			=  	0;		// Solenoid output for shifting gearbox   		(off = low gear)
const int nPlungerSolenoid				=   1; 		// Solenoid output for plunger solenoid			(on = extended)
const int nLeftClawSolenoid				=	2;		// Solenoid output for Left Intake arm   		(on = retracted)
const int nRightClawSolenoid			=	3;		// Solenoid output for Right Intake arm	 		(on = retracted)
const int nArmBrakeSolenoid				=	4;		// Solenoid output for Arm Brake				(on = extended)
const int nPlungerVacuumSolenoid		=	5;		// Solenoid output for plunger vacuum			(on = enabled)

// Talon SRX Device ID Numbers.
const int nLeftDriveMotor1		  		=  	1;		// Talon ID for left drive motor 1
const int nLeftDriveMotor2		  		=  	2;		// Talon ID for left drive motor 2 				(Inverted output)
const int nLeftDriveMotor3		  		=  	3;		// Talon ID for left drive motor 3
const int nRightDriveMotor1		  		=  	4;		// Talon ID for right drive motor 4
const int nRightDriveMotor2		  		=  	5;		// Talon ID for right drive motor 5				(Inverted output)
const int nRightDriveMotor3		  		=  	6;		// Talon ID for right drive motor 6
const int nArmPositionMotor				=	7;		// Talon ID for Arm motor
const int nLeftIntakeMotor				=	8;		// Talon ID for Left Intake arm motor			(Positive = Intake)
const int nRightIntakeMotor				=	9;		// Talon ID for Right Intake arm motor			(Negative = Eject)
const int nLeftRampMotor				=	10;		// Talon ID for Left Ramp deploy motor
const int nRightRampMotor				=	11;		// Talon ID for Right Ramp deploy motor

// PWM Channels.

// Relay Channels.

// Analog Channels.

// Digital Channels.
const int nPowerCubeSensor				=   0;		// Senses Power Cube in Intake Arm
const int nLeftRampHingeSensor			=	1;		// Sensor to detect if ramp is in locked position
const int nRightRampHingeSensor			=	2;		// Sensor to detect if ramp is in locked position

// XBox Controller Button Assignments.
enum XBoxButtons {eButtonA = 1, eButtonB, eButtonX, eButtonY, eLeftBumper, eRightBumper, eBack, eStart, eLeftAnalog, eRightAnalog};
/////////////////////////////////////////////////////////////////////////////
#endif

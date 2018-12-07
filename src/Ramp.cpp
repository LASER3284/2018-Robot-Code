/****************************************************************************
	Description:	Implements the CRamp control class.

	Classes:		CRamp

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#include "WPILib.h"
#include <ctre/Phoenix.h>
#include "Ramp.h"
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CRamp Constructor.

	Arguments:		int nMotorNum 		- Winch motor number
					int nDigitalInput	- Ramp lock sensor
					bool bLeftRamp		- True for Left Ramp, False for Right Ramp

	Derived From:	Nothing
****************************************************************************/
CRamp::CRamp(int nMotorNum, int nDigitalInput, bool bLeftRamp)
{
	// Create the object pointers.
	m_pRampMotor		= new WPI_TalonSRX(nMotorNum);
	m_pHingeSensor		= new DigitalInput(nDigitalInput);
	m_pTimer			= new Timer();

	// Initialize member variables.
	m_bLeftRamp			= bLeftRamp;
	m_bRampMotorOn		= false;
	m_bDeployingRamp	= false;
	m_bRampDeployed		= false;
	m_dDelayStartTime	= 0.000;

	// Start the timer.
	m_pTimer->Start();
}

/****************************************************************************
	Description:	CRamp Destructor.

	Arguments:		None

	Derived From:	None
****************************************************************************/
CRamp::~CRamp()
{
	delete m_pRampMotor;
	delete m_pHingeSensor;
	delete m_pTimer;

	m_pRampMotor		= NULL;
	m_pHingeSensor		= NULL;
	m_pTimer			= NULL;
}

/****************************************************************************
	Description:	Initialize Ramp parameters.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CRamp::Init()
{
}

/****************************************************************************
	Description:	Tick - Called each time through robot main loop.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CRamp::Tick()
{
	// Check if deploying the ramp.
	if ((m_bDeployingRamp) && (m_pTimer->Get() > (m_dDelayStartTime + dDefaultDeployRampDelay)))
	{
		// Turn off ramp motor.
		m_pRampMotor->Set(ControlMode::PercentOutput, 0.000);
		// Set variable that ramp has been deployed (or we hope so!)
		m_bDeployingRamp = false;
		m_bRampDeployed  = true;
	}

	// Check if m_bRampMotorOn is true and if the DeploySolenoid is extended.
	if ((m_bRampMotorOn) && m_bRampDeployed)
	{
		// Turn on ramp motor.
		m_pRampMotor->Set(ControlMode::PercentOutput, (m_bLeftRamp ? dDefaultDeploySpeed : (dDefaultRampMotorSpeed * -1.000)));
		// Check for hinge sensor.
		if (m_pHingeSensor->Get())
		{
			// Turn off ramp motor.
			m_pRampMotor->Set(ControlMode::PercentOutput, 0.000);
			m_bRampMotorOn = false;
		}
	}

	// Put Ramp information on SmartDashboard for Auto/Teleop.
	if (m_bLeftRamp)
	{
		SmartDashboard::PutBoolean("Left Ramp Sensor", 	GetRampSensor());
	}
	else
	{
		SmartDashboard::PutBoolean("Right Ramp Sensor", GetRampSensor());
	}
}

/****************************************************************************
	Description:	DeployRamp - Turn on solenoid to deploy ramp.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CRamp::DeployRamp()
{
	// Get current time.
	m_dDelayStartTime = m_pTimer->Get();
	// Start the Ramp Motor.
	m_pRampMotor->Set(ControlMode::PercentOutput, (m_bLeftRamp ? dDefaultDeploySpeed : (dDefaultDeploySpeed * -1.000)));
	// Set variable that ramp is being deployed.
	m_bDeployingRamp = true;
}

/****************************************************************************
	Description:	StopRampMotor - Emergency stop the ramp motor.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CRamp::StopRampMotor()
{
	// Turn off ramp motor.
	m_pRampMotor->Set(ControlMode::PercentOutput, 0.000);
	m_bRampMotorOn = false;
}

/****************************************************************************
	Description:	GetRampSensor - Returns the state of the ramp sensor.

	Arguments: 		None

	Returns: 		bool bRampSensorState
****************************************************************************/
bool CRamp::GetRampSensor()
{
	return m_pHingeSensor->Get();
}
///////////////////////////////////////////////////////////////////////////////

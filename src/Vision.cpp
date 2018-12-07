/****************************************************************************
	Description:	Implements the CVision control class.

	Classes:		CVision

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#include "Vision.h"
#include "WPILib.h"
#include "Math.h"
#include "IOMap.h"
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CVision Constructor.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
CVision::CVision()
{
	// Create the object pointers.
	m_csFrontCamera				= cs::UsbCamera("cam1", 1);
	m_csRearCamera				= cs::UsbCamera("cam0", 0);
	m_csCameraSink				= CameraServer::GetInstance()->GetVideo(m_csRearCamera);
	m_csCameraSource			= CameraServer::GetInstance()->PutVideo("Current View", nImageWidth, nImageHeight);
	m_Image						= cv::Mat();

	// Initialize variables.
	m_bIsFrontCamera			= false;

	// Set up the front camera.
	m_csFrontCamera.SetResolution(nImageWidth, nImageHeight);
	m_csFrontCamera.SetFPS(nCameraFPS);
	m_csFrontCamera.SetExposureAuto();

	// Set up the rear camera.
	m_csRearCamera.SetResolution(nImageWidth, nImageHeight);
	m_csRearCamera.SetFPS(nCameraFPS);
	m_csRearCamera.SetExposureAuto();
}

/****************************************************************************
	Description:	CVision Destructor.

	Arguments:		None

	Derived From:	None
****************************************************************************/
CVision::~CVision()
{
}

/****************************************************************************
	Description:	Init - Called one time, initializes Vision properties.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CVision::Init()
{
}

/****************************************************************************
	Description:	Tick - main method that does functionality.
					Called each time through robot main loop to update state.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CVision::Tick()
{
	// Grab the current frame to display.
	m_csCameraSink.GrabFrame(m_Image);

	// Draw the vertical crosshair.
	line(m_Image, cv::Point((nImageWidth / 2.0), 0),
				  cv::Point((nImageWidth / 2.0), nImageHeight),
				  cv::Scalar(0, 0, 255), nCrosshairWidth);
	// Draw the horizontal crosshair.
	line(m_Image, cv::Point(0, (nImageHeight / 2.0)),
				  cv::Point(nImageWidth, (nImageHeight / 2.0)),
				  cv::Scalar(0, 0, 255), nCrosshairWidth);

	// Send the current frame to the SmartDashboard.
	m_csCameraSource.PutFrame(m_Image);
}

/****************************************************************************
	Description:	Switch Camera - Switches between the front and rear camera.

	Arguments: 		None

	Returns: 		Nothing
****************************************************************************/
void CVision::SwitchCamera()
{
	// Toggle the camera variable.
	m_bIsFrontCamera = !m_bIsFrontCamera;

	if (m_bIsFrontCamera)
	{
		// Set the new source for the CameraSink as the front camera.
		m_csCameraSink.SetSource(m_csFrontCamera);
	}
	else
	{
		// Set the new source for the CameraSink as the rear camera.
		m_csCameraSink.SetSource(m_csRearCamera);
	}
}

/****************************************************************************
	Description:	SetVisionSettings - Sets up the vision camera for the
					desired exposure settings.

	Arguments: 		bool bVisionSettings - True when vision settings are desired.

	Returns: 		Nothing
****************************************************************************/
void CVision::SetVisionSettings(bool bVisionSettings)
{
	// Check to see what settings are desired.
	if (bVisionSettings)
	{
		// Adjust the front camera settings for vision.
		m_csFrontCamera.SetExposureManual(nCameraVisionExposure);
	}
	else
	{
		// Adjust the front camera settings for TeleOp.
		m_csFrontCamera.SetExposureAuto();
	}
}
///////////////////////////////////////////////////////////////////////////////

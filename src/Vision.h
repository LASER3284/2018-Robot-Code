/****************************************************************************
	Description:	Defines the CVision control class.

	Classes:		CVision

	Project:		2018 PowerUp Robot Code.

	Copyright © 2018 First Team 3284 - Camdenton LASER 4H Robotics.
****************************************************************************/
#ifndef Vision_h
#define Vision_h
#pragma once

#include "WPILib.h"
#include "CameraServer.h"
#include "opencv2/imgproc/imgproc.hpp"

// Camera constants.
const int nImageWidth			=  320;	/// 160;	// Image width.
const int nImageHeight			=  240; /// 120;	// Image height.
const int nCrosshairWidth		=    1;	// Width of crosshairs to draw on front camera.
const int nCameraFPS			=   11;	// Camera FPS.
const int nCameraVisionExposure	=  100;	// Camera Exposure for Vision Processing.
/////////////////////////////////////////////////////////////////////////////


/****************************************************************************
	Description:	CVision Constructor.

	Arguments:		None

	Derived From:	Nothing
****************************************************************************/
class CVision
{
public:
	CVision();
	~CVision();
	void	Init();
	void 	Tick();
	void	SwitchCamera();
	void	SetVisionSettings(bool bVisionSettings);

private:
	// Object pointers.
	cs::UsbCamera			m_csFrontCamera;
	cs::UsbCamera			m_csRearCamera;
	cs::CvSink				m_csCameraSink;
	cs::CvSource			m_csCameraSource;
	cv::Mat					m_Image;

	// Member variables.
	bool					m_bIsFrontCamera;
};
/////////////////////////////////////////////////////////////////////////////
#endif

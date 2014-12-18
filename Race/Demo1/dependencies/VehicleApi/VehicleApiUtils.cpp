/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2014 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
///[TUTORIAL 2]

///[TUTORIAL 3]
#include "stdafx.h"

//#include <Demos/demos.h>
#include "VehicleApiUtils.h"
//#include <Demos/DemoCommon/Utilities/VehicleApi/VehicleApiUtils.h>

#include <Physics2012/Vehicle/hkpVehicleInstance.h>
#include <Physics2012/Vehicle/DriverInput/Default/hkpVehicleDefaultAnalogDriverInput.h>

#include <Common/Internal/ConvexHull/hkGeometryUtility.h>
#include <Physics2012/Vehicle/Camera/hkp1dAngularFollowCam.h>
#include <Physics2012/Utilities/VisualDebugger/Viewer/hkpShapeDisplayBuilder.h>
#include <Physics2012/Collide/Shape/Convex/ConvexVertices/hkpConvexVerticesShape.h>
#include "../../InputHandler.h"

//#include <Graphics/Common/Window/hkgWindow.h>
//#include <Graphics/Common/Camera/hkgCamera.h>

//#include <Graphics/Bridge/DisplayHandler/hkgDisplayHandler.h>

//#include <Common/Visualize/Shape/hkDisplayGeometry.h>


hkpShape* HK_CALL VehicleApiUtils::createDisc(hkReal radius, hkReal thickness, int numSides)
{
	hkArray<hkVector4> vertices;

	//
	// Create the vertices array.
	//
	for(int i = 0; i < numSides; i++)
	{
		hkTransform t;
		t.setIdentity();
		hkVector4 trans = hkVector4(0.0f, radius, 0.0f);

		hkReal angle = HK_REAL_PI * 2 * i / (hkReal) numSides;
		hkVector4 axis(0.0f, 0.0f, 1.0f, 0.0f);
		hkQuaternion q(axis, angle);
		trans.setRotatedDir(q, trans);
		hkVector4 v = trans;
		v(2) = -(thickness / 2.0f);
		vertices.pushBack(v);
		v(2) = (thickness / 2.0f);
		vertices.pushBack(v);
	}
	
	return new hkpConvexVerticesShape(vertices);
}

void HK_CALL VehicleApiUtils::calcWheelsRotation(hkpVehicleInstance& vehicle, hkQuaternion rots[])
{
	for (int i = 0; i < 4; i++)
	{
		hkVector4    pos;
		//hkQuaternion rot;

		//
		// XXX Check if this is the same value as the m_hardPointWS in wheelsInfo
		//
		//
		vehicle.calcCurrentPositionAndRotation(vehicle.getChassis(),
			vehicle.m_suspension,
			i,
			pos, rots[i]);
	}
}

/*
void HK_CALL VehicleApiUtils::createDisplayWheel(hkArray<hkDisplayGeometry*>& displayGeometry,
										 hkReal radius,
										 hkReal thickness)
{
	hkpShape* wheelShape = VehicleApiUtils::createDisc(radius, thickness);

	hkpShapeDisplayBuilder::hkpShapeDisplayBuilderEnvironment env;
	hkpShapeDisplayBuilder shapeBuilder(env);
	shapeBuilder.buildDisplayGeometries(wheelShape, displayGeometry);

	wheelShape->removeReference();
}


void HK_CALL VehicleApiUtils::syncDisplayWheels(hkDemoEnvironment* environment, 
								 hkpVehicleInstance& vehicle,
								 const hkArray<int>& wheels,
								 int tag)
{
	//
	// Sync wheels. This is necessary only because they do not "exist" as bodies in simulation,
	// and so are not automatically updated by the current display. We must explicity tell the
	// display that some other "display obects" (objects which are drawn but are not physical)
	// have moved.
	//
	for (int i = 0; i < wheels.getSize(); i++)
	{
		hkVector4    pos;
		hkQuaternion rot;

		//
		// XXX Check if this is the same value as the m_hardPointWS in wheelsInfo
		//
		//
		vehicle.calcCurrentPositionAndRotation( vehicle.getChassis(), 
						vehicle.m_suspension,					
						i,
						pos, rot );
		hkTransform trans(rot, pos);
		environment->m_displayHandler->updateGeometry(trans, wheels[i], tag);
	}
}



void HK_CALL VehicleApiUtils::createDisplayWheels(hkDemoEnvironment* environment,
										int tag,
										int displayId,
										hkReal radius,
										hkReal thickness)
{
	hkInplaceArray<hkDisplayGeometry*, 1> displayGeometry;

	VehicleApiUtils::createDisplayWheel(displayGeometry, radius, thickness);

	environment->m_displayHandler->addGeometry(displayGeometry,
												hkTransform::getIdentity(),
												displayId,
												tag, 1);
	//
	// Delete the displayGeometry.
	//
	for (int j = 0; j < displayGeometry.getSize(); j++)
	{
		displayGeometry[j]->removeReference();
	}
}



void HK_CALL VehicleApiUtils::createCamera( hkp1dAngularFollowCam& camera )
{
	hkp1dAngularFollowCamCinfo cinfo;

	cinfo.m_yawSignCorrection = 1.0f; 
	cinfo.m_upDirWS.set(0.0f, 1.0f, 0.0f); 
	cinfo.m_rigidBodyForwardDir.set(1.0f, 0.0f, 0.0f); 

	cinfo.m_set[0].m_velocity = 10.0f;
	cinfo.m_set[1].m_velocity = 50.0f;
	cinfo.m_set[0].m_speedInfluenceOnCameraDirection = 1.0f;
	cinfo.m_set[1].m_speedInfluenceOnCameraDirection = 1.0f;
	cinfo.m_set[0].m_angularRelaxation = 3.5f;
	cinfo.m_set[1].m_angularRelaxation = 4.5f;

	// The two camera positions ("slow" and "fast" rest positions) are both the same here,
	// -6 units behind the chassis, and 2 units above it. Again, this is dependent on 
	// m_chassisCoordinateSystem.
	cinfo.m_set[0].m_positionUS.set( -6.0f, 1.5f, 0.0f); 
	cinfo.m_set[1].m_positionUS.set( -6.0f, 1.5f, 0.0f); 

	cinfo.m_set[0].m_lookAtUS.set ( 2.0f, 0.0f, 0.0f );
	cinfo.m_set[1].m_lookAtUS.set ( 2.0f, 0.0f, 0.0f );

	cinfo.m_set[0].m_fov = 60.0f;
	cinfo.m_set[1].m_fov = 60.0f;

	camera.reinitialize( cinfo );
}

///[CODE updateCameraCode]<
void HK_CALL VehicleApiUtils::updateCamera(hkDemoEnvironment* environment,
								   const hkpRigidBody& vehicleChassis,
								   hkReal timestep,
								   hkp1dAngularFollowCam& camera,
								   hkReal nearPlane,
								   hkReal farPlane )
{
	//
	// Update the camera, based on where the vehicle is (a "follow cam").
	//
	{
		//
		// Vehicle specific camera settings.
		//
		hkp1dAngularFollowCam::CameraInput in;
		{
			hkpWorld* world = vehicleChassis.getWorld();
			world->markForRead();
			hkReal time = world->getCurrentTime();
			world->unmarkForRead();
			vehicleChassis.approxTransformAt( time, in.m_fromTrans );	

			in.m_linearVelocity = vehicleChassis.getLinearVelocity();
			in.m_angularVelocity = vehicleChassis.getAngularVelocity();
			in.m_deltaTime.setFromFloat(timestep);
		}

		hkp1dAngularFollowCam::CameraOutput out;
		{
			camera.calculateCamera( in, out );
		}

		// 
		// General camera settings.
		//
		{
			hkgViewport* v1 = environment->m_window->getViewport(0);

			v1->getCamera()->setFrom( &out.m_positionWS(0) );
			v1->getCamera()->setTo( &out.m_lookAtWS(0) );
			v1->getCamera()->setUp( &out.m_upDirWS(0) );
			v1->getCamera()->setFOV(float( out.m_fov.getReal() ));
			v1->getCamera()->setFar(float(farPlane));
			v1->getCamera()->setNear(float(nearPlane));

			v1->getCamera()->computeModelView();
			v1->getCamera()->computeProjection();
		}
	}
}
///[CODE updateCameraCode]>
*/


///[CODE steerCode]<
void HK_CALL VehicleApiUtils::steer(
							hkReal& inputXPosition,
							hkReal& inputYPosition,
							hkpVehicleInstance& vehicle,
							hkReal timestep)
{
	//
	// Update controller "position" within range [-1, 1] for both X, Y directions.
	//

	hkpVehicleDriverInputAnalogStatus*	deviceStatus = (hkpVehicleDriverInputAnalogStatus*)vehicle.m_deviceStatus;

	const hkReal steerSpeed     =  3.0f * timestep;
	const hkReal backSteerSpeed = 10.0f * timestep;
	hkReal deltaY = -inputYPosition  * 0.2f;
	hkReal deltaX = -inputXPosition  * backSteerSpeed;

	if (InputHandler::keyPressed(E_MOVE_FORWARD))
	{
		// Accelerate.
		deltaY = -0.1f;
	}
	else if (InputHandler::keyPressed(E_MOVE_BACK))
	{
		// Brake/reverse.
		deltaY = 0.1f;
	}


	// forward or back
	hkpRigidBody* chassisBody = vehicle.getChassis();

	hkQuaternion rotation = chassisBody->getRotation();
	hkVector4 velocity = chassisBody->getLinearVelocity();

	velocity.setRotatedInverseDir(rotation, velocity);


	if (velocity(0) < 0)
	{
		if (InputHandler::keyPressed(E_TURE_LEFT))
		{
			// Turn left.
			if (inputXPosition <= 0.0f){ deltaX = 0.0f; }
			deltaX -= steerSpeed;
		}
		else if (InputHandler::keyPressed(E_TURE_RIGHT))
		{
			// Turn right.
			if (inputXPosition >= 0.0f){ deltaX = 0.0f; }
			deltaX += steerSpeed;
		}
	}
	else
	{
		if (InputHandler::keyPressed(E_TURE_LEFT))
		{
			// Turn right.
			if (inputXPosition >= 0.0f){ deltaX = 0.0f; }
			deltaX += steerSpeed;
		}
		else if (InputHandler::keyPressed(E_TURE_RIGHT))
		{
			// Turn left.
			if (inputXPosition <= 0.0f){ deltaX = 0.0f; }
			deltaX -= steerSpeed;	
		}
	}

	//deltaY -= .2f * 0.1f;

	//hkReal x = pad->getStickPosX(1);
	//hkReal x = 0.1f;
	//if ( x < 0 && inputXPosition <= 0.0f){	deltaX = 0.0f;	}
	//if ( x > 0 && inputXPosition >= 0.0f){	deltaX = 0.0f;	}
	//deltaX += x * backSteerSpeed;

	/*
	{
		deltaY -= .2f * pad->getStickPosY(0);

		hkReal x = pad->getStickPosX(1);
		//if ( x < 0 && inputXPosition <= 0.0f){	deltaX = 0.0f;	}
		//if ( x > 0 && inputXPosition >= 0.0f){	deltaX = 0.0f;	}
		deltaX += x * backSteerSpeed;
	}
	*/

	/*
	if ( (padButtonState & HKG_PAD_DPAD_UP) != 0 )  // Add HKG_PAD_BUTTON_0 to restore pre-Havok 5.5 control
	{
		// Accelerate.
 		deltaY = -0.1f;
	}
	else if ( (padButtonState & (HKG_PAD_DPAD_DOWN | HKG_PAD_BUTTON_L1)) != 0 )
	{
		// Brake/reverse.
		deltaY = 0.1f;
	}

	if ( (padButtonState & HKG_PAD_DPAD_LEFT) != 0 )
	{
		// Turn left.
		if ( inputXPosition <= 0.0f){	deltaX = 0.0f;	}
		deltaX -= steerSpeed;
	}
	else if ( (padButtonState & HKG_PAD_DPAD_RIGHT) != 0 )
	{
		// Turn right.
		if ( inputXPosition >= 0.0f){	deltaX = 0.0f;	}
		deltaX += steerSpeed;
	}

	// add analog controls
	{
		deltaY -= .2f * pad->getStickPosY(0);

		hkReal x = pad->getStickPosX(1);
		//if ( x < 0 && inputXPosition <= 0.0f){	deltaX = 0.0f;	}
		//if ( x > 0 && inputXPosition >= 0.0f){	deltaX = 0.0f;	}
		deltaX += x * backSteerSpeed;
	}
	*/

	inputXPosition = hkMath::clamp( inputXPosition+deltaX, hkReal(-1.0f), hkReal(1.0f));
	inputYPosition = hkMath::clamp( inputYPosition+deltaY, hkReal(-1.0f), hkReal(1.0f));

	// Now -1 <= m_inputXPosition <= 1 and
	// -1 <= m_inputYPosition <= 1
	deviceStatus->m_positionY = inputYPosition;
	deviceStatus->m_positionX = inputXPosition;

	// AutoReverse
	/*
	deviceStatus->m_reverseButtonPressed = ( padButtonState & HKG_PAD_BUTTON_2) != 0;

	// Handbrake.
	//hkprintf( "%f %f\n", *inputXPosition, *inputYPosition);
	deviceStatus->m_handbrakeButtonPressed = (padButtonState & (HKG_PAD_BUTTON_L1 |  HKG_PAD_BUTTON_1)) != 0;
	*/
}
///[CODE steerCode]>

hkpConvexVerticesShape* HK_CALL VehicleApiUtils::createCarChassisShape()
{
	//hkReal xSize = 1.75f;
	hkReal xSize = 2.5f;
	hkReal ySize = 0.25f;
	hkReal zSize = 1.1f;

	hkReal xBumper = 1.9f;
	hkReal yBumper = 0.15f;
	hkReal zBumper = 1.0f;

	hkReal xRoofFront = 0.4f;
	hkReal xRoofBack = -1.0f;
	hkReal yRoof = ySize + 0.45f;
	hkReal zRoof = 0.7f;

	hkReal xDoorFront = xRoofFront;
	hkReal xDoorBack = xRoofBack;
	hkReal yDoor = ySize;
	hkReal zDoor = zSize + 0.1f;

	int numVertices = 22;

	// 16 = 4 (size of "each float group", 3 for x,y,z, 1 for padding) * 4 (size of float).
	int stride = sizeof(hkReal) * 4;

	HK_ALIGN_REAL(hkReal vertices[] )= { 
		xSize, ySize, zSize, 0.0f,		// v0
		xSize, ySize, -zSize, 0.0f,		// v1
		xSize, -ySize, zSize, 0.0f,		// v2
		xSize, -ySize, -zSize, 0.0f,	// v3
		-xSize, -ySize, zSize, 0.0f,	// v4
		-xSize, -ySize, -zSize, 0.0f,	// v5

		xBumper, yBumper, zBumper, 0.0f,	// v6
		xBumper, yBumper, -zBumper, 0.0f,	// v7
		-xBumper, yBumper, zBumper, 0.0f,	// v8
		-xBumper, yBumper, -zBumper, 0.0f,	// v9

		xRoofFront, yRoof, zRoof, 0.0f,		// v10
		xRoofFront, yRoof, -zRoof, 0.0f,	// v11
		xRoofBack, yRoof, zRoof, 0.0f,		// v12
		xRoofBack, yRoof, -zRoof, 0.0f,		// v13

		xDoorFront, yDoor, zDoor, 0.0f,		// v14
		xDoorFront, yDoor, -zDoor, 0.0f,	// v15
		xDoorFront, -yDoor, zDoor, 0.0f,	// v16
		xDoorFront, -yDoor, -zDoor, 0.0f,	// v17

		xDoorBack, yDoor, zDoor, 0.0f,		// v18
		xDoorBack, yDoor, -zDoor, 0.0f,		// v19
		xDoorBack, -yDoor, zDoor, 0.0f,		// v20
		xDoorBack, -yDoor, -zDoor, 0.0f,	// v21
	};
	
	//
	// SHAPE CONSTRUCTION.
	//
	
	hkStridedVertices		stridedVerts;
	stridedVerts.m_numVertices	=	numVertices;
	stridedVerts.m_striding		=	stride;
	stridedVerts.m_vertices		=	vertices;
	
	return new hkpConvexVerticesShape(stridedVerts);
}

void HK_CALL VehicleApiUtils::reorient(hkBool buttonPressed, hkpAction* action, hkpWorld* world) 	 
{ 	
	HK_ASSERT2(0x0, action != HK_NULL, "Action is NULL in VehicleApiUtils::reorient()!");
	HK_ASSERT2(0x0, world != HK_NULL, "World is NULL in VehicleApiUtils::reorient()!");

	if ( buttonPressed ) 	 
	{ 	 
		// If not already added, add it
		if ( action->getWorld() == HK_NULL ) 	 
		{ 	 
			world->addAction(action); 	
		} 	 
	} 	 
	else 	 
	{ 	 
		// If not already removed, remove it
		if ( action->getWorld() != HK_NULL ) 	 
		{ 	 
			world->removeAction(action); 	 
		} 	 
	} 	 

}

/*
 * Havok SDK - NO SOURCE PC DOWNLOAD, BUILD(#20140907)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2014
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available at www.havok.com/tryhavok.
 * 
 */

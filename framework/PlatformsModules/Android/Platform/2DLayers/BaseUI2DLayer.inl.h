
/*void	BaseUI2DLayer::Platform_GetMousePosition(kfloat & X, kfloat & Y, kfloat & sX, kfloat & sY, kfloat & dRx, kfloat & dRy)
{
	myInput->GetMouse()->getPos(X, Y);
	myInput->GetMouse()->getMouvement(dRx, dRy);

	MultiTouchDevice* mtDevice = NULL;
	if ((mtDevice = myInput->GetMultiTouch()))
	{
		kfloat mtX, mtY;
		if (mtDevice->getTouchState(1))
		{
			mtDevice->getTouchPos(1, mtX, mtY);
			GetRenderingScreen()->GetMousePosInScreen((int)mtX, (int)mtY, sX, sY);
			mMultiTouchPinch->set(1, sX, sY);
		}
		else
		{
			mMultiTouchPinch->release(1);
		}
	}

	GetRenderingScreen()->GetMouseMoveInScreen(dRx, dRy, dRx, dRy);
	GetRenderingScreen()->GetMousePosInScreen((int)X, (int)Y, sX, sY);
}


int	BaseUI2DLayer::Platform_GetInputFlag()
{
	int flag = UIInputEvent::NONE;
	flag |= (myInput->GetMouse()->getButtonState(MouseDevice::LEFT) != 0) ? UIInputEvent::LEFT:0;
	flag |= (myInput->GetMouse()->getButtonState(MouseDevice::MIDDLE) != 0) ? UIInputEvent::MIDDLE :0;
	flag |= (myInput->GetMouse()->getButtonState(MouseDevice::RIGHT) != 0) ? UIInputEvent::RIGHT:0;
	flag |= (myInput->GetMouse()->getButtonState(MouseDevice::LEFT) != 0) ? UIInputEvent::PINCH : 0; // pinch binbed on left

	return flag;
}*/
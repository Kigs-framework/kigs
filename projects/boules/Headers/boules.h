#pragma once

#include <DataDrivenBaseApplication.h>
#include "Ball.h"

class boules : public DataDrivenBaseApplication
{
public:
	DECLARE_CLASS_INFO(boules, DataDrivenBaseApplication, Core);
	DECLARE_CONSTRUCTOR(boules);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	
	void	ProtectedInitSequence(const kstl::string& sequence) override;
	void	ProtectedCloseSequence(const kstl::string& sequence) override;

	// ball list
	std::vector<Ball>		mBalls;
	// wall list
	std::vector<Wall>		mWalls;

	// graphic display
	CMSP					mMainInterface;

	// simulation time management
	double					mFirstTime=-1.0;

	// search all possible future collisions with current trajectories
	void	FindFutureCollisions(double time);

	// check if new trajectories need to be computed (collision occur), if yes compute them and return true 
	// else return false
	bool	computeNewTrajectories(double currentTime);

	// reset all
	void	resetAll(double currentTime);

	// structure to hold collisions
	class collisionStruct
	{
	public:
		double	mCollisionTime;
		Ball* mBall1 = nullptr;
		Ball* mBall2 = nullptr;
		Wall* mWall = nullptr;
	};
	// future collision list
	std::vector<collisionStruct>	mFutureCollisions;

};

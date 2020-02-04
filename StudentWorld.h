#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class Actor;
class Dirt;
class DiggerMan;
class Protester;
class StudentWorld : public GameWorld
{
public:
	StudentWorld(std::string assetDir)
		: GameWorld(assetDir) {}
	virtual int init();
	virtual int move();
	virtual void cleanUp();
	void updateDisplayText();
	void clearDirt(int xLocation, int yLocation);
	bool isOnDirt(int xLocation, int yLocation) const;
	bool isDirtBelow(int xLocation, int yLocation);
	bool isBoulderBelow(int xLocation, int yLocation);
	void addActor(Actor* a);
	void annoyFromBoulder(int xLocation, int yLocation);
	DiggerMan* getDiggerMan();
	bool sameLocation(Actor& a, Actor& b) const;
	bool sameLocation(int firstXLocation, int firstYLocation, int secondxLocation, int secondYLocation) const;
	bool isOnBoulder(int xLocation, int yLocation);
	bool isOnBoulderOrDirt(int xLocation, int yLocation);
	bool isOnBoulderOrDirtDistance(bool horizontal, bool pos, int staticLocation, int startLocation, int distance);
	bool isOnProtester(int xLocation, int yLocation);
	bool isSquirtOnBoulderOrDirt(int xLocation, int yLocation);
	bool isWithinRadiusOf(int xLocation, int xLocation2, int yLocation, int yLocation2, double radius);
	bool isWithinRadiusOfThree(int xLocation, int xLocation2, int yLocation, int yLocation2) const;
	bool isWithinRadiusOfThreeBoulder(int xLocation, int yLocation);
	bool isWithinRadiusOfThreeProtester(int x, int y);
	bool isWithinRadiusOfThreeProtester(int x, int y, int& pX, int& pY);
	bool isWithinRadiusOfSixBoulder(int x, int y);
	bool isWithinRadiusOfSixGoldNugget(int x, int y);
	bool isActorBoulder(Actor* a) const;
	bool isActorGoldNugget(Actor* a) const;
	bool isActorBarrel(Actor* a) const;
	bool isActorProtester(Actor* a) const;
	bool isWithinRadiusOfThreeOfDiggerman(int xLocation, int yLocation);
	void appearIfWithinRadiusOfTwelveGoldOrBarrel(int x, int y);
	bool getProtesterAtLocation(int x, int y, Protester*& a);
	int max(int x, int y);
	int min(int x, int y);
	void decBarrelsCounter();
private:
	Dirt* dirtPointers[64][60];
	void initializeDirt();
	void cleanUpDirt();
	DiggerMan* diggerManPointer;
	std::list<Actor*> actorList;
	void initializeBoulders();
	void initializeGoldNuggets();
	void initializeBarrels();
	void spawnProtester();
	void spawnPossibleGoodie();
	void spawnPossibleProtester();
	int m_tickCountDownForNextProtesterSpawn;
	int m_numOfBarrels;
	int m_targetNumOfProtesters;
	int m_numOfProtesters;
};

#endif // STUDENTWORLD_H_
#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <queue>

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class StudentWorld;

class Actor : public GraphObject {
public:
	Actor(int imageID, int xLocation, int yLocation, Direction dir, double size, unsigned int depth, StudentWorld* studentWorldPointer);
	virtual void doSomething() = 0; // pure virtual; Actor class itself shouldn't be created
	virtual ~Actor();
	StudentWorld* getWorld();
	bool isDead();
	void makeDead();
	void moveDirection();
	bool isInMap();
	void faceActor(Actor& a);
	bool isFacingActor(Actor& a);
	bool hasLineOfSight(Actor& a, int distance);	// does another actor see this one given minimum distance
	void setRandomDirection();
	bool isOnBoulderOrDirtInDirection();
	bool isWithinDistance(Actor& a, int distance);
	bool isWithinRadiusOfThreeBoulderInDirection();
	bool isEndOfMapInFront();
private:
	StudentWorld* m_studentWorldPointer;
	bool m_isDead;
};

class Irritable : public Actor {
public:
	Irritable(int imageID, int xLocation, int yLocation, Direction dir, StudentWorld* studentWorldPointer, int hitPoints);
	virtual ~Irritable();
	void decHitPoints(int amount);
	int getHitPoints();
protected:
	int m_hitPoints;
};

class DiggerMan : public Irritable {
public:
	DiggerMan(StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~DiggerMan();
	int getWater() const;
	int getSonar() const;
	int getGold() const;
	void setWater(int water);
	void setSonar(int sonar);
	void setGold(int gold);
	void incrementSonar();
	void incrementGoldNuggets();
	void increaseWater(int amount);
private:
	int m_unitsOfWater;
	int m_sonarCharge;
	int m_goldNuggets;
	void pressTabImplementation();
	void pressSpaceImplementation();
	void decGold();
	void decSonarKit();
};

struct Node {
	int m_x, m_y;
	Node* up;
	Node* right;
	Node* down;
	Node* left;
	Node* prev;
	Node* next;
	Node(int x, int y);
};

class Tree {
private:
	Node* root;
	int m_finalX, m_finalY;
	StudentWorld* world;
	void add(Node* nodeptr, int x, int y);
	std::queue<int> m_xCoordinates;
	std::queue<int> m_yCoordinates;
	void fillCoordinates(Node* node);
	bool isAlreadyExist(Node* node, int x, int y);
	bool isAlreadyExistsInPath(Node* node, int x, int y);
	void Destroy(Node* node);
public:
	Tree(int x, int y, int finalX, int finalY, StudentWorld* studentWorldPointer);
	~Tree();
	std::queue<int> getXCoordinates();
	std::queue<int> getYCoordinates();
};

class Protester : public Irritable {
public:
	Protester(int imageID, StudentWorld* studentWorldPointer, int hitPoints, int shoutCount);
	void doSomething();
	virtual ~Protester();
	bool leaveTheOilFieldState();
	void changeOilFieldState(bool oilFieldState);
	void decTickCountdown();
	int getTickCountdown();
	void setTickCountdown(int tick);
	void setStatus(int x);	// gives score depending on if died to boulder or squirt
	void leaveTheOilField();
	bool isShoutCooldown();
	bool moveNumberOfSquares();
	int getNumOfSquaresToMoveInDirection();
	void setNumOfSquaresToMoveInDirection(int x);
	void decNumOfSquaresToMoveInDirection();
	void setShoutCooldownCount(int x);
	void decShoutCooldownCount();
	bool isPerpendicularCountdown();
	void setPerpendicularCountdown(int x);
	void decPerpendicularCountdown();
	bool perpendicularChoice(int& numOfChoices, int& choiceNumber1, int& choiceNumber2);
private:
	int perpendicularCountdown;
	bool m_leaveTheOilFieldState;
	int tickCountdown;
	int numSquaresToMoveInCurrentDirection;
	int shoutCooldownCount;
protected:
	std::queue<int> m_xCoordinates;
	std::queue<int> m_yCoordinates;
};

class RegularProtester : public Protester {
public:
	RegularProtester(StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~RegularProtester();
};

class HardcoreProtester : public Protester {
public:
	HardcoreProtester(StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~HardcoreProtester();
};

class Dirt : public Actor {
public:
	Dirt(int xLocation, int yLocation, StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~Dirt();
};

class Boulder : public Actor {
public:
	enum state { stable, waiting, falling };
	Boulder(int xLocation, int yLocation, StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~Boulder();
private:
	state m_state;
	int m_waitingCounter;

};

// appears when user hits spacebar
class Squirt : public Actor {
public:
	Squirt(int xLocation, int yLocation, Direction dir, StudentWorld* studentWorldPointer);
	// when space is entered
	void doSomething();	// how it reacts to protestors and such
	bool isMaxDistance(int currentLocation) const;
	int getMaxDistance() const;
private:
	int maxDistance;
};

class SonarKit : public Actor {
public:
	SonarKit(StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~SonarKit();
private:
	int m_tickLifeTimeCounter;
};

class WaterPool : public Actor {
public:
	WaterPool(int xLocation, int yLocation, StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~WaterPool();
private:
	int m_tickLifeTimeCounter;
};

class GoldNugget : public Actor {
public:
	enum visibility { invisible, visible };
	enum pickupBy { DiggerMan, Protesters };
	enum state { permanent, temporary };
	GoldNugget(int xLocation, int yLocation, StudentWorld* studentWorldPointer, visibility visibility, pickupBy pickupBy, state state);
	void doSomething();
	virtual ~GoldNugget();
private:
	pickupBy m_pickupBy;
	state m_state;
	int m_tickLifeTimeCounter;
};

class Barrel : public Actor {
public:
	Barrel(int xLocation, int yLocation, StudentWorld* studentWorldPointer);
	void doSomething();
	virtual ~Barrel();
private:

};
#endif // ACTOR_H_
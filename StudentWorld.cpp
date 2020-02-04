#include "StudentWorld.h"
#include "Actor.h"
#include <string>
#include <vector>
#include <cmath>
#include <memory>
using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.
int StudentWorld::init()
{
	diggerManPointer = new DiggerMan(this);
	spawnProtester();
	initializeDirt();
	initializeBoulders();
	initializeGoldNuggets();
	initializeBarrels();
	m_tickCountDownForNextProtesterSpawn = max(25, 200 - getLevel());
	m_targetNumOfProtesters = min(15, 2 + (getLevel() * 1.5));
	m_numOfProtesters = 1;

	return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move() {

	//Update the Game Status Line
	updateDisplayText();

	//Spawn Possible Goodie
	spawnPossibleGoodie();

	//Spawn Possible Protester
	spawnPossibleProtester();

	//diggerman do something
	diggerManPointer->doSomething();

	//Every Actor do something
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); p++) {
		(*p)->doSomething();
	}

	//Get rid of dead actors
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
		if ((*p)->isDead()) {
			delete (*p);
			p = actorList.erase(p);
		}
		else
			p++;
	}

	//If diggerman annoyed, giveup, die
	if (diggerManPointer->getHitPoints() <= 0) {
		diggerManPointer->makeDead();
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}

	if (m_numOfBarrels <= 0) {
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}

	return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp() {
	cleanUpDirt();
	delete diggerManPointer;
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
		delete *p;
		p = actorList.erase(p);
	}
}

void StudentWorld::spawnPossibleProtester() {
	if (m_numOfProtesters == m_targetNumOfProtesters)
		return;
	if (m_tickCountDownForNextProtesterSpawn > 0) {
		m_tickCountDownForNextProtesterSpawn--;
		return;
	}
	spawnProtester();
	m_tickCountDownForNextProtesterSpawn = max(25, 200 - getLevel());
	m_numOfProtesters++;
}

void StudentWorld::spawnProtester() {
	int probabilityOfHardcore = min(90, (getLevel() * 10) + 30);
	int lottery = rand() % 100 + 1;

	if (lottery <= probabilityOfHardcore) {
		Actor* h = new HardcoreProtester(this);
		addActor(std::move(h));
	}
	else {
		Actor* r = new RegularProtester(this);
		addActor(std::move(r));
	}
}

void StudentWorld::spawnPossibleGoodie() {
	int G = (getLevel() * 25) + 300;
	int lotteryNumber = rand() % G;

	if (lotteryNumber == 7) {
		int lotteryNumber2 = rand() % 5;
		if (lotteryNumber2 == 3) {
			Actor* s = new SonarKit(this);
			addActor(std::move(s));
		}
		else {
			int x, y;
			do {
				x = rand() % 60;
				y = rand() % 57;
			} while (isOnDirt(x, y));
			Actor* w = new WaterPool(x, y, this);
			addActor(std::move(w));
		}
	}
}

void StudentWorld::cleanUpDirt()
{
	for (int x = 0; x < 64; x++) {
		if (x >= 30 && x < 34) {
			for (int y = 0; y < 4; y++) {
				delete dirtPointers[x][y];
				dirtPointers[x][y] = nullptr;
			}
			continue;
		}
		for (int y = 0; y < 60; y++) {
			delete dirtPointers[x][y];
			dirtPointers[x][y] = nullptr;
		}
	}
}

//MAY NEED REVISING - PG 22
void StudentWorld::updateDisplayText() {
	string stat = "Lvl: ";
	stat += to_string(getLevel());
	stat += " Lives: ";
	stat += to_string(getLives());
	stat += " Hlth: ";
	stat += to_string(diggerManPointer->getHitPoints());
	stat += to_string(diggerManPointer->getHitPoints() == 10 ? 0 : 00);
	stat += "% Wtr: ";
	stat += to_string(diggerManPointer->getWater());
	stat += " Gld: ";
	stat += to_string(diggerManPointer->getGold());
	stat += " Sonar: ";
	stat += to_string(diggerManPointer->getSonar());
	stat += " Oil Left: ";
	stat += to_string(m_numOfBarrels);
	stat += " Scr: ";

	//Padding with zeros for score //CHANGE! USE STRINGSTREAM AS SPECIFIED ON PROFESSOR'S SITE
	if (getScore() < 10)
		stat += "00000";
	else if (getScore() < 100)
		stat += "0000";
	else if (getScore() < 1000)
		stat += "000";
	else if (getScore() < 10000)
		stat += "00";
	else if (getScore() < 100000)
		stat += "0";
	stat += to_string(getScore());

	setGameStatText(stat);
}

void StudentWorld::initializeBoulders() {
	int B = min((getLevel() / 2) + 2, 7);

	for (int i = 0; i < B; i++) {
		int x = rand() % 61;
		int y = rand() % 37 + 20;
		Actor* p = new Boulder(x, y, this);
		addActor(std::move(p));
	}
}

void StudentWorld::initializeGoldNuggets() {
	enum visibility { invisible, visible };
	enum pickupBy { DiggerMan, Protesters };
	enum state { permanent, temporary };
	int G = max(((5 - getLevel()) / 2), 2);

	for (int i = 0; i < G; i++) {
		int x, y;
		do {
			x = rand() % 61;
			y = rand() % 57;
		} while (isWithinRadiusOfSixBoulder(x, y));

		Actor* p = new GoldNugget(x, y, this, static_cast<GoldNugget::visibility>(invisible), static_cast<GoldNugget::pickupBy>(DiggerMan), static_cast<GoldNugget::state>(permanent));
		addActor(std::move(p));
	}
}

bool StudentWorld::isWithinRadiusOfThreeProtester(int x, int y) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorProtester(*p))
			if (isWithinRadiusOf((*p)->getX(), (*p)->getY(), x, y, 3))
				return true;
	}
	return false;
}

bool StudentWorld::isWithinRadiusOfThreeProtester(int x, int y, int& pX, int& pY) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorProtester(*p))
			if (isWithinRadiusOf((*p)->getX(), (*p)->getY(), x, y, 3)) {
				pX = (*p)->getX();
				pY = (*p)->getY();
				return true;
			}
	}
	return false;
}

bool StudentWorld::getProtesterAtLocation(int x, int y, Protester*& a) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorProtester(*p))
			if (sameLocation((*p)->getX(), (*p)->getY(), x, y)) {
				a = dynamic_cast<Protester*>(*p);
				return true;
			}
	}

	return false;
}


bool StudentWorld::isWithinRadiusOfSixBoulder(int xLocation, int yLocation) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorBoulder(*p))
			if (isWithinRadiusOf((*p)->getX(), (*p)->getY(), xLocation, yLocation, 6))
				return true;
	}
	return false;
}

void StudentWorld::appearIfWithinRadiusOfTwelveGoldOrBarrel(int x, int y) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorGoldNugget(*p) || isActorBarrel(*p))
			if (isWithinRadiusOf((*p)->getX(), (*p)->getY(), x, y, 12))
				(*p)->setVisible(true);
	}
}

bool StudentWorld::isWithinRadiusOfSixGoldNugget(int x, int y) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorGoldNugget(*p))
			if (isWithinRadiusOf((*p)->getX(), (*p)->getY(), x, y, 6))
				return true;
	}
	return false;
}

void StudentWorld::initializeBarrels() {
	int L = min((2 + getLevel()), 18);
	m_numOfBarrels = L;

	for (int i = 0; i < L; i++) {
		int x, y;
		do {
			x = rand() % 61;
			y = rand() % 57;
		} while (isWithinRadiusOfSixBoulder(x, y) && isWithinRadiusOfSixGoldNugget(x, y));

		Actor* p = new Barrel(x, y, this);
		addActor(std::move(p));
	}
}

void StudentWorld::decBarrelsCounter() {
	m_numOfBarrels--;
}

//delete dirt 4 spaces up and right (4x4) of the x and y locations. Make sure not to access outside of the bounds of the array.
void StudentWorld::clearDirt(int xLocation, int yLocation) {
	for (int x = xLocation; x < 64 && x < xLocation + 4; x++)
		for (int y = yLocation; y < 60 && y < yLocation + 4; y++) {
			delete dirtPointers[x][y];
			dirtPointers[x][y] = nullptr;
		}
}

bool StudentWorld::isOnDirt(int xLocation, int yLocation) const {
	for (int x = xLocation; x < 64 && x < xLocation + 4; x++)
		for (int y = yLocation; y < 60 && y < yLocation + 4; y++) {
			if (dirtPointers[x][y] != nullptr)
				return true;
		}
	return false;
}

bool StudentWorld::isDirtBelow(int xLocation, int yLocation) {
	for (int x = xLocation; x < xLocation + 4; x++) {
		if (dirtPointers[x][yLocation - 1] != nullptr)
			return true;
	}
	return false;
}

bool StudentWorld::isBoulderBelow(int xLocation, int yLocation) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
		if (dynamic_cast<Boulder*>(*p)) {
			for (int x = xLocation - 3; x < xLocation + 4; x++) {
				if ((*p)->getY() == yLocation - 4 && (*p)->getX() == x)
					return true;
			}
		}
		p++;
	}
	return false;
}

void StudentWorld::annoyFromBoulder(int xLocation, int yLocation) {
	//Annoy Protesters
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end();) {
		if (dynamic_cast<Protester*>(*p)) {
			if (isWithinRadiusOfThree(xLocation, yLocation, (*p)->getX(), (*p)->getY())) {
				dynamic_cast<Protester*>(*p)->decHitPoints(100);
				dynamic_cast<Protester*>(*p)->setStatus(500);
			}
		}
		p++;
	}

	//Annoy Diggerman
	if (isWithinRadiusOfThreeOfDiggerman(xLocation, yLocation))
		diggerManPointer->decHitPoints(100);
}

bool StudentWorld::isWithinRadiusOf(int xLocation, int yLocation, int xLocation2, int yLocation2, double radius) {
	if (sqrt(pow(xLocation - xLocation2, 2) + pow(yLocation - yLocation2, 2)) <= radius)
		return true;
	return false;
}

bool StudentWorld::isWithinRadiusOfThree(int xLocation, int yLocation, int xLocation2, int yLocation2) const {
	if (sqrt(pow(xLocation - xLocation2, 2) + pow(yLocation - yLocation2, 2)) <= 3.0)
		return true;
	return false;
}

void StudentWorld::initializeDirt()
{
	//initialize dirt objects, don't make dirt in mineshaft
	for (int x = 0; x < 64; x++) {
		if (x >= 30 && x < 34) {//if in mineshaft territory in the x coord,
			for (int y = 0; y < 4; y++) {//only create dirt on bottom 4 levels. Upward of y is void of dirt to create mineshaft
				dirtPointers[x][y] = new Dirt(x, y, this);
			}
			continue;
		}
		for (int y = 0; y < 60; y++) {
			dirtPointers[x][y] = new Dirt(x, y, this);
		}
	}

	// Pointers set to nullptr where dirt is absent due to mineshaft; to avoid undefined behavior
	for (int x = 30; x < 34; x++) {
		for (int y = 4; y < 60; y++) {
			dirtPointers[x][y] = nullptr;
		}
	}
}

void StudentWorld::addActor(Actor* a) {
	actorList.push_back(a);
}

DiggerMan* StudentWorld::getDiggerMan() {
	return diggerManPointer;
}

bool StudentWorld::sameLocation(Actor& a, Actor& b) const {
	if (a.getX() == b.getX() && a.getY() == b.getY())
		return true;
	return false;
}

bool StudentWorld::sameLocation(int firstXLocation, int firstYLocation, int secondxLocation, int secondYLocation) const {
	if (firstXLocation == secondxLocation && firstYLocation == secondYLocation)
		return true;
	return false;
}

bool StudentWorld::isActorBoulder(Actor* a) const {
	if (Boulder* b = dynamic_cast<Boulder*>(a))
		return true;
	return false;
}

bool StudentWorld::isActorProtester(Actor* a) const {
	if (Protester* b = dynamic_cast<Protester*>(a))
		return true;
	return false;
}

bool StudentWorld::isActorGoldNugget(Actor* a) const {
	if (GoldNugget* g = dynamic_cast<GoldNugget*>(a))
		return true;
	return false;
}

bool StudentWorld::isActorBarrel(Actor* a) const {
	if (Barrel* g = dynamic_cast<Barrel*>(a))
		return true;
	return false;
}

bool StudentWorld::isOnBoulder(int xLocation, int yLocation) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorBoulder(*p))
			if (sameLocation(xLocation, yLocation, (*p)->getX(), (*p)->getY()))
				return true;
	}
	return false;
}

// vertical or horizontal
bool StudentWorld::isOnBoulderOrDirtDistance(bool horizontal, bool pos, int staticLocation, int startLocation, int distance) {
	int maxDistance;
	// pos means go up or right
	if (pos)
		maxDistance = startLocation + distance;
	else
		maxDistance = startLocation - distance;
	if (horizontal) {
		int count = startLocation;
		while (count != maxDistance) {
			if (isOnBoulderOrDirt(count, staticLocation))
				return true;
			if (pos)
				count++;
			else
				count--;
		}
	}
	else {
		int count = startLocation;
		while (count != maxDistance) {
			if (isOnBoulderOrDirt(staticLocation, count))
				return true;
			if (pos)
				count++;
			else
				count--;
		}
	}

	return false;

}

bool StudentWorld::isWithinRadiusOfThreeBoulder(int xLocation, int yLocation) {
	// go through all boulders
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorBoulder(*p))
			if (isWithinRadiusOfThree((*p)->getX(), (*p)->getY(), xLocation, yLocation))

				return true;
	}
	return false;
}


bool StudentWorld::isOnBoulderOrDirt(int xLocation, int yLocation) {
	if (isOnDirt(xLocation, yLocation) || isOnBoulder(xLocation, yLocation)) {
		return true;
	}
	return false;
}
bool StudentWorld::isSquirtOnBoulderOrDirt(int xLocation, int yLocation) {
	if (isOnBoulderOrDirt(xLocation, yLocation)) {
		playSound(SOUND_PLAYER_SQUIRT);
		return true;
	}
	return false;
}
bool StudentWorld::isOnProtester(int xLocation, int yLocation) {
	for (list<Actor*>::iterator p = actorList.begin(); p != actorList.end(); ++p) {
		if (isActorProtester(*p))
			if (isWithinRadiusOfThree((*p)->getX(), (*p)->getY(), xLocation, yLocation))
				return true;
	}
	return false;
}

bool StudentWorld::isWithinRadiusOfThreeOfDiggerman(int xLocation, int yLocation) {
	if (isWithinRadiusOfThree(diggerManPointer->getX(), diggerManPointer->getY(), xLocation, yLocation))
		return true;
	return false;
}

int StudentWorld::max(int x, int y) {
	if (x >= y)
		return x;
	else
		return y;
}

int StudentWorld::min(int x, int y) {
	if (x <= y)
		return x;
	else
		return y;
}
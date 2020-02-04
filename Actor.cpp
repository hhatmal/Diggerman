#include "Actor.h"
#include "StudentWorld.h"
#include <queue>
#include <thread>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cp

//Actor Implementations
Actor::Actor(int imageID, int xLocation, int yLocation, Direction dir, double size, unsigned int depth, StudentWorld* studentWorldPointer)
	:GraphObject(imageID, xLocation, yLocation, dir, size, depth), m_studentWorldPointer(studentWorldPointer), m_isDead(false)
{}

void Actor::doSomething() {}

Actor::~Actor() {}


bool Actor::isDead() {
	return m_isDead;
}

void Actor::makeDead() {
	m_isDead = true;
}

// is this actor facing another one?
bool Actor::isFacingActor(Actor& a) {
	int x = a.getX();
	int y = a.getY();

	if (getDirection() == left && x < getX())
		return true;
	else if (getDirection() == right && x > getX())
		return true;
	else if (getDirection() == down && y < getY())
		return true;
	else if (getDirection() == up && y > getY())
		return true;

	return false;
}

void Actor::faceActor(Actor& a) {
	if (getX() < a.getX())
		setDirection(right);
	else if (getX() > a.getX())
		setDirection(left);
	else if (getY() < a.getY())
		setDirection(up);
	else if (getY() > a.getY())
		setDirection(down);
}
// directly horizontal or vertical of other actor(no dirts or boulder in way) and the distance
// doesn't have to be facing
bool Actor::hasLineOfSight(Actor& a, int distance) {
	int x = a.getX();
	int y = a.getY();
	int horizontalDistance = x - getX();
	int verticalDistance = y - getY();
	bool posDistance = true;
	// if not facing then not in LOS
	if (!(getX() == x || getY() == y))
		return false;
	// check if pos or neg distanc
	if (getX() == x) {
		if (verticalDistance < 0)
			posDistance = false;
		else if (verticalDistance > 0)
			posDistance = true;
	}
	else if (getY() == y) {
		if (horizontalDistance < 0)
			posDistance = false;
		else if (horizontalDistance > 0)
			posDistance = true;
	}
	// vertical
	if (getX() == x) {
		if (getWorld()->isOnBoulderOrDirtDistance(false, posDistance, x, getY(), abs(verticalDistance)))
			return false;
	}
	else if (getY() == y) {// horizontal
		if (getWorld()->isOnBoulderOrDirtDistance(true, posDistance, y, getX(), abs(horizontalDistance)))
			return false;
	}

	return true;
}

// horizontal/vertical only non-radius type
bool Actor::isWithinDistance(Actor& a, int distance) {
	int x = a.getX();
	int y = a.getY();
	int horizontalDistance = x - getX();
	int verticalDistance = y - getY();

	if (((horizontalDistance == 0 && abs(verticalDistance) <= distance)
		|| verticalDistance == 0 && abs(horizontalDistance) <= distance))	// this checks if <= 4 of diggerman
		return false;

	return true;
}

bool Actor::isWithinRadiusOfThreeBoulderInDirection() {
	if (getDirection() == left)
		return getWorld()->isWithinRadiusOfThreeBoulder(getX() - 1, getY());
	else if (getDirection() == right)
		return getWorld()->isWithinRadiusOfThreeBoulder(getX() + 1, getY());
	else if (getDirection() == up)
		return getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() + 1);
	else
		return getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() - 1);
}
// move in direction facing
void Actor::moveDirection() {
	if (getDirection() == left && getX() > 0)
		moveTo(getX() - 1, getY());
	else if (getDirection() == right && getX() < 60)
		moveTo(getX() + 1, getY());
	else if (getDirection() == down && getY() > 0)
		moveTo(getX(), getY() - 1);
	else if (getDirection() == up && getY() < 60)
		moveTo(getX(), getY() + 1);
}
bool Actor::isInMap() {
	bool isInMap = false;
	if (getDirection() == left && getX() > 0)
		isInMap = true;
	else if (getDirection() == right && getX() < 60)
		isInMap = true;
	else if (getDirection() == down && getY() > 0)
		isInMap = true;
	else if (getDirection() == up && getY() < 60)
		isInMap = true;
	return isInMap;
}

bool Actor::isEndOfMapInFront() {
	bool endOfMap = false;
	if (getDirection() == left && getX() - 1 < 0)
		endOfMap = true;
	else if (getDirection() == right && getX() + 1> 60)
		endOfMap = true;
	else if (getDirection() == down && getY() - 1< 0)
		endOfMap = true;
	else if (getDirection() == up && getY() + 1> 60)
		endOfMap = true;
	return endOfMap;
}

void Actor::setRandomDirection() {
	int x = rand() % 3 + 1;
	if (x == 1)
		setDirection(left);
	else if (x == 2)
		setDirection(right);
	else if (x == 3)
		setDirection(up);
	else
		setDirection(down);
}

bool Actor::isOnBoulderOrDirtInDirection() {
	int xLocation = getX();
	int yLocation = getY();

	if (getDirection() == left)
		xLocation--;
	else if (getDirection() == right)
		xLocation++;
	else if (getDirection() == up)
		yLocation++;
	else if (getDirection() == down)
		yLocation--;

	return getWorld()->isOnBoulderOrDirt(xLocation, yLocation);
}

StudentWorld * Actor::getWorld()
{
	return m_studentWorldPointer;
}


//Irritable Implementations
Irritable::Irritable(int imageID, int xLocation, int yLocation, Direction dir, StudentWorld * studentWorldPointer, int hitPoints)
	:Actor(imageID, xLocation, yLocation, dir, 1, 0, studentWorldPointer), m_hitPoints(hitPoints)
{
	setVisible(true);
}

Irritable::~Irritable() {}

void Irritable::decHitPoints(int amount) {
	m_hitPoints -= amount;
}

int Irritable::getHitPoints() {
	return m_hitPoints;
}

//Dirt Implementations
Dirt::Dirt(int xLocation, int yLocation, StudentWorld* studentWorldPointer)
	:Actor(IMID_DIRT, xLocation, yLocation, right, 0.25, 3, studentWorldPointer)
{
	setVisible(true);
}

void Dirt::doSomething() {}

Dirt::~Dirt() {}


//Boulder Implementations
Boulder::Boulder(int xLocation, int yLocation, StudentWorld* studentWorldPointer)
	:Actor(IMID_BOULDER, xLocation, yLocation, down, 1, 1, studentWorldPointer), m_state(stable), m_waitingCounter(30)
{
	setVisible(true);
	getWorld()->clearDirt(xLocation, yLocation);
}

void Boulder::doSomething() {
	if (isDead())// Is boulder dead? do nothing.
		return;
	if (m_state == stable)// Is boulder in stable state?
		if (getWorld()->isDirtBelow(getX(), getY()))// then if boulder on dirt, do nothing
			return;
		else
			m_state = waiting;// else then it is not on dirt, change state to waiting
	else if (m_state == waiting && m_waitingCounter > 0)//if state = waiting and counter is greater than 1, decrement counter
		m_waitingCounter--;
	else if (m_state == waiting && m_waitingCounter == 0) {// if state = waiting and counter is zero, change state to falling
		m_state = falling;
		getWorld()->playSound(SOUND_FALLING_ROCK);//play falling sound
	}
	else if (m_state == falling) {// if falling
		if (getX() == 0 && getY() == 0)// then if on the bottom floor, make self dead.
			makeDead();
		else if (getWorld()->isBoulderBelow(getX(), getY()))// if another boulder is below, make self dead
			makeDead();
		else if (getWorld()->isDirtBelow(getX(), getY()))//if dirt is below, make self dead
			makeDead();
		else {
			moveDirection();// else move in current direction
			getWorld()->annoyFromBoulder(getX(), getY());// annoy nearby actors from boulder
		}
	}
}

Boulder::~Boulder() {}


//DiggerMan Implementations
DiggerMan::DiggerMan(StudentWorld* studentWorldPointer)
	:Irritable(IMID_PLAYER, 30, 60, right, studentWorldPointer, 10), m_unitsOfWater(5), m_sonarCharge(1), m_goldNuggets(0)//2changewater5
{}

DiggerMan::~DiggerMan() {}

void DiggerMan::doSomething() {
	if (isDead())//If dead, return immediately
		return;

	if (getWorld()->isOnDirt(getX(), getY())) {// if on top of dirt,
		getWorld()->clearDirt(getX(), getY());// call StudentWorld's clearDirt to clear a 4x4 dirt patch where the diggerman stands
		getWorld()->playSound(SOUND_DIG);// call sound
	}

	//user key input code needed below
	int ch;
	if (getWorld()->getKey(ch) == true) {
		switch (ch) {
		case KEY_PRESS_LEFT:
			if (getDirection() != left)
				setDirection(left);
			else
				if (!getWorld()->isWithinRadiusOfThreeBoulder(getX() - 1, getY()))
					moveDirection();
			break;
		case KEY_PRESS_RIGHT:
			if (getDirection() != right)
				setDirection(right);
			else
				if (!getWorld()->isWithinRadiusOfThreeBoulder(getX() + 1, getY()))
					moveDirection();
			break;
		case KEY_PRESS_DOWN:
			if (getDirection() != down)
				setDirection(down);
			else
				if (!getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() - 1))
					moveDirection();
			break;
		case KEY_PRESS_UP:
			if (getDirection() != up)
				setDirection(up);
			else
				if (!getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() + 1))
					moveDirection();
			break;
		case KEY_PRESS_SPACE:
			pressSpaceImplementation();
			break;
		case KEY_PRESS_ESCAPE:
			decHitPoints(100);
			break;
		case KEY_PRESS_TAB:
			pressTabImplementation();
			break;
		case 'Z':
		case 'z':
			getWorld()->appearIfWithinRadiusOfTwelveGoldOrBarrel(getX(), getY());
			decSonarKit();
			break;
		default:
			break;
		}
	}
}

void DiggerMan::pressSpaceImplementation() {
	// if diggerman has no water or next space is out of map then do nothing
	if (getWorld()->getDiggerMan()->getWater() == 0 || !isInMap())
		return;
	// out of map case
	// decrease amount of water by 1
	int currentWater = getWorld()->getDiggerMan()->getWater();
	getWorld()->getDiggerMan()->setWater(currentWater - 1);
	Actor* s;
	// if no boulder or dirt in 1 space of direction then create squirt object
	if (getDirection() == left) {
		if (!getWorld()->isSquirtOnBoulderOrDirt(getX() - 1, getY()))
			s = new Squirt(getX() - 1, getY(), getDirection(), getWorld());
		else
			return;
	}
	else if (getDirection() == right) {
		if (!getWorld()->isSquirtOnBoulderOrDirt(getX() + 1, getY()))
			s = new Squirt(getX() + 1, getY(), getDirection(), getWorld());
		else
			return;
	}
	else if (getDirection() == up) {
		if (!getWorld()->isSquirtOnBoulderOrDirt(getX(), getY() + 1))
			s = new Squirt(getX(), getY() + 1, getDirection(), getWorld());
		else
			return;
	}
	else if (getDirection() == down) {
		if (!getWorld()->isSquirtOnBoulderOrDirt(getX(), getY() - 1))
			s = new Squirt(getX(), getY() - 1, getDirection(), getWorld());
		else
			return;
	}
	getWorld()->addActor(std::move(s));
}

void DiggerMan::pressTabImplementation() {
	if (getGold() > 0) {
		enum visibility { invisible, visible };
		enum pickupBy { DiggerMan, Protesters };
		enum state { permanent, temporary };
		Actor* p = new GoldNugget(getX(), getY(), getWorld(), static_cast<GoldNugget::visibility>(visible), static_cast<GoldNugget::pickupBy>(Protesters), static_cast<GoldNugget::state>(temporary));
		getWorld()->addActor(std::move(p));
		decGold();
	}
}

int DiggerMan::getWater() const {
	return m_unitsOfWater;
}
int DiggerMan::getSonar() const {
	return m_sonarCharge;
}
int DiggerMan::getGold() const {
	return m_goldNuggets;
}
void DiggerMan::setWater(int water) {
	m_unitsOfWater = water;
}
void DiggerMan::setSonar(int sonar) {
	m_sonarCharge = sonar;
}
void DiggerMan::setGold(int gold) {
	m_goldNuggets = gold;
}
void DiggerMan::decGold() {
	m_goldNuggets--;
}
void DiggerMan::decSonarKit() {
	m_sonarCharge--;
}
void DiggerMan::incrementSonar() {
	m_sonarCharge++;
}
void DiggerMan::incrementGoldNuggets() {
	m_goldNuggets++;
}
void DiggerMan::increaseWater(int amount) {
	m_unitsOfWater += amount;
}

//Node Implementations
Node::Node(int x, int y)
	:m_x(x), m_y(y), up(nullptr), right(nullptr), down(nullptr), left(nullptr), next(nullptr), prev(nullptr)
{}

//Tree Implementations
Tree::Tree(int x, int y, int finalX, int finalY, StudentWorld* studentWorldPointer)
	: m_finalX(finalX), m_finalY(finalY), world(studentWorldPointer)
{
	root = new Node(x, y);
	add(root, x, y);
	fillCoordinates(root);
}

bool Tree::isAlreadyExistsInPath(Node* node, int x, int y) {
	if (node->prev == nullptr && (node->m_x != x || node->m_y != y))
		return false;
	if (node->m_x == x && node->m_y == y)
		return true;
	return isAlreadyExistsInPath(node->prev, x, y);
}

bool Tree::isAlreadyExist(Node* node, int x, int y) {
	std::queue<Node*> q;
	Node* guess;
	q.push(node);
	while (!q.empty()) {
		guess = q.front();
		q.pop();
		if (guess == nullptr)
			continue;
		if (guess->m_x == x && guess->m_y == y)
			return true;
		if (guess->prev != guess->up)
			q.push(guess->up);
		if (guess->prev != guess->right)
			q.push(guess->right);
		if (guess->prev != guess->down)
			q.push(guess->down);
		if (guess->prev != guess->left)
			q.push(guess->left);
	}
	return false;
}

void Tree::fillCoordinates(Node* node) {
	std::queue<Node*> q;
	Node* guess;
	q.push(node);
	while (!q.empty()) {
		guess = q.front();
		q.pop();
		if (guess == nullptr)
			continue;
		if (guess->m_x == m_finalX && guess->m_y == m_finalY)
			break;
		if (guess->prev != guess->up)
			q.push(guess->up);
		if (guess->prev != guess->right)
			q.push(guess->right);
		if (guess->prev != guess->down)
			q.push(guess->down);
		if (guess->prev != guess->left)
			q.push(guess->left);
	}

	Node* workingPointer = guess;
	while (workingPointer != root) {
		workingPointer->prev->next = workingPointer;
		workingPointer = workingPointer->prev;
	}
	while (workingPointer != nullptr) {
		m_xCoordinates.push(workingPointer->m_x);
		m_yCoordinates.push(workingPointer->m_y);
		workingPointer = workingPointer->next;
	}
}

std::queue<int> Tree::getXCoordinates() {
	return m_xCoordinates;
}

std::queue<int> Tree::getYCoordinates() {
	return m_yCoordinates;
}

void Tree::add(Node* nodeptr, int x, int y) {
	if (x == m_finalX && y == m_finalY)//Changed
		return;

	if (!world->isOnDirt(x, y + 1) && y + 1 <= 60 && !isAlreadyExist(root, x, y + 1) && !world->isWithinRadiusOfThreeBoulder(x, y + 1)) {//Changed
		nodeptr->up = new Node(x, y + 1);
		nodeptr->up->down = nodeptr;
		nodeptr->up->prev = nodeptr;
		add(nodeptr->up, x, y + 1);
	}
	if (!world->isOnDirt(x + 1, y) && x + 1 <= 60 && !isAlreadyExist(root, x + 1, y) && !world->isWithinRadiusOfThreeBoulder(x + 1, y)) {
		nodeptr->right = new Node(x + 1, y);
		nodeptr->right->left = nodeptr;
		nodeptr->right->prev = nodeptr;
		add(nodeptr->right, x + 1, y);
	}
	if (!world->isOnDirt(x, y - 1) && y - 1 >= 0 && !isAlreadyExist(root, x, y - 1) && !world->isWithinRadiusOfThreeBoulder(x, y - 1)) {
		nodeptr->down = new Node(x, y - 1);
		nodeptr->down->up = nodeptr;
		nodeptr->down->prev = nodeptr;
		add(nodeptr->down, x, y - 1);
	}
	if (!world->isOnDirt(x - 1, y) && x - 1 >= 0 && !isAlreadyExist(root, x - 1, y) && !world->isWithinRadiusOfThreeBoulder(x - 1, y)) {
		nodeptr->left = new Node(x - 1, y);
		nodeptr->left->right = nodeptr;
		nodeptr->left->prev = nodeptr;
		add(nodeptr->left, x - 1, y);
	}
}

//added
void Tree::Destroy(Node* node) {
	if (node == nullptr)
		return;

	if (node->prev != node->up)
		Destroy(node->up);
	if (node->prev != node->right)
		Destroy(node->right);
	if (node->prev != node->down)
		Destroy(node->down);
	if (node->prev != node->left)
		Destroy(node->left);

	delete node;
	node = nullptr;
}

//added
Tree::~Tree() {
	Destroy(root);
}

//Protester Implementations
Protester::Protester(int imageID, StudentWorld* studentWorldPointer, int hitPoints, int shoutCount)
	:Irritable(imageID, 60, 60, left, studentWorldPointer, hitPoints), m_leaveTheOilFieldState(false), 
	shoutCooldownCount(shoutCount), numSquaresToMoveInCurrentDirection(rand() % 53 + 8), perpendicularCountdown(200),
	tickCountdown(getWorld()->max(0, 3 - getWorld()->getLevel() / 4))
{
	setVisible(true);
}

void Protester::doSomething() {}


bool Protester::isPerpendicularCountdown() {
	if (perpendicularCountdown >= 0)
		return true;
	return false;
}
void Protester::setPerpendicularCountdown(int x) {
	perpendicularCountdown = x;
}
void Protester::decPerpendicularCountdown() {
	perpendicularCountdown--;
}
void Protester::leaveTheOilField() {
	if (getX() == 60 && getY() == 60) {
		makeDead();
		return;
	}

	if (m_xCoordinates.front() > getX())
		setDirection(right);
	if (m_xCoordinates.front() < getX())
		setDirection(left);
	if (m_yCoordinates.front() > getY())
		setDirection(up);
	if (m_yCoordinates.front() < getY())
		setDirection(down);

	moveTo(m_xCoordinates.front(), m_yCoordinates.front());
	m_xCoordinates.pop();
	m_yCoordinates.pop();
}
void Protester::decShoutCooldownCount() {
	shoutCooldownCount--;
}
int Protester::getNumOfSquaresToMoveInDirection() {
	return numSquaresToMoveInCurrentDirection;
}
void Protester::setNumOfSquaresToMoveInDirection(int x) {
	numSquaresToMoveInCurrentDirection = x;
}
void Protester::decNumOfSquaresToMoveInDirection() {
	numSquaresToMoveInCurrentDirection--;
}
void Protester::setShoutCooldownCount(int x) {
	shoutCooldownCount = x;
}
bool Protester::leaveTheOilFieldState() {
	return m_leaveTheOilFieldState;
}

void Protester::changeOilFieldState(bool oilFieldState) {
	m_leaveTheOilFieldState = oilFieldState;
	if (oilFieldState == true) {
		Tree tree(getX(), getY(), 60, 60, getWorld());
		m_xCoordinates = tree.getXCoordinates();
		m_yCoordinates = tree.getYCoordinates();
	}
}

void Protester::decTickCountdown() {
	tickCountdown--;
}

int Protester::getTickCountdown() {
	return tickCountdown;
}
void Protester::setTickCountdown(int tick) {
	tickCountdown = tick;
}
void Protester::setStatus(int x) {
	if (getHitPoints() > 0) {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		setTickCountdown(getWorld()->max(50, 100 - getWorld()->getLevel() * 10));
	}
	else {	// died due to squirt
		getWorld()->increaseScore(x);
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		changeOilFieldState(true);
	}
}

bool Protester::isShoutCooldown() {
	if (shoutCooldownCount != 0)
		return true;
	return false;
}
bool Protester::moveNumberOfSquares() {
	if (numSquaresToMoveInCurrentDirection > 0 && !isOnBoulderOrDirtInDirection()
		&& !isWithinRadiusOfThreeBoulderInDirection() && !isEndOfMapInFront()
		&& !hasLineOfSight(*getWorld()->getDiggerMan(), 4)) {
		moveDirection();
		numSquaresToMoveInCurrentDirection--;
		return true;
	}
	else
		return false;
}

Protester::~Protester() {}


//RegularProtester Implementations
RegularProtester::RegularProtester(StudentWorld* studentWorldPointer)
	:Protester(IMID_PROTESTER, studentWorldPointer, 5, 0)
{
	//NOT DONE CONSTRUCTING - PG 39 - SQUARES TO MOVE
}

// choiceNumber when only have one choice
// choiceNumber 0-nochoice/multiple,1-left,2-right,3-up,4-down
bool Protester::perpendicularChoice(int& numOfChoices, int& choiceNumber1, int& choiceNumber2) {
	choiceNumber1 = 0;
	choiceNumber2 = 0;
	if (getDirection() == left || getDirection() == right) {
		if (!getWorld()->isOnBoulderOrDirt(getX(), getY() + 1)) {
			if (!getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() + 1)) {
				numOfChoices++;
				choiceNumber1 = 3;
			}
		}
		if (!getWorld()->isOnBoulderOrDirt(getX(), getY() - 1)) {
			if (!getWorld()->isWithinRadiusOfThreeBoulder(getX(), getY() - 1)) {
				numOfChoices++;
				choiceNumber2 = 4;
			}
		}
	}
	else {
		if (!getWorld()->isOnBoulderOrDirt(getX() + 1, getY())) {
			if (!getWorld()->isWithinRadiusOfThreeBoulder(getX() + 1, getY())) {
				numOfChoices++;
				choiceNumber1 = 2;
			}
		}
		if (!getWorld()->isOnBoulderOrDirt(getX() - 1, getY())) {
			if (!getWorld()->isWithinRadiusOfThreeBoulder(getX() - 1, getY())) {
				numOfChoices++;
				choiceNumber2 = 1;
			}
		}
	}

	if (numOfChoices > 0)
		return true;

	return false;
}


void RegularProtester::doSomething() {
	if (isDead())
		return;

	// non-rest tick
	if (getTickCountdown() == 0) {
		if (getNumOfSquaresToMoveInDirection() <= 0) {
			// TODO:
			// random direction
			if (!hasLineOfSight(*getWorld()->getDiggerMan(), 4))
				setRandomDirection();

			//	so that it will keep changing directions if its stuck
			if (!isOnBoulderOrDirtInDirection() && !isWithinRadiusOfThreeBoulderInDirection())
				setNumOfSquaresToMoveInDirection(rand() % 53 + 8);
		}
		if (leaveTheOilFieldState()) {
			leaveTheOilField();
			return;
		}
		// if shout not on cooldown
		// yell at diggerman and set his status to annoyed
		// else decrement shoutcooldown if it has a cooldown
		if (getWorld()->isWithinRadiusOf(getX(), getY(), getWorld()->getDiggerMan()->getX()
			, getWorld()->getDiggerMan()->getY(), 4) && isFacingActor(*getWorld()->getDiggerMan())
			&& !isShoutCooldown()) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getDiggerMan()->decHitPoints(2);
			setShoutCooldownCount(20);
			return;	// just return if shout
		}
		else if (hasLineOfSight(*getWorld()->getDiggerMan(), 4)
			&& isWithinDistance(*getWorld()->getDiggerMan(), 4)) {
			if (!isFacingActor(*getWorld()->getDiggerMan())) {
				faceActor(*getWorld()->getDiggerMan());
				moveDirection();
			}
			else
				moveDirection();

			setNumOfSquaresToMoveInDirection(0);
			return;
		}
		else
			decNumOfSquaresToMoveInDirection();

		if (!isPerpendicularCountdown()) {
			//	using number to do direction
			//	choiceNumber is returned 0 when no choice
			int numOfChoices = 0;
			int choiceNumber1 = 0;
			int choiceNumber2 = 0;
			if (perpendicularChoice(numOfChoices, choiceNumber1, choiceNumber2)) {
				//	randomize when 2 choices
				int choice = 0;
				if (numOfChoices == 2) {
					int randArray[2] = { choiceNumber1, choiceNumber2 };
					choice = randArray[rand() % 3 + 1];
				}
				else if (choiceNumber1 == 0)
					choice = choiceNumber2;
				else
					choice = choiceNumber1;

				if (choice == 1)
					setDirection(left);
				else if (choice == 2)
					setDirection(right);
				else if (choice == 3)
					setDirection(up);
				else
					setDirection(down);

				setPerpendicularCountdown(200);
			}
		}

		if (!moveNumberOfSquares())
			setNumOfSquaresToMoveInDirection(0);

		if (isShoutCooldown())
			decShoutCooldownCount();

		if (isPerpendicularCountdown())
			decPerpendicularCountdown();

		setTickCountdown(getWorld()->max(0, 3 - getWorld()->getLevel() / 4));
	}
	else
		decTickCountdown();
}



RegularProtester::~RegularProtester() {}


//HardcoreProtester Implementations
HardcoreProtester::HardcoreProtester(StudentWorld* studentWorldPointer)
	:Protester(IMID_HARD_CORE_PROTESTER, studentWorldPointer, 20, 0)
{
	//NOT DONE CONSTRUCTING - PG 44 - SQUARES TO MOVE
}

void HardcoreProtester::doSomething() {
	if (isDead())
		return;

	if (getTickCountdown() == 0) {
		if (leaveTheOilFieldState()) {
			leaveTheOilField();
			return;
		}
		if (getNumOfSquaresToMoveInDirection() <= 0) {
			// TODO:
			// random direction
			if (!hasLineOfSight(*getWorld()->getDiggerMan(), 4))
				setRandomDirection();

			//	so that it will keep changing directions if its stuck
			if (!isOnBoulderOrDirtInDirection() && !isWithinRadiusOfThreeBoulderInDirection())
				setNumOfSquaresToMoveInDirection(rand() % 53 + 8);
		}

		if (getWorld()->isWithinRadiusOf(getX(), getY(), getWorld()->getDiggerMan()->getX()
			, getWorld()->getDiggerMan()->getY(), 4) && isFacingActor(*getWorld()->getDiggerMan())
			&& !isShoutCooldown()) {
			getWorld()->playSound(SOUND_PROTESTER_YELL);
			getWorld()->getDiggerMan()->decHitPoints(2);
			setShoutCooldownCount(20);
			return;	// just return if shout
		}
		else if (hasLineOfSight(*getWorld()->getDiggerMan(), 4)
			&& isWithinDistance(*getWorld()->getDiggerMan(), 4)) {
			if (!isFacingActor(*getWorld()->getDiggerMan())) {
				faceActor(*getWorld()->getDiggerMan());
				moveDirection();
			}
			else
				moveDirection();

			setNumOfSquaresToMoveInDirection(0);
			return;
		}
		else
			decNumOfSquaresToMoveInDirection();
		/*
		else if (!getWorld()->isWithinRadiusOf(getX(), getY(), getWorld()->getDiggerMan()->getX(),
			getWorld()->getDiggerMan()->getY(), 4)) {	// not within radius of 4 over diggerman
			int m = 16 + getWorld()->getLevel() * 2;	// number of moves
				Tree tree(getX(), getY(), getWorld()->getDiggerMan()->getX(), getWorld()->getDiggerMan()->getY(),
					getWorld());
				m_xCoordinates = tree.getXCoordinates();
				m_yCoordinates = tree.getYCoordinates();
			if (m_xCoordinates.size() <= m) {	// steps to get to diggerman
				if (m_xCoordinates.front() > getX())
						setDirection(right);
				if (m_xCoordinates.front() < getX())
						setDirection(left);
				if (m_yCoordinates.front() > getY())
						setDirection(up);
				if (m_yCoordinates.front() < getY())
						setDirection(down);

				m_xCoordinates.pop();
				m_yCoordinates.pop();

				moveTo(m_xCoordinates.front(), m_yCoordinates.front());

				return;
			}
			while (!m_xCoordinates.empty()) {
				m_xCoordinates.pop();
				m_yCoordinates.pop();
			}
		}
		*/

		if (!isPerpendicularCountdown()) {
			//	using number to do direction
			//	choiceNumber is returned 0 when no choice
			int numOfChoices = 0;
			int choiceNumber1 = 0;
			int choiceNumber2 = 0;
			if (perpendicularChoice(numOfChoices, choiceNumber1, choiceNumber2)) {
				//	randomize when 2 choices
				int choice = 0;
				if (numOfChoices == 2) {
					int randArray[2] = { choiceNumber1, choiceNumber2 };
					choice = randArray[rand() % 3 + 1];
				}
				else if (choiceNumber1 == 0)
					choice = choiceNumber2;
				else
					choice = choiceNumber1;

				if (choice == 1)
					setDirection(left);
				else if (choice == 2)
					setDirection(right);
				else if (choice == 3)
					setDirection(up);
				else
					setDirection(down);

				setPerpendicularCountdown(200);
			}
		}

		if (!moveNumberOfSquares())
			setNumOfSquaresToMoveInDirection(0);

		if (isShoutCooldown())
			decShoutCooldownCount();
	}
	else
		decTickCountdown();
}

HardcoreProtester::~HardcoreProtester() {}

//Squirt Implementations
Squirt::Squirt(int xLocation, int yLocation, Direction dir, StudentWorld* studentWorldPointer)
	:Actor(IMID_WATER_SPURT, xLocation, yLocation, dir, 1.0, 1, studentWorldPointer) {
	setVisible(true);
	// spawn in front of diggerman by 1 unit
	// maxDistance depends on direction travelling
	// 3 units in direction because initialized counts as 1 unit
	if (dir == left)
		maxDistance = getX() - 3;
	else if (dir == right)
		maxDistance = getX() + 3;
	else if (dir == up)
		maxDistance = getY() + 3;
	else if (dir == down)
		maxDistance = getY() - 3;
}

bool Squirt::isMaxDistance(int currentLocation) const {
	if (currentLocation == getMaxDistance())
		return true;
	return false;
}
int Squirt::getMaxDistance() const {
	return maxDistance;
}
void Squirt::doSomething() {
	// if nothing is in the way
	// move 4 units in direction it's facing
	int currentLocation;	// depends on direction it's going
	if (getDirection() == left || getDirection() == right)
		currentLocation = getX();
	else if (getDirection() == up || getDirection() == down)
		currentLocation = getY();
	if (!isDead() && !isMaxDistance(currentLocation)) {
		// TODO: interact with other actor
		// if hits dirt, boulder, or runs out of map then set status to dead
		// if squirt on protester then make squirt dead
		// annoy protester by 2 points
		int x = 0;
		int y = 0;
		// x and y returns location of protester
		if (getWorld()->isWithinRadiusOfThreeProtester(getX(), getY(), x, y)) {
			makeDead();
			Protester* a;
			if (getWorld()->getProtesterAtLocation(x, y, a) && !(a->leaveTheOilFieldState())) {
				// if in oilfield then just dec and annoy
				a->decHitPoints(2);
				if (Protester* b = dynamic_cast<RegularProtester*>(a)) {
					a->setStatus(100);
				}
				else {
					a->setStatus(250);
				}
			}
		}

		if (getWorld()->isOnDirt(getX(), getY()) || getWorld()->isOnBoulder(getX(), getY()) || !isInMap()) {
			makeDead();
		}

		if (!isDead()) {
			moveDirection();
			getWorld()->playSound(SOUND_PLAYER_SQUIRT);	// not working?
		}

	}
	else
		makeDead();
}


//SonarKit Implementations
SonarKit::SonarKit(StudentWorld* studentWorldPointer)
	: Actor(IMID_SONAR, 0, 60, right, 1, 2, studentWorldPointer)
{
	setVisible(true);
	if (300 - 10 * getWorld()->getLevel() > 100)
		m_tickLifeTimeCounter = 300 - (10 * getWorld()->getLevel());
	else
		m_tickLifeTimeCounter = 100;
}

void SonarKit::doSomething() {
	if (isDead())
		return;
	if (getWorld()->isWithinRadiusOfThreeOfDiggerman(getX(), getY())) {
		makeDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getDiggerMan()->incrementSonar();
		getWorld()->getDiggerMan()->incrementSonar();//Twice
		getWorld()->increaseScore(75);
	}
	if (m_tickLifeTimeCounter > 0)
		m_tickLifeTimeCounter--;
	else
		makeDead();
}

SonarKit::~SonarKit() {}


//WaterPool Implementations
WaterPool::WaterPool(int xLocation, int yLocation, StudentWorld* studentWorldPointer)
	:Actor(IMID_WATER_POOL, xLocation, yLocation, right, 1, 2, studentWorldPointer)
{
	setVisible(true);
	if (300 - (10 * getWorld()->getLevel()) > 100)
		m_tickLifeTimeCounter = 300 - 10 * getWorld()->getLevel();
	else
		m_tickLifeTimeCounter = 100;
}

void WaterPool::doSomething() {
	if (isDead())
		return;
	if (getWorld()->isWithinRadiusOfThreeOfDiggerman(getX(), getY())) {
		makeDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->getDiggerMan()->increaseWater(5);
		getWorld()->increaseScore(100);
	}
	if (m_tickLifeTimeCounter > 0)
		m_tickLifeTimeCounter--;
	else
		makeDead();
}

WaterPool::~WaterPool() {}


//Gold Nugget Implementations
GoldNugget::GoldNugget(int xLocation, int yLocation, StudentWorld* studentWorldPointer, visibility visibility, pickupBy pickupBy, state state)
	:Actor(IMID_GOLD, xLocation, yLocation, right, 1, 2, studentWorldPointer), m_pickupBy(pickupBy), m_state(state)
{
	if (visibility == visible)
		setVisible(true);
	if (state == temporary)
		if (300 - 10 * getWorld()->getLevel() > 100)
			m_tickLifeTimeCounter = 300 - 10 * getWorld()->getLevel();
		else
			m_tickLifeTimeCounter = 100;
}

void GoldNugget::doSomething() {
	if (isDead())
		return;
	if (!isVisible() && getWorld()->isWithinRadiusOf(getX(), getY(), getWorld()->getDiggerMan()->getX(), getWorld()->getDiggerMan()->getY(), 4.0)) {//if not visible, and not within rad of 4 of Dman
		setVisible(true);
		return;
	}
	if (m_pickupBy == DiggerMan && getWorld()->isWithinRadiusOfThreeOfDiggerman(getX(), getY())) {
		makeDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->getDiggerMan()->incrementGoldNuggets();
	}
	int x = 0;
	int y = 0;
	if (m_pickupBy == Protesters && getWorld()->isWithinRadiusOfThreeProtester(getX(), getY(), x, y)) {
		//INFORM PROTESTER OBJECT THAT IT JUST RECEIVED A NEW NUGGET SO IT CAN REACT APPROPRIATELY(E.G., BE BRIBED)
		//IF MORE THAN ONE PROTESTER IS WITHIN RANGE, ONLY ONE OF THEM CAN BE BRIBED
		getWorld()->increaseScore(25);
		// get protester at that location
		Protester* a;
		if (getWorld()->getProtesterAtLocation(x, y, a)) {
			if (RegularProtester* rp = dynamic_cast<RegularProtester*>(a)) {
				getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				a->changeOilFieldState(true);
				makeDead();
			}
			else {
				getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
				getWorld()->increaseScore(50);
				a->setTickCountdown(getWorld()->max(50, 100 - getWorld()->getLevel() * 10));
			}
		}
	}
	if (m_state == temporary) {
		if (m_tickLifeTimeCounter > 0)
			m_tickLifeTimeCounter--;
		else
			makeDead();
	}
}

GoldNugget::~GoldNugget() {}


//Barrel Implementations
Barrel::Barrel(int xLocation, int yLocation, StudentWorld* studentWorldPointer)
	:Actor(IMID_BARREL, xLocation, yLocation, right, 1, 2, studentWorldPointer)
{}

void Barrel::doSomething() {
	if (isDead())
		return;
	if (!isVisible() && getWorld()->isWithinRadiusOf(getX(), getY(), getWorld()->getDiggerMan()->getX(), getWorld()->getDiggerMan()->getY(), 4.0)) {//if not visible, and not within rad of 4 of Dman
		setVisible(true);
		return;
	}
	if (getWorld()->isWithinRadiusOfThreeOfDiggerman(getX(), getY())) {
		makeDead();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
		getWorld()->decBarrelsCounter();
	}
}

Barrel::~Barrel() {}
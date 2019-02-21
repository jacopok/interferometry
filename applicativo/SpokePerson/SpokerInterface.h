#ifndef SPOKERINTERFACE_H
#define SPOKERINTERFACE_H

class SpokerInterface {
public:
	virtual void setCStep(int cstep) = 0;
	virtual void noSerPort() = 0;
};

#endif


#ifndef CoalescenceManagerHeader
#define CoalescenceManagerHeader

#include <RayPlatform/actors/Actor.h>

class CoalescenceManager : public Actor {

public:

	CoalescenceManager();
	~CoalescenceManager();

	void receive(Message & message);
	void receivePayload(Message & message);

	enum {
		PAYLOAD = 3,
		PAYLOAD_RESPONSE
	};
};

#endif

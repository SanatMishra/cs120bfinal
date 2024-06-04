#ifndef _ACTORLISTH_
#define _ACTORLISTH_

#include "common.h"

template <typename T, uchar MAX_ACTORS>
class ActorList {
public:
  // static array of actors. each actor maintains forward and back pointers (na, pa), 
  // so we can run a doubly-linked ring of actors from MAX_ACTORS to actb to acte to 
  // MAX_ACTORS. Actors that have not been yet assigned are entered into a stack starting
  // at actfb, to all other free elements and finally to MAX_ACTORS. For the free stack, 
  // only the forward pointer (na) is maintained. Upon actor creation, actfb is inserted
  // in between acte and MAX_ACTORS, acte is set to actfb, and actfb is set to the next 
  // element in the free stack. Upon actor deletion, the reverse happens. Upon attempting
  // to delete a nonexistent actor, the game crashes. Upon attempting to add beyond 
  // MAX_ACTORS, no add occurs but the game continues as usual.
  T actors[MAX_ACTORS];
  uchar actb, acte, actfb;

  ActorList() {
    actb = acte = MAX_ACTORS;
    actfb = 0;
    for (uchar i = 0; i < MAX_ACTORS; i++) {
      actors[i].na = i + 1;
      actors[i].pa = MAX_ACTORS + 1;
    }
  }

  const T& operator[](int i) const { return actors[i]; }
  T& operator[](int i) { return actors[i]; }

  // returns index of new actor. Expect bogus data already inside
  uchar addActor() {
  if (actfb == MAX_ACTORS) {
      serial_println("NO FREE ACTORS");
      return MAX_ACTORS;
  }
  uchar actfbOrig = actfb;
  if (acte < MAX_ACTORS) {
      uchar nextActfb = actors[actfb].na;
      actors[acte].na = actfb;
      actors[actfb].na = MAX_ACTORS;
      actors[actfb].pa = acte;
      acte = actfb;
      actfb = nextActfb;
  } else {
      actb = acte = actfb;
      actfb = actors[actfb].na;
      actors[actb].pa = MAX_ACTORS;
      actors[actb].na = MAX_ACTORS;
  }
  return actfbOrig;
}
  void deleteActor(uchar x) {
  if (actors[x].pa == MAX_ACTORS + 1) {
      serial_println("DELETING ALREADY-FREE ACTOR");
      actfb = 1/0;
  }
  if (actors[x].pa < MAX_ACTORS)
      actors[actors[x].pa].na = actors[x].na;
  else
      actb = actors[actb].na;
  if (actors[x].na < MAX_ACTORS)
      actors[actors[x].na].pa = actors[x].pa;
  else
      acte = actors[acte].pa;
  actors[x].na = actfb;
  actors[x].pa = MAX_ACTORS + 1;
  actfb = x;
}

};


#endif
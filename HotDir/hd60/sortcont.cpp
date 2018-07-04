/* sortcont.cpp - a container with sorted content

see 'sortcont.h' for details

http://vividos.home.pages.de - vividos@asamnet.de
(c) 1998-1999 Michael Fink
*/

#include "sortcont.h"

sortable_object::sortable_object(){
};

bool sortable_object::is_bigger_than(sortable_object *so){
  so; return true;
};

sorted_container::sorted_container(){

};

void sorted_container::sorted_insert(sortable_object *so){
/* Zuerst wird geschaut, ob so kleiner als das erste Objekt ist. Wenn ja,
 wird eingefgt. Wenn nein, wird geschaut, ob es gr”áer als das letzte Objekt
 ist. Bei ja wird eingefgt, bei nein wird die ganze Liste von hinten nach
 vorne durchsucht und verglichen, bis es reinpasst.
*/

  if (is_empty()) // ist der container noch leer?
    insert(so); // normales insert aufrufen
  else {  // nein, nicht leer:
      // test: muá ich vorne einfgen?
      // dazu schauen, ob 'so' > 'nose' ist
    sortable_object *noseobj; // und dazu: objekt in Liste ganz vorne=noseobj
    move(0,true); noseobj=(sortable_object*)getObject();

    if (!(so->is_bigger_than(noseobj))) // schaun obs nicht gr”áer ist
      insert(so,true); // nein:dann vorne einfgen
    else { // ja, entweder hinten einfgen, oder liste durchgehen, bis passend
	now=tail; // test: ist 'so' > 'tail' ?
      if ((so->is_bigger_than((sortable_object*)getObject() )))
	// ja: hinten einfgen, now steht ja schon auf tail
	insert(so);
      else { // nein: liste von hinten nach vorne durchgehen, bis es reinpasst
	while(nose!=now&&now!=NULL){ // solange nicht wieder vorne: schleife
	  move(-1); // eins nach vorne bewegen
	  if ((so->is_bigger_than((sortable_object*)getObject() ))){
	    insert(so); // passt rein: einfgen
	    return; // und raus aus der Schleife
	  };
	};
	  now=tail; insert(so); // keins gefunden, einfach hinten einfgen
      };
    };
  };
};

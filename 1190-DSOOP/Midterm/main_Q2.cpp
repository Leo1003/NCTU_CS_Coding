#include <iostream>
#include <cstring>
#include <string.h>
#include "ZooAnimal.h"
using namespace std;

int main()
{
  int today;
  int animal_id;
  cin >> animal_id >> today;

  if (animal_id == 1)
  {
    ZooAnimal pig;
    cout << "Name: " << pig.reptName() << endl;
    cout << "Weight: " << pig.reptWeight() << endl;
    cout << "Weight Date: " << pig.reptWeightDate() << endl;
    cout << "Since Last Weight Date: " << pig.daysSinceLastWeighed(today);
  }
  else if (animal_id == 2)
  {
    ZooAnimal dinosaur("titanosaurs", 208, 90000);
    cout << "Name: " << dinosaur.reptName() << endl;
    cout << "Weight: " << dinosaur.reptWeight() << endl;
    cout << "Weight Date: " << dinosaur.reptWeightDate() << endl;
    cout << "Since Last Weight Date: " << dinosaur.daysSinceLastWeighed(today);
  }
  else if (animal_id == 3)
  {
    LargeAnimal elephant("Lin_Wang", 1225, 5500, "Mammal", 28.5);
    cout << "Name: " << elephant.ZooAnimal::reptName() << endl;
    cout << "Species: " << elephant.LargeAnimal::reptName() << endl;
    cout << "Cage Minimum Volume: " << elephant.reptCageMinVolume() << endl;
    cout << "Weight: " << elephant.reptWeight() << endl;
    cout << "Weight Date: " << elephant.reptWeightDate() << endl;
    cout << "Since Last Weight Date: " << elephant.daysSinceLastWeighed(today);
    if (elephant.daysSinceLastWeighed(today) > 100)
    {
	  cout << endl << "Reweigh..." << endl;
      elephant.changeWeight(5000);
      elephant.changeWeightDate(today);
      cout << "Weight: " << elephant.reptWeight() << endl;
      cout << "Weight Date: " << elephant.reptWeightDate();
    }
  }
  return 0;
}

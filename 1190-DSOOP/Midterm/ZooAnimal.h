#ifndef ZooAnimal_H
#define ZooAnimal_H

#include <iostream>
#include <cstring>
#include <string.h>
using namespace std;

/*-----Write Your Code Here----*/

class ZooAnimal {
public:
    ZooAnimal(const char *name = "Nameless", int date = 101, int weight = 100)
        : weightDate(date), weight(weight)
    {
        strncpy(this->name, name, sizeof(this->name));
    }

    const char *reptName()
    {
        return name;
    }

    int reptWeight()
    {
        return weight;
    }

    inline int reptWeightDate()
    {
        return weightDate;
    }

    int daysSinceLastWeighed(int today)
    {
        int last =  ZooAnimal::intoday(today) - ZooAnimal::intoday(weightDate);
        if (last < 0) {
            last += 360;
        }
        return last;
    }

    inline void changeWeightDate(int date)
    {
        weightDate = date;
    }

    inline void changeWeight(int new_value)
    {
        weight = new_value;
    }

private:
    static int intoday(int date)
    {
        int month = date / 100;
        int day = date % 100;
        return (month - 1) * 30 + day;
    }

protected:
    char name[1001];
    int weightDate, weight;
};

class LargeAnimal: public ZooAnimal {
public:
    LargeAnimal(const char *name, int date, int weight, const char *spec, float cagemin)
        : ZooAnimal(name, date, weight), cageMinimumVolume(cagemin)
    {
        strncpy(this->species, spec, sizeof(this->species));
    }

    const char *reptName()
    {
        return species;
    }

    float reptCageMinVolume()
    {
        return cageMinimumVolume;
    }

private:
    char species[1001];
    float cageMinimumVolume;
};

/*------------------------------*/

#endif

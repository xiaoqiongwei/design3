#ifndef ANTENNAPARAM_H_
#define ANTENNAPARAM_H_

#include <iostream>

class AntennaParam {

public:
    const static int ORIENTATION_NORTH = 1;
    const static int ORIENTATION_SOUTH = 2;
    const static int ORIENTATION_EAST = 3;
    const static int ORIENTATION_WEST = 4;

    int number;
    bool isBig;
    int orientation;

    AntennaParam();
    virtual ~AntennaParam();
    void set(int number, bool isBig, int orientation);
    std::string getOrientationLetter();
    std::string getIsBigLetter();

private:

};

#endif /* ANTENNAPARAM_H_ */

#ifndef ANTENNAPARAM_H_
#define ANTENNAPARAM_H_

class AntennaParam {

public:
    const static int ORIENTATION_NORTH = 1;
    const static int ORIENTATION_SOUTH = 2;
    const static int ORIENTATION_EAST = 3;
    const static int ORIENTATION_WEST = 4;

    AntennaParam();
    virtual ~AntennaParam();

    bool isBig();
    int getNumber();
    int getOrientation();

private:
    int number;
    bool big;
    int orientation;
};

#endif /* ANTENNAPARAM_H_ */
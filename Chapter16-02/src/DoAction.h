#pragma once
#include "Action.h"
#include "ActionGenerator.h"
#include "Robot.h"
class DoAction {
    int currActionIndex = 0;
    int currStep = 0;
    Action *currAction;
    Robot *robot;
    ActionGenerator *actionGen;

public:
    DoAction(Robot *robot);
    void run();
    ~DoAction();
};

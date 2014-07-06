//
//  NDKCallbackNode.h
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#ifndef __EasyNDK_for_cocos2dx__NDKCallbackNode__
#define __EasyNDK_for_cocos2dx__NDKCallbackNode__

#include "cocos2d.h"
#include <string>
USING_NS_CC;
using namespace std;

class NDKCallbackNode
{
    private :
    //SEL_CallFuncN sel;
    std::function<void(Ref*, void*)> sel;
    string name;
    string groupName;
    Ref *target;
    
    public :
    NDKCallbackNode(const char *groupName, const char *name, std::function<void(Ref*, void*)> sel, Ref *target);
    string getName();
    string getGroup();
    std::function<void(Ref*, void*)> getSelector();
    Ref* getTarget();
};

#endif /* defined(__EasyNDK_for_cocos2dx__NDKCallbackNode__) */

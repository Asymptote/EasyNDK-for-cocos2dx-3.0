//
//  NDKHelper.cpp
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#include "NDKHelper.h"

#define __CALLED_METHOD__           "calling_method_name"
#define __CALLED_METHOD_PARAMS__    "calling_method_params"

vector<NDKCallbackNode> NDKHelper::selectorList;

void NDKHelper::AddSelector(const char *groupName, const char *name, std::function<void(Ref*, void*)> selector, Ref* target)
{
    NDKHelper::selectorList.push_back(NDKCallbackNode(groupName, name, selector, target));
    //selector.
}

void NDKHelper::RemoveAtIndex(int index)
{
    NDKHelper::selectorList[index] = NDKHelper::selectorList.back();
    NDKHelper::selectorList.pop_back();
}

void NDKHelper::RemoveSelectorsInGroup(const char *groupName)
{
    std::vector<int> markedIndices;
    
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i)
    {
        if (NDKHelper::selectorList[i].getGroup().compare(groupName) == 0)
        {
            markedIndices.push_back(i);
        }
    }
    
    for (unsigned int i = 0; i < markedIndices.size(); ++i)
    {
        NDKHelper::RemoveAtIndex(markedIndices[i]);
    }
}

Value NDKHelper::GetCCObjectFromJson(json_t *obj)
{
    if (obj == NULL)
        return Value();
    
    if (json_is_object(obj))
    {
        //CCDictionary *dictionary = new CCDictionary();
        //CCDictionary::create();
        //cocos2d::Map<string, Value> dictionary = cocos2d::Map<string, Value>();
        ValueMap dictionary = ValueMap();
        
        const char *key;
        json_t *value;
        
        void *iter = json_object_iter(obj);
        while(iter)
        {
            key = json_object_iter_key(iter);
            value = json_object_iter_value(iter);
            
            std::pair<std::string,Value> bla (string(key),NDKHelper::GetCCObjectFromJson(value));
            
            dictionary.insert(bla);
            //dictionary->setObject(NDKHelper::GetCCObjectFromJson(value)->autorelease(), string(key));
            
            iter = json_object_iter_next(obj, iter);
        }
        
        return Value(dictionary);
    }
    else if (json_is_array(obj))
    {
        size_t sizeArray = json_array_size(obj);
        //CCArray *array = new CCArray();
        //CCArray::createWithCapacity(sizeArray);
        ValueVector array = ValueVector();
        
        for (unsigned int i = 0; i < sizeArray; i++)
        {
            array.insert(array.end(), NDKHelper::GetCCObjectFromJson(json_array_get(obj, i)));
            //array->addObject(NDKHelper::GetCCObjectFromJson(json_array_get(obj, i))->autorelease());
        }
        
        return Value(array);
    }
    else if (json_is_boolean(obj))
    {
        stringstream str;
        Value val;
        if (json_is_true(obj))
            val = Value(true);
        else if (json_is_false(obj))
            val = Value(false);
        
        //CCString *ccString = new CCString(str.str());
        //CCString::create(str.str());
        //return ccString;
        //std::string ccString(str.str());
        //Value val = Value(ccString);
        return val;
    }
    else if (json_is_integer(obj))
    {
        stringstream str;
        str << json_integer_value(obj);
        
        //CCString *ccString = new CCString(str.str());
        //CCString::create(str.str());
        std::string ccString(str.str());
        Value val = Value(ccString);
        return val;
    }
    else if (json_is_real(obj))
    {
        stringstream str;
        str << json_real_value(obj);
        
        std::string ccString(str.str());
        Value val = Value(ccString);
        
        return val;
    }
    else if (json_is_string(obj))
    {
        stringstream str;
        str << json_string_value(obj);
        
        string ccString(str.str());
        Value val = Value(ccString);
        //CCString *ccString = new CCString(str.str());
        //CCString::create(str.str());
        return val;
    }
    
    return Value();
}

json_t* NDKHelper::GetJsonFromCCObject(Value obj)
{
    if (obj.getType() == Value::Type::MAP)
    {
        
        std::vector<string> allKeys;
        allKeys.reserve(obj.asValueMap().size());
        for(auto kv : obj.asValueMap()) {
            allKeys.push_back(kv.first);
        }
        
        json_t* jsonDict = json_object();
        
        if(allKeys.empty()) return jsonDict;

        for (unsigned int i = 0; i < allKeys.size(); i++)
        {
            string key = allKeys[i];
            Value val = obj.asValueMap().at(key);
            json_object_set_new(jsonDict,
                                key.c_str(),
                                NDKHelper::GetJsonFromCCObject(val));
        }
        
        return jsonDict;
    }
    //else if (dynamic_cast<Vector<Ref>*>(obj))
    else if (obj.getType() == Value::Type::VECTOR)
    {
        //Vector<Ref*> mainArray = *(Vector<Ref*>*)obj;
        //CCArray* mainArray = (CCArray*)obj;
        json_t* jsonArray = json_array();
        
        for (unsigned int i = 0; i < obj.asValueVector().size(); i++)
        {
            Value val = obj.asValueVector().at(i);
            json_array_append_new(jsonArray,
                                  NDKHelper::GetJsonFromCCObject(val));
        }
        
        return jsonArray;
    }
    //else if (dynamic_cast<CCString*>(obj))
    if (obj.getType() == Value::Type::STRING)
    {
        //CCString* mainString = (CCString*)obj;
        string str = obj.asString();
        json_t* jsonString = json_string(str.c_str());
        
        return jsonString;
    }
    
    return NULL;
}


void NDKHelper::PrintSelectorList()
{
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i)
    {
        string s = NDKHelper::selectorList[i].getGroup();
        s.append(NDKHelper::selectorList[i].getName());
        CCLOG("%s",s.c_str());
        //CCLog(s.c_str());
    }
}

void NDKHelper::HandleMessage(json_t *methodName, json_t* methodParams)
{
    if (methodName == NULL)
        return;
    
    const char *methodNameStr = json_string_value(methodName);
    
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i)
    {
        if (NDKHelper::selectorList[i].getName().compare(methodNameStr) == 0)
        {
            Value dataToPass = NDKHelper::GetCCObjectFromJson(methodParams);
            
            //if (dataToPass.isNull())
                //CCLOG("wtf");
                //dataToPass->retain();
            
            //SEL_CallFuncN sel = NDKHelper::selectorList[i].getSelector();
            std::function<void(Ref*, void*)> sel = NDKHelper::selectorList[i].getSelector();
            Ref *target = NDKHelper::selectorList[i].getTarget();
            
            //CCFiniteTimeAction* action = CCSequence::create(__CCCallFuncND::create(target, sel, (void*)dataToPass), NULL);
            //FiniteTimeAction* action = Sequence::create(, NULL);
            
            sel(target, &dataToPass);
            
            //((Node*)target)->runAction(action);
            
            //if (dataToPass != NULL)
                //dataToPass->autorelease();
            break;
        }
    }
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "../cocos2dx/platform/android/jni/JniHelper.h"
    #include <android/log.h>
    #include <jni.h>
    #define  LOG_TAG    "EasyNDK-for-cocos2dx"

    #define CLASS_NAME "com/easyndk/classes/AndroidNDKHelper"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #import "IOSNDKHelper-C-Interface.h"
#endif

extern "C"
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Method for recieving NDK messages from Java, Android
    void Java_com_easyndk_classes_AndroidNDKHelper_CPPNativeCallHandler(JNIEnv* env, jobject thiz, jstring json)
    {
        string jsonString = JniHelper::jstring2string(json);
        const char *jsonCharArray = jsonString.c_str();
        
        json_error_t error;
        json_t *root;
        root = json_loads(jsonCharArray, 0, &error);
        
        if (!root)
        {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return;
        }
        
        json_t *jsonMethodName, *jsonMethodParams;
        jsonMethodName = json_object_get(root, __CALLED_METHOD__);
        jsonMethodParams = json_object_get(root, __CALLED_METHOD_PARAMS__);
        
        // Just to see on the log screen if messages are propogating properly
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, jsonCharArray);
        
        NDKHelper::HandleMessage(jsonMethodName, jsonMethodParams);
        json_decref(root);
    }
    #endif
    
    // Method for sending message from CPP to the targetted platform
    void SendMessageWithParams(string methodName, Value methodParams)
    {
        if (0 == strcmp(methodName.c_str(), ""))
            return;
        
        json_t *toBeSentJson = json_object();
        json_object_set_new(toBeSentJson, __CALLED_METHOD__, json_string(methodName.c_str()));
        
        if (!methodParams.isNull())
        {
            json_t* paramsJson = NDKHelper::GetJsonFromCCObject(methodParams);
            json_object_set_new(toBeSentJson, __CALLED_METHOD_PARAMS__, paramsJson);
        }
        
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo t;
        
		if (JniHelper::getStaticMethodInfo(t,
                                           CLASS_NAME,
                                           "RecieveCppMessage",
                                           "(Ljava/lang/String;)V"))
		{
            char* jsonStrLocal = json_dumps(toBeSentJson, JSON_COMPACT | JSON_ENSURE_ASCII);
            string jsonStr(jsonStrLocal);
            free(jsonStrLocal);
            
            jstring stringArg1 = t.env->NewStringUTF(jsonStr.c_str());
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1);
            t.env->DeleteLocalRef(stringArg1);
			t.env->DeleteLocalRef(t.classID);
		}
        #endif
        
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        json_t *jsonMessageName = json_string(methodName.c_str());
        
        if (!methodParams.isNull())
        {
            json_t *jsonParams = NDKHelper::GetJsonFromCCObject(methodParams);
            IOSNDKHelperImpl::RecieveCPPMessage(jsonMessageName, jsonParams);
            json_decref(jsonParams);
        }
        else
        {
            IOSNDKHelperImpl::RecieveCPPMessage(jsonMessageName, NULL);
        }
        
        json_decref(jsonMessageName);
        #endif
        
        json_decref(toBeSentJson);
    }
}
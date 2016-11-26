

#include <stdio.h>
#include "jet_javafx_study_NativeTest.h"

JNIEXPORT jstring JNICALL Java_jet_javafx_study_NativeTest_getStringFromC
(JNIEnv * env, jobject clazz)
{
	jclass obj_class = env->FindClass("jet/javafx/study/NativeTest");
	if (obj_class == nullptr)
	{
		printf("obj_class is NULL");
		return env->NewStringUTF("obj_class is NULL");
	}

//	env->GetStaticMethodID()
	jmethodID method_id = env->GetMethodID(obj_class, "getStringFromJava", "()Ljava/lang/String;");
	if (method_id == nullptr)
	{
		printf("method_id is NULL");
		return env->NewStringUTF("method_id is NULL");
	}

	jstring java_str = (jstring)(env->CallObjectMethod(clazz, method_id));
	
//	env->CallStaticVoidMethod
//	env->GetStringUTFChars(java_str, 0);

	return java_str;
}
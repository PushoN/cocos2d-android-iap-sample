## Overview

This is sample of cocos2d-x In-App-Purchase for android.

* support Billing Library Version 4
* cocos2d-x v3

The interface is similar to sdkbox::IAP with a few differences.
It does not have all the features of sdkbox::IAP.
However, you can retrieve a list of products, send purchase requests, and receive the results.

You are free to modify all sources in Java and C++.
Add or modify the functionality you need.

## Usage

### step1.

Place the cocos2d folder directly under your project.

### step2. 

Overwrite Resources/sdkbox_config.json with the files used in your project.

### step3.

Put the correct information in the following line in proj.android/gradle.properties.
This is required for the purchase test to work properly.

```
RELEASE_STORE_FILE=
RELEASE_STORE_PASSWORD=
RELEASE_KEY_PASSWORD=
RELEASE_KEY_ALIAS=
```

### step4.

Correct the package name in the following file to the package name of your project.
In the sample, it says "your package name", but of course it will not work as it is.

* proj.android/app/AndroidManifest.xml
* proj.android/app/build.gradle

### step5.

Start Android Studio and open the proj.android directory.

## Author

mingos

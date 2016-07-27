# Dream

This is the DreamOS Engine Git repo. 

## Setting Up Your Environemnt 

Currently the DreamOS engine is built through the use of the VS2015 project which can be found in `DreamOS\Project\Windows\DreamOS`

### Windows

**DREAMOSPATH**

This is the path of the DreamOS folder in the DreamGarage repo. Since there may be multiple project / build systems in the future depending on the platform, this environment variable allows the engine to understand where any of it's given assets are located based on the dreampaths.txt which should be located at teh root DreamOS folder. 

**CEFPATH**

Currently DreamOS is using CEF for all cloud connectivity, or WebRTC/WebSockets for peer to peer.  CEF can get quite large, especially the debug binaries.  At the moment CEF is included through the use the C++ wrapper for the binaries, and since these are so large they are not kept in the repo.  This can also allow for rapid updating of the most recent version of CEF, the current supported build is reflected in the external\CEF\CEF_BINARARY_X.XXXX.XXXX folder.

**WEBRTCSRCPATH**

This path should point to the src/ path of the WebRTC source/build as described in the below WebRTC section


## Network

## Cloud Controller

DreamOS utilizes factory to construct a Cloud Controller that may use various Cloud Implementations to connect to the network.  Currently CEF is implemented, and if the CEFPATH above is not set this will result in issues.  The current version supported is 3.2556, as can be found in the External path of DOS

### Making Requests Using Cloud Controller

Making a request is as simple as the following to create a request for www.google.com


```c++
CloudController->CreateNewURLRequest(std::wstring(L"http://www.google.com"));
```

The request handling is TBD at the moment

### CEF

Using CEF should only be done through the Cloud Controller


## Using Dream OS 

DreamGarage is a child object of the DreamOS object which can be instantiated to set up the platform sandbox and HAL subsystems.  DreamOS can then be used to manipulate the internal state of the engine, such as creating / adding new objects and manipulating them.  

On initialization DreamOS will call `virtual RESULT LoadScene()` which is an opportunity for the DreamGarage object to set up the objects that it wants in the scene graph.  Handles to these objects can then be manipulated in the `virtual RESULT Update()` function which is called once per update loop in the engine.

The following functions can be used to create / Add objects:

```c++
light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);
sphere *AddSphere(float radius, int numAngularDivisions, int numVerticalDivisions);
volume *AddVolume(double side);
texture* MakeTexture(wchar_t *pszFilename, texture::TEXTURE_TYPE type);
skybox *AddSkybox();
model *AddModel(wchar_t *pszModelName);
```

Currently no notifications / events are set up but this will be added to help notify the applicatio of things like keyboard entry, mouse movement or other pertinent application layer events.


# Using Dream OS

## Dream Garage

### Lights

Dream supports lights by the way of the `light *AddLight(LIGHT_TYPE type, light_precision intensity, point ptOrigin, color colorDiffuse, color colorSpecular, vector vectorDirection);` function.  Retaining the pointer to the light allows the application layer to update the light position or other parameters during the update cycle or otherwise.

#### Shadows

To enable shadows on a given light, utilize the `RESULT EnableShadows()` function of the light object, which turns a given light into a shadow emitter.  Currently the engine is only capable of supporting one directional light emitter, although point lights casting shadows are future feature. 

## WebRTC Setup and Installation

https://webrtc.org/native-code/development/prerequisite-sw/

You will need to install Update 2 for VS2015 https://www.visualstudio.com/en-us/news/vs2015-update2-vs.aspx

### Installing Chromium Depot Tools

http://dev.chromium.org/developers/how-tos/install-depot-tools

Install as described and then run gclient for the first time through CMD.  This will install a bunch of other stuff too, but will only work if the PATH is set correctly.  

###  Build Chromium

https://chromium.googlesource.com/chromium/src/+/master/docs/windows_build_instructions.md

Keep note that to run gclient sync you will need to run the command prompt as administrator in windows to set up the Ninja build files correctly

In some cases it might be needed to run the build ninja with x64 indicated as the architectuer per the following `python webrtc\build\gyp_webrtc -Dtarget_arch=x64`

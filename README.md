# Dream

This is the DreamOS Engine Git repo. 

## Setting Up Your Environemnt 

Currently the DreamOS engine is built through the use of the VS2015 project which can be found in `DreamOS\Project\Windows\DreamOS`

### Windows

**DREAMOSPATH**

This is the path of the DreamOS folder in the DreamGarage repo. Since there may be multiple project / build systems in the future depending on the platform, this environment variable allows the engine to understand where any of it's given assets are located based on the dreampaths.txt which should be located at teh root DreamOS folder. 

**CEFPATH**

Currently DreamOS is using CEF for all cloud connectivity, or WebRTC/WebSockets for peer to peer.  CEF can get quite large, especially the debug binaries.  At the moment CEF is included through the use the C++ wrapper for the binaries, and since these are so large they are not kept in the repo.  This can also allow for rapid updating of the most recent version of CEF, the current supported build is reflected in the external\CEF\CEF_BINARARY_X.XXXX.XXXX folder.

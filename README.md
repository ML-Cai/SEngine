# SEngine

Sayers Engine is a simple visualization engin based on openGL, MikuMikuDance and MikuMiku Editor.

============================

This engine include ：
  1. The model visualization based on MikuMikuDance software, include : PMD, PMX
  2. The model skeleton and motion controller based on MikuMikuDance software, include : PMD, VMD
  3. The parallel universe and world line system to manage the location, audio, motion of model.

============================

**The SEngine containt**

**1. libsut** : The basis function of engine, such as mathematical, data structure, image ...
  
**2. libMMDCppDev** : The profiler of MikuMikuDance file, include : PMD, PMX, VMD, VPD
  
**3. libVisualizationAvatar** : Avatar is the nickname of 3D model in SEngine.
  
**4. libParallelUniverse** : The simple controller of motion, audio, avatar, both of them is handled in one world line in parallel universe, and the world line could specify the elapsed time to change the time stamp and presnet the different motion or audio.
  
============================

**The Demo**

**1. AvatarVisualization** : This demo shows the instructions of libVisualizationAvatar to presnet the Miku model. This demo is created by the Qt 5.5.0.

![ScreenShot](https://github.com/VegetableAvenger/SEngine/blob/master/image/Demo_VisualizationAvatar.png)

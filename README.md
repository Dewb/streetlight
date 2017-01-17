streetlight
===========

Streetlight sends live video from a Syphon- or Spout-enabled application to an arbitrary, heterogenous collection of LED light fixtures. Mappings from regions of video to fixtures can be defined and adjusted interactively, or loaded from disk.

This is pre-alpha software.

Streetlight has been tested and is expected to work with:
* Philips Color Kinectics ColorBlast, ColorBurst, and similar RGB fixtures on a PDS-150e or similar
* Philips iColorTile MX 12x12 or iColorModule FX 6x6 (but not simultaneously) on a PDS-60 or PDS-480
* Philips iColor Flex or similar light strand/chain/strip on a PDS-60 or similar
* Video input on Mac OS X with Syphon
* Video input on Windows with Spout

Future enhancements:
* Improved UI with JUCE
* Support for mixing iColorTile MX 12x12 and iColorModule FX 6x6 
* Support for [Fadecandy](https://github.com/scanlime/fadecandy) and [LEDscape](https://github.com/Yona-Appletree/LEDscape) devices via [OpenPixelControl](http://openpixelcontrol.org/)
* Support for [PixelPusher](http://www.heroicrobotics.com/) devices
* Support for DMX devices

Licensing:
* Streetlight is licensed under [GPL v3](http://www.gnu.org/licenses/gpl-3.0.html)
* Kinet protocol support is based on [kinet.py](https://github.com/vishnubob/kinet), copyright (c) 2011 by Giles Hall and subject to [these terms](https://github.com/vishnubob/kinet/blob/master/LICENSE).

Installation instructions:
--------------------------

1. Download [openFrameworks v0.8.4](http://openframeworks.cc/download/)

2. Clone the following oF extensions into the addons folder:
  * ofxSyphon (head revision, OSX only)
  * ofxTangibleUI (head revision)
  * ofxUI (fork)
  ```
  cd of_v0.8.4_osx_release/addons
  git clone https://github.com/astellato/ofxSyphon
  git clone https://github.com/fx-lange/ofxTangibleUI
  git clone https://github.com/Dewb/ofxUI
  ```

3. Download or pull streetlight into the same parent folder as openFrameworks. (Do not put it in the apps folder.)
4. Open the project file (streetlight.xcodeproj for OSX/Xcode, streetlight.sln for Visual Studio 2012) and build.

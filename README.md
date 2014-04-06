streetlight
===========

Streetlight sends live video from a Syphon-enabled application to an arbitrary, heterogenous collection of LED light fixtures. Mappings from regions of video to fixtures can be defined and adjusted interactively, or loaded from disk.

This is pre-alpha software.

Currently supports:
* Philips Color Kinectics ColorBlast and similar RGB fixtures on a PDS-150e or similar
* Philips iColorTile MX 12x12 or iColorModule FX 6x6 (but not simultaneously) on a PDS-60 or PDS-480
* Mac OS X video sources w/ Syphon

Coming soonish:
* Support for mixing iColorTile MX 12x12 and iColorModule FX 6x6 
* Support for [Fadecandy](https://github.com/scanlime/fadecandy) devices via [OpenPixelControl](http://openpixelcontrol.org/)

Coming later:
* Support for [PixelPusher](http://www.heroicrobotics.com/) devices
* Windows support via FFGL or other frame-sharing technology?

(Formerly known as CKVideoDriver in http://github.com/Dewb/alphadep)

Licensing:
* Streetlight is licensed under [GPL v3](http://www.gnu.org/licenses/gpl-3.0.html)
* Kinet protocol support is based on [kinet.py](https://github.com/vishnubob/kinet), copyright (c) 2011 by Giles Hall and subject to [these terms](https://github.com/vishnubob/kinet/blob/master/LICENSE).

Required oF addons:
* ofxSyphon -- https://github.com/astellato/ofxSyphon (head)
* ofxTangibleUI -- https://github.com/fx-lange/ofxTangibleUI (head)
* ofxUI -- https://github.com/rezaali/ofxUI (51b2dcb5d6bef17e18c23adf91ae94fce095f42d)

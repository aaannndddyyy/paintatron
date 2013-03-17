paintatron
==========

<img src="http://sluggish.dyndns.org/images/1/1f/Gargoyle.jpg"/>

Paintatron can be used to generate collage artworks based upon a set of source images. The user repeatedly selects the most aesthetic images from which the next generation of images are produced.

This is a GUI application which uses Qt version 4.8 or later, so you won't be able to use it on the command line.

Usage
-----

First load a few sample images using the option on the File menu.  These will be used to generate the artworks.  Once you've done that click on one of the small preview images to select the one you like best.  A larger version of it will appear on the right hand side.  Repeat the process as many times as you wish.

If you find an image which you want to save then this can be done from the File menu.

If you want to use a different set of source images then clear the existing images from the Edit menu, then load the new source images from the File menu.  Note that clearing the source images does not delete the original images themselves.

Packaging
---------

In order to create a package for a Debian based system first ensure that the code is within a directory called paintertron-x.xx where the x's are the version number, and that you have the libgpr0 and libgpr0-dev packages installed (see https://launchpad.net/libgpr), then run the debian.sh script.  You will need to make sure that Qt packages are installed and the easiest way to do this is just to install Qt Creator.

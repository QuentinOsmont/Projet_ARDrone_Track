webflight-trollface
===================

This is a plugin for the browser-based AR.Drone ground control station
[webflight](http://eschnou.github.io/ardrone-webflight/) that overlays
trollfaces on detected faces using opencv.

Extracted from a fork of `ardrone-webflight` created at Nodecopter Berlin: https://github.com/bluemaex/ardrone-webflight

![Preview Image](http://distilleryimage2.ak.instagram.com/be281f881c9311e3ac4722000a9f393d_7.jpg)

## Running the software

You will need the
[ardrone-webflight](https://github.com/eschnou/ardrone-webflight) and
webflight-trollface repos:

```
git clone git://github.com/eschnou/ardrone-webflight.git
git clone git://github.com/andrew/webflight-trollface.git
```

Run `npm install` for each:

```
(cd ardrone-webflight && npm install)
(cd webflight-trollface && npm install)
```

Link `webflight-trollface` into webflight's `plugins` directory:

```
(cd ardrone-webflight/plugins && ln -s ../../webflight-trollface trollface)
```

Copy ardrone-webflight's `config.js.example` to `config.js`:

```
(cd ardrone-webflight && cp config.js.example config.js)
```

Add `"trollface"` to the `plugins` array in `config.js`
and remove or comment out `"video-png"` and `"video-stream"`,
so it looks something like this:

```
var config = {
    plugins: [
      "hud"         // Display the artificial horizon, altimeter, compass, etc.
      , "pilot"       // Pilot the drone with the keyboard
      , "trollface"   // Detect faces and add trolls to them
    ]
};

module.exports = config;
```


### Start the server

Now you can start the webflight server:

```
(cd ardrone-webflight && node app.js)
```

Plugin your gamepad, point your browser at http://localhost:3000/ and
then press a button on your gamepad to activate the plugin.


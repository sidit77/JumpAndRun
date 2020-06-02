# Jump and Run Game
## TODO list
### Gameplay
* ~~horizontal velocity~~
* ~~ability to jump for ~0.25s after leaving platform~~
* ~~ability to control jump height~~
* ~~horizontal sleep~~
* ~~use callbacks for jump~~
* ~~save jump input when not on ground~~
* ~~fix getting stuck on corners~~
* ~~fix physics bug~~
* short lockout after walljump?
* keep an internal, unclamped version of `vel`?

### Engine
#### General
* use `glBufferStorage`
* add uniform location caching
* remove color support from `CreatureRenderer`?
* ~~Move Config into Game~~
* ~~Figure out a save format for config files~~
* Proper screen resize events and maybe windowless fullscreen

#### Resource Manager
* central resource list that gets loaded during startup
* resources can be stored in internal archive or external asset folder
* if a resource is both internal and external the external version is used
* the resource list offers the option to exclude resources from the internal archive
* debug version only uses external?
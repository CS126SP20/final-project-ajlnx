# FallingBalls

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)\
**Author**: Andy Lennox - [`alennox3@illinois.edu`](mailto:alennox3@illinois.edu)

![](https://imgur.com/alTK9vJ)\
![](https://imgur.com/ViCT5wR)

## Dependencies
 - [gcc] or [clang]
 - [cmake]
 - [Box2d]
 - [ImGui]
 - [SQLite]
 - [cinder]
 
 #### Building
Install ```cinder``` and add a directory for projects
```$xslt
$ git clone --recursive https://github.com/cinder/Cinder.git
$ cd Cinder
$ mkdir my-projects
```

Clone Falling Balls
```$xslt
$ cd my-projects
$ git clone https://github.com/CS126SP20/final-project-ajlnx.git
```
Open the project in C++ IDE and build

## Controls

### Game

#### Keyboard

| Key       | Action                                                      |
|---------- |-------------------------------------------------------------|
| `d`       | Move to the right                                           |
| `RIGHT`   | Move to the right                                           |
| `a`       | Move to the left                                            |                                                       |
| `LEFT`    | Move to the left                                           |
| `CMD+q`  | Quit the game                                               |

#### Mouse
In 2 player games, `LEFT CLICK` will spawn a ball at the cursor.


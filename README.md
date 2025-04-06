# [Amazing Aliens] - midterm project

**robert cameron**
**csci 178**
**4/5/2025**

## description

simple 2d game for the csci 178 midterm. includes landing page, menu, help, game, and pause states as required.

## features

* landing page (enter/click -> menu)
* menu page (new game, help, exit)
* menu interaction (keys 'n', 'h', 'e', 'esc' or mouse click)
* help page ('esc' -> menu)
* game screen (player, enemies, bullets, parallax background)
* in-game pause ('esc' -> pause menu)
* pause menu (confirm quit: 'enter' -> yes, 'esc' -> no/resume)
* bitmap font rendering
* texture loading

## how to compile and run (code::blocks)

1.  open the code::blocks project file (`.cbp`).
2.  ensure opengl and glut (or equivalent like freeglut) libraries are linked in the build options (linker settings).
3.  build the project (f9 or build -> build).
4.  run the executable from the `bin/debug` or `bin/release` folder.
5.  make sure the `images/` folder (with textures and `.fnt` file) is in the same directory as the executable.
6.  you may have to put 'GLUT_DISABLE_ATEXIT_HACK' in Project -> Buld Options -> MidTermBaseCode -> compiler settings -> #defines

## controls

* **landing:** `enter` / `click` -> menu
* **menu:** `n` / `click new game` -> game | `h` / `click help` -> help | `e` -> exit | `esc` -> landing
* **help:** `esc` -> menu
* **game:** `left`/`right arrows` -> move | `spacebar` -> shoot | `esc` -> pause
* **pause menu:** `enter` -> quit | `esc` -> resume


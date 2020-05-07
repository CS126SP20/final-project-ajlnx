# Development

---
 - **4/20/20** Setup reactphysics3d and Cinder-Kinect
   - Need to link block to project
   - created basic example with reactphysics3d to demonstrate functionality

 - **4/21/20** Linked Cinder-Kinect cinder block to project.
 
 - **4/25/20** removed reactphysics3d from project. Replaced with Box2D
 
 - **4/26/20** Switched project to falling balls game due to errors with Cinder-Kinect block.
   - Setup basic world with Box2D and learned how to use library
   
 - **4/27/20** Created simple physics sandbox using Box2D
  - added functionalities:
    - Clicking with mouse spawns circle body
    - dragging mouse spawns circle bodies
    - rendered sandbox with opengl
    
 - **4/29/20** Added player body to physics world and added game states
 
 - **4/30/20** added collision detection such that balls hitting the player end the game
 
 - **5/2/20** added keyEvent logic so that the player can move with `a`, `d`, `LEFT`, `RIGHT` keys
 
 - **5/3/20** add sqlite database, scoring, and leaderboard functionality
   - player's score is denoted as the number of seconds before a ball hits them
   - implement same scoreboard from snake project
   
 - **5/4/20** add menu, pause / resume logic, 2 player mode, view scores option in menu
    - two player mode allows mouse to be used to spawn balls to try and kill the other player
    - view scores option displays high scores before game starts
  
 - **5/6/20** added tests for engine and leaderboard, formatted code, added documentation, added teleport option
    - enable / disable teleport mode in menu allows user to change game behavior. option is available in menu when game is paused.
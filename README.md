My master thesis, written in 2015, quite rough due to concentration on robotics related part: invese kinematics and path planning.
Aim was to create application that allows to plan maniplator movement in 3D space full of obstacles.

Quicks summary after four years:

Inverse kinematics was based on 'transposed jacobian' method which allows to handle efectively manipulators with redundant joints.

Path plannig is simple A* implementation in 3D space, to detect and avoid obstacles I used Bullet physics engine. Bullet is feed from 3D scene.

Application has bindings to python and can run python scripts.

Visualisation is done with OpenGL 3.3 on my own engine.

[![](http://img.youtube.com/vi/2yAug-XaY9g/0.jpg)](http://www.youtube.com/watch?v=2yAug-XaY9g "")

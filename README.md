# image_morphing
 2d image morphing program using Delaunay triangulation

(The following program was designed on Windows)

HOW TO RUN:

SETUP REQUIRED: 
It is required for the user to know how to set up an OpenGL project to proceed with running the code.
For those unfamiliar it is recommended to follow the Learn OpenGL online resource guide: 
https://learnopengl.com/Getting-started/Creating-a-window  

To explain briefly, the tutorial involves setting up library extensions, compiling and linking to a project.
All additional libraries needed to run the project can be found in the libraries folder but links to each library will be necessary. 

Library websites links:

Libraries included: 

Glad: https://glad.dav1d.de/

GLFW: https://www.glfw.org/download

stb: https://github.com/nothings/stb 

glm: https://github.com/g-truc/glm


All Linker Input additional dependencies:
glfw3.lib
opengl32.lib
user32.lib
gdi32.lib
shell32.lib
legacy_stdio_definitions.lib
ucrtd.lib



RUNNING THE PROGRAM:

Once all the dependencies and include files are linked, then running the main 'main.cpp' should display a screen to the user. If no textures have loaded its recommended to check the file path on lines 153 and 179. The textures for the program have been provided in the repository. 

Left image represents source and right is the destination image.

INPUTS:
Left mouse click: represents feature point selection for the source image. Left click on the left image to select its control points. MAXIMUM of 25 control points.
 
Right mouse click: represents feature point selection for the destination image. Right click on the right image to select its control points. MAXIMUM of 25 control points.

Select the same amount of control points for each image and keep control point selection representing features consistent (if 4 control points are selected on the source image for both eyes and both ends of the mouth then do the same for the destination image). The program works without these consistencies but would defeat the point of the program.  

Once both control points have been selected:

Pressing 'S': Should perform the triangulation  of your inputted control points and display the triangulation on screen for the source image 

Pressing 'D': Should perform the triangulation of your inputted control points and display the triangulation on screen for the destination image

End of functionalities. 

Notes about errors: Little to no validation is controlled for the program as of yet, due to time constraints focus went into the main functionalities and many errors exist if a given user would try and test the strength of the program. Following the framework above WILL produce expected results but any divergence will most likely cause errors.  



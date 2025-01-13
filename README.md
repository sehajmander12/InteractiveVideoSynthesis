# Interactive Video Synthesis using Vibrational Modal Analysis
## Navtegh Singh Gill, Stuti Wadhwa and Sehajpreet Kaur

### Introduction
This is the README for our CSC2529: Computational Imaging project at University 
of Toronto in Fall 2024. The topic is an extension to Abe Davis' previous paper 
in SIGGRAPH Asia 2015 on the idea of interactive dynamic video through image 
space modal analysis.

The project in '/Lukas Kanade + Simulation' is built in Qt Creator and would need a Qt
Creator to build and run.  We used Qt version of 5.8 and its associated Qt Creator,
with the Qt Charts and Qt Data Visualization components installed (see 
http://doc.qt.io/qtinstallerframework/ifw-use-cases-add.html for how to choose
Qt components to install). We have attached the required OpenCV and Eigen modules
and compiled libraries and they should be linked locally ( ModalAnalysis.pro
for the Makefile).


### Analysis Source Code
The source code can be roughly divided into analysis and simulation. The analysis
pipeline consists of:

1. VideoSelector.(h|cpp): The first UI that shows up is a video converter. You
select a video and a directory DIR to put the frames and future data in, and
give the video a NAME that'll be used later down the pipeline, and the converter
would convert the video into frames and store in DIR/frames as NAMExxx.png files.
If you've done all the analysis step beforehand, there is also a shortcut to
directly go to simulation, but you'll have to provide the NAME used for that data.

2a. FeatureSelector.(h|cpp), featuredetector.(h|cpp): The second is the feature
detectorand selector. You can use automatic feature detection or manually add
feature points onto the first / reference frame. If the feature point toggle is
not on, the added points would be stored as fixed points, which would not be
tracked but used to fixate background during simulation. Saving the data produces
a file DIR/NAME-fp.txt that stores the points and the triangulation. For simplicity
I included a button to load previously constructed feature point sets.
2b. DelaunyTriangulation.(h|cpp): An API for storing Delauny Triangulation info
of the reference frame based on OpenCV's Subdiv2D.
2c. DisplayImageWidget.(h|cpp): A Qt class that displays an OpenCV image.
2d. fileutil.(h|cpp), utility.(h|cpp): The utility files that governs reading
from and writing to the various data formats in the project.

3a. FeatureTracker.(h|cpp): The next is the feature tracker. Select a NAME-fp.txt
file and the directory that stores the corresponding frames, and first click
the Track button, which would track the feature points through the frames, store
the displacements in DIR/NAME-flowvec.txt, perform DFT on the time series, and
store the resulting FFT and frequencies in DIR/NAME-fft.txt. 
3b. ModalShape(Drawer|Producer).(h|cpp), OpenGL(Window|Warper).(h.cpp): Drawing
warped frames draws the feature points being tracked onto each frame in
DIR/frames/warped, and drawing modal shapes draws the x and y modal shapes with
the described visualization in DIR/xShape and DIR/yShape for each frequency.
The warped frames are unnecessary to continue in the pipeline, but the shapes
are required and useful. Drawing is the lengthiest part of the pipeline,
averaging 3 frames per second, or 180 frames per minute.
3c. ComplexUtility.(h|cpp): A utility class that transforms complex value to
color through HSV color space.

4. FrequencySelector.(h|cpp): The last one is the frequency selector. In this UI
you can see the modes associated with each frequency and the total magnitudes of
the mode. Use the cursor to select desired modes and save the selected modes in
DIR/NAME-selected-fft.txt for each method. The Simulate button opens a window 
that uses the current selected FFT file.

After the entire analysis, the directory structure should look like:
DIR
 |--- frames
 |       |--- goodflower1.png - goodflower150.png
 |       |--- warped
 |                |--- goodflower1.png - goodflower336.png
 |-- xShape
 |       |--- goodflower-x-0.714286.png ...
 |-- yShape
 |       |--- goodflower-y-0.714286.png ...
 |-- goodflower-fp.txt // feature position file
 |-- goodflower-fft.txt // all frequency/mode file
 |-- goodflowerlk-fft.txt // all frequency/mode file for Lukas-Kanade method
 |-- goodflowerlkberthy-fft.txt // all frequency/mode file for our proposed method
 |-- goodflowerberthy-fft.txt // all frequency/mode file for CSP method
 |-- goodflowerraft-fft.txt // all frequency/mode file for Raft method
 |-- goodflower-selected-fft.txt // selected mode file
 |-- goodflowerlk-selected-fft.txt // selected mode file for Lukas-Kanade method
 |-- goodflowerlkberthy-selected-fft.txt // selected mode file for our proposed method
 |-- goodflowerberthy-selected-fft.txt // selected mode file for CSP method
 |-- goodflowerraft-selected-fft.txt // selected mode file for Raft method
 |-- goodflower-flowvec.txt // the displacements of feature points through time.
 
The above was to build basic files for simulation and Lukas Kanade methods. For the simulation 
with other methods as well, goodflower-fft.txt can be generated using jupyter notebook files 
provided in the folders of the respective methods ['CSP','lk+csp','RAFT/core/']. Once generated, the 
files should be renamed as 'goodflower'method-name'-fft.txt' where method names are 'lk', 'berthy', 
'lkberthy' and 'raft' respectively. 

And that is how ModalAnalysis/tmp/ is set up to be.


#### Simulation Source Code
The simulation pipeline consists of:

1. featurepoint.(h|cpp): Represents a feature point at simulation time. Contains
information about the point's original texture coordinate, its modes, and its
positions. Calls to fixPosition and updatePosition will calculate deformation
from the original position using the given time and modal coordinates.

2. triangle.(h|cpp): Represents the triangle in the reference frame. Can determine
whether the mouse position falls into the triangle or not.

3. triangulationdata.h: Wrapper struct for all things needed for the triangulation.

4. simulationdata.h: Wrapper struct for all structure data for the simulation.

5. simulationparams.(h|cpp): Parameters for the simulation, including movement and
force scales, and other housekeeping parameters for loading images and creating
the texture/simulation. The simulation is very parameter sensitive, so I included
controls in the simulation window to adjust them at runtime.

6. readutility.(h|cpp): Loads data from the files from the analysis part to create
simulation params and data.

7. InteractiveWindow.(h|cpp): The simulation window that allows interaction with
the reference frame! The title shows the frequency being simulated or that it is
simulating with composite modes. Controls include:
    - Mouse: interact with the object by pressing and dragging
    - w: shows the wireframe view
    - s: shows the modal shapes being simulated
    - x/y: changes between viewing the x or y modal shapes
    - Up/Down: changes the mode to be simulating on
    - c: Changes into composite mode, sums up deformations from all selected modes
    - F/G: increase/decrease the force by the mouse
    - D/T: increase/decrease damping effects
    - Delete: enable/disable damping
    - A/Z: accelerate/decelerate the simulation speed
    - M/N: increase/decrease the movements
    - E/R: excite or decrease all modes' coordinates
Parameters need to be tuned for different input data, so a lot of times the window
might just be messed up and can't see anything. We usually wait until the simulation
damps down, drag it a little and then use N/G to tone down the parameters.


#### Data
The original videos are stored in the ModalAnalysis'tmp directory. The tmp directory 
is the DIR that we have been using in developing the project, so to make the process 
easier select it in the first video selection to avoid directory or file not found errors.

We have two examples within the data directory 'Lukas Kanade + Simulation/tmp':
    - man: the wireman originally used by Abe in his 2015 paper
    - goodflower: A video of a flower moving in the wind


The video files are needed as starting points of the analysis pipeline. The modal shapes are
generated data from the ModalShapeDrawer component, and is not entirely necessary
unless you want to directly run FrequencySelector and simulation without generating
data through the analysis pipeline first. Otherwise the generated data files
NAME-(fp|flowvec|fft|selected-fft).txt files are kept in tmp/ so that you can
simply load those files without rerunning data generation.


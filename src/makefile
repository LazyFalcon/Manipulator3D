CXXFLAGS			=  -I c:\MinGW\include  -std=c++1y   -msse2 -mfpmath=sse
CXX						= C:\MinGw\bin\g++.exe

OBJS					= ../bin/Utils.o  ../bin/FontRenderer.o  ../bin/IMGUI.o ../bin/Camera.o ../bin/CFGparser.o ../bin/ResourceLoader.o ../bin/Engine.o ../bin/JacobianTransposed.o ../bin/Robot.o ../bin/Mech.o ../bin/IInterpolator.o ../bin/RobotController.o ../bin/Widgets.o   ../bin/SomeTests.o  ../bin/BigSplineTest.o ../bin/Editor.o  ../bin/Logger.o ../bin/Graph.o \
 ../bin/Robot-Commands.o \
 ../bin/Robot-CommandBuilders.o \
 ../bin/Menu-SideBar.o \
../bin/MAIN.o

LIBS					= -lBulletSoftBody -lBulletDynamics -lBulletCollision -lLinearMath  -lglew32  -lglfw3 -lgdi32 -lFreeImage -lglu32  -lopengl32

BUILDDIR			= bin
SRC						= src
UTIL					= C:/MinGW/include/Utils

../bin/Manipulatork3D: $(OBJS)
	$(CXX) $(OBJS) -D _GLIBCXX_USE_CXX11_ABI=0 -o ../bin/Manipulatork3D $(LIBS)

../bin/MAIN.o: MAIN.cpp $(UTIL)/Utils.h $(UTIL)/Camera.h $(UTIL)/IMGUI_V4.h JacobianTransposed.h CFGparser.h Engine.h Menu.h Robot.h  PathCreator.h SomeTests.h BigSplineTest.h Menu-SideBar.h Robot-Commands.h Robot-CommandBuilders.h Widgets.h
	$(CXX) -c $(CXXFLAGS) MAIN.cpp -o ../bin/MAIN.o

../bin/Graph.o: Graph.cpp Graph.h Widgets.h $(UTIL)/IMGUI_V4.h
	$(CXX) -c $(CXXFLAGS) -o2  Graph.cpp -o ../bin/Graph.o

../bin/Editor.o: Editor.cpp Editor.h Widgets.h $(UTIL)/IMGUI_V4.h
	$(CXX) -c $(CXXFLAGS) -o2  Editor.cpp -o ../bin/Editor.o

../bin/BigSplineTest.o: BigSplineTest.cpp BigSplineTest.h Widgets.h $(UTIL)/IMGUI_V4.h
	$(CXX) -c $(CXXFLAGS) -o2  BigSplineTest.cpp -o ../bin/BigSplineTest.o

../bin/SomeTests.o: SomeTests.cpp SomeTests.h Widgets.h $(UTIL)/IMGUI_V4.h
	$(CXX) -c $(CXXFLAGS) -o2  SomeTests.cpp -o ../bin/SomeTests.o

../bin/Widgets.o: Widgets.cpp Widgets.h $(UTIL)/IMGUI_V4.h
	$(CXX) -c $(CXXFLAGS) -o2  Widgets.cpp -o ../bin/Widgets.o

#-----------------------------------------------------

../bin/Robot-Commands.o: $(UTIL)/IMGUI_V4.h Robot-Commands.cpp Robot-Commands.h
	$(CXX) -c $(CXXFLAGS) -o2  Robot-Commands.cpp -o ../bin/Robot-Commands.o

../bin/Robot-CommandBuilders.o: $(UTIL)/IMGUI_V4.h Robot-CommandBuilders.cpp Robot-CommandBuilders.h
	$(CXX) -c $(CXXFLAGS) -o2  Robot-CommandBuilders.cpp -o ../bin/Robot-CommandBuilders.o

../bin/Menu-SideBar.o: $(UTIL)/IMGUI_V4.h Menu-SideBar.cpp Menu-SideBar.h Menu-Tabs.cpp
	$(CXX) -c $(CXXFLAGS) -o2  Menu-SideBar.cpp -o ../bin/Menu-SideBar.o

#-----------------------------------------------------

../bin/JacobianTransposed.o: JacobianTransposed.cpp JacobianTransposed.h Robot.h
	$(CXX) -c $(CXXFLAGS) -o2  JacobianTransposed.cpp -o ../bin/JacobianTransposed.o

../bin/RobotController.o: RobotController.cpp RobotController.h JacobianTransposed.h Robot.h IInterpolator.h
	$(CXX) -c $(CXXFLAGS) -o2  RobotController.cpp -o ../bin/RobotController.o

../bin/IInterpolator.o: IInterpolator.cpp IInterpolator.h Robot.h
	$(CXX) -c $(CXXFLAGS) -o2  IInterpolator.cpp -o ../bin/IInterpolator.o

#-----------------------------------------------------

../bin/Mech.o: Mech.cpp Mech.h Robot.h
	$(CXX) -c $(CXXFLAGS) -o2  Mech.cpp -o ../bin/Mech.o

../bin/Robot.o: Robot.cpp Robot.h
	$(CXX) -c $(CXXFLAGS) -o2  Robot.cpp -o ../bin/Robot.o

../bin/CFGparser.o: CFGparser.cpp CFGparser.h $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) CFGparser.cpp -o ../bin/CFGparser.o

../bin/Engine.o: Engine.cpp Engine.h $(UTIL)/glUtils.h  glUtils.cpp Engine-DrawUtils.h Engine-Externals.cpp $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) Engine.cpp -o ../bin/Engine.o

../bin/ResourceLoader.o: ResourceLoader.cpp ResourceLoader.h $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) ResourceLoader.cpp -o ../bin/ResourceLoader.o

../bin/Camera.o: $(UTIL)/Camera.cpp $(UTIL)/Camera.h $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) -o2 $(UTIL)/Camera.cpp -o ../bin/Camera.o

../bin/IMGUI.o: $(UTIL)/IMGUI_V4.cpp $(UTIL)/IMGUI_V4.h $(UTIL)/Utils.h $(UTIL)/FontRenderer.h
	$(CXX) -c $(CXXFLAGS) -o2 $(UTIL)/IMGUI_V4.cpp -o ../bin/IMGUI.o

../bin/FontRenderer.o: $(UTIL)/FontRenderer.cpp $(UTIL)/FontRenderer.h $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) -o2 $(UTIL)/FontRenderer.cpp -o ../bin/FontRenderer.o

../bin/Logger.o: $(UTIL)/Logger.cpp $(UTIL)/Logger.h
	$(CXX) -c $(CXXFLAGS) -o2 $(UTIL)/Logger.cpp -o ../bin/Logger.o

../bin/Utils.o: $(UTIL)/Utils.cpp $(UTIL)/Utils.h
	$(CXX) -c $(CXXFLAGS) -o2 $(UTIL)/Utils.cpp -o ../bin/Utils.o
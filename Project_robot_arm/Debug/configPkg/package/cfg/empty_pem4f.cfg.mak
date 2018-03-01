# invoke SourceDir generated makefile for empty.pem4f
empty.pem4f: .libraries,empty.pem4f
.libraries,empty.pem4f: package/cfg/empty_pem4f.xdl
	$(MAKE) -f C:\Users\thors_000\workspace_v7\Project_robot_arm/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\thors_000\workspace_v7\Project_robot_arm/src/makefile.libs clean


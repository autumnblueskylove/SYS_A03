# The Hoochamacallit System
## The third assignment in system programming course

> 1. Overall System Idea
> > Background
> > > The Hoochamacallit System is meant to model a network of specialized production machines (called 
Hoochamacallits) all communicating their current running status with a backend server.  The server keeps tabs 
on which machines are running and which are not.  This is a “Thorton Proof-of-Concept Simulator” system and 
as such, the server and all of the production machines will be actually running on a single Linux installation. 

> > > In order to complete this simulator, you need to create an application suite (a system) that consists of three
distinct processing components:  
> > > >•  a "data creator" (DC) application – call the application DC.  This models the production machine. 
> > > >
> > > >•  a "data reader" (DR) application – call the application DR.  This models the server. 
> > > >
> > > >•  a “data corruptor” (DX) application – call the application DX.  This acts as a stress testing module. 

> > > In this system’s development, you will design and implement the data creator application, the data reader 
application, and the data corruptor application.   

> > > The purpose of this assignment is to force all involved in the solution to see how / why and when certain IPC
mechanisms are used within UNIX / Linux System Application programming.  That is, what IPC mechanism is
better suited for performing a certain type of communication job over another form of IPC. 

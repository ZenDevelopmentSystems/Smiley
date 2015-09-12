all: 				ges
					
clean:				clean_all

main.o: 			main.cpp
					g++ -o main.o -c main.cpp `pkg-config --cflags --libs opencv`
					
faceDetector.o:		faceDetector/faceDetector.cpp faceDetector/faceDetector.hpp
					g++ -o faceDetector/faceDetector.o -c faceDetector/faceDetector.cpp `pkg-config --cflags opencv`
					
airGest.o:			airGest/airGest.cpp airGest/airGest.hpp
					g++ -o airGest/airGest.o -c airGest/airGest.cpp `pkg-config --cflags opencv`

gallery.o:			gallery/gallery.cpp gallery/gallery.hpp
					g++ -o gallery/gallery.o -c gallery/gallery.cpp `pkg-config --cflags opencv`
					
ges:				faceDetector.o airGest.o gallery.o main.o
					g++ -o ges faceDetector/faceDetector.o airGest/airGest.o gallery/gallery.o main.o `pkg-config --libs opencv`

clean_all:
					rm -rf *.o
					rm -f ges

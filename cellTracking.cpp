#include "CImg.h"
#include <iostream>
#include <string>
using namespace cimg_library;

/* Main program */
int main(int argc,char **argv)
{
	std::string prgrm(argv[0]);
	/* Create and load the 3D image */
	/* See http://cimg.eu/reference/group__cimg__storage.html */
	/* See "Construct image from reading an image file" at http://cimg.eu/reference/structcimg__library_1_1CImg.html */
	CImg<unsigned short> img;
	float voxelsize[3];
	/* Load in Analyze format and get the voxel size in an array */
	if(argc<2){
		std::cerr << "Usage : " + prgrm + " <img> [threshold] [erosion count] [dilation count]" << std::endl;
		return -1;
	}else{
		img.load_analyze(argv[1],voxelsize);
	}
	
	int threshold, erosionCount, dilationCount;
	
	threshold = 350;
	if(argc>2){
		threshold = atoi(argv[2]);
	}
	
	erosionCount = 8;
	if(argc>3){
		erosionCount = atoi(argv[3]);
	}
	
	dilationCount = 8;
	if(argc>4){
		dilationCount = atoi(argv[4]);
	}
	
	/* Get the image dimensions */
    unsigned int dim[]={img.width(),img.height(),img.depth()}; 
	printf("Reading %s. Dimensions=%d %d %d\n",argv[1],dim[0],dim[1],dim[2]);
	printf("Voxel size=%f %f %f\n",voxelsize[0],voxelsize[1],voxelsize[2]);
		
	/* Create the display window */
	/* See http://cimg.eu/reference/structcimg__library_1_1CImgDisplay.html */
	/* Looak at normalization at http://cimg.eu/reference/group__cimg__displays.html */
	CImgDisplay disp(512,512,"");    
	CImgDisplay dispHist;
	
	// PROCESSING
	
	//mpr_img2.laplacian();
	//3D
	//CImg<unsigned char> meanMask(meanMaskDimension,meanMaskDimension,meanMaskDimension,1,0);
	//CImg<unsigned char> laplacianMask(3,3,3,1,0);
	//laplacianMask.fill(1);
	////meani
	//mpr_img2.convolve(laplacianMask);
	
	//Mean filter
	int meanMaskDimension = 3;
	//3D
	CImg<unsigned char> meanMask(meanMaskDimension,meanMaskDimension,meanMaskDimension,1,0);
	meanMask.fill(1);
	img.convolve(meanMask);
	
	int off;
	cimg_foroff(img,off){
		if(img[off]<threshold){
			img[off]=0;
		//		
		}else{
			img[off]=255;
		//
		}
	}
	
	//TODO test this median filter + TODO test with different parameters
	//img.blur_median(5);
	
	//Close : erode, then dilate
	img.erode(erosionCount);
	img.dilate(dilationCount);
	
	//Labelize
	img.label();
	
	//Compute features for each label
	/*int labelCount = img.max();
	//CImg< float > histogram = mpr_img.histogram(labelCount);
	int regionSize[labelCount], regionX[labelCount], regionY[labelCount];
	int i;
	for(i=0;i<labelCount;i++){
		regionSize[i]=0;
		regionX[i]=0;
		regionY[i]=0;
	}
	cimg_foroff(img,off){
		regionSize[(int)img[off]]++;
		//regionX[(int)mpr_img[off]]+=
		//regionY[(int)mpr_img[off]]+=
		//std::cout << regionSize[(int)mpr_img[off]] << std::endl;
	}*/
	
	// END OF PROCESSING
	
	/* The 3 displayed slices of the MPR visualisation */
	int displayedSlice[]={dim[0]/2,dim[1]/2,dim[2]/2}; 
		
	/* Slice corresponding to mouse position: */
	unsigned int coord[]={0,0,0};
	
	/* in MPR [2=original slice size=x y       0 size=z y
	1= size=x z                                */
	/* -1 corresponds to the 4th quarter where there is no MPR plane */
	int plane=2;
	
	/* For a first drawing, activate the redrawing */
	bool redraw=true;
	
	/* Manage the display windows: ESC, or closed -> close the main window*/
	while (!disp.is_closed() && !disp.is_keyESC()) // main loop
	{
		/* Wait for any event on the display windows during 1 ms.
		If nothing, at the beginning draw for the first time the window */
		disp.wait(1);
		
		/* List of events */
		
		/* - Resizing */
		if (disp.is_resized()) 
		{
			disp.resize();
		}
		/* Movement of the mouse */
		
		/* If the mouse is inside the window, find its position w.r.t. to the 3 planes */
		if(disp.mouse_x()>=0 && disp.mouse_y()>=0) 
		{
			
			unsigned int mX = disp.mouse_x()*(dim[0]+dim[2])/disp.width();
			unsigned int mY = disp.mouse_y()*(dim[1]+dim[2])/disp.height();
			
			if (mX>=dim[0] && mY<dim[1]) 
			{ 
				plane = 0; 
				coord[1] = mY; 
				coord[2] = mX - dim[0];   
				coord[0] = displayedSlice[0]; 
			}
			else 
			{
				if (mX<dim[0] && mY>=dim[1]) 
				{ 
					plane = 1; 
					coord[0] = mX; 
					coord[2] = mY - dim[1];   
					coord[1] = displayedSlice[1]; 
				}
				else 
				{
					if (mX<dim[0] && mY<dim[1])       
					{ 
						plane = 2; 
						coord[0] = mX; 
						coord[1] = mY;     
						coord[2] = displayedSlice[2]; 
					}
					else 
					{
						plane = -1; 
						coord[0] = 0;
						coord[1] = 0;
						coord[2] = 0;
					}
				}
			}
			redraw = true;
		}
		
		/* - Click Right button */
		if (disp.button()&2  && (plane!=-1))  
		{
			for(unsigned int i=0;i<3;i++) 
			{
				displayedSlice[i]=coord[i];
			}
			redraw = true;
		}
		/* - Wheel interaction */
		if (disp.wheel()) 
		{
			displayedSlice[plane]=displayedSlice[plane]+disp.wheel();
			
			if (displayedSlice[plane]<0) 
			{
				displayedSlice[plane] = 0;
			}
			else 
			{
				if (displayedSlice[plane]>=(int)dim[plane])
				{
					displayedSlice[plane] = (int)dim[plane]-1;
				}
			}
			
		/* Flush all mouse wheel events in order to not repeat the wheel event */
		disp.set_wheel();
			
		redraw = true;
		}
		
		/* If any change, we need to redraw the main windows */
		if (redraw)
		{
			/* Create a 2D image based on the MPR projections given by a projection point which is the intersection of the displayed slices */
			CImg<> mpr_img=img.get_projections2d(displayedSlice[0],displayedSlice[1],displayedSlice[2]);  
				
			/* The MPR image has a given size. It needs to be resized in order to fit at best in the display window */
			mpr_img.resize(512,512); 

			/* Display the MPR image in the display windows */
			disp.display(mpr_img);
			
			/* To avoid repetitive continuous redrawing */
			redraw=false;
		}
	}
	return 0;
}



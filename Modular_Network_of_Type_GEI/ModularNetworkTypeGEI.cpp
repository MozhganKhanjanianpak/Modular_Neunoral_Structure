/////////////////////////////////////////////////////////////////
// Code for creating a Modular random network of type GEI
/////////////////////////////////////////////////////////////////

#include <iostream>
#include <math.h>
#include <fstream>
#include <cstdlib>  // defines rand
#include <ctime>    // defines time() function

using namespace std;

#define N	2000    // Total number of nodes
#define Block  8  // number of blocks
#define p_in  0.6  // connection probability inside blocks
#define p_out 0.029  // connection probability outside blocks
#define Eper  0.8  // excitatory percentage
#define Wexcitatory   1 // weight of excitatoty synapses
#define Winhibitory   -1 // weight of inhibitory synapses

int    A[N][N] = {0};  // Adjacency Matrix layer Exc

/********************************************/
// Main
/********************************************/
int main()
{
 ofstream outputAdjMat("./ModularAdjMatTypeGEI.txt");
 ofstream outputBlockNodes("./BlockNodesNumberTypeGEI.txt");
 ofstream outputAdjList("./ModularAdjListTypeGEI.txt");

 outputBlockNodes <<  Block << "\t" << N << "\t" << 0 << "\t" << 0 << endl;

 unsigned seed = time(NULL);
 srand(seed);
 
 int Nmean=int(N/Block);  // mean number of nodes inside blocks
 int Nmin=Nmean-Nmean*0.1; // minimum number of neurons inside blocks
 int Nmax=Nmean+Nmean*0.1; // maximum number of neurons inside blocks
 double r=0; //random number between (0,1)
 int newN=0;  // random number of neurons inside blocks; Nmin < newN < Nmax
 int sumN=0;
 
 int b=0; //the counter of blocks
 while (b<Block){
   if (b<Block-1){
   	newN=rand()%(Nmax-Nmin+1)+Nmin; // number of neurons in the cuurent block
   	outputBlockNodes <<  b << "\t" << newN << "\t" <<  int(sumN+Eper*newN)<< "\t" << int(sumN+newN)<< endl; 
 	
	for (int i=sumN ; i<sumN+newN ; i++){
 		for (int j=0 ; j<N ; j++){
 			if (i==j) continue;
 			if ( j>=sumN && j<sumN+newN ){
 				r = rand()/double(RAND_MAX); 
 				if (r<p_in){
 					int start=sumN+Eper*newN;  
 					if (i<start) A[i][j]=Wexcitatory;
 					else A[i][j]=Winhibitory;
				}
			}
			else{
				r = rand()/double(RAND_MAX); 
 				if (r<p_out){
 					int start=sumN+Eper*newN;
 					if (i<start) A[i][j]=Wexcitatory;
 					else A[i][j]=Winhibitory;
				}				
			} 
		}
	}
	sumN += newN;
   }
   
   else{
   	newN=N-sumN; // number of neurons in the last block
   	outputBlockNodes <<  b << "\t" << newN << "\t" <<  int(sumN+Eper*newN)<< "\t" << int(sumN+newN)<< endl;

	for (int i=sumN ; i<N ; i++){
 		for (int j=0 ; j<N ; j++){
 			if (i==j) continue;
 			if ( j>=sumN && j<N ){
 				r = rand()/double(RAND_MAX); 
 				if (r<p_in){
 					int start=sumN+Eper*newN;
 					if (i<start) A[i][j]=Wexcitatory;
 					else A[i][j]=Winhibitory;
				 }
			}
			else{
				r = rand()/double(RAND_MAX); 
 				if (r<p_out){
 					int start=sumN+Eper*newN;
 					if (i<start) A[i][j]=Wexcitatory;
 					else A[i][j]=Winhibitory;
				 }				
			} 
		}
	}	
   }
   
   b++;
 }

 //-----------------------------------------------------------------
 // output; Adjacency matrix
 for (int i=0;i<N;i++){
 	for (int j=0;j<N;j++){
 		outputAdjMat << "\t" << A[i][j];
	}
	outputAdjMat << endl;
 }//*/
 
 //-----------------------------------------------------------------
 // Output: Adjacency List
 int linkNum=0;
 for (int i=0;i<N;i++)
 	for (int j=0;j<N;j++)
 		if (A[i][j] != 0) linkNum++;
 
 outputAdjList << N << "\t" << linkNum << "\t" << 0 << endl;
 
 for (int i=0;i<N;i++)
 	for (int j=0;j<N;j++){
 		if (A[i][j] != 0)
 			outputAdjList << i << "\t" << j << "\t" << A[i][j] << endl; 					
	}
 //*/
return 0;
}

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <assert.h>

struct cacheBlock{
	int tag;
	bool valid;
	bool dirtyBit;
	int replacement;
};


//Funciones genéricas
bool isPowerOf2(int x);
bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way);



//Funciones para la cache SRRIP
void updateSRRIP(struct cacheBlock tags[], int index, int way, int assoc);
int getVictimSRRIP(struct cacheBlock tags[], int index, int assoc);




//Inicio del main

int main(int argc, char** argv)
{
	int cacheSize;
	int assoc;
	int blockSize;
	std::string addressesFile;
	int numTags;
	int numSets;
	int offsetBits;
	int indexBits;
	int indexMask;

    if(argc==5)
    {
      //Tamaño de la memoria caché en bytes
        cacheSize=std::stoi(argv[1]);
    	assert(isPowerOf2(cacheSize)&&"cacheSize must be power of 2");

      //Tamaño del bloque en bytes
        assoc=std::stoi(argv[2]);
    	assert(isPowerOf2(assoc)&&"assoc must be power of 2");
    	assert(cacheSize>assoc&&"cacheSize must be greater than assoc");

        blockSize=std::stoi(argv[3]);
    	assert(isPowerOf2(blockSize)&&"blockSize must be power of 2");
    	assert(cacheSize>blockSize&&"cacheSize must be greater than blockSize");
      //archivo de direcciones
        addressesFile.append(argv[4]);

        //std::cout<<cacheSize<<assoc<<blockSize<<addressesFile<<std::endl;
    }
    else{
    	std::cout<<"usage: ./main cache_size Block_size Assoc Accesses_file"<<std::endl;
    	return -1;
    }

    numTags=cacheSize/blockSize;
    numSets=numTags/assoc;
    offsetBits=log2(blockSize);
    indexBits=log2(numSets);
    indexMask=numSets-1;

	std::cout<<"Simulando cache de "<<cacheSize<<" bytes, "<<numTags<<" bloques, "<<numSets<<" conjuntos, "<<assoc<<" vías, "<<offsetBits<<" bits de offset y "<<indexBits<<" bits de index"<<std::endl;

	//struct cacheBlock *tagsLRU=new cacheBlock[numTags];
	//struct cacheBlock *tagsLFU=new cacheBlock[numTags];
	//struct cacheBlock *tagsLIFO=new cacheBlock[numTags];
	//struct cacheBlock *tagsFIFO=new cacheBlock[numTags];
	struct cacheBlock *tagsSRRIP=new cacheBlock[numTags];

	for(int i=0; i<numSets; i++){
//		setsLRU[i]=0x00010203;
		for(int j=0;j<assoc;j++){
			//tagsLRU[(i*assoc)+j].valid=0;
			//tagsLRU[(i*assoc)+j].tag=0;
			//tagsLRU[(i*assoc)+j].dirtyBit=0;
			//tagsLRU[(i*assoc)+j].replacement=j;

			//tagsLFU[(i*assoc)+j].valid=0;
			//tagsLFU[(i*assoc)+j].tag=0;
			//tagsLFU[(i*assoc)+j].dirtyBit=0;
			//tagsLFU[(i*assoc)+j].replacement=0;

			//tagsLIFO[(i*assoc)+j].valid=0;
			//tagsLIFO[(i*assoc)+j].tag=0;
			//tagsLIFO[(i*assoc)+j].dirtyBit=0;
			//tagsLIFO[(i*assoc)+j].replacement=0;

			//tagsFIFO[(i*assoc)+j].valid=0;
			//tagsFIFO[(i*assoc)+j].tag=0;
			//tagsFIFO[(i*assoc)+j].dirtyBit=0;
			//tagsFIFO[(i*assoc)+j].replacement=0;

			tagsSRRIP[(i*assoc)+j].valid=0;
			tagsSRRIP[(i*assoc)+j].tag=0;
			tagsSRRIP[(i*assoc)+j].dirtyBit=0;
			tagsSRRIP[(i*assoc)+j].replacement=3;
		}
	}
	std::cout<<"Inicializados los array"<<std::endl;

	int accesses=0;
	//int missesLRU=0;
	//int missesLFU=0;
	//int missesLIFO=0;
	//int missesFIFO=0;
	int missesSRRIP=0;

    //Create an input file stream
    std::ifstream in(addressesFile,std::ios::in);

    /*
     As long as we haven't reached the end of the file, keep reading entries.
    */
    int address;  //Variable to hold each number as it is read
            //Read number using the extraction (>>) operator
    while (in >> address) {
		//int myAccess = accesses[acc];
		int way;
		accesses++;

		int index = (address>>offsetBits)&indexMask;
		int tag = (address>>(indexBits+offsetBits));


      



//Actualizo la SRRIP
		if(isHit(tagsSRRIP, index, tag, assoc, &way)==false)
		{
			way = getVictimSRRIP(tagsSRRIP, index, assoc);
			tagsSRRIP[(index*assoc)+way].tag=tag;
			tagsSRRIP[(index*assoc)+way].valid=true;
			missesSRRIP+=1;
		}
		updateSRRIP(tagsSRRIP, index, way, assoc);
    }
    //Close the file stream
    in.close();

	
	
  
  std::cout<<"Resultados SRRIP\n"<<"Accesses= "<<accesses<<" Misses= "<<missesSRRIP<<" Miss rate= "<<(float)missesSRRIP/accesses<<std::endl;	

	delete [] tagsSRRIP;

	return true;
}

bool isHit(struct cacheBlock tags[], int index, int tag, int assoc, int *way){
	for(int i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].tag==tag&&tags[(index*assoc)+i].valid==true)
		{
			*way=i;
			return(true);
		}
	}
	return(false);
}

bool isPowerOf2(int x)
{
	return !(x == 0) && !(x & (x - 1));
}






// SRRIP
void updateSRRIP(struct cacheBlock tags[], int index, int way, int assoc){
	tags[(index*assoc)+way].replacement=0;
}

int getVictimSRRIP(struct cacheBlock tags[], int index, int assoc){
	//check empty way
	int i;
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].valid==false)
		{
			return i;
		}
	}

	//look for the victim way
	for(i=0; i<assoc;i++)
	{
		if(tags[(index*assoc)+i].replacement==(assoc-1))
			tags[(index*assoc)+i].replacement-=1;
			return i;
	}
	tags[(index*assoc)+i].replacement+=1;
	return i;
}

#include "../common/APxLinkData.hh"
#include <ap_int.h>
#include <bitset>

using namespace std;

#define ETA 17 //()
#define PHI 32 //(1+6+1)*4
#define NLINKS 32 // only one eta half

class Tower {
  public:  
    Tower() : cluster_et(0), tower_et(0), peak_phi(0), peak_eta(0), peak_time(0), hOe(0) {};

    Tower(uint32_t i) {
      this->cluster_et = i;
      this->tower_et  = i >> 10;
      this->peak_phi  = i >> 20;
      this->peak_eta  = i >> 23;
      this->peak_time = i >> 26;
      this->hOe       = i >> 29;
    }

    inline operator uint32_t() {
#pragma HLS INLINE

      return ((uint32_t) (this->hOe)    << 29)
        | ((uint32_t) (this->peak_time) << 26)
	| ((uint32_t) (this->peak_eta)  << 23)
	| ((uint32_t) (this->peak_phi)  << 20)
	| ((uint32_t) (this->tower_et)  << 10)
	| (uint32_t) this->cluster_et;
    }

    ap_uint<10> cluster_et;
    ap_uint<10> tower_et;
    ap_uint<3> peak_phi;
    ap_uint<3> peak_eta;
    ap_uint<3> peak_time;
    ap_uint<3> hOe;
};       

// Packs a tower object into 6 64-bit words, useful for link formatting
bool pack(Tower towers[17], ap_uint<64> out[9]) {

  for (size_t word = 0; word < 8; word++){ 
    int iTower = word*2;

    out[word].range(  9,  0) = ap_uint<10>(towers[iTower].cluster_et); //0
    out[word].range( 19, 10) = ap_uint<10>(towers[iTower].tower_et);     //0
    out[word].range( 22, 20) = ap_uint<10>(towers[iTower].peak_phi);     //0
    out[word].range( 25, 23) = ap_uint<10>(towers[iTower].peak_eta);     //0
    out[word].range( 28, 26) = ap_uint<10>(towers[iTower].peak_time);    //0
    out[word].range( 31, 29) = ap_uint<10>(towers[iTower].hOe);          //0

    out[word].range( 41, 32) = ap_uint<10>(towers[iTower+1].cluster_et); //0
    out[word].range( 51, 42) = ap_uint<10>(towers[iTower+1].tower_et);     //0
    out[word].range( 54, 52) = ap_uint<10>(towers[iTower+1].peak_phi);     //0
    out[word].range( 57, 55) = ap_uint<10>(towers[iTower+1].peak_eta);     //0
    out[word].range( 60, 58) = ap_uint<10>(towers[iTower+1].peak_time);    //0
    out[word].range( 63, 61) = ap_uint<10>(towers[iTower+1].hOe);          //0
  }

    out[8].range(  9,  0) = ap_uint<10>(towers[16].cluster_et); //0
    out[8].range( 19, 10) = ap_uint<10>(towers[16].tower_et);     //0
    out[8].range( 22, 20) = ap_uint<10>(towers[16].peak_phi);     //0
    out[8].range( 25, 23) = ap_uint<10>(towers[16].peak_eta);     //0
    out[8].range( 28, 26) = ap_uint<10>(towers[16].peak_time);    //0
    out[8].range( 31, 29) = ap_uint<10>(towers[16].hOe);          //0

    out[8].range( 41, 32) = ap_uint<10>(0x0); //0
    out[8].range( 51, 42) = ap_uint<10>(0x0); //0
    out[8].range( 54, 52) = ap_uint<10>(0x0); //0
    out[8].range( 57, 55) = ap_uint<10>(0x0); //0
    out[8].range( 60, 58) = ap_uint<10>(0x0); //0
    out[8].range( 63, 61) = ap_uint<10>(0x0); //0
  //for(size_t kk=0; kk< 6;  kk++)
  //  cout<<std::setw(20)<<out[kk]<<"   "<<std::bitset<64>(out[kk])<<std::endl;

  return true;
}

int main(int argn, char *argp[]) {

  Tower towersInPhi[PHI][ETA];

  for(int ieta = 0; ieta < ETA; ieta++){
    for(int iphi = 0; iphi < PHI; iphi++){
      towersInPhi[iphi][ieta].cluster_et   =0;
      towersInPhi[iphi][ieta].tower_et     =0;
      towersInPhi[iphi][ieta].peak_phi     =7;
      towersInPhi[iphi][ieta].peak_eta     =7;
      towersInPhi[iphi][ieta].peak_time    =7;
      towersInPhi[iphi][ieta].hOe          =7;

    }
  }

  //-- Manual inputs to generate test vectors
  towersInPhi[3][10].cluster_et   = 23; //fdc06c17 -
  towersInPhi[3][10].tower_et     = 27;
  towersInPhi[3][10].peak_phi     = 4;
  towersInPhi[3][10].peak_eta     = 3;

  towersInPhi[4][10].cluster_et   = 40; //fd80b428 -
  towersInPhi[4][10].tower_et     = 45;
  towersInPhi[4][10].peak_phi     = 0;
  towersInPhi[4][10].peak_eta     = 3;

  towersInPhi[15][3].cluster_et   = 37; //fd409c25 -
  towersInPhi[15][3].tower_et     = 39;
  towersInPhi[15][3].peak_phi     = 4;
  towersInPhi[15][3].peak_eta     = 2;

  towersInPhi[16][3].cluster_et   = 12; //fd00300c -
  towersInPhi[16][3].tower_et     = 12;
  towersInPhi[16][3].peak_phi     = 0;
  towersInPhi[16][3].peak_eta     = 2;

  towersInPhi[25][16].cluster_et   = 52; //fe30e034 -
  towersInPhi[25][16].tower_et     = 56;
  towersInPhi[25][16].peak_phi     = 3;
  towersInPhi[25][16].peak_eta     = 4;

  towersInPhi[11][16].cluster_et   = 9; //fe102409 -
  towersInPhi[11][16].tower_et     = 9;
  towersInPhi[11][16].peak_phi     = 1;
  towersInPhi[11][16].peak_eta     = 4;

  towersInPhi[25][0].cluster_et    = 21; //fc306015 -
  towersInPhi[25][0].tower_et      = 24;
  towersInPhi[25][0].peak_phi      = 3;
  towersInPhi[25][0].peak_eta      = 0;

  towersInPhi[11][0].cluster_et   = 70; //fc113c46 -
  towersInPhi[11][0].tower_et     = 79;
  towersInPhi[11][0].peak_phi     = 1;
  towersInPhi[11][0].peak_eta     = 0;

  towersInPhi[27][7].cluster_et   = 66; //fd411c42 -
  towersInPhi[27][7].tower_et     = 71;
  towersInPhi[27][7].peak_phi     = 4;
  towersInPhi[27][7].peak_eta     = 2;

  towersInPhi[28][7].cluster_et = 14; //fd00400e -
  towersInPhi[28][7].tower_et     = 16;
  towersInPhi[28][7].peak_phi     = 0;
  towersInPhi[28][7].peak_eta     = 2;



  ap_uint<64> packed[NLINKS][9];
  for(int ilink = 0; ilink<NLINKS; ilink++){
    int phi = ilink;

    pack(towersInPhi[phi], packed[ilink]); 
  }

  APxLinkData link_in(NLINKS);

  for (size_t i = 0; i < 9; i++){
    for (size_t k = 0; k < NLINKS; k++){

      link_in.add(i, k, {0x00, packed[k][i]});
//      cout<<"Word="<<i<<setw(4)<<" Link="<<k<<setw(4)<<"["<<eta<<"]"<<setw(3)<<"["<<phi<<"]   "<<std::hex<<packed[eta][phi][i]<<endl;

    }
  }

  link_in.write("test_in-gct.txt");

  return 0;
}

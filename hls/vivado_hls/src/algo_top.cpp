#include "algo_top_parameters.h"
#include "algo_top.h"
#include <algorithm>
#include <utility>
#include <iostream>
#include "objects.h"
#include "stitchTowers.h"

using namespace std;
using namespace algo;


TowersInEta unpackInputLink(hls::stream<algo::axiword> &link) {
#pragma HLS INTERFACE axis port=link
#pragma HLS PIPELINE II=N_WORDS_PER_FRAME
#pragma HLS INLINE

  TowersInEta tEta_;
  ap_uint<576> bigWord;

#ifndef __SYNTHESIS__
  // Avoid simulation warnings
  if (link.empty()) return tEta_ ;
#endif

  bigWord = link.read().data;
/*
for(size_t tEta=0, start=0, end=31; tEta<17; tEta++, start +=32, end +=32)

  {
#pragma LOOP UNROLL
    tEta_.towers[tEta] = Tower(bigWord(end, start));
  }

*/

    tEta_.towers[0] = Tower(bigWord(31, 0));
    tEta_.towers[1] = Tower(bigWord(63, 32));
    tEta_.towers[2] = Tower(bigWord(95, 64));
    tEta_.towers[3] = Tower(bigWord(127, 96));
    tEta_.towers[4] = Tower(bigWord(159,128));
    tEta_.towers[5] = Tower(bigWord(191, 160));
    tEta_.towers[6] = Tower(bigWord(223,192));
    tEta_.towers[7] = Tower(bigWord(255, 224));
    tEta_.towers[8] = Tower(bigWord(287, 256));
    tEta_.towers[9] = Tower(bigWord(319, 288));
    tEta_.towers[10] = Tower(bigWord(351, 320));
    tEta_.towers[11] = Tower(bigWord(383, 352));
    tEta_.towers[12] = Tower(bigWord(415, 384));
    tEta_.towers[13] = Tower(bigWord(447, 416));
    tEta_.towers[14] = Tower(bigWord(479, 448));
    tEta_.towers[15] = Tower(bigWord(511, 480));
    tEta_.towers[16] = Tower(bigWord(543, 512));
    //tEta_.towers[17] = Tower(bigWord(575, 544));



  return tEta_;
}

//hls::stream<algo::axiword> packOutput(TowersInEta tEta_ )hls::stream<algo::axiword> packOutput(TowersInEta tEta_ ){
bool packOutput(TowersInEta tEta_, hls::stream<algo::axiword> &olink){
#pragma HLS INTERFACE axis port=link
#pragma HLS PIPELINE II=N_OUTPUT_WORDS_PER_FRAME
#pragma HLS INLINE

  ap_uint<576> outBigWord;


/*
   for(size_t tEta=0, start=0, end=31; tEta<17; tEta++, start +=32, end +=32)

  {
#pragma LOOP UNROLL
    outBigWord(end, start) = (ap_uint<32>) tEta_.towers[tEta].data;
  }
*/

    outBigWord(31, 0) = (ap_uint<32>) tEta_.towers[0].data;
    outBigWord(63, 32) = (ap_uint<32>) tEta_.towers[1].data;
    outBigWord(95, 64) = (ap_uint<32>) tEta_.towers[2].data;
    outBigWord(127, 96) = (ap_uint<32>) tEta_.towers[3].data;
    outBigWord(159,128) = (ap_uint<32>) tEta_.towers[4].data;
    outBigWord(191, 160) = (ap_uint<32>) tEta_.towers[5].data;
    outBigWord(223,192) = (ap_uint<32>) tEta_.towers[6].data;
    outBigWord(255, 224) = (ap_uint<32>) tEta_.towers[7].data;
    outBigWord(287, 256) = (ap_uint<32>) tEta_.towers[8].data;
    outBigWord(319, 288) = (ap_uint<32>) tEta_.towers[9].data;
    outBigWord(351, 320) = (ap_uint<32>) tEta_.towers[10].data;
    outBigWord(383, 352) = (ap_uint<32>) tEta_.towers[11].data;
    outBigWord(415, 384) = (ap_uint<32>) tEta_.towers[12].data;
    outBigWord(447, 416) = (ap_uint<32>) tEta_.towers[13].data;
    outBigWord(479, 448) = (ap_uint<32>) tEta_.towers[14].data;
    outBigWord(511, 480) = (ap_uint<32>) tEta_.towers[15].data;
    outBigWord(543, 512) = (ap_uint<32>) tEta_.towers[16].data;
    //outBigWord(575, 544) = (ap_uint<32>) tEta_.towers[17].data;


  axiword r; r.last = 0; r.user = 0;
  r.data = outBigWord;
  olink.write(r);

  return true;
}

void algo_top(hls::stream<algo::axiword> link_in[N_INPUT_LINKS], hls::stream<algo::axiword> link_out[N_OUTPUT_LINKS]) {
#pragma HLS INTERFACE axis port=link_in
#pragma HLS INTERFACE axis port=link_out
#pragma HLS PIPELINE II=N_WORDS_PER_FRAME

#pragma HLS ARRAY_PARTITION variable=link_in complete dim=0
#pragma HLS ARRAY_PARTITION variable=link_out complete dim=0

  // Step 1: Unpack links
  TowersInEta towersInPhi[TOWERS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=towersInPhi  complete dim=0

  for (size_t ilink = 0; ilink < N_INPUT_LINKS; ilink++) {
#pragma LOOP UNROLL
    //#pragma HLS latency min=6 max=6
    towersInPhi[ilink] = unpackInputLink(link_in[ilink]);
  }


  // Step 2: Stitch accross phi boundaries
  TowersInEta stitchedInPhi[TOWERS_IN_PHI];
#pragma HLS ARRAY_PARTITION variable=stitchedInPhi  complete dim=0

  for(size_t tphi = 0; tphi < TOWERS_IN_PHI-4; tphi += 4){
#pragma LOOP UNROLL
    stitchedInPhi[tphi+1] = towersInPhi[tphi+1];
    stitchedInPhi[tphi+2] = towersInPhi[tphi+2];

#ifndef __SYNTHESIS__  
    cout<<"tphi = "<<tphi+3<<", "<<tphi+4<<":--"<<endl;
#endif
    stitchInEta(towersInPhi[tphi+3], towersInPhi[tphi+4], stitchedInPhi[tphi+3], stitchedInPhi[tphi+4]);
  }
  stitchedInPhi[0]               = towersInPhi[0];
  stitchedInPhi[TOWERS_IN_PHI-1] = towersInPhi[TOWERS_IN_PHI-1];
  stitchedInPhi[TOWERS_IN_PHI-2] = towersInPhi[TOWERS_IN_PHI-2];
  stitchedInPhi[TOWERS_IN_PHI-3] = towersInPhi[TOWERS_IN_PHI-3];

#ifndef __SYNTHESIS__  
  for(int tphi=0; tphi<TOWERS_IN_PHI; tphi++){
    for(int teta=0; teta<TOWERS_IN_ETA; teta++){

      if(stitchedInPhi[tphi].towers[teta].cluster_et() != 0 )
	cout<<std::dec<<"Pos: [tphi, teta] = ["<<tphi<<", "<<teta<<"]: "<<stitchedInPhi[tphi].towers[teta].toString()<<"  "<<std::hex<<stitchedInPhi[tphi].towers[teta].data;
	cout<<endl;
    }
  }
#endif


  // Step 4: Pack the outputs
  for (size_t i = 0; i < N_OUTPUT_LINKS; i++) {
#pragma LOOP UNROLL
    packOutput(stitchedInPhi[i], link_out[i]);

  }

}
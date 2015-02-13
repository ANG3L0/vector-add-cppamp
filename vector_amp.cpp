#include <wb.h> 
#include <amp.h>
using namespace concurrency;

void vecAdd(float* in1, float* in2, float* out, int N){
  //one-dimensional arrays for AMP
  array_view<float,1> in1V(N,in1), in2V(N,in2);
  array_view<float,1> outV(N,out);
  outV.discard_data(); //ignore existing data as we'll overwrite it anyway
  parallel_for_each(outV.get_extent(), [=](index<1> i)
      restrict(amp) { //only AMP can do this
      outV[i] = in1V[i] + in2V[i];
      });
  outV.synchronize(); //tell everyone i'm done so dependent processing will wait.

}


int main(int argc, char **argv) {
  wbArg_t args;
  int inputLength;
  float *hostInput1;
  float *hostInput2;
  float *hostOutput;

  args = wbArg_read(argc, argv);

  wbTime_start(Generic, "Importing data and creating memory on host");
  hostInput1 = (float *)wbImport(wbArg_getInputFile(args, 0), &inputLength);
  hostInput2 = (float *)wbImport(wbArg_getInputFile(args, 1), &inputLength);
  hostOutput = (float *)malloc(inputLength * sizeof(float));
  wbTime_stop(Generic, "Importing data and creating memory on host");

  wbLog(TRACE, "The input length is ", inputLength);

  vecAdd(hostInput1, hostInput2, hostOutput, inputLength);

  wbSolution(args, hostOutput, inputLength);


  return 0;
}


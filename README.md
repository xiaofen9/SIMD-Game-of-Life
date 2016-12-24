#SIMD 

##Intro
This is a  project about Single Instruction Multiple Data (SIMD) above PC for the propose of accelerating code execution. I leverage Conway's Game of Life to show the effectiveness of SIMD.

I implemented SIMD by sse intrinsic instructions (e.g., _mm_load_ps). 

##Code Structure
In fusion.cpp, there are two versions to run the Game of Life. The first is SIMD while the other is serilized one.
<pre><code>
  init();
  //SIMD enabled
  startTiming();  
  evolutionSIMD();
  stopWithPrintTiming();


  init();
  //serilized
  startTiming();  
  evolution();
  stopWithPrintTiming(); 
</code></pre>

There are further explain about the project in report.pdf in CHINESE.
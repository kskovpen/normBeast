void test()
{
   gROOT->SetBatch();
   
   // load NORM library
   gSystem->Load("libNORM.so");

   // create an instance of NORM
   NORM::beast be("input.txt");
//   cf.silentMode();
   
   // run NORM
   be.run();
 
   // user methods
//   std::cout << cf.getCHISQ() << std::endl;
//   for(int i=0;i<3;i++)
//     std::cout << cf.getPAR(i) << " +- " << cf.getERR(i) << std::endl;
   
   gApplication->Terminate();
}

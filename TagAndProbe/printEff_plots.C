/*#include <vector>


#include "effZ_3comp_etaBins2.cpp"


int  run_24_2p1_SingleMu(){


std::cout<<"AAAA"<<std::endl;
std::vector<TFile *> files;
std::cout<<files.size()<<std::endl;


return 0;*/
{
        string dirs[6]={"/IsoMu24_2p1_1/","/JPsi_13_1/","/JPsi_MuOnia_13_2/","/JPsi_MuOnia_HLTTwiki/","/JPsi_SingleMu_TkCuts/","/Z_Data_24_2p1/"};
        string castorDir("/tmp/rybinska/effFeb");                         
        cout<<"test"<<endl;
        string prefix_JPsi("Efficiency_0.5_");
        string prefix_Z("Efficiency_");
        string postfix_other_old("_RPCrange.root");
        string postfix_other(".root");
        string postfix_RPC[3]={"_barrel.root","_overlap.root","_endcap.root"};
        string postfix_out_RPC[3]={"barrel","overlap","endcap"};
        string flagsRPC[6]={"hasL1RPCMatch","hasL1RPCptValue5","hasL1RPCptValue10","hasL1RPCptValue15","hasL1RPCptValue20","hasL1RPCptValue30"};
        string flagsDT[6]={"hasL1DTMatch","hasL1DTptValue5","hasL1DTptValue10","hasL1DTptValue15","hasL1DTptValue20","hasL1DTptValue30"};
        string flagsCSC[6]={"hasL1CSCMatch","hasL1CSCptValue5","hasL1CSCptValue10","hasL1CSCptValue15","hasL1CSCptValue20","hasL1CSCptValue30"};
        string etaRPC("etabins2_RPC.root");
        string etaDT("etabins2_DT.root");
        string etaCSC("etabins2_CSC.root");
        // First plot goes without proper formating
        // Single flag plots vs pt, Z+JPsi
        for (int i = 0; i<6;i++){
                 for (int j=0;j<3;j++) 
                        makeEffPlots(
                                castorDir+dirs[2]+prefix_JPsi+flagsRPC[i]+postfix_RPC[j],
                                castorDir+dirs[1]+prefix_JPsi+flagsRPC[i]+postfix_RPC[j],
                                "Trigger Efficiency comparison",
                                "ZandJPsi_"+flagsRPC[i]+postfix_out_RPC[j]+".png",
                                true);                         
               /* makeEffPlots(
                        castorDir+dirs[8]+prefix_Z+flagsDT[i]+postfix_other,
                        castorDir+dirs[0]+prefix_JPsi+flagsDT[i]+postfix_other,                        
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+flagsDT[i]+".png",
                        true);
                makeEffPlots(
                        castorDir+dirs[8]+prefix_Z+flagsCSC[i]+postfix_other,
                        castorDir+dirs[0]+prefix_JPsi+flagsCSC[i]+postfix_other,                        
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+flagsCSC[i]+".png",
                        true);       */                                          
        }
        //Six flags plots vs pt, Z+JPsi
        //for (int j=0;j<3;j++)
                for (int i = 0; i<6;i++){
                 for (int j=0;j<3;j++) 
                        makeEffPlots(
                                castorDir+dirs[2]+prefix_JPsi+flagsRPC[i]+postfix_RPC[j],
                                castorDir+dirs[1]+prefix_JPsi+flagsRPC[i]+postfix_RPC[j],
                                "Trigger Efficiency comparison",
                                "ZandJPsi_"+flagsRPC[i]+postfix_out_RPC[j]+".png");     
                }
        }
        // Final Plots Old
/*for (int j=0;j<3;j++)         
                makeEffPlots(
                        castorDir+dirs[5]+prefix_Z+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[5]+postfix_RPC[j],
                        castorDir+dirs[4]+prefix_JPsi+flagsRPC[5]+postfix_RPC[j],                                                                                
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+postfix_out_RPC[j]+"SingleMu_old.png",
                        true);
                        
                  // Final Plots new
for (int j=0;j<3;j++)         
                makeEffPlots(
                        castorDir+dirs[0]+prefix_Z+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[5]+postfix_RPC[j],
                        castorDir+dirs[1]+prefix_JPsi+flagsRPC[5]+postfix_RPC[j],                                                                                
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+postfix_out_RPC[j]+"SingleMu_new.png",
                        true);
// Final Plots Old
for (int j=0;j<3;j++)         
                makeEffPlots(
                        castorDir+dirs[5]+prefix_Z+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[5]+prefix_Z+flagsRPC[5]+postfix_RPC[j],
                        castorDir+dirs[3]+prefix_JPsi+flagsRPC[5]+postfix_RPC[j],                                                                                
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+postfix_out_RPC[j]+"_old.png",
                        true);
                        
                  // Final Plots new
for (int j=0;j<3;j++)         
                makeEffPlots(
                        castorDir+dirs[0]+prefix_Z+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[0]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[1]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[2]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[3]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[4]+postfix_RPC[j],
                        castorDir+dirs[0]+prefix_Z+flagsRPC[5]+postfix_RPC[j],
                        castorDir+dirs[2]+prefix_JPsi+flagsRPC[5]+postfix_RPC[j],                                                                                
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+postfix_out_RPC[j]+"_new.png",
                        true);*/
       /* makeEffPlots(
                castorDir+dirs[8]+prefix_Z+flagsDT[0]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[0]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsDT[1]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[1]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsDT[2]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[2]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsDT[2]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[2]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsDT[4]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[4]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsDT[5]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsDT[5]+postfix_other,                                                                                
                "Trigger Efficiency comparison",
                "ZandJPsi_allDT.png",
                true);*/
     /*   makeEffPlots(
                castorDir+dirs[8]+prefix_Z+flagsCSC[0]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[0]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsCSC[1]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[1]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsCSC[2]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[2]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsCSC[3]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[3]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsCSC[4]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[4]+postfix_other,
                castorDir+dirs[8]+prefix_Z+flagsCSC[5]+postfix_other,
                castorDir+dirs[0]+prefix_JPsi+flagsCSC[5]+postfix_other,                                                                                
                "Trigger Efficiency comparison",
                "ZandJPsi_allCSC.png",
                true);       */
                
   /*    // Single flag comparison RPC with DT
        for (int i = 0; i<6;i++)
                makeEffPlots(
                        castorDir+dirs[8]+prefix_Z+flagsRPC[i]+postfix_RPC[0],
                        castorDir+dirs[0]+prefix_JPsi+flagsRPC[i]+postfix_RPC[0],
                        castorDir+dirs[8]+prefix_Z+flagsDT[i]+postfix_other,
                        castorDir+dirs[0]+prefix_JPsi+flagsDT[i]+postfix_other,                                                                               
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+flagsRPC[i]+"RPCbarrel_DT"+".png",
                        true); 
               // Single flag comparison RPC with CSC                              
                makeEffPlots(
                        castorDir+dirs[8]+prefix_Z+flagsRPC[i]+postfix_RPC[2],
                        castorDir+dirs[0]+prefix_JPsi+flagsRPC[i]+postfix_RPC[2],
                        castorDir+dirs[8]+prefix_Z+flagsDT[i]+postfix_other,
                        castorDir+dirs[0]+prefix_JPsi+flagsDT[i]+postfix_other,                                                                               
                        "Trigger Efficiency comparison",
                        "ZandJPsi_"+flagsRPC[i]+"RPCendcap_CSC"+".png",
                        true);            */                           
}

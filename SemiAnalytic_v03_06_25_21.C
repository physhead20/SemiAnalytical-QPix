#define SemiAnalytic_v03_06_25_21_cxx
#include "SemiAnalytic_v03_06_25_21.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <Math/Vector3D.h>
#include <TVector3.h>
#include <math.h> 
#include<TRandom.h> 
#include <string>

// ### Define which event you are looking at ###
int Event = 000;
int Event_plus_one = Event+3;

std::string EV = "001";

// #################################
// ### Global Detector Variables ###
// #################################
//
// These variables define properties of the detector I am simulating
double xDimension = 350.; // (cm)
double yDimension = 600.; // (cm)
double zDimension = 600.; // (cm)

// ### Define the size of pixels (in cm) ###
double PixelPitch = 4.0; // (cm)

// ### Compute the number of pixels in each direction ####
int nZPixels = zDimension/PixelPitch;
int nYPixels = yDimension/PixelPitch;
int nXPixels = xDimension/PixelPitch;



// ### Define the Pixel Map in terms of the number of pixels ###
//double PixelMap[nentries][nYPixels][nZPixels];

// ### Define the Pixel Plane Orientation ###
TVector3 planeNormalYZ(1, 0, 0);
// ##################################


TH2D *hDis_v_N1 = new TH2D("hDis_v_N1", "N1/NGamma vs Distance", 600, 0, 600, 100, 0, 10);
TH2D *hDis_v_N1_onAxis = new TH2D("hDis_v_N1_onAxis", "N1/NGamma vs Distance", 600, 0, 600, 100, 0, 10);
TH2D *hDis_v_N1_offAxis = new TH2D("hDis_v_N1_offAxis", "N1/NGamma vs Distance", 600, 0, 600, 100, 0, 10);
TH2D *hDis_v_zeta = new TH2D("hDis_v_zeta", "zeta vs Distance", 600, 0, 600, 200, 0, 200);
TH2D *hDis_v_mu = new TH2D("hDis_v_mu", "mu vs Distance", 600, 0, 600, 200, 0, 200);
TH2D *hDis_v_N2 = new TH2D("hDis_v_N2", "N2/N1 vs Distance", 600, 0, 600, 100, 0, 10);
TH2D *hDis_v_kappa = new TH2D("hDis_v_kappa", "kappa vs Distance", 600, 0, 600, 100, -1, 1);
//TH2D *hxz = new TH2D("hxz", "hxz", 350, 0, 350, 800, 0, 800);

// ###################################################################
// ### Loading TGraph's from csv files for On-Axis parametrization ###
// ###################################################################
TGraph *grN1NGamma_OnAxis = new TGraph("CSV_Files/DUNE_N1_over_NGamma_OnAxis.csv", "%lg %lg", ",");
TGraph *grZeta_OnAxis = new TGraph("CSV_Files/DUNE_Eta_OnAxis.csv", "%lg %lg", ",");
TGraph *grMu_OnAxis = new TGraph("CSV_Files/DUNE_mu_OnAxis.csv", "%lg %lg", ",");
TGraph *grN2N1_OnAxis = new TGraph("CSV_Files/DUNE_N2N1_OnAxis.csv", "%lg %lg", ",");
TGraph *grKappa_OnAxis = new TGraph("CSV_Files/DUNE_kappa_OnAxis_v2.csv", "%lg %lg", ",");

// ####################################################################
// ### Loading TGraph's from csv files for Off-Axis parametrization ###
// ####################################################################
TGraph *grN1NGamma_OffAxis = new TGraph("CSV_Files/DUNE_N1_over_NGamma_OffAxis.csv", "%lg %lg", ",");
TGraph *grZeta_OffAxis = new TGraph("CSV_Files/DUNE_Eta_OffAxis.csv", "%lg %lg", ",");
TGraph *grMu_OffAxis = new TGraph("CSV_Files/DUNE_mu_OffAxis.csv", "%lg %lg", ",");
TGraph *grN2N1_OffAxis = new TGraph("CSV_Files/DUNE_N2N1_OffAxis.csv", "%lg %lg", ",");
TGraph *grKappa_OffAxis = new TGraph("CSV_Files/DUNE_kappa_OffAxis_v2.csv", "%lg %lg", ",");


// ################################################################################################################
// ### Loading Gaisser-Hillas Functions for the modification of G4 predicition to the pure geometric prediction ###
// ################################################################################################################
TGraph *grGaisser_Hillas_00_10 = new TGraph("CSV_Files/DUNE_0_10degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_10_20 = new TGraph("CSV_Files/DUNE_10_20degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_20_30 = new TGraph("CSV_Files/DUNE_20_30degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_30_40 = new TGraph("CSV_Files/DUNE_30_40degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_40_50 = new TGraph("CSV_Files/DUNE_40_50degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_50_60 = new TGraph("CSV_Files/DUNE_50_60degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_60_70 = new TGraph("CSV_Files/DUNE_60_70degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_70_80 = new TGraph("CSV_Files/DUNE_70_80degree.csv", "%lg %lg", ",");
TGraph *grGaisser_Hillas_80_90 = new TGraph("CSV_Files/DUNE_80_90degree.csv", "%lg %lg", ",");

// This function will take in the number of photons, distance to the pixel,  and the incident angle w.r.t to the pixel
// and return the 5 parameters (N1, Zeta, mu, N2, and Kappa) needed to compute the transport time
std::vector<double> TransportTimeParameters(double Distance, double Angle, int NPhotons)
   {
   
   std::vector<double> Parameters;
   // ### First determine if we are on-axis (< 45 degrees) or off-axis (>45 degrees)
   if(abs(Angle) < 45)
      {
      // --------------------
      // ---- N1/NPhotons ---
      // --------------------
      double N1_NPho;

      // ### Evaluate from the TGraph ### 
      N1_NPho = grN1NGamma_OnAxis->Eval(Distance);
      // ### Multiply by the number of photons ###
      double N1 = N1_NPho * NPhotons;
      // ### Store N1 ###
      Parameters.push_back(N1);
      // ### Fill a histogram to check the parameters ###
      hDis_v_N1->Fill(Distance, N1/NPhotons);
      hDis_v_N1_onAxis->Fill(Distance, N1_NPho);
      
      // --------------------
      // ----    Zeta    ----
      // --------------------
			   
      // ### Evaluate from the TGraph ### 
      double zeta = grZeta_OnAxis->Eval(Distance);
      // ### Store Zeta ###
      Parameters.push_back(zeta); 
      // ### Fill a histogram to check the parameters ###
      hDis_v_zeta->Fill(Distance, zeta);

      // --------------------
      // ----     mu     ----
      // --------------------      

      // ### Evaluate from the TGraph ### 
      double mu = grMu_OnAxis->Eval(Distance);
      // ### Store mu ###
      Parameters.push_back(mu);
      // ### Fill a histogram to check the parameters ###
      hDis_v_mu->Fill(Distance, mu);
      
      //### From zeta, mu, and the distance we can calculate lambda, but I'm not sure we need it ###
      double Lambda = (Distance - mu) / zeta;

      // --------------------
      // ----    N2/N1    ---
      // --------------------  
          
      // ### Evaluate from the TGraph ### 
      double N2_N1 = grN2N1_OnAxis->Eval(Distance);
      // ### Multiply by the N1 ###
      double N2 = N2_N1 * N1;
      // ### Store N2 ###
      Parameters.push_back(N2);
      // ### Fill a histogram to check the parameters ###
      hDis_v_N2->Fill(Distance, N2_N1); 

      // --------------------
      // ----   Kappa     ---
      // --------------------
      
      // ### Evaluate from the TGraph ###
      double kappa = grKappa_OnAxis->Eval(Distance);
      // ### Store Kappa ###
      Parameters.push_back(kappa);
      // ### Fill a histogram to check the parameters ###
      hDis_v_kappa->Fill(Distance, kappa);

      }
   // ### Here we are off axis ###
   if(abs(Angle)>=45)
      {
      // --------------------
      // ---- N1/NPhotons ---
      // --------------------
      double N1_NPho;
      // ### Evaluate from the TGraph ###   
      N1_NPho = grN1NGamma_OffAxis->Eval(Distance);
      // ### Multiply by the number of photons ###
      double N1 = N1_NPho * NPhotons;
      // ### Store N1 ###
      Parameters.push_back(N1);
      // ### Fill a histogram to check the parameters ###
      hDis_v_N1->Fill(Distance, N1/NPhotons);
      hDis_v_N1_offAxis->Fill(Distance, N1_NPho);

      // --------------------
      // ----    Zeta    ----
      // --------------------
			   
      // ### Evaluate from the TGraph ### 		  
      double zeta = grZeta_OffAxis->Eval(Distance);
      // ### Store Zeta ###
      Parameters.push_back(zeta); 
      // ### Fill a histogram to check the parameters ###
      hDis_v_zeta->Fill(Distance, zeta);

      // --------------------
      // ----     mu     ----
      // --------------------      

      // ### Evaluate from the TGraph ###
      double mu = grMu_OffAxis->Eval(Distance);
      // ### Store mu ###
      Parameters.push_back(mu);
      // ### Fill a histogram to check the parameters ###
      hDis_v_mu->Fill(Distance, mu);
      
      //### From zeta, mu, and the distance we can calculate lambda, but I'm not sure we need it ###
      double Lambda = (Distance - mu) / zeta;

      // --------------------
      // ----    N2/N1    ---
      // --------------------      
      // ### Evaluate from the TGraph ###
      double N2_N1 = grN2N1_OffAxis->Eval(Distance);
      // ### Multiply by the N1 ###
      double N2 = N2_N1 * N1;
      // ### Store N2 ###
      Parameters.push_back(N2);
      // ### Fill a histogram to check the parameters ###
      hDis_v_N2->Fill(Distance, N2/N1); 

      // --------------------
      // ----   Kappa     ---
      // --------------------      
      // ### Evaluate from the TGraph ###
      double kappa = grKappa_OffAxis->Eval(Distance);
      // ### Store Kappa ###
      Parameters.push_back(kappa);
      // ### Fill a histogram to check the parameters ###
      hDis_v_kappa->Fill(Distance, kappa);
      }
   

   return Parameters;
   }//<---End Transport Time Parameters Function

// ================================================================================================
// ============				Decalare Histograms Here 			===========
// ================================================================================================
TH2D *hEnergyDepositedVSPhotons = new TH2D("hEnergyDepositedVSPhotons", "Energy Deposited vs # of Photons", 40, 0, 6000, 40, 0, 2E8);
TH1D *hEnergyDeposited = new TH1D("hEnergyDeposited", "Energy Deposited", 50, 0, 6000);
TH1D *hNumOfPhotons = new TH1D("hNumOfPhotons", "Number of Photons", 50, 0, 2E8);
TH1D *hTransportTime = new TH1D("hTransportTime", "Transport Time", 1000, -0.02, 0.02);
TH1D *hDepositionTime = new TH1D("hDepositionTime", "Transport Time", 1000, 0, 2000);
TH1D *hTotalTime = new TH1D("hTotalTime", "Total Time", 1000, 0, 2000);

TH1D *hXPos = new TH1D("hXPos", "X Position", 350, 0, 350);
TH1D *hYPos = new TH1D("hYPos", "Y Position", 600, 0, 600);
TH1D *hZPos = new TH1D("hZPos", "Z Position", 600, 0, 600);

TH1D *hXPosRaw = new TH1D("hXPosRaw", "X Position", 350, 0, 350);
TH1D *hYPosRaw = new TH1D("hYPosRaw", "Y Position", 600, 0, 600);
TH1D *hZPosRaw = new TH1D("hZPosRaw", "Z Position", 600, 0, 600);

TH2D *hyz = new TH2D("hyz", "hyz", 600, 0, 600, 600, 0, 600);
TH2D *hxy = new TH2D("hxy", "hxy", 350, 0, 350, 600, 0, 600);
TH2D *hxz = new TH2D("hxz", "hxz", 350, 0, 350, 600, 0, 600);

TH2D *hyizi = new TH2D("hyizi", "hyizi", 200, 0, 200, 200, 0, 200);
TH2D *hxiyi = new TH2D("hxiyi", "hxiyi", 350, 0, 350, 200, 0, 200);
TH2D *hxizi = new TH2D("hxizi", "hxizi", 350, 0, 350, 200, 0, 200);

TH2D *YprimaryZPrimary = new TH2D("YprimaryZPrimary", "YprimaryZPrimary", 200, 0, 200, 200, 0, 200);
TH2D *XprimaryYPrimary = new TH2D("XprimaryYPrimary", "XprimaryYPrimary", 350, 0, 350, 200, 0, 200);
TH2D *XprimaryZPrimary = new TH2D("XprimaryZPrimary", "XprimaryZPrimary", 350, 0, 350, 200, 0, 200); 
// ================================================================================================  
// ================================================================================================

// ############ Time Sliced Histograms ######################
TH2D *hPixelMap_000ns_010ns = new TH2D("hPixelMap_000ns_010ns", "Y-Z Pixel Map 0ns to 10ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_010ns_020ns = new TH2D("hPixelMap_010ns_020ns", "Y-Z Pixel Map 10ns to 20ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_020ns_030ns = new TH2D("hPixelMap_020ns_030ns", "Y-Z Pixel Map 20ns to 30ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_030ns_040ns = new TH2D("hPixelMap_030ns_040ns", "Y-Z Pixel Map 30ns to 40ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_040ns_050ns = new TH2D("hPixelMap_040ns_050ns", "Y-Z Pixel Map 40ns to 50ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_050ns_060ns = new TH2D("hPixelMap_050ns_060ns", "Y-Z Pixel Map 50ns to 60ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_060ns_070ns = new TH2D("hPixelMap_060ns_070ns", "Y-Z Pixel Map 60ns to 70ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_070ns_080ns = new TH2D("hPixelMap_070ns_080ns", "Y-Z Pixel Map 70ns to 80ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_080ns_090ns = new TH2D("hPixelMap_080ns_090ns", "Y-Z Pixel Map 80ns to 90ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_090ns_100ns = new TH2D("hPixelMap_090ns_100ns", "Y-Z Pixel Map 90ns to 100ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_100ns_110ns = new TH2D("hPixelMap_100ns_110ns", "Y-Z Pixel Map 100ns to 110ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_110ns_120ns = new TH2D("hPixelMap_110ns_120ns", "Y-Z Pixel Map 110ns to 120ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_120ns_130ns = new TH2D("hPixelMap_120ns_130ns", "Y-Z Pixel Map 120ns to 130ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_130ns_140ns = new TH2D("hPixelMap_130ns_140ns", "Y-Z Pixel Map 130ns to 140ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_140ns_150ns = new TH2D("hPixelMap_140ns_150ns", "Y-Z Pixel Map 140ns to 150ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_150ns_160ns = new TH2D("hPixelMap_150ns_160ns", "Y-Z Pixel Map 150ns to 160ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_160ns_170ns = new TH2D("hPixelMap_160ns_170ns", "Y-Z Pixel Map 160ns to 170ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_170ns_180ns = new TH2D("hPixelMap_170ns_180ns", "Y-Z Pixel Map 170ns to 180ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_180ns_190ns = new TH2D("hPixelMap_180ns_190ns", "Y-Z Pixel Map 180ns to 190ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_190ns_200ns = new TH2D("hPixelMap_190ns_200ns", "Y-Z Pixel Map 190ns to 200ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);



///////////////////////////////////////////////////////////////////////
TH2D *hPixelMap_000ns_001ns = new TH2D("hPixelMap_000ns_001ns", "Y-Z Pixel Map 0ns to 1ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_001ns_002ns = new TH2D("hPixelMap_001ns_002ns", "Y-Z Pixel Map 1ns to 2ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_002ns_003ns = new TH2D("hPixelMap_002ns_003ns", "Y-Z Pixel Map 2ns to 3ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_003ns_004ns = new TH2D("hPixelMap_003ns_004ns", "Y-Z Pixel Map 3ns to 4ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_004ns_005ns = new TH2D("hPixelMap_004ns_005ns", "Y-Z Pixel Map 4ns to 5ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_005ns_006ns = new TH2D("hPixelMap_005ns_006ns", "Y-Z Pixel Map 5ns to 6ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_006ns_007ns = new TH2D("hPixelMap_006ns_007ns", "Y-Z Pixel Map 6ns to 7ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_007ns_008ns = new TH2D("hPixelMap_007ns_008ns", "Y-Z Pixel Map 7ns to 8ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_008ns_009ns = new TH2D("hPixelMap_008ns_009ns", "Y-Z Pixel Map 8ns to 9ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_009ns_010ns = new TH2D("hPixelMap_009ns_010ns", "Y-Z Pixel Map 9ns to 10ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_010ns_011ns = new TH2D("hPixelMap_010ns_011ns", "Y-Z Pixel Map 10ns to 11ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_011ns_012ns = new TH2D("hPixelMap_011ns_012ns", "Y-Z Pixel Map 11ns to 12ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_012ns_013ns = new TH2D("hPixelMap_012ns_013ns", "Y-Z Pixel Map 12ns to 13ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_013ns_014ns = new TH2D("hPixelMap_013ns_014ns", "Y-Z Pixel Map 13ns to 14ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_014ns_015ns = new TH2D("hPixelMap_014ns_015ns", "Y-Z Pixel Map 14ns to 15ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_015ns_016ns = new TH2D("hPixelMap_015ns_016ns", "Y-Z Pixel Map 15ns to 16ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_016ns_017ns = new TH2D("hPixelMap_016ns_017ns", "Y-Z Pixel Map 16ns to 17ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_017ns_018ns = new TH2D("hPixelMap_017ns_018ns", "Y-Z Pixel Map 17ns to 18ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_018ns_019ns = new TH2D("hPixelMap_018ns_019ns", "Y-Z Pixel Map 18ns to 19ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_019ns_020ns = new TH2D("hPixelMap_019ns_020ns", "Y-Z Pixel Map 19ns to 20ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_020ns_021ns = new TH2D("hPixelMap_020ns_021ns", "Y-Z Pixel Map 20ns to 21ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_021ns_022ns = new TH2D("hPixelMap_021ns_022ns", "Y-Z Pixel Map 21ns to 22ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_022ns_023ns = new TH2D("hPixelMap_022ns_023ns", "Y-Z Pixel Map 22ns to 23ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_023ns_024ns = new TH2D("hPixelMap_023ns_024ns", "Y-Z Pixel Map 23ns to 24ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_024ns_025ns = new TH2D("hPixelMap_024ns_025ns", "Y-Z Pixel Map 24ns to 25ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_025ns_026ns = new TH2D("hPixelMap_025ns_026ns", "Y-Z Pixel Map 25ns to 26ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_026ns_027ns = new TH2D("hPixelMap_026ns_027ns", "Y-Z Pixel Map 26ns to 27ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_027ns_028ns = new TH2D("hPixelMap_027ns_028ns", "Y-Z Pixel Map 27ns to 28ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_028ns_029ns = new TH2D("hPixelMap_028ns_029ns", "Y-Z Pixel Map 28ns to 29ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_029ns_030ns = new TH2D("hPixelMap_029ns_030ns", "Y-Z Pixel Map 29ns to 30ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_030ns_031ns = new TH2D("hPixelMap_030ns_031ns", "Y-Z Pixel Map 30ns to 31ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_031ns_032ns = new TH2D("hPixelMap_031ns_032ns", "Y-Z Pixel Map 31ns to 32ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_032ns_033ns = new TH2D("hPixelMap_032ns_033ns", "Y-Z Pixel Map 32ns to 33ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_033ns_034ns = new TH2D("hPixelMap_033ns_034ns", "Y-Z Pixel Map 33ns to 34ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_034ns_035ns = new TH2D("hPixelMap_034ns_035ns", "Y-Z Pixel Map 34ns to 35ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_035ns_036ns = new TH2D("hPixelMap_035ns_036ns", "Y-Z Pixel Map 35ns to 36ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_036ns_037ns = new TH2D("hPixelMap_036ns_037ns", "Y-Z Pixel Map 36ns to 37ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_037ns_038ns = new TH2D("hPixelMap_037ns_038ns", "Y-Z Pixel Map 37ns to 38ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_038ns_039ns = new TH2D("hPixelMap_038ns_039ns", "Y-Z Pixel Map 38ns to 39ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_039ns_040ns = new TH2D("hPixelMap_039ns_040ns", "Y-Z Pixel Map 39ns to 40ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_040ns_041ns = new TH2D("hPixelMap_040ns_041ns", "Y-Z Pixel Map 40ns to 41ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_041ns_042ns = new TH2D("hPixelMap_041ns_042ns", "Y-Z Pixel Map 41ns to 42ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_042ns_043ns = new TH2D("hPixelMap_042ns_043ns", "Y-Z Pixel Map 42ns to 43ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_043ns_044ns = new TH2D("hPixelMap_043ns_044ns", "Y-Z Pixel Map 43ns to 44ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_044ns_045ns = new TH2D("hPixelMap_044ns_045ns", "Y-Z Pixel Map 44ns to 45ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_045ns_046ns = new TH2D("hPixelMap_045ns_046ns", "Y-Z Pixel Map 45ns to 46ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_046ns_047ns = new TH2D("hPixelMap_046ns_047ns", "Y-Z Pixel Map 46ns to 47ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_047ns_048ns = new TH2D("hPixelMap_047ns_048ns", "Y-Z Pixel Map 47ns to 48ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_048ns_049ns = new TH2D("hPixelMap_048ns_049ns", "Y-Z Pixel Map 48ns to 49ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_049ns_050ns = new TH2D("hPixelMap_049ns_050ns", "Y-Z Pixel Map 49ns to 50ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_050ns_051ns = new TH2D("hPixelMap_050ns_051ns", "Y-Z Pixel Map 50ns to 51ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_051ns_052ns = new TH2D("hPixelMap_051ns_052ns", "Y-Z Pixel Map 51ns to 52ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_052ns_053ns = new TH2D("hPixelMap_052ns_053ns", "Y-Z Pixel Map 52ns to 53ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_053ns_054ns = new TH2D("hPixelMap_053ns_054ns", "Y-Z Pixel Map 53ns to 54ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_054ns_055ns = new TH2D("hPixelMap_054ns_055ns", "Y-Z Pixel Map 54ns to 55ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_055ns_056ns = new TH2D("hPixelMap_055ns_056ns", "Y-Z Pixel Map 55ns to 56ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_056ns_057ns = new TH2D("hPixelMap_056ns_057ns", "Y-Z Pixel Map 56ns to 57ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_057ns_058ns = new TH2D("hPixelMap_057ns_058ns", "Y-Z Pixel Map 57ns to 58ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_058ns_059ns = new TH2D("hPixelMap_058ns_059ns", "Y-Z Pixel Map 58ns to 59ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_059ns_060ns = new TH2D("hPixelMap_059ns_060ns", "Y-Z Pixel Map 59ns to 60ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_060ns_061ns = new TH2D("hPixelMap_060ns_061ns", "Y-Z Pixel Map 60ns to 61ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_061ns_062ns = new TH2D("hPixelMap_061ns_062ns", "Y-Z Pixel Map 61ns to 62ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_062ns_063ns = new TH2D("hPixelMap_062ns_063ns", "Y-Z Pixel Map 62ns to 63ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_063ns_064ns = new TH2D("hPixelMap_063ns_064ns", "Y-Z Pixel Map 63ns to 64ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_064ns_065ns = new TH2D("hPixelMap_064ns_065ns", "Y-Z Pixel Map 64ns to 65ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_065ns_066ns = new TH2D("hPixelMap_065ns_066ns", "Y-Z Pixel Map 65ns to 66ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_066ns_067ns = new TH2D("hPixelMap_066ns_067ns", "Y-Z Pixel Map 66ns to 67ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_067ns_068ns = new TH2D("hPixelMap_067ns_068ns", "Y-Z Pixel Map 67ns to 68ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_068ns_069ns = new TH2D("hPixelMap_068ns_069ns", "Y-Z Pixel Map 68ns to 69ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_069ns_070ns = new TH2D("hPixelMap_069ns_070ns", "Y-Z Pixel Map 69ns to 70ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_070ns_071ns = new TH2D("hPixelMap_070ns_071ns", "Y-Z Pixel Map 70ns to 71ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_071ns_072ns = new TH2D("hPixelMap_071ns_072ns", "Y-Z Pixel Map 71ns to 72ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_072ns_073ns = new TH2D("hPixelMap_072ns_073ns", "Y-Z Pixel Map 72ns to 73ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_073ns_074ns = new TH2D("hPixelMap_073ns_074ns", "Y-Z Pixel Map 73ns to 74ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_074ns_075ns = new TH2D("hPixelMap_074ns_075ns", "Y-Z Pixel Map 74ns to 75ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_075ns_076ns = new TH2D("hPixelMap_075ns_076ns", "Y-Z Pixel Map 75ns to 76ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_076ns_077ns = new TH2D("hPixelMap_076ns_077ns", "Y-Z Pixel Map 76ns to 77ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_077ns_078ns = new TH2D("hPixelMap_077ns_078ns", "Y-Z Pixel Map 77ns to 78ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_078ns_079ns = new TH2D("hPixelMap_078ns_079ns", "Y-Z Pixel Map 78ns to 79ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_079ns_080ns = new TH2D("hPixelMap_079ns_080ns", "Y-Z Pixel Map 79ns to 80ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_080ns_081ns = new TH2D("hPixelMap_080ns_081ns", "Y-Z Pixel Map 80ns to 81ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_081ns_082ns = new TH2D("hPixelMap_081ns_082ns", "Y-Z Pixel Map 81ns to 82ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_082ns_083ns = new TH2D("hPixelMap_082ns_083ns", "Y-Z Pixel Map 82ns to 83ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_083ns_084ns = new TH2D("hPixelMap_083ns_084ns", "Y-Z Pixel Map 83ns to 84ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_084ns_085ns = new TH2D("hPixelMap_084ns_085ns", "Y-Z Pixel Map 84ns to 85ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_085ns_086ns = new TH2D("hPixelMap_085ns_086ns", "Y-Z Pixel Map 85ns to 86ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_086ns_087ns = new TH2D("hPixelMap_086ns_087ns", "Y-Z Pixel Map 86ns to 87ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_087ns_088ns = new TH2D("hPixelMap_087ns_088ns", "Y-Z Pixel Map 87ns to 88ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_088ns_089ns = new TH2D("hPixelMap_088ns_089ns", "Y-Z Pixel Map 88ns to 89ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_089ns_090ns = new TH2D("hPixelMap_089ns_090ns", "Y-Z Pixel Map 89ns to 90ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_090ns_091ns = new TH2D("hPixelMap_090ns_091ns", "Y-Z Pixel Map 90ns to 91ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_091ns_092ns = new TH2D("hPixelMap_091ns_092ns", "Y-Z Pixel Map 91ns to 92ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_092ns_093ns = new TH2D("hPixelMap_092ns_093ns", "Y-Z Pixel Map 92ns to 93ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_093ns_094ns = new TH2D("hPixelMap_093ns_094ns", "Y-Z Pixel Map 93ns to 94ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_094ns_095ns = new TH2D("hPixelMap_094ns_095ns", "Y-Z Pixel Map 94ns to 95ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_095ns_096ns = new TH2D("hPixelMap_095ns_096ns", "Y-Z Pixel Map 95ns to 96ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_096ns_097ns = new TH2D("hPixelMap_096ns_097ns", "Y-Z Pixel Map 96ns to 97ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_097ns_098ns = new TH2D("hPixelMap_097ns_098ns", "Y-Z Pixel Map 97ns to 98ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_098ns_099ns = new TH2D("hPixelMap_098ns_099ns", "Y-Z Pixel Map 98ns to 99ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_099ns_100ns = new TH2D("hPixelMap_099ns_100ns", "Y-Z Pixel Map 99ns to 100ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_100ns_101ns = new TH2D("hPixelMap_100ns_101ns", "Y-Z Pixel Map 100ns to 101ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_101ns_102ns = new TH2D("hPixelMap_101ns_102ns", "Y-Z Pixel Map 101ns to 102ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_102ns_103ns = new TH2D("hPixelMap_102ns_103ns", "Y-Z Pixel Map 102ns to 103ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_103ns_104ns = new TH2D("hPixelMap_103ns_104ns", "Y-Z Pixel Map 103ns to 104ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_104ns_105ns = new TH2D("hPixelMap_104ns_105ns", "Y-Z Pixel Map 104ns to 105ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_105ns_106ns = new TH2D("hPixelMap_105ns_106ns", "Y-Z Pixel Map 105ns to 106ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_106ns_107ns = new TH2D("hPixelMap_106ns_107ns", "Y-Z Pixel Map 106ns to 107ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_107ns_108ns = new TH2D("hPixelMap_107ns_108ns", "Y-Z Pixel Map 107ns to 108ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_108ns_109ns = new TH2D("hPixelMap_108ns_109ns", "Y-Z Pixel Map 108ns to 109ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_109ns_110ns = new TH2D("hPixelMap_109ns_110ns", "Y-Z Pixel Map 109ns to 110ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_110ns_111ns = new TH2D("hPixelMap_110ns_111ns", "Y-Z Pixel Map 110ns to 111ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_111ns_112ns = new TH2D("hPixelMap_111ns_112ns", "Y-Z Pixel Map 111ns to 112ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_112ns_113ns = new TH2D("hPixelMap_112ns_113ns", "Y-Z Pixel Map 112ns to 113ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_113ns_114ns = new TH2D("hPixelMap_113ns_114ns", "Y-Z Pixel Map 113ns to 114ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_114ns_115ns = new TH2D("hPixelMap_114ns_115ns", "Y-Z Pixel Map 114ns to 115ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_115ns_116ns = new TH2D("hPixelMap_115ns_116ns", "Y-Z Pixel Map 115ns to 116ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_116ns_117ns = new TH2D("hPixelMap_116ns_117ns", "Y-Z Pixel Map 116ns to 117ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_117ns_118ns = new TH2D("hPixelMap_117ns_118ns", "Y-Z Pixel Map 117ns to 118ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_118ns_119ns = new TH2D("hPixelMap_118ns_119ns", "Y-Z Pixel Map 118ns to 119ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_119ns_120ns = new TH2D("hPixelMap_119ns_120ns", "Y-Z Pixel Map 119ns to 120ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_120ns_121ns = new TH2D("hPixelMap_120ns_121ns", "Y-Z Pixel Map 120ns to 121ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_121ns_122ns = new TH2D("hPixelMap_121ns_122ns", "Y-Z Pixel Map 121ns to 122ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_122ns_123ns = new TH2D("hPixelMap_122ns_123ns", "Y-Z Pixel Map 122ns to 123ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_123ns_124ns = new TH2D("hPixelMap_123ns_124ns", "Y-Z Pixel Map 123ns to 124ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_124ns_125ns = new TH2D("hPixelMap_124ns_125ns", "Y-Z Pixel Map 124ns to 125ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_125ns_126ns = new TH2D("hPixelMap_125ns_126ns", "Y-Z Pixel Map 125ns to 126ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_126ns_127ns = new TH2D("hPixelMap_126ns_127ns", "Y-Z Pixel Map 126ns to 127ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_127ns_128ns = new TH2D("hPixelMap_127ns_128ns", "Y-Z Pixel Map 127ns to 128ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_128ns_129ns = new TH2D("hPixelMap_128ns_129ns", "Y-Z Pixel Map 128ns to 129ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_129ns_130ns = new TH2D("hPixelMap_129ns_130ns", "Y-Z Pixel Map 129ns to 130ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_130ns_131ns = new TH2D("hPixelMap_130ns_131ns", "Y-Z Pixel Map 130ns to 131ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_131ns_132ns = new TH2D("hPixelMap_131ns_132ns", "Y-Z Pixel Map 131ns to 132ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_132ns_133ns = new TH2D("hPixelMap_132ns_133ns", "Y-Z Pixel Map 132ns to 133ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_133ns_134ns = new TH2D("hPixelMap_133ns_134ns", "Y-Z Pixel Map 133ns to 134ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_134ns_135ns = new TH2D("hPixelMap_134ns_135ns", "Y-Z Pixel Map 134ns to 135ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_135ns_136ns = new TH2D("hPixelMap_135ns_136ns", "Y-Z Pixel Map 135ns to 136ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_136ns_137ns = new TH2D("hPixelMap_136ns_137ns", "Y-Z Pixel Map 136ns to 137ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_137ns_138ns = new TH2D("hPixelMap_137ns_138ns", "Y-Z Pixel Map 137ns to 138ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_138ns_139ns = new TH2D("hPixelMap_138ns_139ns", "Y-Z Pixel Map 138ns to 139ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_139ns_140ns = new TH2D("hPixelMap_139ns_140ns", "Y-Z Pixel Map 139ns to 140ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_140ns_141ns = new TH2D("hPixelMap_140ns_141ns", "Y-Z Pixel Map 140ns to 141ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_141ns_142ns = new TH2D("hPixelMap_141ns_142ns", "Y-Z Pixel Map 141ns to 142ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_142ns_143ns = new TH2D("hPixelMap_142ns_143ns", "Y-Z Pixel Map 142ns to 143ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_143ns_144ns = new TH2D("hPixelMap_143ns_144ns", "Y-Z Pixel Map 143ns to 144ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_144ns_145ns = new TH2D("hPixelMap_144ns_145ns", "Y-Z Pixel Map 144ns to 145ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_145ns_146ns = new TH2D("hPixelMap_145ns_146ns", "Y-Z Pixel Map 145ns to 146ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_146ns_147ns = new TH2D("hPixelMap_146ns_147ns", "Y-Z Pixel Map 146ns to 147ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_147ns_148ns = new TH2D("hPixelMap_147ns_148ns", "Y-Z Pixel Map 147ns to 148ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_148ns_149ns = new TH2D("hPixelMap_148ns_149ns", "Y-Z Pixel Map 148ns to 149ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_149ns_150ns = new TH2D("hPixelMap_149ns_150ns", "Y-Z Pixel Map 149ns to 150ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_150ns_151ns = new TH2D("hPixelMap_150ns_151ns", "Y-Z Pixel Map 150ns to 151ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_151ns_152ns = new TH2D("hPixelMap_151ns_152ns", "Y-Z Pixel Map 151ns to 152ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_152ns_153ns = new TH2D("hPixelMap_152ns_153ns", "Y-Z Pixel Map 152ns to 153ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_153ns_154ns = new TH2D("hPixelMap_153ns_154ns", "Y-Z Pixel Map 153ns to 154ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_154ns_155ns = new TH2D("hPixelMap_154ns_155ns", "Y-Z Pixel Map 154ns to 155ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_155ns_156ns = new TH2D("hPixelMap_155ns_156ns", "Y-Z Pixel Map 155ns to 156ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_156ns_157ns = new TH2D("hPixelMap_156ns_157ns", "Y-Z Pixel Map 156ns to 157ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_157ns_158ns = new TH2D("hPixelMap_157ns_158ns", "Y-Z Pixel Map 157ns to 158ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_158ns_159ns = new TH2D("hPixelMap_158ns_159ns", "Y-Z Pixel Map 158ns to 159ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_159ns_160ns = new TH2D("hPixelMap_159ns_160ns", "Y-Z Pixel Map 159ns to 160ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_160ns_161ns = new TH2D("hPixelMap_160ns_161ns", "Y-Z Pixel Map 160ns to 161ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_161ns_162ns = new TH2D("hPixelMap_161ns_162ns", "Y-Z Pixel Map 161ns to 162ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_162ns_163ns = new TH2D("hPixelMap_162ns_163ns", "Y-Z Pixel Map 162ns to 163ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_163ns_164ns = new TH2D("hPixelMap_163ns_164ns", "Y-Z Pixel Map 163ns to 164ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_164ns_165ns = new TH2D("hPixelMap_164ns_165ns", "Y-Z Pixel Map 164ns to 165ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_165ns_166ns = new TH2D("hPixelMap_165ns_166ns", "Y-Z Pixel Map 165ns to 166ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_166ns_167ns = new TH2D("hPixelMap_166ns_167ns", "Y-Z Pixel Map 166ns to 167ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_167ns_168ns = new TH2D("hPixelMap_167ns_168ns", "Y-Z Pixel Map 167ns to 168ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_168ns_169ns = new TH2D("hPixelMap_168ns_169ns", "Y-Z Pixel Map 168ns to 169ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_169ns_170ns = new TH2D("hPixelMap_169ns_170ns", "Y-Z Pixel Map 169ns to 170ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_170ns_171ns = new TH2D("hPixelMap_170ns_171ns", "Y-Z Pixel Map 170ns to 171ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_171ns_172ns = new TH2D("hPixelMap_171ns_172ns", "Y-Z Pixel Map 171ns to 172ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_172ns_173ns = new TH2D("hPixelMap_172ns_173ns", "Y-Z Pixel Map 172ns to 173ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_173ns_174ns = new TH2D("hPixelMap_173ns_174ns", "Y-Z Pixel Map 173ns to 174ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_174ns_175ns = new TH2D("hPixelMap_174ns_175ns", "Y-Z Pixel Map 174ns to 175ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_175ns_176ns = new TH2D("hPixelMap_175ns_176ns", "Y-Z Pixel Map 175ns to 176ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_176ns_177ns = new TH2D("hPixelMap_176ns_177ns", "Y-Z Pixel Map 176ns to 177ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_177ns_178ns = new TH2D("hPixelMap_177ns_178ns", "Y-Z Pixel Map 177ns to 178ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_178ns_179ns = new TH2D("hPixelMap_178ns_179ns", "Y-Z Pixel Map 178ns to 179ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_179ns_180ns = new TH2D("hPixelMap_179ns_180ns", "Y-Z Pixel Map 179ns to 180ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_180ns_181ns = new TH2D("hPixelMap_180ns_181ns", "Y-Z Pixel Map 180ns to 181ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_181ns_182ns = new TH2D("hPixelMap_181ns_182ns", "Y-Z Pixel Map 181ns to 182ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_182ns_183ns = new TH2D("hPixelMap_182ns_183ns", "Y-Z Pixel Map 182ns to 183ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_183ns_184ns = new TH2D("hPixelMap_183ns_184ns", "Y-Z Pixel Map 183ns to 184ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_184ns_185ns = new TH2D("hPixelMap_184ns_185ns", "Y-Z Pixel Map 184ns to 185ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_185ns_186ns = new TH2D("hPixelMap_185ns_186ns", "Y-Z Pixel Map 185ns to 186ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_186ns_187ns = new TH2D("hPixelMap_186ns_187ns", "Y-Z Pixel Map 186ns to 187ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_187ns_188ns = new TH2D("hPixelMap_187ns_188ns", "Y-Z Pixel Map 187ns to 188ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_188ns_189ns = new TH2D("hPixelMap_188ns_189ns", "Y-Z Pixel Map 188ns to 189ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_189ns_190ns = new TH2D("hPixelMap_189ns_190ns", "Y-Z Pixel Map 189ns to 190ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_190ns_191ns = new TH2D("hPixelMap_190ns_191ns", "Y-Z Pixel Map 190ns to 191ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_191ns_192ns = new TH2D("hPixelMap_191ns_192ns", "Y-Z Pixel Map 191ns to 192ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_192ns_193ns = new TH2D("hPixelMap_192ns_193ns", "Y-Z Pixel Map 192ns to 193ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_193ns_194ns = new TH2D("hPixelMap_193ns_194ns", "Y-Z Pixel Map 193ns to 194ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_194ns_195ns = new TH2D("hPixelMap_194ns_195ns", "Y-Z Pixel Map 194ns to 195ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_195ns_196ns = new TH2D("hPixelMap_195ns_196ns", "Y-Z Pixel Map 195ns to 196ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_196ns_197ns = new TH2D("hPixelMap_196ns_197ns", "Y-Z Pixel Map 196ns to 197ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_197ns_198ns = new TH2D("hPixelMap_197ns_198ns", "Y-Z Pixel Map 197ns to 198ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_198ns_199ns = new TH2D("hPixelMap_198ns_199ns", "Y-Z Pixel Map 198ns to 199ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);
TH2D *hPixelMap_199ns_200ns = new TH2D("hPixelMap_199ns_200ns", "Y-Z Pixel Map 199ns to 200ns", nYPixels, 0., nYPixels, nZPixels, 0., nZPixels);





// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ### This function writes out ROOT file for a given event ###
void WriteFile(int eventNumber)
{
// ===============================================
// ======  Make histogram file for sample  ======= 

// ### Construct this files name ###
std::string Histo = "./SemiAnalytic-Histos/Histos_Event";
std::string EVNum = to_string(eventNumber);
std::string file  = ".root";
std::string str;
str = Histo+EVNum+file;

// ### Name the file ###
TFile myfile2(str.c_str(),"RECREATE");

// ### Write all the histograms ###
hEnergyDepositedVSPhotons->Write();
hEnergyDeposited->Write();
hNumOfPhotons->Write();
hTransportTime->Write();
hDepositionTime->Write();
hTotalTime->Write();
hDis_v_N1->Write();
hDis_v_N1_onAxis->Write();
hDis_v_N1_offAxis->Write();
hDis_v_zeta->Write();
hDis_v_mu->Write();
hDis_v_N2->Write();
hDis_v_kappa->Write();
hXPos->Write();
hYPos->Write();
hZPos->Write();
hXPosRaw->Write();
hYPosRaw->Write();
hZPosRaw->Write();
hyz->Write();
hxz->Write();
hxy->Write();
hyizi->Write();
hxiyi->Write();
hxizi->Write();
YprimaryZPrimary->Write();
XprimaryYPrimary->Write();
XprimaryZPrimary->Write();
// ### Close the file ###
myfile2.Close();

// ### Reset all the histograms ###
hEnergyDepositedVSPhotons->Reset("ICESM");
hEnergyDeposited->Reset("ICESM");
hNumOfPhotons->Reset("ICESM");
hTransportTime->Reset("ICESM");
hDepositionTime->Reset("ICESM");
hTotalTime->Reset("ICESM");
hDis_v_N1->Reset("ICESM");
hDis_v_N1_onAxis->Reset("ICESM");
hDis_v_N1_offAxis->Reset("ICESM");
hDis_v_zeta->Reset("ICESM");
hDis_v_mu->Reset("ICESM");
hDis_v_N2->Reset("ICESM");
hDis_v_kappa->Reset("ICESM");
hXPos->Reset("ICESM");
hYPos->Reset("ICESM");
hZPos->Reset("ICESM");
hXPosRaw->Reset("ICESM");
hYPosRaw->Reset("ICESM");
hZPosRaw->Reset("ICESM");
hyz->Reset("ICESM");
hxz->Reset("ICESM");
hxy->Reset("ICESM");
hyizi->Reset("ICESM");
hxiyi->Reset("ICESM");
hxizi->Reset("ICESM");
YprimaryZPrimary->Reset("ICESM");
XprimaryYPrimary->Reset("ICESM");
XprimaryZPrimary->Reset("ICESM");


}
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------



// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ### This function writes out ROOT file for a given event for the time sliced histos ###
void WriteFileTimeSlices(int EvNum)
{
std::string Files = "./SemiAnalytic-ROOTFiles/SemiAnalytic-KaonEvents_Event";
std::string Enum = to_string(EvNum);
std::string Ending = "_Rebin.root";

std::string FullStr;

FullStr = Files+Enum+Ending;

TFile myfile(FullStr.c_str(),"RECREATE");

hPixelMap_000ns_010ns->Write();
hPixelMap_010ns_020ns->Write();
hPixelMap_020ns_030ns->Write();
hPixelMap_030ns_040ns->Write();
hPixelMap_040ns_050ns->Write();
hPixelMap_050ns_060ns->Write();
hPixelMap_060ns_070ns->Write();
hPixelMap_070ns_080ns->Write();
hPixelMap_080ns_090ns->Write();
hPixelMap_090ns_100ns->Write();
hPixelMap_100ns_110ns->Write();
hPixelMap_110ns_120ns->Write();
hPixelMap_120ns_130ns->Write();
hPixelMap_130ns_140ns->Write();
hPixelMap_140ns_150ns->Write();
hPixelMap_150ns_160ns->Write();
hPixelMap_160ns_170ns->Write();
hPixelMap_170ns_180ns->Write();
hPixelMap_180ns_190ns->Write();
hPixelMap_190ns_200ns->Write();

hPixelMap_000ns_001ns->Write();
hPixelMap_001ns_002ns->Write();
hPixelMap_002ns_003ns->Write();
hPixelMap_003ns_004ns->Write();
hPixelMap_004ns_005ns->Write();
hPixelMap_005ns_006ns->Write();
hPixelMap_006ns_007ns->Write();
hPixelMap_007ns_008ns->Write();
hPixelMap_008ns_009ns->Write();
hPixelMap_009ns_010ns->Write();
hPixelMap_010ns_011ns->Write();
hPixelMap_011ns_012ns->Write();
hPixelMap_012ns_013ns->Write();
hPixelMap_013ns_014ns->Write();
hPixelMap_014ns_015ns->Write();
hPixelMap_015ns_016ns->Write();
hPixelMap_016ns_017ns->Write();
hPixelMap_017ns_018ns->Write();
hPixelMap_018ns_019ns->Write();
hPixelMap_019ns_020ns->Write();
hPixelMap_020ns_021ns->Write();
hPixelMap_021ns_022ns->Write();
hPixelMap_022ns_023ns->Write();
hPixelMap_023ns_024ns->Write();
hPixelMap_024ns_025ns->Write();
hPixelMap_025ns_026ns->Write();
hPixelMap_026ns_027ns->Write();
hPixelMap_027ns_028ns->Write();
hPixelMap_028ns_029ns->Write();
hPixelMap_029ns_030ns->Write();
hPixelMap_030ns_031ns->Write();
hPixelMap_031ns_032ns->Write();
hPixelMap_032ns_033ns->Write();
hPixelMap_033ns_034ns->Write();
hPixelMap_034ns_035ns->Write();
hPixelMap_035ns_036ns->Write();
hPixelMap_036ns_037ns->Write();
hPixelMap_037ns_038ns->Write();
hPixelMap_038ns_039ns->Write();
hPixelMap_039ns_040ns->Write();
hPixelMap_040ns_041ns->Write();
hPixelMap_041ns_042ns->Write();
hPixelMap_042ns_043ns->Write();
hPixelMap_043ns_044ns->Write();
hPixelMap_044ns_045ns->Write();
hPixelMap_045ns_046ns->Write();
hPixelMap_046ns_047ns->Write();
hPixelMap_047ns_048ns->Write();
hPixelMap_048ns_049ns->Write();
hPixelMap_049ns_050ns->Write();
hPixelMap_050ns_051ns->Write();
hPixelMap_051ns_052ns->Write();
hPixelMap_052ns_053ns->Write();
hPixelMap_053ns_054ns->Write();
hPixelMap_054ns_055ns->Write();
hPixelMap_055ns_056ns->Write();
hPixelMap_056ns_057ns->Write();
hPixelMap_057ns_058ns->Write();
hPixelMap_058ns_059ns->Write();
hPixelMap_059ns_060ns->Write();
hPixelMap_060ns_061ns->Write();
hPixelMap_061ns_062ns->Write();
hPixelMap_062ns_063ns->Write();
hPixelMap_063ns_064ns->Write();
hPixelMap_064ns_065ns->Write();
hPixelMap_065ns_066ns->Write();
hPixelMap_066ns_067ns->Write();
hPixelMap_067ns_068ns->Write();
hPixelMap_068ns_069ns->Write();
hPixelMap_069ns_070ns->Write();
hPixelMap_070ns_071ns->Write();
hPixelMap_071ns_072ns->Write();
hPixelMap_072ns_073ns->Write();
hPixelMap_073ns_074ns->Write();
hPixelMap_074ns_075ns->Write();
hPixelMap_075ns_076ns->Write();
hPixelMap_076ns_077ns->Write();
hPixelMap_077ns_078ns->Write();
hPixelMap_078ns_079ns->Write();
hPixelMap_079ns_080ns->Write();
hPixelMap_080ns_081ns->Write();
hPixelMap_081ns_082ns->Write();
hPixelMap_082ns_083ns->Write();
hPixelMap_083ns_084ns->Write();
hPixelMap_084ns_085ns->Write();
hPixelMap_085ns_086ns->Write();
hPixelMap_086ns_087ns->Write();
hPixelMap_087ns_088ns->Write();
hPixelMap_088ns_089ns->Write();
hPixelMap_089ns_090ns->Write();
hPixelMap_090ns_091ns->Write();
hPixelMap_091ns_092ns->Write();
hPixelMap_092ns_093ns->Write();
hPixelMap_093ns_094ns->Write();
hPixelMap_094ns_095ns->Write();
hPixelMap_095ns_096ns->Write();
hPixelMap_096ns_097ns->Write();
hPixelMap_097ns_098ns->Write();
hPixelMap_098ns_099ns->Write();
hPixelMap_099ns_100ns->Write();
hPixelMap_100ns_101ns->Write();
hPixelMap_101ns_102ns->Write();
hPixelMap_102ns_103ns->Write();
hPixelMap_103ns_104ns->Write();
hPixelMap_104ns_105ns->Write();
hPixelMap_105ns_106ns->Write();
hPixelMap_106ns_107ns->Write();
hPixelMap_107ns_108ns->Write();
hPixelMap_108ns_109ns->Write();
hPixelMap_109ns_110ns->Write();
hPixelMap_110ns_111ns->Write();
hPixelMap_111ns_112ns->Write();
hPixelMap_112ns_113ns->Write();
hPixelMap_113ns_114ns->Write();
hPixelMap_114ns_115ns->Write();
hPixelMap_115ns_116ns->Write();
hPixelMap_116ns_117ns->Write();
hPixelMap_117ns_118ns->Write();
hPixelMap_118ns_119ns->Write();
hPixelMap_119ns_120ns->Write();
hPixelMap_120ns_121ns->Write();
hPixelMap_121ns_122ns->Write();
hPixelMap_122ns_123ns->Write();
hPixelMap_123ns_124ns->Write();
hPixelMap_124ns_125ns->Write();
hPixelMap_125ns_126ns->Write();
hPixelMap_126ns_127ns->Write();
hPixelMap_127ns_128ns->Write();
hPixelMap_128ns_129ns->Write();
hPixelMap_129ns_130ns->Write();
hPixelMap_130ns_131ns->Write();
hPixelMap_131ns_132ns->Write();
hPixelMap_132ns_133ns->Write();
hPixelMap_133ns_134ns->Write();
hPixelMap_134ns_135ns->Write();
hPixelMap_135ns_136ns->Write();
hPixelMap_136ns_137ns->Write();
hPixelMap_137ns_138ns->Write();
hPixelMap_138ns_139ns->Write();
hPixelMap_139ns_140ns->Write();
hPixelMap_140ns_141ns->Write();
hPixelMap_141ns_142ns->Write();
hPixelMap_142ns_143ns->Write();
hPixelMap_143ns_144ns->Write();
hPixelMap_144ns_145ns->Write();
hPixelMap_145ns_146ns->Write();
hPixelMap_146ns_147ns->Write();
hPixelMap_147ns_148ns->Write();
hPixelMap_148ns_149ns->Write();
hPixelMap_149ns_150ns->Write();
hPixelMap_150ns_151ns->Write();
hPixelMap_151ns_152ns->Write();
hPixelMap_152ns_153ns->Write();
hPixelMap_153ns_154ns->Write();
hPixelMap_154ns_155ns->Write();
hPixelMap_155ns_156ns->Write();
hPixelMap_156ns_157ns->Write();
hPixelMap_157ns_158ns->Write();
hPixelMap_158ns_159ns->Write();
hPixelMap_159ns_160ns->Write();
hPixelMap_160ns_161ns->Write();
hPixelMap_161ns_162ns->Write();
hPixelMap_162ns_163ns->Write();
hPixelMap_163ns_164ns->Write();
hPixelMap_164ns_165ns->Write();
hPixelMap_165ns_166ns->Write();
hPixelMap_166ns_167ns->Write();
hPixelMap_167ns_168ns->Write();
hPixelMap_168ns_169ns->Write();
hPixelMap_169ns_170ns->Write();
hPixelMap_170ns_171ns->Write();
hPixelMap_171ns_172ns->Write();
hPixelMap_172ns_173ns->Write();
hPixelMap_173ns_174ns->Write();
hPixelMap_174ns_175ns->Write();
hPixelMap_175ns_176ns->Write();
hPixelMap_176ns_177ns->Write();
hPixelMap_177ns_178ns->Write();
hPixelMap_178ns_179ns->Write();
hPixelMap_179ns_180ns->Write();
hPixelMap_180ns_181ns->Write();
hPixelMap_181ns_182ns->Write();
hPixelMap_182ns_183ns->Write();
hPixelMap_183ns_184ns->Write();
hPixelMap_184ns_185ns->Write();
hPixelMap_185ns_186ns->Write();
hPixelMap_186ns_187ns->Write();
hPixelMap_187ns_188ns->Write();
hPixelMap_188ns_189ns->Write();
hPixelMap_189ns_190ns->Write();
hPixelMap_190ns_191ns->Write();
hPixelMap_191ns_192ns->Write();
hPixelMap_192ns_193ns->Write();
hPixelMap_193ns_194ns->Write();
hPixelMap_194ns_195ns->Write();
hPixelMap_195ns_196ns->Write();
hPixelMap_196ns_197ns->Write();
hPixelMap_197ns_198ns->Write();
hPixelMap_198ns_199ns->Write();
hPixelMap_199ns_200ns->Write();


// ### Now reset these histograms ###
hPixelMap_000ns_010ns->Reset("ICESM");
hPixelMap_010ns_020ns->Reset("ICESM");
hPixelMap_020ns_030ns->Reset("ICESM");
hPixelMap_030ns_040ns->Reset("ICESM");
hPixelMap_040ns_050ns->Reset("ICESM");
hPixelMap_050ns_060ns->Reset("ICESM");
hPixelMap_060ns_070ns->Reset("ICESM");
hPixelMap_070ns_080ns->Reset("ICESM");
hPixelMap_080ns_090ns->Reset("ICESM");
hPixelMap_090ns_100ns->Reset("ICESM");
hPixelMap_100ns_110ns->Reset("ICESM");
hPixelMap_110ns_120ns->Reset("ICESM");
hPixelMap_120ns_130ns->Reset("ICESM");
hPixelMap_130ns_140ns->Reset("ICESM");
hPixelMap_140ns_150ns->Reset("ICESM");
hPixelMap_150ns_160ns->Reset("ICESM");
hPixelMap_160ns_170ns->Reset("ICESM");
hPixelMap_170ns_180ns->Reset("ICESM");
hPixelMap_180ns_190ns->Reset("ICESM");
hPixelMap_190ns_200ns->Reset("ICESM");

hPixelMap_000ns_001ns->Reset("ICESM");
hPixelMap_001ns_002ns->Reset("ICESM");
hPixelMap_002ns_003ns->Reset("ICESM");
hPixelMap_003ns_004ns->Reset("ICESM");
hPixelMap_004ns_005ns->Reset("ICESM");
hPixelMap_005ns_006ns->Reset("ICESM");
hPixelMap_006ns_007ns->Reset("ICESM");
hPixelMap_007ns_008ns->Reset("ICESM");
hPixelMap_008ns_009ns->Reset("ICESM");
hPixelMap_009ns_010ns->Reset("ICESM");
hPixelMap_010ns_011ns->Reset("ICESM");
hPixelMap_011ns_012ns->Reset("ICESM");
hPixelMap_012ns_013ns->Reset("ICESM");
hPixelMap_013ns_014ns->Reset("ICESM");
hPixelMap_014ns_015ns->Reset("ICESM");
hPixelMap_015ns_016ns->Reset("ICESM");
hPixelMap_016ns_017ns->Reset("ICESM");
hPixelMap_017ns_018ns->Reset("ICESM");
hPixelMap_018ns_019ns->Reset("ICESM");
hPixelMap_019ns_020ns->Reset("ICESM");
hPixelMap_020ns_021ns->Reset("ICESM");
hPixelMap_021ns_022ns->Reset("ICESM");
hPixelMap_022ns_023ns->Reset("ICESM");
hPixelMap_023ns_024ns->Reset("ICESM");
hPixelMap_024ns_025ns->Reset("ICESM");
hPixelMap_025ns_026ns->Reset("ICESM");
hPixelMap_026ns_027ns->Reset("ICESM");
hPixelMap_027ns_028ns->Reset("ICESM");
hPixelMap_028ns_029ns->Reset("ICESM");
hPixelMap_029ns_030ns->Reset("ICESM");
hPixelMap_030ns_031ns->Reset("ICESM");
hPixelMap_031ns_032ns->Reset("ICESM");
hPixelMap_032ns_033ns->Reset("ICESM");
hPixelMap_033ns_034ns->Reset("ICESM");
hPixelMap_034ns_035ns->Reset("ICESM");
hPixelMap_035ns_036ns->Reset("ICESM");
hPixelMap_036ns_037ns->Reset("ICESM");
hPixelMap_037ns_038ns->Reset("ICESM");
hPixelMap_038ns_039ns->Reset("ICESM");
hPixelMap_039ns_040ns->Reset("ICESM");
hPixelMap_040ns_041ns->Reset("ICESM");
hPixelMap_041ns_042ns->Reset("ICESM");
hPixelMap_042ns_043ns->Reset("ICESM");
hPixelMap_043ns_044ns->Reset("ICESM");
hPixelMap_044ns_045ns->Reset("ICESM");
hPixelMap_045ns_046ns->Reset("ICESM");
hPixelMap_046ns_047ns->Reset("ICESM");
hPixelMap_047ns_048ns->Reset("ICESM");
hPixelMap_048ns_049ns->Reset("ICESM");
hPixelMap_049ns_050ns->Reset("ICESM");
hPixelMap_050ns_051ns->Reset("ICESM");
hPixelMap_051ns_052ns->Reset("ICESM");
hPixelMap_052ns_053ns->Reset("ICESM");
hPixelMap_053ns_054ns->Reset("ICESM");
hPixelMap_054ns_055ns->Reset("ICESM");
hPixelMap_055ns_056ns->Reset("ICESM");
hPixelMap_056ns_057ns->Reset("ICESM");
hPixelMap_057ns_058ns->Reset("ICESM");
hPixelMap_058ns_059ns->Reset("ICESM");
hPixelMap_059ns_060ns->Reset("ICESM");
hPixelMap_060ns_061ns->Reset("ICESM");
hPixelMap_061ns_062ns->Reset("ICESM");
hPixelMap_062ns_063ns->Reset("ICESM");
hPixelMap_063ns_064ns->Reset("ICESM");
hPixelMap_064ns_065ns->Reset("ICESM");
hPixelMap_065ns_066ns->Reset("ICESM");
hPixelMap_066ns_067ns->Reset("ICESM");
hPixelMap_067ns_068ns->Reset("ICESM");
hPixelMap_068ns_069ns->Reset("ICESM");
hPixelMap_069ns_070ns->Reset("ICESM");
hPixelMap_070ns_071ns->Reset("ICESM");
hPixelMap_071ns_072ns->Reset("ICESM");
hPixelMap_072ns_073ns->Reset("ICESM");
hPixelMap_073ns_074ns->Reset("ICESM");
hPixelMap_074ns_075ns->Reset("ICESM");
hPixelMap_075ns_076ns->Reset("ICESM");
hPixelMap_076ns_077ns->Reset("ICESM");
hPixelMap_077ns_078ns->Reset("ICESM");
hPixelMap_078ns_079ns->Reset("ICESM");
hPixelMap_079ns_080ns->Reset("ICESM");
hPixelMap_080ns_081ns->Reset("ICESM");
hPixelMap_081ns_082ns->Reset("ICESM");
hPixelMap_082ns_083ns->Reset("ICESM");
hPixelMap_083ns_084ns->Reset("ICESM");
hPixelMap_084ns_085ns->Reset("ICESM");
hPixelMap_085ns_086ns->Reset("ICESM");
hPixelMap_086ns_087ns->Reset("ICESM");
hPixelMap_087ns_088ns->Reset("ICESM");
hPixelMap_088ns_089ns->Reset("ICESM");
hPixelMap_089ns_090ns->Reset("ICESM");
hPixelMap_090ns_091ns->Reset("ICESM");
hPixelMap_091ns_092ns->Reset("ICESM");
hPixelMap_092ns_093ns->Reset("ICESM");
hPixelMap_093ns_094ns->Reset("ICESM");
hPixelMap_094ns_095ns->Reset("ICESM");
hPixelMap_095ns_096ns->Reset("ICESM");
hPixelMap_096ns_097ns->Reset("ICESM");
hPixelMap_097ns_098ns->Reset("ICESM");
hPixelMap_098ns_099ns->Reset("ICESM");
hPixelMap_099ns_100ns->Reset("ICESM");
hPixelMap_100ns_101ns->Reset("ICESM");
hPixelMap_101ns_102ns->Reset("ICESM");
hPixelMap_102ns_103ns->Reset("ICESM");
hPixelMap_103ns_104ns->Reset("ICESM");
hPixelMap_104ns_105ns->Reset("ICESM");
hPixelMap_105ns_106ns->Reset("ICESM");
hPixelMap_106ns_107ns->Reset("ICESM");
hPixelMap_107ns_108ns->Reset("ICESM");
hPixelMap_108ns_109ns->Reset("ICESM");
hPixelMap_109ns_110ns->Reset("ICESM");
hPixelMap_110ns_111ns->Reset("ICESM");
hPixelMap_111ns_112ns->Reset("ICESM");
hPixelMap_112ns_113ns->Reset("ICESM");
hPixelMap_113ns_114ns->Reset("ICESM");
hPixelMap_114ns_115ns->Reset("ICESM");
hPixelMap_115ns_116ns->Reset("ICESM");
hPixelMap_116ns_117ns->Reset("ICESM");
hPixelMap_117ns_118ns->Reset("ICESM");
hPixelMap_118ns_119ns->Reset("ICESM");
hPixelMap_119ns_120ns->Reset("ICESM");
hPixelMap_120ns_121ns->Reset("ICESM");
hPixelMap_121ns_122ns->Reset("ICESM");
hPixelMap_122ns_123ns->Reset("ICESM");
hPixelMap_123ns_124ns->Reset("ICESM");
hPixelMap_124ns_125ns->Reset("ICESM");
hPixelMap_125ns_126ns->Reset("ICESM");
hPixelMap_126ns_127ns->Reset("ICESM");
hPixelMap_127ns_128ns->Reset("ICESM");
hPixelMap_128ns_129ns->Reset("ICESM");
hPixelMap_129ns_130ns->Reset("ICESM");
hPixelMap_130ns_131ns->Reset("ICESM");
hPixelMap_131ns_132ns->Reset("ICESM");
hPixelMap_132ns_133ns->Reset("ICESM");
hPixelMap_133ns_134ns->Reset("ICESM");
hPixelMap_134ns_135ns->Reset("ICESM");
hPixelMap_135ns_136ns->Reset("ICESM");
hPixelMap_136ns_137ns->Reset("ICESM");
hPixelMap_137ns_138ns->Reset("ICESM");
hPixelMap_138ns_139ns->Reset("ICESM");
hPixelMap_139ns_140ns->Reset("ICESM");
hPixelMap_140ns_141ns->Reset("ICESM");
hPixelMap_141ns_142ns->Reset("ICESM");
hPixelMap_142ns_143ns->Reset("ICESM");
hPixelMap_143ns_144ns->Reset("ICESM");
hPixelMap_144ns_145ns->Reset("ICESM");
hPixelMap_145ns_146ns->Reset("ICESM");
hPixelMap_146ns_147ns->Reset("ICESM");
hPixelMap_147ns_148ns->Reset("ICESM");
hPixelMap_148ns_149ns->Reset("ICESM");
hPixelMap_149ns_150ns->Reset("ICESM");
hPixelMap_150ns_151ns->Reset("ICESM");
hPixelMap_151ns_152ns->Reset("ICESM");
hPixelMap_152ns_153ns->Reset("ICESM");
hPixelMap_153ns_154ns->Reset("ICESM");
hPixelMap_154ns_155ns->Reset("ICESM");
hPixelMap_155ns_156ns->Reset("ICESM");
hPixelMap_156ns_157ns->Reset("ICESM");
hPixelMap_157ns_158ns->Reset("ICESM");
hPixelMap_158ns_159ns->Reset("ICESM");
hPixelMap_159ns_160ns->Reset("ICESM");
hPixelMap_160ns_161ns->Reset("ICESM");
hPixelMap_161ns_162ns->Reset("ICESM");
hPixelMap_162ns_163ns->Reset("ICESM");
hPixelMap_163ns_164ns->Reset("ICESM");
hPixelMap_164ns_165ns->Reset("ICESM");
hPixelMap_165ns_166ns->Reset("ICESM");
hPixelMap_166ns_167ns->Reset("ICESM");
hPixelMap_167ns_168ns->Reset("ICESM");
hPixelMap_168ns_169ns->Reset("ICESM");
hPixelMap_169ns_170ns->Reset("ICESM");
hPixelMap_170ns_171ns->Reset("ICESM");
hPixelMap_171ns_172ns->Reset("ICESM");
hPixelMap_172ns_173ns->Reset("ICESM");
hPixelMap_173ns_174ns->Reset("ICESM");
hPixelMap_174ns_175ns->Reset("ICESM");
hPixelMap_175ns_176ns->Reset("ICESM");
hPixelMap_176ns_177ns->Reset("ICESM");
hPixelMap_177ns_178ns->Reset("ICESM");
hPixelMap_178ns_179ns->Reset("ICESM");
hPixelMap_179ns_180ns->Reset("ICESM");
hPixelMap_180ns_181ns->Reset("ICESM");
hPixelMap_181ns_182ns->Reset("ICESM");
hPixelMap_182ns_183ns->Reset("ICESM");
hPixelMap_183ns_184ns->Reset("ICESM");
hPixelMap_184ns_185ns->Reset("ICESM");
hPixelMap_185ns_186ns->Reset("ICESM");
hPixelMap_186ns_187ns->Reset("ICESM");
hPixelMap_187ns_188ns->Reset("ICESM");
hPixelMap_188ns_189ns->Reset("ICESM");
hPixelMap_189ns_190ns->Reset("ICESM");
hPixelMap_190ns_191ns->Reset("ICESM");
hPixelMap_191ns_192ns->Reset("ICESM");
hPixelMap_192ns_193ns->Reset("ICESM");
hPixelMap_193ns_194ns->Reset("ICESM");
hPixelMap_194ns_195ns->Reset("ICESM");
hPixelMap_195ns_196ns->Reset("ICESM");
hPixelMap_196ns_197ns->Reset("ICESM");
hPixelMap_197ns_198ns->Reset("ICESM");
hPixelMap_198ns_199ns->Reset("ICESM");
hPixelMap_199ns_200ns->Reset("ICESM");



}
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------------------------------

void SemiAnalytic_v03_06_25_21::Loop()
{

// #### Check to make sure the file is good ###
if (fChain == 0) return;
Long64_t nentries = fChain->GetEntriesFast();
Long64_t nbytes = 0, nb = 0;


//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
// Filling a histogram for the scintillation light time constants
// 
// ### time constants for the fast and slow scintillation coming from the singlet and triplet states (in ns)
double tau_fast = 5.0;
double tau_slow = 2100.;

// ### Fractional content coming from fast and slow components 
double fast_frac = 0.15;
double slow_frac = 1.0 - fast_frac;

// ### Histogram we will draw from for the scintillation time ###
TH1D *hScintTime = new TH1D("hScintTime", "Scintillation Time", 10000, 0, 2000);

// ### Fill over the first 2000 ns ###
for(double t = 0; t<2000; t+=0.1)
   {
   double temp = (fast_frac/tau_fast)*exp(-t/tau_fast) + (slow_frac/tau_slow)*exp(-t/tau_slow);
   hScintTime->Fill(t,temp);
   //std::cout<<temp<<std::endl;
   }//<---End scinttime histo

// ### Normalize the histogram ###   
hScintTime->Scale(1/ hScintTime->Integral() );
//-------------------------------------------------------------------------------------


// ##############################
// #### Loop over all events ####
// ##############################
for (Long64_t jentry=0; jentry<nentries;jentry++)
//for (Long64_t jentry=Event; jentry<Event_plus_one;jentry++) 
   {
   
   // ### Define a random x, y, z offset for the event ###
   gRandom->SetSeed(jentry);
   double xoffset = gRandom->Uniform(-10,150);
   double yoffset = gRandom->Uniform(-20,300);
   double zoffset = gRandom->Uniform(-20,300);
   
   if(jentry%1 == 0){std::cout<<"Event: "<<jentry<<std::endl;}
   
   // ### Check the file ###
   Long64_t ientry = LoadTree(jentry);
   if (ientry < 0) break;
   nb = fChain->GetEntry(jentry);   nbytes += nb;
   
   // ### Define a variable for the total amount of charge deposited
   double TotalChargeDep = 0;
   
   // ### Define a variable for the total number of photons detected
   double TotalPhotons = 0;
   
   std::vector<double> YPosition;
   std::vector<double> ZPosition;
   std::vector<double> Time;
   
   std::cout<<"Number of Particles: "<<number_particles<<std::endl;
   double MuonTrackID = 0;
   // ### List out all the particles in the event ###
   for (int aa = 0; aa < number_particles; aa++)
      {
      
      // ### Skipping ionization electrons ###
      if(particle_process_key->at(aa) == 1){continue;}
      // ### And skip anything that doesn't look interesting ###
      
      if(abs(particle_pdg_code->at(aa)) == 13){MuonTrackID = particle_track_id->at(aa); std::cout<<"mutrkID: "<<MuonTrackID<<std::endl;}
      if(particle_parent_track_id->at(aa) == 0 || particle_parent_track_id->at(aa) == 1 || particle_parent_track_id->at(aa) == MuonTrackID)
         {
         std::cout<<"Track ID: "<<particle_track_id->at(aa)<<" PDG: "<<particle_pdg_code->at(aa)<<std::endl;
         std::cout<<"Process key: "<<particle_process_key->at(aa)<<std::endl;
         std::cout<<"Parent Track ID: "<<particle_parent_track_id->at(aa)<<std::endl;
         std::cout<<std::endl;
         YprimaryZPrimary->Fill(particle_initial_y->at(aa) + yoffset, particle_initial_z->at(aa) + zoffset);
         XprimaryYPrimary->Fill(particle_initial_x->at(aa) + xoffset, particle_initial_y->at(aa) + yoffset);
         XprimaryZPrimary->Fill(particle_initial_x->at(aa) + xoffset, particle_initial_z->at(aa) + zoffset);
         }
      
      
      }//<---End aa loop
   
   
   double generator_xi = generator_initial_particle_x->at(0) + xoffset;
   double generator_yi = generator_initial_particle_y->at(0) + yoffset;
   double generator_zi = generator_initial_particle_z->at(0) + zoffset;
   
   hyizi->Fill(generator_yi,generator_zi);
   hxiyi->Fill(generator_xi,generator_yi);
   hxizi->Fill(generator_xi,generator_zi);

   // ##################################################
   // #### Loop over all the deposited energy points ###
   // ##################################################
   for (int Edep = 0; Edep < number_hits; Edep ++)
   	{
	if(Edep % 100 == 0){std::cout<<"Edep = "<<Edep<<" out of "<<number_hits<<std::endl;}
        double x_value = hit_start_x->at(Edep);
	double y_value = hit_start_y->at(Edep);
	double z_value = hit_start_z->at(Edep);
	
	
	double xpos = x_value + xoffset;
	double ypos = y_value + yoffset;
	double zpos = z_value + zoffset;
	//std::cout<<"X: "<<xpos<<" Y: "<<ypos<<" Z: "<<zpos<<std::endl;
	
	// ### Keep track of the positions we are using ###
	hXPos->Fill(xpos);
        hYPos->Fill(ypos);
        hZPos->Fill(zpos);
        
        // ### Keep track of the generated X, Y, Z position ###
        hXPosRaw->Fill(x_value);
        hYPosRaw->Fill(y_value);
        hZPosRaw->Fill(x_value);
	
	// ### Grab the deposition time as recored by Geant4 ###
	double DepositionTime = hit_start_t->at(Edep);
	
	hDepositionTime->Fill(DepositionTime);
	// ### Skip the energy deposition if its outside the detector volume ###
	if(xpos < 0. || xpos > xDimension || 
	   ypos < 0. || ypos > yDimension ||
	   zpos < 0. || zpos > zDimension)
	   	{
		if(jentry == 1){std::cout<<"X: "<<xpos<<" Y: "<<ypos<<" Z: "<<zpos<<std::endl;}
		continue;
		}
	
	// ### Fill some projective charge histograms to help with visualization ###
	hyz->Fill(ypos, zpos, hit_energy_deposit->at(Edep));
	hxy->Fill(xpos, ypos, hit_energy_deposit->at(Edep));
	hxz->Fill(xpos, zpos, hit_energy_deposit->at(Edep));
	
	// ### Add to the total amount of charge for this event
	TotalChargeDep += hit_energy_deposit->at(Edep);
	
	// ### Taking the "W-Value" for scintillation from https://lar.bnl.gov/properties/#scint
        // ### we estimate 19.5 eV / photon, so we can easily calculate the number of photons 
        // ### produced by each energy deposition (note: we have to convert the units of Hit_Energy
        // ### to eV from MeV)
        int n_photons_raw = hit_energy_deposit->at(Edep)*1E6 / 19.5;// ### note: I am using the 'int' function to round
	
	int nYPix = 0, nZPix = 0;
	// ### Loop over each pixel in the z-direction ###
	for (int zloc = 0; zloc < zDimension; zloc+= PixelPitch)
	   {
	   
	   // ### For a given z-position, loop over all the pixels in the y-direction ###
	   for(int yloc = 0; yloc< yDimension; yloc+= PixelPitch)
	      {
	      
	      // ### Define the distance connecting the two points ###
    	      double distanceX = 0 - xpos;
	      double distanceY = yloc - ypos; 
	      double distanceZ = zloc - zpos;
              // ### Normalize the vector ####
              double dis = sqrt(distanceX*distanceX + distanceY*distanceY + distanceZ*distanceZ);
    
              // ### Calculate the Normalized Distance Vector from this point ###
              TVector3 direction (distanceX / dis, distanceY / dis, distanceZ / dis);
	      
	      // ### Calculate little Omega (solid angle assuming the pixel is on axis)
              double pixelPitchSquared =  PixelPitch*PixelPitch;
              double numerator = pixelPitchSquared;
              double denomenator = pixelPitchSquared + (4*dis*dis);
              double littleOmega = 4* asin(numerator / denomenator);
                
              // ### Calculate the full solid angle by taking the dot product of the pixel with the distance vec
              double Omega = abs(littleOmega * direction.Dot(planeNormalYZ));
              
              double OmegaInDegrees = Omega * (180/3.14159);
                
              // #### Include attenuation due to impurities ####
              double attenuation = exp(-dis/2000); //### 20 meters (2000 cm) corresponds to 3ppm nitrogen
              
	      // ### Converting the angle to degrees ####
	      double ang = direction.Dot(planeNormalYZ) * (180/3.14159);
	      
          

              // ########################################################
              // ### Number of photons calculated to be at this pixel ###
              // ########################################################
              int PhotonCount_angleOnly = (attenuation * (Omega * n_photons_raw)/(4*3.14159));
              
              // ### Declare a variable used to the GH correction ###
              double GaisserHillsCorrection;
              // ### Load the appropriate Gaisser Hills TGraph based on the projected solid angle in degrees
              if(abs(OmegaInDegrees) < 10){GaisserHillsCorrection = grGaisser_Hillas_00_10->Eval(dis);}
              if(abs(OmegaInDegrees) >= 10 && abs(OmegaInDegrees) < 20){GaisserHillsCorrection = grGaisser_Hillas_10_20->Eval(dis);}
              if(abs(OmegaInDegrees) >= 20 && abs(OmegaInDegrees) < 30){GaisserHillsCorrection = grGaisser_Hillas_20_30->Eval(dis);}
              if(abs(OmegaInDegrees) >= 30 && abs(OmegaInDegrees) < 40){GaisserHillsCorrection = grGaisser_Hillas_30_40->Eval(dis);}
              if(abs(OmegaInDegrees) >= 40 && abs(OmegaInDegrees) < 50){GaisserHillsCorrection = grGaisser_Hillas_40_50->Eval(dis);}
              if(abs(OmegaInDegrees) >= 50 && abs(OmegaInDegrees) < 60){GaisserHillsCorrection = grGaisser_Hillas_50_60->Eval(dis);}
              if(abs(OmegaInDegrees) >= 60 && abs(OmegaInDegrees) < 70){GaisserHillsCorrection = grGaisser_Hillas_60_70->Eval(dis);}
              if(abs(OmegaInDegrees) >= 70 && abs(OmegaInDegrees) < 80){GaisserHillsCorrection = grGaisser_Hillas_70_80->Eval(dis);}
              if(abs(OmegaInDegrees) >= 80 && abs(OmegaInDegrees) < 90){GaisserHillsCorrection = grGaisser_Hillas_80_90->Eval(dis);}
              
              // ##########################################
              // ### Number of photons fully calculated ###
              // ##########################################
              int PhotonCount = PhotonCount_angleOnly * GaisserHillsCorrection;
              
	      if(PhotonCount == 0){nYPix++; continue;}
              TotalPhotons+= PhotonCount;
	      
	      // ### This function returns the transport parameters (N1, Zeta, mu, N2, and Kappa)
	      // ### in a vector in the order listed here
	      std::vector<double> Params = TransportTimeParameters(dis, ang, PhotonCount);
	      
	      //=================================================================
	      // ### The TMath::Landau function takes as it's arguments (Distance, mu, Zeta)
	      // ### so that is TMath::Landau(dis, Params[2], Params[1]) and the last argument
	      // ### set to true should have it return the value divided by zeta
              double Landau = Params[0]*TMath::Landau(dis, Params[2], Params[1],  true); 
              double Expo = Params[3]*exp(Params[4]*dis);
	      
              //std::cout<<"Landau: "<<Landau<<std::endl;
              //std::cout<<"Expo: "<<Expo<<std::endl;

	      double TransportTime = Landau+Expo;
	      
	      hTransportTime->Fill(TransportTime);
	      
	      // ### Calculate the initial time from the distance and the speed of 
	      // ### light in LAr (11.23 cm/ns)
	      double t0 = dis/11.23;
	      
	      // ### Loop over all the photons and calculate each photons time ###
	      for(int npho = 0; npho < PhotonCount; npho++)
	         {
		 // ### Grab a random Scintillation time from the histogram ###
		 double ScintTime = hScintTime->GetRandom();
		 
		 // ### Total Time is t0 + Scintillation Time + Deposition Time + Transport Time ###
		 double total_time = t0 + ScintTime + DepositionTime + TransportTime;
		 
		 // ### Store the Y, Z, and time for each photon in a vector ###
		 YPosition.push_back(nYPix);
		 ZPosition.push_back(nZPix);
		 Time.push_back(total_time);
		 
		 //std::cout<<"Total Time: "<<total_time<<std::endl;
		 hTotalTime->Fill(total_time);
		 }//<---End npho loop
	      
	      
              //=================================================================
	      //std::cout<<"Photon Count: "<<PhotonCount<<std::endl;
    	      
	      //PixelMap[jentry][nYPix][nZPix] = PhotonCount;

	      
	      
	      // Bump the YPixel Counter
	      nYPix++;
	      }//<--End yloc loop
	   
	   
	   
	   // Bump the Z Pixel Counter
	   nZPix++;
	   // Zero the Y Pixel Counter
	   nYPix = 0;
	   }//<---end zloc loop
	
	
	
	}//<---End Edep Loop
   
   // ### Looping over all the photons for this event ###
   for(int aa= 0; aa < ZPosition.size(); aa++)
      {
      if(Time[aa] > 0 && Time[aa] < 10){hPixelMap_000ns_010ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 10 && Time[aa] < 20){hPixelMap_010ns_020ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 20 && Time[aa] < 30){hPixelMap_020ns_030ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 30 && Time[aa] < 40){hPixelMap_030ns_040ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 40 && Time[aa] < 50){hPixelMap_040ns_050ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 50 && Time[aa] < 60){hPixelMap_050ns_060ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 60 && Time[aa] < 70){hPixelMap_060ns_070ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 70 && Time[aa] < 80){hPixelMap_070ns_080ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 80 && Time[aa] < 90){hPixelMap_080ns_090ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 90 && Time[aa] < 100){hPixelMap_090ns_100ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 100 && Time[aa] < 110){hPixelMap_100ns_110ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 110 && Time[aa] < 120){hPixelMap_110ns_120ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 120 && Time[aa] < 130){hPixelMap_120ns_130ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 130 && Time[aa] < 140){hPixelMap_130ns_140ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 140 && Time[aa] < 150){hPixelMap_140ns_150ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 150 && Time[aa] < 160){hPixelMap_150ns_160ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 160 && Time[aa] < 170){hPixelMap_160ns_170ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 170 && Time[aa] < 180){hPixelMap_170ns_180ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 180 && Time[aa] < 190){hPixelMap_180ns_190ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 190 && Time[aa] < 200){hPixelMap_190ns_200ns->Fill(YPosition[aa], ZPosition[aa]);}


      if(Time[aa] > 0 && Time[aa] < 1){hPixelMap_000ns_001ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 1 && Time[aa] < 2){hPixelMap_001ns_002ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 2 && Time[aa] < 3){hPixelMap_002ns_003ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 3 && Time[aa] < 4){hPixelMap_003ns_004ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 4 && Time[aa] < 5){hPixelMap_004ns_005ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 5 && Time[aa] < 6){hPixelMap_005ns_006ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 6 && Time[aa] < 7){hPixelMap_006ns_007ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 7 && Time[aa] < 8){hPixelMap_007ns_008ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 8 && Time[aa] < 9){hPixelMap_008ns_009ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 9 && Time[aa] < 10){hPixelMap_009ns_010ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 10 && Time[aa] < 11){hPixelMap_010ns_011ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 11 && Time[aa] < 12){hPixelMap_011ns_012ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 12 && Time[aa] < 13){hPixelMap_012ns_013ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 13 && Time[aa] < 14){hPixelMap_013ns_014ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 14 && Time[aa] < 15){hPixelMap_014ns_015ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 15 && Time[aa] < 16){hPixelMap_015ns_016ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 16 && Time[aa] < 17){hPixelMap_016ns_017ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 17 && Time[aa] < 18){hPixelMap_017ns_018ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 18 && Time[aa] < 19){hPixelMap_018ns_019ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 19 && Time[aa] < 20){hPixelMap_019ns_020ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 20 && Time[aa] < 21){hPixelMap_020ns_021ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 21 && Time[aa] < 22){hPixelMap_021ns_022ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 22 && Time[aa] < 23){hPixelMap_022ns_023ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 23 && Time[aa] < 24){hPixelMap_023ns_024ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 24 && Time[aa] < 25){hPixelMap_024ns_025ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 25 && Time[aa] < 26){hPixelMap_025ns_026ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 26 && Time[aa] < 27){hPixelMap_026ns_027ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 27 && Time[aa] < 28){hPixelMap_027ns_028ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 28 && Time[aa] < 29){hPixelMap_028ns_029ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 29 && Time[aa] < 30){hPixelMap_029ns_030ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 30 && Time[aa] < 31){hPixelMap_030ns_031ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 31 && Time[aa] < 32){hPixelMap_031ns_032ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 32 && Time[aa] < 33){hPixelMap_032ns_033ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 33 && Time[aa] < 34){hPixelMap_033ns_034ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 34 && Time[aa] < 35){hPixelMap_034ns_035ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 35 && Time[aa] < 36){hPixelMap_035ns_036ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 36 && Time[aa] < 37){hPixelMap_036ns_037ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 37 && Time[aa] < 38){hPixelMap_037ns_038ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 38 && Time[aa] < 39){hPixelMap_038ns_039ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 39 && Time[aa] < 40){hPixelMap_039ns_040ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 40 && Time[aa] < 41){hPixelMap_040ns_041ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 41 && Time[aa] < 42){hPixelMap_041ns_042ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 42 && Time[aa] < 43){hPixelMap_042ns_043ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 43 && Time[aa] < 44){hPixelMap_043ns_044ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 44 && Time[aa] < 45){hPixelMap_044ns_045ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 45 && Time[aa] < 46){hPixelMap_045ns_046ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 46 && Time[aa] < 47){hPixelMap_046ns_047ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 47 && Time[aa] < 48){hPixelMap_047ns_048ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 48 && Time[aa] < 49){hPixelMap_048ns_049ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 49 && Time[aa] < 50){hPixelMap_049ns_050ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 50 && Time[aa] < 51){hPixelMap_050ns_051ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 51 && Time[aa] < 52){hPixelMap_051ns_052ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 52 && Time[aa] < 53){hPixelMap_052ns_053ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 53 && Time[aa] < 54){hPixelMap_053ns_054ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 54 && Time[aa] < 55){hPixelMap_054ns_055ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 55 && Time[aa] < 56){hPixelMap_055ns_056ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 56 && Time[aa] < 57){hPixelMap_056ns_057ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 57 && Time[aa] < 58){hPixelMap_057ns_058ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 58 && Time[aa] < 59){hPixelMap_058ns_059ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 59 && Time[aa] < 60){hPixelMap_059ns_060ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 60 && Time[aa] < 61){hPixelMap_060ns_061ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 61 && Time[aa] < 62){hPixelMap_061ns_062ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 62 && Time[aa] < 63){hPixelMap_062ns_063ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 63 && Time[aa] < 64){hPixelMap_063ns_064ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 64 && Time[aa] < 65){hPixelMap_064ns_065ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 65 && Time[aa] < 66){hPixelMap_065ns_066ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 66 && Time[aa] < 67){hPixelMap_066ns_067ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 67 && Time[aa] < 68){hPixelMap_067ns_068ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 68 && Time[aa] < 69){hPixelMap_068ns_069ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 69 && Time[aa] < 70){hPixelMap_069ns_070ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 70 && Time[aa] < 71){hPixelMap_070ns_071ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 71 && Time[aa] < 72){hPixelMap_071ns_072ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 72 && Time[aa] < 73){hPixelMap_072ns_073ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 73 && Time[aa] < 74){hPixelMap_073ns_074ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 74 && Time[aa] < 75){hPixelMap_074ns_075ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 75 && Time[aa] < 76){hPixelMap_075ns_076ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 76 && Time[aa] < 77){hPixelMap_076ns_077ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 77 && Time[aa] < 78){hPixelMap_077ns_078ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 78 && Time[aa] < 79){hPixelMap_078ns_079ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 79 && Time[aa] < 80){hPixelMap_079ns_080ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 80 && Time[aa] < 81){hPixelMap_080ns_081ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 81 && Time[aa] < 82){hPixelMap_081ns_082ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 82 && Time[aa] < 83){hPixelMap_082ns_083ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 83 && Time[aa] < 84){hPixelMap_083ns_084ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 84 && Time[aa] < 85){hPixelMap_084ns_085ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 85 && Time[aa] < 86){hPixelMap_085ns_086ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 86 && Time[aa] < 87){hPixelMap_086ns_087ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 87 && Time[aa] < 88){hPixelMap_087ns_088ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 88 && Time[aa] < 89){hPixelMap_088ns_089ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 89 && Time[aa] < 90){hPixelMap_089ns_090ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 90 && Time[aa] < 91){hPixelMap_090ns_091ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 91 && Time[aa] < 92){hPixelMap_091ns_092ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 92 && Time[aa] < 93){hPixelMap_092ns_093ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 93 && Time[aa] < 94){hPixelMap_093ns_094ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 94 && Time[aa] < 95){hPixelMap_094ns_095ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 95 && Time[aa] < 96){hPixelMap_095ns_096ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 96 && Time[aa] < 97){hPixelMap_096ns_097ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 97 && Time[aa] < 98){hPixelMap_097ns_098ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 98 && Time[aa] < 99){hPixelMap_098ns_099ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 99 && Time[aa] < 100){hPixelMap_099ns_100ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 100 && Time[aa] < 101){hPixelMap_100ns_101ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 101 && Time[aa] < 102){hPixelMap_101ns_102ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 102 && Time[aa] < 103){hPixelMap_102ns_103ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 103 && Time[aa] < 104){hPixelMap_103ns_104ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 104 && Time[aa] < 105){hPixelMap_104ns_105ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 105 && Time[aa] < 106){hPixelMap_105ns_106ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 106 && Time[aa] < 107){hPixelMap_106ns_107ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 107 && Time[aa] < 108){hPixelMap_107ns_108ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 108 && Time[aa] < 109){hPixelMap_108ns_109ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 109 && Time[aa] < 110){hPixelMap_109ns_110ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 110 && Time[aa] < 111){hPixelMap_110ns_111ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 111 && Time[aa] < 112){hPixelMap_111ns_112ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 112 && Time[aa] < 113){hPixelMap_112ns_113ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 113 && Time[aa] < 114){hPixelMap_113ns_114ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 114 && Time[aa] < 115){hPixelMap_114ns_115ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 115 && Time[aa] < 116){hPixelMap_115ns_116ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 116 && Time[aa] < 117){hPixelMap_116ns_117ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 117 && Time[aa] < 118){hPixelMap_117ns_118ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 118 && Time[aa] < 119){hPixelMap_118ns_119ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 119 && Time[aa] < 120){hPixelMap_119ns_120ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 120 && Time[aa] < 121){hPixelMap_120ns_121ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 121 && Time[aa] < 122){hPixelMap_121ns_122ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 122 && Time[aa] < 123){hPixelMap_122ns_123ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 123 && Time[aa] < 124){hPixelMap_123ns_124ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 124 && Time[aa] < 125){hPixelMap_124ns_125ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 125 && Time[aa] < 126){hPixelMap_125ns_126ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 126 && Time[aa] < 127){hPixelMap_126ns_127ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 127 && Time[aa] < 128){hPixelMap_127ns_128ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 128 && Time[aa] < 129){hPixelMap_128ns_129ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 129 && Time[aa] < 130){hPixelMap_129ns_130ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 130 && Time[aa] < 131){hPixelMap_130ns_131ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 131 && Time[aa] < 132){hPixelMap_131ns_132ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 132 && Time[aa] < 133){hPixelMap_132ns_133ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 133 && Time[aa] < 134){hPixelMap_133ns_134ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 134 && Time[aa] < 135){hPixelMap_134ns_135ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 135 && Time[aa] < 136){hPixelMap_135ns_136ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 136 && Time[aa] < 137){hPixelMap_136ns_137ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 137 && Time[aa] < 138){hPixelMap_137ns_138ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 138 && Time[aa] < 139){hPixelMap_138ns_139ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 139 && Time[aa] < 140){hPixelMap_139ns_140ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 140 && Time[aa] < 141){hPixelMap_140ns_141ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 141 && Time[aa] < 142){hPixelMap_141ns_142ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 142 && Time[aa] < 143){hPixelMap_142ns_143ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 143 && Time[aa] < 144){hPixelMap_143ns_144ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 144 && Time[aa] < 145){hPixelMap_144ns_145ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 145 && Time[aa] < 146){hPixelMap_145ns_146ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 146 && Time[aa] < 147){hPixelMap_146ns_147ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 147 && Time[aa] < 148){hPixelMap_147ns_148ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 148 && Time[aa] < 149){hPixelMap_148ns_149ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 149 && Time[aa] < 150){hPixelMap_149ns_150ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 150 && Time[aa] < 151){hPixelMap_150ns_151ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 151 && Time[aa] < 152){hPixelMap_151ns_152ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 152 && Time[aa] < 153){hPixelMap_152ns_153ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 153 && Time[aa] < 154){hPixelMap_153ns_154ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 154 && Time[aa] < 155){hPixelMap_154ns_155ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 155 && Time[aa] < 156){hPixelMap_155ns_156ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 156 && Time[aa] < 157){hPixelMap_156ns_157ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 157 && Time[aa] < 158){hPixelMap_157ns_158ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 158 && Time[aa] < 159){hPixelMap_158ns_159ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 159 && Time[aa] < 160){hPixelMap_159ns_160ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 160 && Time[aa] < 161){hPixelMap_160ns_161ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 161 && Time[aa] < 162){hPixelMap_161ns_162ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 162 && Time[aa] < 163){hPixelMap_162ns_163ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 163 && Time[aa] < 164){hPixelMap_163ns_164ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 164 && Time[aa] < 165){hPixelMap_164ns_165ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 165 && Time[aa] < 166){hPixelMap_165ns_166ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 166 && Time[aa] < 167){hPixelMap_166ns_167ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 167 && Time[aa] < 168){hPixelMap_167ns_168ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 168 && Time[aa] < 169){hPixelMap_168ns_169ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 169 && Time[aa] < 170){hPixelMap_169ns_170ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 170 && Time[aa] < 171){hPixelMap_170ns_171ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 171 && Time[aa] < 172){hPixelMap_171ns_172ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 172 && Time[aa] < 173){hPixelMap_172ns_173ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 173 && Time[aa] < 174){hPixelMap_173ns_174ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 174 && Time[aa] < 175){hPixelMap_174ns_175ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 175 && Time[aa] < 176){hPixelMap_175ns_176ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 176 && Time[aa] < 177){hPixelMap_176ns_177ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 177 && Time[aa] < 178){hPixelMap_177ns_178ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 178 && Time[aa] < 179){hPixelMap_178ns_179ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 179 && Time[aa] < 180){hPixelMap_179ns_180ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 180 && Time[aa] < 181){hPixelMap_180ns_181ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 181 && Time[aa] < 182){hPixelMap_181ns_182ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 182 && Time[aa] < 183){hPixelMap_182ns_183ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 183 && Time[aa] < 184){hPixelMap_183ns_184ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 184 && Time[aa] < 185){hPixelMap_184ns_185ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 185 && Time[aa] < 186){hPixelMap_185ns_186ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 186 && Time[aa] < 187){hPixelMap_186ns_187ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 187 && Time[aa] < 188){hPixelMap_187ns_188ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 188 && Time[aa] < 189){hPixelMap_188ns_189ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 189 && Time[aa] < 190){hPixelMap_189ns_190ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 190 && Time[aa] < 191){hPixelMap_190ns_191ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 191 && Time[aa] < 192){hPixelMap_191ns_192ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 192 && Time[aa] < 193){hPixelMap_192ns_193ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 193 && Time[aa] < 194){hPixelMap_193ns_194ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 194 && Time[aa] < 195){hPixelMap_194ns_195ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 195 && Time[aa] < 196){hPixelMap_195ns_196ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 196 && Time[aa] < 197){hPixelMap_196ns_197ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 197 && Time[aa] < 198){hPixelMap_197ns_198ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 198 && Time[aa] < 199){hPixelMap_198ns_199ns->Fill(YPosition[aa], ZPosition[aa]);}
      if(Time[aa] > 199 && Time[aa] < 200){hPixelMap_199ns_200ns->Fill(YPosition[aa], ZPosition[aa]);}

      
      }//<---end loop over photons
   
   std::cout<<"Number of photons: "<<TotalPhotons<<std::endl;
   hEnergyDepositedVSPhotons->Fill(TotalChargeDep, TotalPhotons);
   hEnergyDeposited->Fill(TotalChargeDep);
   hNumOfPhotons->Fill(TotalPhotons);
   
   WriteFile(jentry);
   WriteFileTimeSlices(jentry);
   
   }//<--End event loop




} // <--- End Loop() function





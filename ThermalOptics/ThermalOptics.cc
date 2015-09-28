#include "DetectorConstruction.hh"
#include "ActionInitialization.hh"
#include "Messenger.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif

#include "FTFP_BERT_HP.hh"
#include "G4UImanager.hh"
#include "G4UIcommand.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif
#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

// Usage info
namespace {
  void PrintUsage() {
    G4cerr << " Usage: " << G4endl;
    G4cerr << " ./ThermalOptics [-n nEvents ] [-u UIsession] [-t nThreads]" << G4endl;
    G4cerr << "   Note: -t option is available only for multi-threaded mode."
           << G4endl;
  }
}

// Main routine
int main(int argc, char** argv) {

  // Declare vars
  G4int nEvents = 0;
  G4String session;
#ifdef G4MULTITHREADED
  G4int nThreads = 0;
#endif
  G4double LDs[6] = {1, 25, 50, 75, 100, 200},
           productionCut = 0.1*mm;
  G4String syscmd, dataDir = "data/";
  std::ostringstream syscmdStream;

  // Evaluate arguments, don't allow breaking
  if ( argc > 9 ) { PrintUsage(); return 1; }  
  for ( G4int i=1; i<argc; i=i+2 ) {
    if      ( G4String(argv[i]) == "-n" ) nEvents = std::atoi(argv[i+1]);
    else if ( G4String(argv[i]) == "-u" ) session = argv[i+1];
#ifdef G4MULTITHREADED
    else if ( G4String(argv[i]) == "-t" ) {
      nThreads = G4UIcommand::ConvertToInt(argv[i+1]);
    }
#endif
    else {
      PrintUsage();
      return 1;
    }
  }  
  
  // Choose the Random engine
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
#ifdef G4MULTITHREADED
  G4MTRunManager * runManager = new G4MTRunManager;
  if ( nThreads > 0 ) { 
    runManager->SetNumberOfThreads(nThreads);
  }  
#else
  G4RunManager * runManager = new G4RunManager;
#endif

  // Instantiate Geometry
  G4cout << "======================" << G4endl << "Initializing Materials" << G4endl;
  DetectorConstruction* detConstruction = new DetectorConstruction();
  runManager->SetUserInitialization(detConstruction);

  // Instantiate Physics
  G4cout << "====================" << G4endl << "Initializing Physics" << G4endl;
  G4VModularPhysicsList* physicsList = new FTFP_BERT_HP;
  physicsList->SetDefaultCutValue(productionCut);
  runManager->SetUserInitialization(physicsList);

  // Instantiate and initialize running Action sequence
  G4cout << "===================" << G4endl << "Initializing Action" << G4endl << G4endl;
  ActionInitialization* actionInitialization = new ActionInitialization(detConstruction);
  runManager->SetUserInitialization(actionInitialization);
  runManager->Initialize();

#ifdef G4VIS_USE
  // Instantiate and Initialize Visualization
  G4cout << G4endl << "==========================" << G4endl << "Initializing Visualization" << G4endl << G4endl;
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
  G4UImanager* UImanager = G4UImanager::GetUIpointer();

  // Thermal Neutron beam, track tracking cuts
  UImanager->ApplyCommand("/gun/particle neutron");
  UImanager->ApplyCommand("/gun/energy 0.025 eV");

  // Initialize main data directory
  syscmdStream.str(""); syscmdStream << "mkdir -p " << dataDir;
  syscmd = syscmdStream.str(); system(syscmd);

  // Save data directory via Messenger for RunAction
  Messenger* messenger = Messenger::GetMessenger();
  messenger->SaveDataDir(dataDir);
  
  // Begin simulation
  if ( nEvents > 0 ) {

    // Model loop
    for ( G4int LD_i = 0; LD_i < 6; LD_i++ ) {

      // Announce geometry
      G4cout << G4endl << "=================================="
             << G4endl << "Simulating collimator model LD=" << LDs[LD_i]
             << G4endl << G4endl;

      // Set model LD
      messenger->SaveLDRatio(LDs[LD_i]);

      // Reconfigure scatter rejection geometry
      detConstruction->ModelLDConfiguration(LDs[LD_i]);

      // Run beam
      syscmdStream.str(""); syscmdStream << "/run/beamOn " << nEvents;
      syscmd = syscmdStream.str(); UImanager->ApplyCommand(syscmd);
    }

  } else {

    // Initialize visualizer and gui macros
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv, session);
#ifdef G4VIS_USE
    UImanager->ApplyCommand("/control/execute init_vis.mac"); 
#else
    UImanager->ApplyCommand("/control/execute init.mac"); 
#endif
    if (ui->IsGUI()) { UImanager->ApplyCommand("/control/execute gui.mac"); }
    ui->SessionStart();
    delete ui;
#endif
  }

  // Dump manager objects from memory
#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}

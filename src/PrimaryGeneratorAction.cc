#include "PrimaryGeneratorAction.hh"
#include "Messenger.hh"

#include "G4RunManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
#include <math.h>

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0) {

  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // Default particle kinematic
  G4ParticleDefinition* particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
  fParticleGun->SetParticleEnergy(0.025*eV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

// This function is called at the begining of event
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

  // Gaussian profile of given FWHM via radial StDev; Azimuthal symmetry assumed
  G4double beamFWHM = 1*mm;
  G4double beamSigmaR = beamFWHM/(2*pow(2*log(2), 0.5));
  G4double beamX = G4RandGauss::shoot(0, beamSigmaR/pow(2, 0.5)),
           beamY = G4RandGauss::shoot(0, beamSigmaR/pow(2, 0.5));

  // Acquire model height via Messenger
  Messenger* messenger = Messenger::GetMessenger();
  G4double worldZLength = messenger->GetModelHeight();

  // Apply position and fire
  fParticleGun->SetParticlePosition(G4ThreeVector(beamX, beamY, -worldZLength));
  fParticleGun->GeneratePrimaryVertex(anEvent);
}

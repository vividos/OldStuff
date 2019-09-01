#pragma once

class CBacklightManager
{
public:
   enum EBacklightLevel
   {
      D0 = 0, // Full On: full power,  full functionality
      D1,     // Low Power On: fully functional at low power/performance
      D2,     // Standby: partially powered with automatic wake
      D3,     // Sleep: partially powered with device initiated wake
      D4,     // Off: unpowered
   };

   CBacklightManager(EBacklightLevel enBacklightLevel) throw();
   ~CBacklightManager() throw();

private:
   HANDLE m_hPower;
};

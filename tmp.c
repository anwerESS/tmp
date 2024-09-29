Subject: Summary of actions to debug the BDR module with the team

Hi [team or recipient names],

Please find below a summary of the steps and actions we have taken to resolve the debugging issues of the BDR module:

Dependency analysis
Result:
The LRT 6.0 version that BDR points to is no longer available on GitHub. The source code is required to generate debug symbols and enable debugging of the LRT module in BDR.

Attempt to compile BDR with LRT version 5.0
Result:
Build issue due to missing packages (version 5.0), which are no longer available on Nexus or GitHub.

Attempt to compile BDR with the latest LRT version 7.26.0
Result:
Linker error when running the executable db_gft_acc_2, as other BDR modules (PFC, UTI, CMN, etc.) are not aligned with this version.

Research and retrieval of the LRT version closest to the old LRT 6.0 version
We created a new branch tagged 6.0.CD and updated the pom.xml file with the latest dependency versions.
Result:
Breakpoints are now accessible in the RDG function through a configuration in Visual Studio, allowing us to debug different source code.

Thank you for your attention, and feel free to reach out if you have any further questions.

Best regards,
[your name]
